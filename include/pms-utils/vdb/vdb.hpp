#pragma once

#include "pms-utils/atom/atom.hpp"
#include "pms-utils/depend/depend.hpp"
#include "pms-utils/ebuild/ebuild.hpp"
#include "pms-utils/misc/meta.hpp"

#include <boost/container_hash/hash.hpp>
#include <boost/describe/class.hpp>
#include <boost/mp11/detail/mp_list.hpp>
#include <boost/mp11/list.hpp> // IWYU pragma: keep
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <ostream>
#include <string>
#include <variant>
#include <vector>

//
#include "pms-utils/misc/macro-begin.hpp"

namespace [[gnu::visibility("default")]] pms_utils {
namespace vdb {

struct Obj {
private:
    std::ostream &ostream_impl(std::ostream &out) const;

public:
    std::filesystem::path path;
    std::string md5;
    std::uint64_t size;

    explicit operator std::string() const;
    friend std::ostream &operator<<(std::ostream &out, const Obj &obj) { return obj.ostream_impl(out); }

    BOOST_DESCRIBE_CLASS(Obj, (), (path, md5, size), (), (ostream_impl));
};

struct Dir : public std::filesystem::path {
private:
    std::ostream &ostream_impl(std::ostream &out) const;

public:
    explicit operator std::string() const;
    friend std::ostream &operator<<(std::ostream &out, const Dir &dir) { return dir.ostream_impl(out); };

    BOOST_DESCRIBE_CLASS(Dir, (), (), (), (ostream_impl));
};

struct Sym {
private:
    std::ostream &ostream_impl(std::ostream &out) const;

public:
    std::filesystem::path src;
    std::filesystem::path dest;
    std::uint64_t size;

    explicit operator std::string() const;
    friend std::ostream &operator<<(std::ostream &out, const Sym &sym) { return sym.ostream_impl(out); };

    BOOST_DESCRIBE_CLASS(Sym, (), (src, dest, size), (), (ostream_impl));
};

struct Content : public std::variant<Obj, Dir, Sym> {
private:
    std::ostream &ostream_impl(std::ostream &out) const;

public:
    explicit operator std::string() const;
    friend std::ostream &operator<<(std::ostream &out, const Content &content) {
        return content.ostream_impl(out);
    };

    BOOST_DESCRIBE_CLASS(Content, (), (), (), (ostream_impl));
};

class Package {
private:
    std::ostream &ostream_impl(std::ostream &out) const;

public:
    pms_utils::atom::Name name;
    pms_utils::atom::Version version;

    explicit operator std::string() const;
    friend std::ostream &operator<<(std::ostream &out, const Package &package) {
        return package.ostream_impl(out);
    };

    BOOST_DESCRIBE_CLASS(Package, (), (name, version), (), (ostream_impl));
};

class Entry {
private:
    std::filesystem::path path_;
    Package package_;
    pms_utils::depend::DependExpr depend_;
    pms_utils::depend::DependExpr bdepend_;
    pms_utils::depend::DependExpr rdepend_;
    pms_utils::ebuild::keywords keywords_;
    pms_utils::ebuild::iuse iuse_;
    std::vector<pms_utils::atom::Useflag> use_;
    std::string repository_;
    std::uint64_t size_;
    std::vector<Content> contents_;

public:
    [[nodiscard]] Entry(std::filesystem::path path);

    [[nodiscard]] const std::filesystem::path &path() const noexcept [[clang::lifetimebound]] {
        return path_;
    };

    [[nodiscard]] const Package &package() const noexcept [[clang::lifetimebound]] { return package_; };

    [[nodiscard]] const depend::DependExpr &depend() const noexcept [[clang::lifetimebound]] {
        return depend_;
    };

    [[nodiscard]] const depend::DependExpr &bdepend() const noexcept [[clang::lifetimebound]] {
        return depend_;
    };

    [[nodiscard]] const depend::DependExpr &rdepend() const noexcept [[clang::lifetimebound]] {
        return depend_;
    };

    [[nodiscard]] const ebuild::keywords &keywords() const noexcept [[clang::lifetimebound]] {
        return keywords_;
    };

    [[nodiscard]] const ebuild::iuse &iuse() const noexcept [[clang::lifetimebound]] { return iuse_; };

    [[nodiscard]] const std::vector<atom::Useflag> &use() const noexcept [[clang::lifetimebound]] {
        return use_;
    };

    [[nodiscard]] const std::string &repository() const noexcept [[clang::lifetimebound]] {
        return repository_;
    };

    [[nodiscard]] std::uint64_t size() const noexcept { return size_; };

    [[nodiscard]] const std::vector<Content> &contents() const noexcept [[clang::lifetimebound]] {
        return contents_;
    };

    BOOST_DESCRIBE_CLASS(Entry, (),
                         (path, package, depend, bdepend, rdepend, keywords, iuse, use, repository, size,
                          contents),
                         (),
                         (path_, package_, depend_, bdepend_, rdepend_, keywords_, iuse_, use_, repository_,
                          size_, contents_));
};

class Category;

class Vdb {
public:
    class Iterator;
    using difference_type = std::ptrdiff_t;
    using value_type = Category;
    using pointer = value_type *;
    using reference = value_type &;
    using const_iterator = Iterator;

private:
    std::filesystem::path path_;

public:
    [[nodiscard]] Vdb(std::filesystem::path path);

    [[nodiscard]] const std::filesystem::path &path() const { return path_; };

    [[nodiscard]] const_iterator begin() const [[clang::lifetimebound]];
    [[nodiscard]] const_iterator cbegin() const [[clang::lifetimebound]];
    [[nodiscard]] const_iterator end() const [[clang::lifetimebound]];
    [[nodiscard]] const_iterator cend() const [[clang::lifetimebound]];

    BOOST_DESCRIBE_CLASS(Vdb, (), (path, begin, cbegin, end, cend), (), (path_));
};

class Category {
public:
    class Iterator;
    using difference_type = std::ptrdiff_t;
    using value_type = Entry;
    using pointer = value_type *;
    using reference = value_type &;
    using const_iterator = Iterator;

    friend Vdb;

private:
    std::filesystem::path path_;
    pms_utils::atom::Category category_;

public:
    [[nodiscard]] Category(std::filesystem::path path);

    [[nodiscard]] const std::filesystem::path &path() const [[clang::lifetimebound]] { return path_; };

    [[nodiscard]] const pms_utils::atom::Category &category() const [[clang::lifetimebound]] {
        return category_;
    };

    [[nodiscard]] const_iterator begin() const [[clang::lifetimebound]];
    [[nodiscard]] const_iterator cbegin() const [[clang::lifetimebound]];
    [[nodiscard]] const_iterator end() const [[clang::lifetimebound]];
    [[nodiscard]] const_iterator cend() const [[clang::lifetimebound]];

    BOOST_DESCRIBE_CLASS(Category, (), (path, category, begin, cbegin, end, cend), (), (path_, category_));
};

class Vdb::Iterator {
public:
    using difference_type = std::ptrdiff_t;
    using value_type = Category;
    using pointer = value_type *;
    using reference = value_type &;

    friend Vdb;

private:
    Iterator(std::filesystem::directory_iterator iter, std::optional<Category> category);

    std::filesystem::directory_iterator it;
    mutable std::optional<Category> category;

public:
    [[nodiscard]] bool operator==(const Iterator &other) const;

    reference operator*() const [[clang::lifetimebound]];
    pointer operator->() const [[clang::lifetimebound]];

    Iterator &operator++();
    Iterator &operator++(int amt);

    BOOST_DESCRIBE_CLASS(Vdb::Iterator, (), (), (), (it, category));
};

class Category::Iterator {
public:
    using difference_type = std::ptrdiff_t;
    using value_type = Entry;
    using pointer = value_type *;
    using reference = value_type &;

    friend Category;

private:
    Iterator(std::filesystem::directory_iterator iter, std::optional<Entry> entry);

    std::filesystem::directory_iterator it;
    mutable std::optional<Entry> entry;

public:
    [[nodiscard]] bool operator==(const Iterator &other) const;

    reference operator*() const [[clang::lifetimebound]];
    pointer operator->() const [[clang::lifetimebound]];

    Iterator &operator++();
    Iterator &operator++(int amt);

    BOOST_DESCRIBE_CLASS(Category::Iterator, (), (), (), (it, entry));
};

[[nodiscard]] inline std::size_t hash_value(const Vdb &vdb) {
    return boost::hash<std::filesystem::path>{}(vdb.path());
}

[[nodiscard]] inline std::size_t hash_value(const Category &category) {
    return boost::hash<std::filesystem::path>{}(category.path());
}

namespace meta {
using all = boost::mp11::mp_list<Obj, Dir, Sym, Content, Package, Entry, Vdb, Vdb::Iterator, Category,
                                 Category::Iterator>;
}

} // namespace vdb
} // namespace pms_utils
// namespace pms_utils

#include "pms-utils/misc/macro-end.hpp"

PMS_UTILS_FOOTER(vdb);
