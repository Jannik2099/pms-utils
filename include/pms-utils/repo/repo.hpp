#pragma once

#include "pms-utils/atom/atom.hpp"
#include "pms-utils/ebuild/ebuild.hpp"
#include "pms-utils/misc/meta.hpp"

#include <boost/container_hash/hash.hpp>
#include <boost/describe/class.hpp>
#include <boost/mp11/list.hpp> // IWYU pragma: keep
#include <cstddef>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

//
#include "pms-utils/misc/macro-begin.hpp"

namespace [[gnu::visibility("default")]] pms_utils {
namespace repo {

struct Ebuild {
private:
    mutable std::optional<ebuild::Metadata> _metadata;

public:
    std::filesystem::path path;
    pms_utils::atom::Name name;
    pms_utils::atom::Version version;
    const ebuild::Metadata &metadata() const [[clang::lifetimebound]];

    [[nodiscard]] Ebuild() = default;
    [[nodiscard]] Ebuild(std::filesystem::path path, pms_utils::atom::Name name,
                         pms_utils::atom::Version version);

    BOOST_DESCRIBE_CLASS(Ebuild, (), (path, name, version, metadata), (), (_metadata));
};

class Category;

class Repository {
public:
    class Iterator;

    using difference_type = std::ptrdiff_t;
    using value_type = Category;
    using pointer = value_type *;
    using reference = value_type &;
    using const_iterator = Iterator;

private:
    std::filesystem::path path_;
    std::string name_;

public:
    [[nodiscard]] const_iterator begin() const noexcept [[clang::lifetimebound]];
    [[nodiscard]] const_iterator cbegin() const noexcept [[clang::lifetimebound]];
    [[nodiscard]] const_iterator end() const noexcept [[clang::lifetimebound]];
    [[nodiscard]] const_iterator cend() const noexcept [[clang::lifetimebound]];

    [[nodiscard]] explicit Repository(std::filesystem::path path);

    [[nodiscard]] constexpr const std::filesystem::path &path() const noexcept [[clang::lifetimebound]] {
        return path_;
    }
    [[nodiscard]] constexpr const std::string &name() const noexcept [[clang::lifetimebound]] {
        return name_;
    }

    [[nodiscard]] std::optional<Category> operator[](std::string_view category) const;

    BOOST_DESCRIBE_CLASS(Repository, (), (begin, cbegin, end, cend, path, name), (), (path_, name_));
};

class Package;

class Category {
public:
    class Iterator;

    using difference_type = std::ptrdiff_t;
    using value_type = Package;
    using pointer = value_type *;
    using reference = value_type &;
    using const_iterator = Iterator;

private:
    std::filesystem::path path_;
    pms_utils::atom::Category name_;

    [[nodiscard]] Category() = default;

    friend Repository::Iterator;

public:
    [[nodiscard]] const_iterator begin() const noexcept [[clang::lifetimebound]];
    [[nodiscard]] const_iterator cbegin() const noexcept [[clang::lifetimebound]];
    [[nodiscard]] const_iterator end() const noexcept [[clang::lifetimebound]];
    [[nodiscard]] const_iterator cend() const noexcept [[clang::lifetimebound]];

    [[nodiscard]] explicit Category(std::filesystem::path path);

    [[nodiscard]] constexpr const std::filesystem::path &path() const noexcept [[clang::lifetimebound]] {
        return path_;
    }
    [[nodiscard]] constexpr const pms_utils::atom::Category &name() const noexcept [[clang::lifetimebound]] {
        return name_;
    }

    [[nodiscard]] std::optional<Package> operator[](std::string_view package) const;

    BOOST_DESCRIBE_CLASS(Category, (), (begin, cbegin, end, cend, path, name), (), (path_, name_));
};

class Package {
public:
    class Iterator;

    using difference_type = std::ptrdiff_t;
    using value_type = Ebuild;
    using pointer = value_type *;
    using reference = value_type &;
    using const_iterator = Iterator;

private:
    std::filesystem::path path_;
    pms_utils::atom::Name name_;

    [[nodiscard]] Package() = default;

    friend Category::Iterator;

public:
    [[nodiscard]] const_iterator begin() const noexcept [[clang::lifetimebound]];
    [[nodiscard]] const_iterator cbegin() const noexcept [[clang::lifetimebound]];
    [[nodiscard]] const_iterator end() const noexcept [[clang::lifetimebound]];
    [[nodiscard]] const_iterator cend() const noexcept [[clang::lifetimebound]];

    [[nodiscard]] explicit Package(std::filesystem::path path);

    [[nodiscard]] constexpr const std::filesystem::path &path() const noexcept [[clang::lifetimebound]] {
        return path_;
    }
    [[nodiscard]] constexpr const pms_utils::atom::Name &name() const noexcept [[clang::lifetimebound]] {
        return name_;
    }

    [[nodiscard]] std::optional<Ebuild> operator[](const atom::Version &version) const;
    [[nodiscard]] std::optional<Ebuild> operator[](std::string_view version) const;

    BOOST_DESCRIBE_CLASS(Package, (), (begin, cbegin, end, cend, path, name), (), (path_, name_));
};

// BEGIN HASH

[[nodiscard]] inline std::size_t hash_value(const Ebuild &ebuild) {
    return boost::hash<std::filesystem::path>{}(ebuild.path);
}

[[nodiscard]] inline std::size_t hash_value(const Repository &repository) {
    return boost::hash<std::filesystem::path>{}(repository.path());
}

[[nodiscard]] inline std::size_t hash_value(const Category &category) {
    return boost::hash<std::filesystem::path>{}(category.path());
}

[[nodiscard]] inline std::size_t hash_value(const Package &package) {
    return boost::hash<std::filesystem::path>{}(package.path());
}

// END HASH

// BEGIN ITERATOR

class Package::Iterator {
public:
    using difference_type = std::ptrdiff_t;
    using value_type = Ebuild;
    using pointer = const value_type *;
    using reference = const value_type &;

private:
    std::filesystem::path path;
    std::filesystem::directory_iterator iter;
    value_type elem;

    [[nodiscard]] value_type make_value() const;

    friend class Package;

public:
    [[nodiscard]] constexpr reference operator*() const noexcept [[clang::lifetimebound]] { return elem; };
    [[nodiscard]] constexpr pointer operator->() const noexcept [[clang::lifetimebound]] { return &elem; };

    Iterator &operator++();
    Iterator operator++(int);
    [[nodiscard]] bool operator==(const Iterator &rhs) const;

    [[nodiscard]] explicit Iterator(const Package &package);

    BOOST_DESCRIBE_CLASS(Iterator, (), (), (), (path, iter, elem));
};

class Category::Iterator {
public:
    using difference_type = std::ptrdiff_t;
    using value_type = Package;
    using pointer = const value_type *;
    using reference = const value_type &;

private:
    std::filesystem::path path;
    std::filesystem::directory_iterator iter;
    value_type elem;

    [[nodiscard]] value_type init_value();

    friend class Category;

public:
    [[nodiscard]] constexpr reference operator*() const noexcept [[clang::lifetimebound]] { return elem; };
    [[nodiscard]] constexpr pointer operator->() const noexcept [[clang::lifetimebound]] { return &elem; };

    Iterator &operator++();
    Iterator operator++(int);
    [[nodiscard]] bool operator==(const Iterator &rhs) const;

    [[nodiscard]] explicit Iterator(const Category &category);

    BOOST_DESCRIBE_CLASS(Iterator, (), (), (), (path, iter, elem));
};

class Repository::Iterator {
public:
    using difference_type = std::ptrdiff_t;
    using value_type = Category;
    using pointer = const value_type *;
    using reference = const value_type &;

private:
    std::filesystem::path path;
    std::vector<std::filesystem::path> categories;
    std::vector<std::filesystem::path>::size_type index{};
    value_type elem;

    [[nodiscard]] std::vector<std::filesystem::path> init_categories() const;
    [[nodiscard]] value_type init_value() const;

    friend class Repository;

public:
    [[nodiscard]] constexpr reference operator*() const noexcept [[clang::lifetimebound]] { return elem; };
    [[nodiscard]] constexpr pointer operator->() const noexcept [[clang::lifetimebound]] { return &elem; };

    Iterator &operator++();
    Iterator operator++(int);
    [[nodiscard]] bool operator==(const Iterator &rhs) const;

    [[nodiscard]] explicit Iterator(const Repository &repository);

    BOOST_DESCRIBE_CLASS(Iterator, (), (), (), (path, categories, index, elem));
};

// END ITERATOR

// BEGIN DESCRIBE

namespace meta {

using all = boost::mp11::mp_list<Ebuild, Category, Repository, Package, Package::Iterator, Category::Iterator,
                                 Repository::Iterator>;

} // namespace meta

// END DESCRIBE

} // namespace repo

namespace meta {

template <> struct is_owning_iterator<repo::Package::Iterator> {
    constexpr static bool value = true;
};
template <> struct is_owning_iterator<repo::Category::Iterator> {
    constexpr static bool value = true;
};
template <> struct is_owning_iterator<repo::Repository::Iterator> {
    constexpr static bool value = true;
};

} // namespace meta

} // namespace pms_utils

PMS_UTILS_FOOTER(repo);

//
#include "pms-utils/misc/macro-end.hpp"
