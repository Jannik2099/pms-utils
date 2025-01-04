#include "pms-utils/profile/profile.hpp"

#include "pms-utils/atom/atom.hpp"
#include "pms-utils/atom/atom_parser.hpp"
#include "pms-utils/profile/profile_parser.hpp"
#include "pms-utils/repo/repo.hpp"
#include "profile_wildcard.hpp"

#include <algorithm>
#include <boost/parser/parser.hpp>
#include <cstddef>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <memory>
#include <optional>
#include <ostream>
#include <ranges>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace [[gnu::visibility("default")]] pms_utils {
namespace profile {

namespace _internal {
std::ostream &WildcardAtom::ostream_impl(std::ostream &out) const {
    if (version_specifier.has_value()) {
        out << version_specifier.value();
    }
    out << category << "/" << name;
    if (version.has_value()) {
        out << "-";
        std::visit([&out](auto &&arg) { out << arg; }, version.value());
        if (version_specifier.value() == pms_utils::atom::VersionSpecifier::ea) {
            out << "*";
        }
    }
    if (slot.has_value()) {
        out << ":" << slot.value();
    }
    if (repo.has_value()) {
        out << "::" << repo.value();
    }
    return out;
}
} // namespace _internal

namespace {

std::string map_helper(const std::tuple<atom::PackageExpr, std::size_t> &elem,
                       const std::vector<repo::Repository> &repos) {
    return std::format("{}::{}", std::string{std::get<0>(elem)}, repos[std::get<1>(elem)].name());
}

// combines sacrifice into absorber
void combine_package_use_sets(_internal::unordered_str_set<std::string> &absorber,
                              const _internal::unordered_str_set<std::string> &sacrifice) {
    if (sacrifice.contains("*")) {
        absorber.clear();
        absorber.emplace("*");
    }
    if (sacrifice.contains("-*")) {
        absorber.clear();
        absorber.emplace("-*");
    }

    for (auto iter = sacrifice.begin(); iter != sacrifice.end();) {
        // simplify once P2077R3 becomes available
        if (const std::string_view val = *iter; val.starts_with("-")) {
            if (auto search = absorber.find(val.substr(1)); search != absorber.end()) {
                absorber.erase(search);
            }
        } else {
            if (auto search = absorber.find(std::string{"-"}.append(val)); search != absorber.end()) {
                absorber.erase(search);
            }
        }

        absorber.emplace(*iter);
        iter++;
    }
}

std::vector<std::filesystem::path> get_parent_paths(const std::filesystem::path &path) {
    std::vector<std::filesystem::path> ret;
    const auto file = path / "parent";
    std::ifstream stream{file};
    for (std::string line; std::getline(stream, line);) {
        auto test_path = path / line;
        if (!std::filesystem::is_directory(test_path)) {
            throw std::invalid_argument{
                std::format("parent profile directory {} specified in {} does not exist", test_path.string(),
                            file.string())};
        }
        ret.emplace_back(std::move(test_path));
    }
    return ret;
}

[[nodiscard]] std::vector<std::filesystem::path> get_config_files(const std::filesystem::path &path) {
    std::vector<std::filesystem::path> ret;
    if (!std::filesystem::exists(path)) {
        return {};
    }
    if (std::filesystem::is_regular_file(path)) {
        return {path};
    }
    for (const auto &file : std::filesystem::directory_iterator{path}) {
        if (!file.is_regular_file()) {
            continue;
        }
        const auto &file_path = file.path();
        if (file_path.filename().string().starts_with(".")) {
            continue;
        }
        ret.push_back(file_path);
    }
    // we could use a sorted set to begin with, but this is probably faster at low container sizes
    std::ranges::sort(ret);
    return ret;
}

[[nodiscard]] std::string read_config_files(const std::filesystem::path &path) {
    std::string ret;
    for (const std::vector<std::filesystem::path> files = get_config_files(path); const auto &file : files) {
        std::ifstream stream{file};
        for (std::string line; std::getline(stream, line);) {
            if (line.empty()) {
                continue;
            }
            if (line.starts_with("#")) {
                continue;
            }
            ret += line + '\n';
        }
    }
    return ret;
}

std::optional<std::filesystem::path> find_repo(const std::filesystem::path &path) {
    auto my_path = path;
    while (true) {
        if (my_path == "/") {
            return {};
        }
        my_path = my_path.parent_path();
        if ((my_path.stem() == "profiles") && std::filesystem::is_regular_file(my_path / "repo_name")) {
            break;
        }
    }
    return std::filesystem::canonical(my_path.parent_path());
}

void init_eapi(std::string &EAPI, const std::filesystem::path &path) {
    const auto file = path / "eapi";
    if (!std::filesystem::is_regular_file(file)) {
        return;
    }
    std::ifstream fstream{file};
    std::getline(fstream, EAPI);
}

void init_use_impl(_internal::unordered_str_set<std::string> &set, std::string_view line) {
    const auto *begin = line.begin();
    const auto *const end = line.end();
    if (!prefix_parse(begin, end, parsers::profile::package_use_values, set)) {
        throw std::invalid_argument{std::format("failed to parse use-style line {}", line)};
    }
    if (begin != end) {
        throw std::invalid_argument{std::format("incomplete parse on use-style line {}, remainder {}", line,
                                                std::string_view{begin, end})};
    }
}

void init_package_use_impl(
    const std::vector<repo::Repository> &repos,
    std::unordered_map<std::string, Filters, _internal::StringHash, std::equal_to<>> &map,
    _internal::unordered_str_set<std::string> Filters::*member, std::string_view lines) {
    for (const auto &line_ :
         std::views::split(lines, '\n') | std::views::filter([](auto line_) { return !std::empty(line_); })) {
        std::string_view line{line_.begin(), line_.end()};
        const auto *begin = line.begin();
        const auto *const end = line.end();
        std::tuple<_internal::WildcardAtom, _internal::unordered_str_set<std::string>> parsed;
        if (!prefix_parse(begin, end, parsers::profile::package_use_line, parsed)) {
            throw std::invalid_argument{std::format("failed to parse package.use-style line {}", line)};
        }
        if (begin != end) {
            throw std::invalid_argument{
                std::format("incomplete parse on package.use-style line {}, remainder {}", line,
                            std::string_view{begin, end})};
        }
        const auto ebuilds = expand_package_expr(std::get<0>(parsed), repos);
        for (const auto &ebuild : ebuilds) {
            combine_package_use_sets(map[map_helper(ebuild, repos)].*member, std::get<1>(parsed));
        }
    }
}

template <typename M, typename K, typename V>
void set_or_append(M &map, K &&key, V &&value)
    requires std::is_same_v<std::remove_cvref_t<K>, std::string> &&
             std::is_same_v<std::remove_cvref_t<V>, std::string>
{
    // use const& up until the last usage of key / value to prevent calling non-const overloads
    const auto &c_key = std::as_const(key);
    const auto &c_value = std::as_const(value);
    if ((c_key == "USE") || (c_key == "USE_EXPAND") || (c_key == "USE_EXPAND_HIDDEN") ||
        (c_key == "CONFIG_PROTECT") || (c_key == "CONFIG_PROTECT_MASK") || (c_key == "IUSE_IMPLICIT") ||
        (c_key == "USE_EXPAND_IMPLICIT") || (c_key == "USE_EXPAND_UNPREFIXED") || (c_key == "ENV_UNSET")) {
        const auto iter = map.find(c_key);
        const std::string &prev = iter != map.end() ? iter->second : "";
        if (prev.empty()) {
            map[std::forward<K>(key)] = std::forward<V>(value);
            return;
        }
        std::string temp = prev;
        if (!c_value.empty()) {
            temp += " " + std::forward<V>(value);
        }
        map[std::forward<K>(key)] = std::move(temp);
    } else {
        map[std::forward<K>(key)] = std::forward<V>(value);
    }
}

// I really ought to make this a common helper
template <typename P> auto parser_helper(P parser, std::string_view str) {
    typename P::parser_type::attr_type res;
    if (str.empty()) {
        return res;
    }
    const auto *begin = str.begin();
    if (const auto *const end = str.end(); (!prefix_parse(begin, end, parser, res)) || (begin != end)) {
        throw std::invalid_argument{std::format("make.defaults element {} appears invalid", str)};
    }
    return res;
}

std::vector<repo::Repository> repos_from_portage(const std::filesystem::path &path) {
    std::vector<repo::Repository> ret;
    if (!std::filesystem::exists(path / "repos.conf")) {
        return {repo::Repository{"/var/db/repos/gentoo"}};
    }
    for (const auto lines = read_config_files(path / "repos.conf");
         const auto &line : std::views::split(lines, '\n')) {
        if (line.empty()) {
            continue;
        }
        const std::string_view line_str{line.data(), line.size()};
        if (line_str.starts_with("location = ")) {
            repo::Repository repo{line_str.substr(sizeof("location = ") - 1)};
            if (std::ranges::find(ret, repo) != ret.end()) {
                throw std::invalid_argument{
                    std::format("duplicate repos.conf entry {}", repo.path().string())};
            }
            ret.emplace_back(std::move(repo));
        }
    }

    return ret;
}

void algo_5_1_helper(const _internal::unordered_str_set<std::string> &set, const atom::Useflag &useflag,
                     std::optional<bool> &val) {
    if (set.contains("*")) {
        val = true;
    } else if (set.contains("-*")) {
        val = false;
    }
    if (set.contains(useflag)) {
        val = true;
        return;
    }
    if (set.contains("-" + useflag)) {
        val = false;
        return;
    }
}

} // namespace

std::vector<std::tuple<atom::PackageExpr, std::size_t>>
expand_package_expr(std::string_view expr, const std::vector<repo::Repository> &repos) {
    _internal::WildcardAtom atom;
    const auto *begin = expr.begin();
    const auto *const end = expr.end();
    if (!prefix_parse(begin, end, parsers::profile::wildcard_atom, atom)) {
        throw std::invalid_argument{std::format("expression {} does not match valid wildcard syntax", expr)};
    }
    if (begin != end) {
        throw std::invalid_argument{
            std::format("expression {} matches wildcard syntax partially, remainder {}", expr,
                        std::string_view{begin, end})};
    }
    return expand_package_expr(atom, repos);
}
std::vector<std::tuple<atom::PackageExpr, std::size_t>>
expand_package_expr(const _internal::WildcardAtom &atom, const std::vector<repo::Repository> &repos) {
    if (repos.empty()) {
        throw std::invalid_argument{"expand_package_expr called with empty Repository list"};
    }

    _internal::Expander expander{atom, repos};
    return std::move(expander.atoms());
}

// make.defaults get concatenated first, between parents and their children (depth-first), and then evaluated.
// this means that two parents A: FOO="bar", B: BAR="$FOO"
// with an inheritance like C: parents = A, B
// will end up with C: BAR="bar"
// in other words, multiple parents act as if they were to inherit each other
void Profile::combine_parents() {
    for (const auto &parent : parents_) {
        if (parent == nullptr) {
            throw std::runtime_error{"parent is nullptr"};
        }
        make_defaults_unevaluated_.insert(make_defaults_unevaluated_.end(),
                                          parent->make_defaults_unevaluated_.begin(),
                                          parent->make_defaults_unevaluated_.end());

        packages_.insert(parent->packages_.begin(), parent->packages_.end());
    }
}

void Profile::init_make_defaults() {
    if (const auto file = path_ / "make.defaults"; std::filesystem::is_regular_file(file)) {
        const std::ifstream fstream{file};
        std::stringstream stream;
        stream << fstream.rdbuf();
        const std::string_view content = stream.view();
        std::vector<std::tuple<std::string, std::string>> my_make_defaults;
        const auto *begin = content.begin();
        const auto *const end = content.end();
        if (!prefix_parse(begin, end, parsers::profile::make_defaults, my_make_defaults)) {
            throw std::invalid_argument{"make.defaults appear invalid"};
        }
        if (begin != end) {
            throw std::invalid_argument{"make.defaults appear invalid"};
        }
        make_defaults_unevaluated_.insert(make_defaults_unevaluated_.end(), my_make_defaults.begin(),
                                          my_make_defaults.end());
    }

    for (const auto &[key, line] : make_defaults_unevaluated_) {
        if (line.find('$') == decltype(line)::npos) {
            set_or_append(make_defaults_, key, line);
            continue;
        }
        std::string expanded_line;
        auto begin = line.begin();
        const auto end = line.end();
        std::vector<std::tuple<std::string, bool>> parsed_line;
        if ((!prefix_parse(begin, end, parsers::profile::make_defaults_shlex, parsed_line)) ||
            (begin != end)) {
            throw std::invalid_argument{std::format("make.defaults element {} appears invalid", line)};
        }
        for (const auto &[elem, is_variable] : parsed_line) {
            if (!is_variable) {
                expanded_line += elem + ' ';
                continue;
            }
            const auto iter = make_defaults_.find(expanded_line);
            if (iter != make_defaults_.end()) {
                expanded_line += iter->second + ' ';
            }
        }
        if (expanded_line.ends_with(' ')) {
            expanded_line.pop_back();
        }
        set_or_append(make_defaults_, key, expanded_line);
    }
}

void Profile::init_packages() {
    const auto lines = read_config_files(path_ / "packages");
    for (const auto &line_ :
         std::views::split(lines, '\n') | std::views::filter([](auto line_) { return !std::empty(line_); })) {
        std::string_view line{line_.begin(), line_.end()};
        bool negate = false;
        if (line.starts_with("#")) {
            continue;
        }
        if (line.starts_with("-")) {
            negate = true;
            line = line.substr(1);
        }
        if (line.starts_with("*")) {
            line = line.substr(1);
        } else {
            throw std::invalid_argument{"packages entry without leading * is deprecated"};
        }
        atom::PackageExpr val;
        const auto *begin = line.begin();
        const auto *const end = line.end();
        if (!prefix_parse(begin, end, parsers::atom::package_dep, val)) {
            throw std::invalid_argument{std::format("failed to parse {} as a PackageExpr", line)};
        }
        if (begin != end) {
            throw std::invalid_argument{std::format("incomplete parse on {} as a PackageExpr, remainder {}",
                                                    line, std::string_view{begin, end})};
        }
        if (val.blocker.has_value()) {
            throw std::invalid_argument{std::format("expression {} may not have blocker", std::string{val})};
        }
        if (!val.usedeps.empty()) {
            throw std::invalid_argument{std::format("expression {} may not have usedeps", std::string{val})};
        }
        if (negate) {
            packages_.erase(val);
        } else {
            packages_.emplace(std::move(val));
        }
    }
}

void Profile::init_package_mask() {
    const auto lines = read_config_files(path_ / "package.mask");
    for (const auto &line_ :
         std::views::split(lines, '\n') | std::views::filter([](auto line_) { return !std::empty(line_); })) {
        std::string_view line{line_.begin(), line_.end()};
        bool is_masked = true;
        if (line.starts_with("-")) {
            line = line.substr(1);
            is_masked = false;
        }
        const auto ebuilds = expand_package_expr(line, repos_);
        for (const auto &ebuild : ebuilds) {
            if (const auto iter = filters_.find(map_helper(ebuild, repos_)); iter != filters_.end()) {
                iter->second.masked = is_masked;
                continue;
            }
            // if the filter does not exist and would not be changed from the default value, we avoid
            // creating it
            if (!is_masked) {
                continue;
            }
            filters_[map_helper(ebuild, repos_)].masked = is_masked;
        }
    }
}

std::optional<bool> Profile::algorithm_5_1_force(std::string_view package, const atom::Useflag &useflag,
                                                 bool is_stable, const Filters *filter) const {
    std::optional<bool> forced;
    for (const auto &parent : parents()) {
        const auto iter = parent->filters().find(package);
        const Filters *parent_filter = iter != parent->filters().end() ? &iter->second : nullptr;
        const auto parent_forced = parent->algorithm_5_1_force(package, useflag, is_stable, parent_filter);
        if (parent_forced.has_value()) {
            forced = parent_forced;
        }
    }
    algo_5_1_helper(use_force(), useflag, forced);
    if (is_stable) {
        algo_5_1_helper(use_stable_force(), useflag, forced);
    }
    if (filter != nullptr) {
        algo_5_1_helper(filter->use_force, useflag, forced);
        if (is_stable) {
            algo_5_1_helper(filter->use_stable_force, useflag, forced);
        }
    }

    return forced;
}

std::optional<bool> Profile::algorithm_5_1_mask(std::string_view package, const atom::Useflag &useflag,
                                                bool is_stable, const Filters *filter) const {
    std::optional<bool> masked;
    for (const auto &parent : parents()) {
        const auto iter = parent->filters().find(package);
        const Filters *parent_filter = iter != parent->filters().end() ? &iter->second : nullptr;
        const auto parent_masked = parent->algorithm_5_1_mask(package, useflag, is_stable, parent_filter);
        if (parent_masked.has_value()) {
            masked = parent_masked;
        }
    }
    algo_5_1_helper(use_mask(), useflag, masked);
    if (is_stable) {
        algo_5_1_helper(use_stable_mask(), useflag, masked);
    }
    if (filter != nullptr) {
        algo_5_1_helper(filter->use_mask, useflag, masked);
        if (is_stable) {
            algo_5_1_helper(filter->use_stable_mask, useflag, masked);
        }
    }

    return masked;
}

_internal::unordered_str_set<atom::Useflag> Profile::effective_useflags(const atom::PackageExpr &atom) const {
    _internal::unordered_str_set<atom::Useflag> ret;

    // TODO: get ebuild from repo priorities, masks etc.
    // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
    const auto ebuild = repos_.at(0)[atom.category].value()[atom.name].value()[atom.version.value()].value();
    const auto &ebuild_useflags = ebuild.metadata().IUSE;
    const auto &arch = ARCH();
    const bool is_stable = std::ranges::find_if(ebuild_useflags, [&arch](const auto &elem) {
                               return elem.useflag == arch;
                           }) != std::ranges::end(ebuild_useflags);
    for (const auto &flag : ebuild_useflags) {
        std::optional<bool> do_flag;
        if (flag.default_enabled) {
            do_flag = true;
        }
        if (USE().contains(flag.useflag)) {
            do_flag = true;
        } else if (USE().contains('-' + flag.useflag)) {
            do_flag = false;
        }
        if (IUSE_IMPLICIT().contains(flag.useflag)) {
            do_flag = true;
        }
        const auto package_str = std::string{atom} + "::" + repos_.at(0).name();
        const auto iter = filters().find(package_str);
        const Filters *filter = iter != filters().end() ? &iter->second : nullptr;
        if (filter != nullptr) {
            if (filter->use.contains(flag.useflag)) {
                do_flag = true;
            } else if (filter->use.contains('-' + flag.useflag)) {
                do_flag = false;
            }
        }
        if (algorithm_5_1_force(package_str, flag.useflag, is_stable, filter).value_or(false)) {
            do_flag = true;
        }
        if (algorithm_5_1_mask(package_str, flag.useflag, is_stable, filter).value_or(false)) {
            do_flag = false;
        }

        if (!do_flag.has_value()) {
            continue;
        }
        if (do_flag.value()) {
            ret.emplace(flag.useflag);
        } else {
            ret.erase(flag.useflag);
        }
    }

    return ret;
}

Profile::Profile(const std::filesystem::path &path, std::vector<repo::Repository> repos)
    : Profile{path, std::move(repos), nullptr} {}

Profile::Profile(const std::filesystem::path &path, std::vector<repo::Repository> repos,
                 const std::shared_ptr<Profile> &injected_parent, bool is_portage_profile)
    : is_portage_profile_{is_portage_profile} {
    if (!path.has_filename()) {
        path_ = path.parent_path();
    } else {
        path_ = path;
    }
    path_ = std::filesystem::canonical(path_);

    if (injected_parent != nullptr) {
        parents_.emplace_back(injected_parent);
    } else {
        for (const auto &dir : get_parent_paths(path_)) {
            parents_.emplace_back(std::make_shared<Profile>(dir));
        }
    }

    // TODO
    const auto my_repo = find_repo(path_);
    if (!repos.empty()) {
        repos_ = std::move(repos);
    } else {
        if (my_repo.has_value()) {
            repo::Repository own_repo{my_repo.value()};
            if (std::ranges::find(repos_, own_repo) == repos_.end()) {
                repos_.emplace_back(std::move(own_repo));
            }
        } else {
            throw std::invalid_argument{
                std::format("could not find Repository belonging to Profile {}", path_.string())};
        }
    }
    if (my_repo.has_value()) {
        name_ = path_.lexically_relative(my_repo.value() / "profiles");
    } else {
        name_ = path_;
    }

    combine_parents();
    init_make_defaults();
    init_packages();
    init_package_mask();
    init_eapi(EAPI_, path_);

    // TODO: should verify against profiles/arch.list
    ARCH_ = make_defaults_["ARCH"];

    for (const std::vector<std::tuple<decltype(&Filters::use), std::string_view>> package_elems =
             {{&Filters::use, "package.use"},
              {&Filters::use_mask, "package.use.mask"},
              {&Filters::use_force, "package.use.force"},
              {&Filters::use_stable_mask, "package.use.stable.mask"},
              {&Filters::use_stable_force, "package.use.stable.force"}};
         const auto &[filter_elem, path_suffix] : package_elems) {
        const auto lines = read_config_files(path_ / path_suffix);
        init_package_use_impl(repos_, filters_, filter_elem, lines);
    }

    for (const std::vector<std::tuple<decltype(use_mask_) &, std::string_view>> use_elems =
             {{use_mask_, "use.mask"},
              {use_force_, "use.force"},
              {use_stable_mask_, "use.stable.mask"},
              {use_stable_force_, "use.stable.force"}};
         const auto &[set, path_suffix] : use_elems) {
        const auto lines = read_config_files(path_ / path_suffix);
        for (const auto &line : std::views::split(lines, '\n') |
                                    std::views::filter([](auto line_) { return !std::empty(line_); })) {
            init_use_impl(set, std::string_view{line.begin(), line.end()});
        }
    }

    USE_ = parser_helper(parsers::profile::USE, make_defaults_["USE"]);
    USE_EXPAND_ = parser_helper(parsers::profile::USE_EXPAND, make_defaults_["USE_EXPAND"]);
    USE_EXPAND_HIDDEN_ =
        parser_helper(parsers::profile::USE_EXPAND_HIDDEN, make_defaults_["USE_EXPAND_HIDDEN"]);
    CONFIG_PROTECT_ = parser_helper(parsers::profile::CONFIG_PROTECT, make_defaults_["CONFIG_PROTECT"]);
    CONFIG_PROTECT_MASK_ =
        parser_helper(parsers::profile::CONFIG_PROTECT_MASK, make_defaults_["CONFIG_PROTECT_MASK"]);
    IUSE_IMPLICIT_ = parser_helper(parsers::profile::IUSE_IMPLICIT, make_defaults_["IUSE_IMPLICIT"]);
    USE_EXPAND_IMPLICIT_ =
        parser_helper(parsers::profile::USE_EXPAND_IMPLICIT, make_defaults_["USE_EXPAND_IMPLICIT"]);
    USE_EXPAND_UNPREFIXED_ =
        parser_helper(parsers::profile::USE_EXPAND_UNPREFIXED, make_defaults_["USE_EXPAND_UNPREFIXED"]);
    ENV_UNSET_ = parser_helper(parsers::profile::ENV_UNSET, make_defaults_["ENV_UNSET"]);
}

Profile PortageProfile::init_base(const std::filesystem::path &path, std::vector<repo::Repository> repos) {
    const std::filesystem::path user_profile_path = path / "profile";
    const auto parent_profile = std::make_shared<Profile>(path / "make.profile");
    std::shared_ptr<Profile> user_profile;
    if (std::filesystem::is_directory(user_profile_path)) {
        user_profile = std::make_shared<Profile>(Profile{user_profile_path, repos, parent_profile});
    }
    return {path, std::move(repos), user_profile, true};
}

PortageProfile::PortageProfile(const std::filesystem::path &path)
    : Profile{init_base(path, repos_from_portage(path))} {}

} // namespace profile
} // namespace pms_utils
