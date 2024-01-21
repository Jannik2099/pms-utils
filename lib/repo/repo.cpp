#include "repo/repo.hpp"

#include "atom/atom.hpp"
#include "atom/atom_parser.hpp"

#include <boost/spirit/home/x3/nonterminal/rule.hpp>
#include <filesystem>
#include <format>
#include <fstream>
#include <stdexcept>
#include <string_view>
#include <utility>

namespace x3 = boost::spirit::x3;

namespace {

struct ebuild_plus_version_t {
    pms_utils::atom::Name name;
    pms_utils::atom::Version version;
};

} // namespace

BOOST_FUSION_ADAPT_STRUCT(ebuild_plus_version_t, name, version);

namespace {

PARSER_RULE_T(ebuild_plus_version,
              ebuild_plus_version_t) = pms_utils::parsers::name
                                       >> x3::lit("-") >> pms_utils::parsers::package_version;

} // namespace

namespace pms_utils::repo {

Package::Package(std::filesystem::path path) : _path(std::move(path)), _name(_path.filename().string()) {}
Package::const_iterator Package::begin() const noexcept { return Package::const_iterator(*this); }
Package::const_iterator Package::cbegin() const noexcept { return begin(); }
Package::const_iterator Package::end() const noexcept {
    Package::const_iterator ret(*this);
    ret.iter = std::filesystem::end(ret.iter);
    return ret;
}
Package::const_iterator Package::cend() const noexcept { return end(); }

Category::Category(std::filesystem::path path) : _path(std::move(path)), _name(_path.filename().string()) {}
Category::const_iterator Category::begin() const noexcept { return Category::const_iterator(*this); }
Category::const_iterator Category::cbegin() const noexcept { return begin(); }
Category::const_iterator Category::end() const noexcept {
    Category::const_iterator ret(*this);
    ret.iter = std::filesystem::end(ret.iter);
    return ret;
}
Category::const_iterator Category::cend() const noexcept { return end(); }

Repository::Repository(std::filesystem::path path, std::string_view name)
    : _path(std::move(path)), _name(name) {}
Repository::const_iterator Repository::begin() const noexcept { return Repository::const_iterator(*this); }
Repository::const_iterator Repository::cbegin() const noexcept { return begin(); }
Repository::const_iterator Repository::end() const noexcept {
    Repository::const_iterator ret(*this);
    ret.index = ret.categories.size();
    return ret;
}
Repository::const_iterator Repository::cend() const noexcept { return end(); }

// BEGIN ITERATOR

// BEGIN PACKAGE

Ebuild Package::Iterator::make_value() const {
    ebuild_plus_version_t res;
    const std::string pathstr = iter->path().stem();
    auto begin = pathstr.begin();
    const auto end = pathstr.end();

    if (!parse(begin, end, ebuild_plus_version, res)) {
        // TODO
        throw std::runtime_error(std::format("failed to parse ebuild {}", iter->path().string()));
    }
    if (begin != end) {
        // TODO
        throw std::runtime_error(std::format(
            "failed to fully parse ebuild {}\n\tconsumed: {}\n\tremaining: {}", iter->path().string(),
            std::string_view(pathstr.begin(), begin), std::string_view(begin, end)));
    }
    if (res.name != path.filename().string()) {
        // TODO
        throw std::runtime_error(
            std::format("ebuild name {} did not match package name {}", res.name, path.filename().string()));
    }
    return Ebuild{iter->path(), res.name, res.version};
}

Package::Iterator::Iterator(const Package &package) : path(package.path()) {
    iter = std::filesystem::directory_iterator(path);
    while (iter != std::filesystem::end(iter) && iter->path().extension() != ".ebuild") {
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
    while (iter != std::filesystem::end(iter) && iter->path().extension() != ".ebuild") {
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
        throw std::runtime_error(
            std::format("attempted to compare Package iterators of different paths:\n{}\t{}", path.string(),
                        rhs.path.string()));
    }
    return iter == rhs.iter;
}

// END PACKAGE

// BEGIN CATEGORY

Category::Iterator::value_type Category::Iterator::init_value() {
    iter = std::filesystem::directory_iterator(path);
    while (iter != std::filesystem::end(iter)) {
        if (!iter->is_directory()) {
            iter++;
            continue;
        }

        std::string name;
        const std::string pathstr = iter->path().filename().string();
        auto begin = pathstr.begin();
        const auto end = pathstr.end();
        if (!parse(begin, end, pms_utils::parsers::name, name)) {
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
    return Package(*iter);
}

Category::Iterator::Iterator(const Category &category) : path(category.path()), elem(init_value()) {}

Category::Iterator &Category::Iterator::operator++() {
    iter++;
    while (iter != std::filesystem::end(iter) && !iter->is_directory()) {
        iter++;
    }
    if (iter != std::filesystem::end(iter) && iter->is_directory()) {
        elem = Package(*iter);
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
        throw std::runtime_error(
            std::format("attempted to compare Category iterators of different paths:\n{}\t{}", path.string(),
                        rhs.path.string()));
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
        throw std::runtime_error(std::format("categories file {} does not exist", catfile.string()));
    }
    std::ifstream fstream(catfile);

    for (std::string line; std::getline(fstream, line);) {
        auto begin = line.begin();
        const auto end = line.end();
        std::string parsed;
        if (!parse(begin, end, pms_utils::parsers::category, parsed) || begin != end) {
            // TODO
            throw std::runtime_error(std::format("malformed line in categories file: {}", line));
        }
        std::filesystem::path category = path / parsed;
        if (!std::filesystem::is_directory(category)) {
            // TODO
            throw std::runtime_error(std::format("category {} does not exist", category.string()));
        }
        ret.push_back(category);
    }
    return ret;
}

Repository::Iterator::value_type Repository::Iterator::init_value() const {
    if (index >= categories.size()) {
        return {};
    }
    return Category(categories.at(index));
}

Repository::Iterator::Iterator(const Repository &repository)
    : path(repository.path()), categories(init_categories()), elem(init_value()) {}

Repository::Iterator &Repository::Iterator::operator++() {
    index++;
    if (index < categories.size()) {
        elem = Category(categories.at(index));
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
        throw std::runtime_error(
            std::format("attempted to compare Repository iterators of different paths:\n{}\t{}",
                        path.string(), rhs.path.string()));
    }
    return index == rhs.index;
}

// END REPOSITORY

// END ITERATOR

} // namespace pms_utils::repo
