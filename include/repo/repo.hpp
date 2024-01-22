#pragma once

#include "atom/atom.hpp"

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace [[gnu::visibility("default")]] pms_utils {
namespace repo {

struct Ebuild {
    std::filesystem::path path;
    pms_utils::atom::Name name;
    pms_utils::atom::Version version;
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
};

// END ITERATOR

} // namespace repo
} // namespace pms_utils
