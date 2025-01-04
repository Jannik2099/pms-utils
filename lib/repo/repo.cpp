#include "pms-utils/repo/repo.hpp"

#include "pms-utils/atom/atom.hpp"
#include "pms-utils/atom/atom_parser.hpp"
#include "pms-utils/depend/depend_parser.hpp"
#include "pms-utils/ebuild/ebuild.hpp"
#include "pms-utils/ebuild/ebuild_parser.hpp"

#include <boost/parser/parser.hpp>
#include <cstring>
#include <filesystem>
#include <format>
#include <fstream>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace bp = boost::parser;

namespace {

struct ebuild_plus_version_t {
    pms_utils::atom::Name name;
    pms_utils::atom::Version version;
};

} // namespace

namespace {

const auto ebuild_plus_version = pms_utils::parsers::atom::name >>
                                 bp::lit('-') >> pms_utils::parsers::atom::package_version;

} // namespace

namespace [[gnu::visibility("default")]] pms_utils {
namespace repo {

// BEGIN EBUILD

namespace {

template <typename Member, typename Rule>
bool metadata_parser(std::string_view line, std::string_view name, Member &member, Rule rule) {
    if (!line.starts_with(name)) {
        return false;
    }
    line = line.substr(name.length());
    const auto *begin = line.begin();
    if (const auto *const end = line.end(); prefix_parse(begin, end, rule, member)) {
        if (begin == end) {
            return true;
        }
        member = Member{};
    }
    return false;
}

} // namespace

// not sure how to better organize, suggestions welcome
// NOLINTNEXTLINE(readability-function-cognitive-complexity)
const ebuild::Metadata &Ebuild::metadata() const {
    if (_metadata.has_value()) {
        return _metadata.value();
    }
    ebuild::Metadata meta;
    const std::filesystem::path category = path.parent_path().parent_path();
    const std::filesystem::path repopath = category.parent_path();
    const std::filesystem::path cachefile = repopath / "metadata" / "md5-cache" / category.filename() /
                                            std::string{name + "-" + std::string{version}};
    std::ifstream stream{cachefile};
    for (std::string line; std::getline(stream, line);) {
        bool parsed = false;
        parsed = metadata_parser(line, "DEPEND=", meta.DEPEND, parsers::depend::nodes);
        if (parsed) {
            continue;
        }
        parsed = metadata_parser(line, "RDEPEND=", meta.RDEPEND, parsers::depend::nodes);
        if (parsed) {
            continue;
        }
        parsed = metadata_parser(line, "SLOT=", meta.SLOT, parsers::atom::slot);
        if (parsed) {
            continue;
        }
        parsed = metadata_parser(line, "SRC_URI=", meta.SRC_URI, parsers::ebuild::SRC_URI);
        if (parsed) {
            continue;
        }
        parsed = metadata_parser(line, "RESTRICT=", meta.RESTRICT, parsers::ebuild::RESTRICT);
        if (parsed) {
            continue;
        }
        parsed = metadata_parser(line, "HOMEPAGE=", meta.HOMEPAGE, parsers::ebuild::HOMEPAGE);
        if (parsed) {
            continue;
        }
        parsed = metadata_parser(line, "LICENSE=", meta.LICENSE, parsers::ebuild::LICENSE);
        if (parsed) {
            continue;
        }
        if (line.starts_with("DESCRIPTION=")) {
            meta.DESCRIPTION = line.substr(strlen("DESCRIPTION="));
            continue;
        }
        parsed = metadata_parser(line, "KEYWORDS=", meta.KEYWORDS, parsers::ebuild::KEYWORDS);
        if (parsed) {
            continue;
        }
        parsed = metadata_parser(line, "INHERIT=", meta.INHERITED, parsers::ebuild::INHERITED);
        if (parsed) {
            continue;
        }
        parsed = metadata_parser(line, "IUSE=", meta.IUSE, parsers::ebuild::IUSE);
        if (parsed) {
            continue;
        }
        parsed = metadata_parser(line, "REQUIRED_USE=", meta.REQUIRED_USE, parsers::ebuild::REQUIRED_USE);
        if (parsed) {
            continue;
        }
        parsed = metadata_parser(line, "PDEPEND=", meta.PDEPEND, parsers::depend::nodes);
        if (parsed) {
            continue;
        }
        parsed = metadata_parser(line, "BDEPEND=", meta.BDEPEND, parsers::depend::nodes);
        if (parsed) {
            continue;
        }
        parsed = metadata_parser(line, "EAPI=", meta.EAPI, parsers::ebuild::EAPI);
        if (parsed) {
            continue;
        }
        parsed = metadata_parser(line, "PROPERTIES=", meta.PROPERTIES, parsers::ebuild::PROPERTIES);
        if (parsed) {
            continue;
        }
        parsed =
            metadata_parser(line, "DEFINED_PHASES=", meta.DEFINED_PHASES, parsers::ebuild::DEFINED_PHASES);
        if (parsed) {
            continue;
        }
        parsed = metadata_parser(line, "IDEPEND=", meta.IDEPEND, parsers::depend::nodes);
        if (parsed) {
            continue;
        }
    }
    _metadata = std::move(meta);
    return _metadata.value();
}

Ebuild::Ebuild(std::filesystem::path _path, pms_utils::atom::Name _name, pms_utils::atom::Version _version)
    : path{std::move(_path)}, name{std::move(_name)}, version{std::move(_version)} {
    if (!std::filesystem::is_regular_file(path)) {
        throw std::invalid_argument{std::format("provided file {} does not exist", path.string())};
    }
    if (path.stem() != std::format("{}-{}", std::string{name}, std::string{version})) {
        throw std::invalid_argument{std::format("filename {} did not match provided Name + Version {}-{}",
                                                path.stem().string(), std::string{name},
                                                std::string{version})};
    }
    if (path.extension() != ".ebuild") {
        throw std::invalid_argument{std::format("provided file {} does not end in .ebuild", path.string())};
    }
}

// END EBUILD

Package::Package(std::filesystem::path path) : path_{std::move(path)}, name_{path_.filename().string()} {
    if (!std::filesystem::is_directory(path_)) {
        throw std::invalid_argument{std::format("provided path {} does not exist", path_.string())};
    }
}
Package::const_iterator Package::begin() const noexcept { return Package::const_iterator{*this}; }
Package::const_iterator Package::cbegin() const noexcept { return begin(); }
Package::const_iterator Package::end() const noexcept {
    Package::const_iterator ret{*this};
    ret.iter = std::filesystem::end(ret.iter);
    return ret;
}
Package::const_iterator Package::cend() const noexcept { return end(); }

std::optional<Ebuild> Package::operator[](const atom::Version &version) const {
    const std::filesystem::path ebuild_path =
        path_ / std::format("{}-{}.ebuild", std::string{name_}, std::string{version});
    if (!std::filesystem::is_regular_file(ebuild_path)) {
        return {};
    }
    return Ebuild{ebuild_path, name_, version};
}
std::optional<Ebuild> Package::operator[](std::string_view version) const {
    const auto *begin = version.begin();
    const auto *const end = version.end();
    atom::Version ver;
    if ((!prefix_parse(begin, end, parsers::atom::package_version, ver)) || (begin != end)) {
        throw std::invalid_argument{std::format("argument {} is not a valid Version expression", version)};
    }
    return (*this)[ver];
}

Category::Category(std::filesystem::path path) : path_{std::move(path)}, name_{path_.filename().string()} {
    if (!std::filesystem::is_directory(path_)) {
        throw std::invalid_argument{std::format("provided path {} does not exist", path_.string())};
    }
}
Category::const_iterator Category::begin() const noexcept { return Category::const_iterator{*this}; }
Category::const_iterator Category::cbegin() const noexcept { return begin(); }
Category::const_iterator Category::end() const noexcept {
    Category::const_iterator ret{*this};
    ret.iter = std::filesystem::end(ret.iter);
    return ret;
}
Category::const_iterator Category::cend() const noexcept { return end(); }

std::optional<Package> Category::operator[](std::string_view package) const {
    const auto *begin = package.begin();
    const auto *const end = package.end();
    atom::Name package_name;
    if ((!prefix_parse(begin, end, parsers::atom::name, package_name)) || (begin != end)) {
        throw std::invalid_argument{
            std::format("argument {} is not a valid Package Name expression", package)};
    }
    const std::filesystem::path package_path = path_ / std::string{package_name};
    if (!std::filesystem::is_directory(package_path)) {
        return {};
    }
    return Package{package_path};
}

Repository::Repository(std::filesystem::path path) : path_{std::move(path)} {
    if (!std::filesystem::is_directory(path_)) {
        throw std::invalid_argument{std::format("provided path {} does not exist", path_.string())};
    }
    const auto repo_name_file = path_ / "profiles" / "repo_name";
    if (!std::filesystem::is_regular_file(repo_name_file)) {
        throw std::invalid_argument{
            std::format("Repository {} does not appear valid, missing profiles/repo_name", path_.string())};
    }
    std::ifstream stream{repo_name_file};
    // TODO: validate
    std::getline(stream, name_);
}
Repository::const_iterator Repository::begin() const noexcept { return Repository::const_iterator{*this}; }
Repository::const_iterator Repository::cbegin() const noexcept { return begin(); }
Repository::const_iterator Repository::end() const noexcept {
    Repository::const_iterator ret{*this};
    ret.index = ret.categories.size();
    return ret;
}
Repository::const_iterator Repository::cend() const noexcept { return end(); }

std::optional<Category> Repository::operator[](std::string_view category) const {
    const auto *begin = category.begin();
    const auto *const end = category.end();
    atom::Category category_name;
    if ((!prefix_parse(begin, end, parsers::atom::category, category_name)) || (begin != end)) {
        throw std::invalid_argument{
            std::format("argument {} is not a valid Package Name expression", category)};
    }
    const std::filesystem::path category_path = path_ / std::string{category_name};
    if (!std::filesystem::is_directory(category_path)) {
        return {};
    }
    return Category{category_path};
}

// BEGIN ITERATOR

// BEGIN PACKAGE

Ebuild Package::Iterator::make_value() const {
    ebuild_plus_version_t res;
    const std::string pathstr = iter->path().stem();
    auto begin = pathstr.begin();
    const auto end = pathstr.end();

    if (!prefix_parse(begin, end, ebuild_plus_version, res)) {
        // TODO
        throw std::runtime_error{std::format("failed to parse ebuild {}", iter->path().string())};
    }
    if (begin != end) {
        // TODO
        throw std::runtime_error{std::format(
            "failed to fully parse ebuild {}\n\tconsumed: {}\n\tremaining: {}", iter->path().string(),
            std::string_view{pathstr.begin(), begin}, std::string_view{begin, end})};
    }
    if (res.name != path.filename().string()) {
        // TODO
        throw std::runtime_error{std::format("ebuild name {} did not match package name {}",
                                             std::string_view{res.name}, path.filename().string())};
    }
    return Ebuild{iter->path(), res.name, res.version};
}

Package::Iterator::Iterator(const Package &package) : path{package.path()} {
    iter = std::filesystem::directory_iterator{path};
    while ((iter != std::filesystem::end(iter)) && (iter->path().extension() != ".ebuild")) {
        iter++;
    }
    if (iter == std::filesystem::end(iter)) {
        elem = {};
        return;
    }
    elem = make_value();
}

Package::Iterator &Package::Iterator::operator++() {
    iter++;
    while ((iter != std::filesystem::end(iter)) && (iter->path().extension() != ".ebuild")) {
        iter++;
    }
    if (iter != std::filesystem::end(iter)) {
        elem = make_value();
    }
    return *this;
}

Package::Iterator Package::Iterator::operator++(int) {
    Iterator ret = *this;
    operator++();
    return ret;
}

bool Package::Iterator::operator==(const Iterator &rhs) const {
    if (path != rhs.path) {
        // TODO
        throw std::runtime_error{
            std::format("attempted to compare Package iterators of different paths:\n{}\t{}", path.string(),
                        rhs.path.string())};
    }
    return iter == rhs.iter;
}

// END PACKAGE

// BEGIN CATEGORY

Category::Iterator::value_type Category::Iterator::init_value() {
    iter = std::filesystem::directory_iterator{path};
    while (iter != std::filesystem::end(iter)) {
        if (!iter->is_directory()) {
            iter++;
            continue;
        }

        std::string name;
        const std::string pathstr = iter->path().filename().string();
        auto begin = pathstr.begin();
        const auto end = pathstr.end();
        if (!prefix_parse(begin, end, pms_utils::parsers::atom::name, name)) {
            iter++;
            continue;
        }
        if (begin != end) {
            iter++;
            continue;
        }
        break;
    }
    if (iter == std::filesystem::end(iter)) {
        return {};
    }
    return Package{*iter};
}

Category::Iterator::Iterator(const Category &category) : path{category.path()}, elem{init_value()} {}

Category::Iterator &Category::Iterator::operator++() {
    iter++;
    while ((iter != std::filesystem::end(iter)) && (!iter->is_directory())) {
        iter++;
    }
    if ((iter != std::filesystem::end(iter)) && (iter->is_directory())) {
        elem = Package{*iter};
    }
    return *this;
}

Category::Iterator Category::Iterator::operator++(int) {
    Iterator ret = *this;
    operator++();
    return ret;
}

bool Category::Iterator::operator==(const Iterator &rhs) const {
    if (path != rhs.path) {
        // TODO
        throw std::runtime_error{
            std::format("attempted to compare Category iterators of different paths:\n{}\t{}", path.string(),
                        rhs.path.string())};
    }

    return iter == rhs.iter;
}

// END CATEGORY

// BEGIN REPOSITORY

std::vector<std::filesystem::path> Repository::Iterator::init_categories() const {
    std::vector<std::filesystem::path> ret;
    const std::filesystem::path catfile = path / "profiles" / "categories";
    if (!std::filesystem::is_regular_file(catfile)) {
        // TODO
        throw std::runtime_error{std::format("categories file {} does not exist", catfile.string())};
    }
    std::ifstream fstream{catfile};

    for (std::string line; std::getline(fstream, line);) {
        auto begin = line.begin();
        const auto end = line.end();
        std::string parsed;
        if ((!prefix_parse(begin, end, pms_utils::parsers::atom::category, parsed)) || (begin != end)) {
            // TODO
            throw std::runtime_error{std::format("malformed line in categories file: {}", line)};
        }
        const std::filesystem::path category = path / parsed;
        if (!std::filesystem::is_directory(category)) {
            // TODO
            throw std::runtime_error{std::format("category {} does not exist", category.string())};
        }
        ret.push_back(category);
    }
    return ret;
}

Repository::Iterator::value_type Repository::Iterator::init_value() const {
    if (index >= categories.size()) {
        return {};
    }
    return Category{categories.at(index)};
}

Repository::Iterator::Iterator(const Repository &repository)
    : path{repository.path()}, categories{init_categories()}, elem{init_value()} {}

Repository::Iterator &Repository::Iterator::operator++() {
    index++;
    if (index < categories.size()) {
        elem = Category{categories.at(index)};
    }
    return *this;
}

Repository::Iterator Repository::Iterator::operator++(int) {
    Iterator ret = *this;
    operator++();
    return ret;
}

bool Repository::Iterator::operator==(const Iterator &rhs) const {
    if (path != rhs.path) {
        // TODO
        throw std::runtime_error{
            std::format("attempted to compare Repository iterators of different paths:\n{}\t{}",
                        path.string(), rhs.path.string())};
    }
    return index == rhs.index;
}

// END REPOSITORY

// END ITERATOR

} // namespace repo
} // namespace pms_utils
