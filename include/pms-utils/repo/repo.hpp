#pragma once

#include "pms-utils/atom/atom.hpp"
#include "pms-utils/depend/depend.hpp"
#include "pms-utils/ebuild/ebuild.hpp"
#include "pms-utils/misc/meta.hpp"

#include <boost/describe.hpp>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace [[gnu::visibility("default")]] pms_utils {
namespace repo {

struct Metadata {
    depend::DependExpr DEPEND;
    depend::DependExpr RDEPEND;
    atom::Slot SLOT;
    ebuild::SRC_URI SRC_URI;
    ebuild::RESTRICT RESTRICT;
    ebuild::HOMEPAGE HOMEPAGE;
    ebuild::LICENSE LICENSE;
    std::string DESCRIPTION;
    ebuild::KEYWORDS KEYWORDS;
    ebuild::INHERITED INHERITED;
    ebuild::IUSE IUSE;
    ebuild::REQUIRED_USE REQUIRED_USE;
    depend::DependExpr PDEPEND;
    depend::DependExpr BDEPEND;
    ebuild::EAPI EAPI;
    ebuild::PROPERTIES PROPERTIES;
    ebuild::DEFINED_PHASES DEFINED_PHASES;
    depend::DependExpr IDEPEND;
};

struct Ebuild {
private:
    mutable std::optional<Metadata> _metadata;

public:
    std::filesystem::path path;
    pms_utils::atom::Name name;
    pms_utils::atom::Version version;
    const Metadata &metadata() const;

    Ebuild() = default;
    Ebuild(std::filesystem::path path, pms_utils::atom::Name name, pms_utils::atom::Version version);

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
    std::filesystem::path _path;
    std::string _name;

public:
    [[nodiscard]] const_iterator begin() const noexcept;
    [[nodiscard]] const_iterator cbegin() const noexcept;
    [[nodiscard]] const_iterator end() const noexcept;
    [[nodiscard]] const_iterator cend() const noexcept;

    [[nodiscard]] Repository(std::filesystem::path path, std::string_view name);

    [[nodiscard]] constexpr const std::filesystem::path &path() const noexcept { return _path; }
    [[nodiscard]] constexpr const std::string &name() const noexcept { return _name; }

    BOOST_DESCRIBE_CLASS(Repository, (), (begin, cbegin, end, cend, path, name), (), (_path, _name));
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
    std::filesystem::path _path;
    pms_utils::atom::Category _name;

    [[nodiscard]] Category() = default;

    friend Repository::Iterator;

public:
    [[nodiscard]] const_iterator begin() const noexcept;
    [[nodiscard]] const_iterator cbegin() const noexcept;
    [[nodiscard]] const_iterator end() const noexcept;
    [[nodiscard]] const_iterator cend() const noexcept;

    [[nodiscard]] explicit Category(std::filesystem::path path);

    [[nodiscard]] constexpr const std::filesystem::path &path() const noexcept { return _path; }
    [[nodiscard]] constexpr const std::string &name() const noexcept { return _name; }

    BOOST_DESCRIBE_CLASS(Category, (), (begin, cbegin, end, cend, path, name), (), (_path, _name));
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
    std::filesystem::path _path;
    pms_utils::atom::Name _name;

    [[nodiscard]] Package() = default;

    friend Category::Iterator;

public:
    [[nodiscard]] const_iterator begin() const noexcept;
    [[nodiscard]] const_iterator cbegin() const noexcept;
    [[nodiscard]] const_iterator end() const noexcept;
    [[nodiscard]] const_iterator cend() const noexcept;

    [[nodiscard]] explicit Package(std::filesystem::path path);

    [[nodiscard]] constexpr const std::filesystem::path &path() const noexcept { return _path; }
    [[nodiscard]] constexpr const std::string &name() const noexcept { return _name; }

    BOOST_DESCRIBE_CLASS(Package, (), (begin, cbegin, end, cend, path, name), (), (_path, _name));
};

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
    [[nodiscard]] constexpr reference operator*() const noexcept { return elem; };
    [[nodiscard]] constexpr pointer operator->() const noexcept { return &elem; };

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
    [[nodiscard]] constexpr reference operator*() const noexcept { return elem; };
    [[nodiscard]] constexpr pointer operator->() const noexcept { return &elem; };

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
    [[nodiscard]] constexpr reference operator*() const noexcept { return elem; };
    [[nodiscard]] constexpr pointer operator->() const noexcept { return &elem; };

    Iterator &operator++();
    Iterator operator++(int);
    [[nodiscard]] bool operator==(const Iterator &rhs) const;

    [[nodiscard]] explicit Iterator(const Repository &repository);

    BOOST_DESCRIBE_CLASS(Iterator, (), (), (), (path, categories, index, elem));
};

// END ITERATOR

// BEGIN DESCRIBE

BOOST_DESCRIBE_STRUCT(Metadata, (),
                      (DEPEND, RDEPEND, SLOT, SRC_URI, RESTRICT, HOMEPAGE, LICENSE, DESCRIPTION, KEYWORDS,
                       INHERITED, IUSE, REQUIRED_USE, PDEPEND, BDEPEND, EAPI, PROPERTIES, DEFINED_PHASES,
                       IDEPEND));

namespace meta {

using all = boost::mp11::mp_list<Metadata, Ebuild, Category, Repository, Package, Package::Iterator,
                                 Category::Iterator, Repository::Iterator>;
static_assert(boost::mp11::mp_is_set<all>{});
static_assert(boost::mp11::mp_all_of<all, pms_utils::meta::is_described>{});

} // namespace meta

// END DESCRIBE

} // namespace repo
} // namespace pms_utils
