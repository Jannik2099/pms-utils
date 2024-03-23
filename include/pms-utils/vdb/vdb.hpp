#pragma once

#include "pms-utils/atom/atom.hpp"

#include <cstddef>
#include <filesystem>
#include <vector>

namespace [[gnu::visibility("default")]] pms_utils {
namespace vdb {

class Pkg;
class Category;

class Vdb {
public:
    using difference_type = std::ptrdiff_t;
    using value_type = Category;
    using pointer = value_type *;
    using reference = value_type &;
    using const_iterator = std::vector<Category>::const_iterator;

private:
    std::filesystem::path _path;
    std::vector<Category> categories;

public:
    Vdb(std::filesystem::path);
    [[nodiscard]] const_iterator begin() const noexcept { return categories.begin(); };
    [[nodiscard]] const_iterator cbegin() const noexcept { return begin(); };
    [[nodiscard]] const_iterator end() const noexcept { return categories.end(); };
    [[nodiscard]] const_iterator cend() const noexcept { return end(); };
    [[nodiscard]] constexpr const std::filesystem::path &path() const noexcept { return _path; };
};

class Category {
public:
    using difference_type = std::ptrdiff_t;
    using value_type = Category;
    using pointer = value_type *;
    using reference = value_type &;
    using const_iterator = std::vector<Pkg>::const_iterator;

private:
    friend class Vdb;
    Category(std::filesystem::path);
    std::filesystem::path _path;
    pms_utils::atom::Category _category;
    std::vector<Pkg> pkgs;

public:
    [[nodiscard]] const_iterator begin() const noexcept { return pkgs.begin(); };
    [[nodiscard]] const_iterator cbegin() const noexcept { return begin(); };
    [[nodiscard]] const_iterator end() const noexcept { return pkgs.end(); };
    [[nodiscard]] const_iterator cend() const noexcept { return end(); };
    [[nodiscard]] constexpr const std::filesystem::path &path() const noexcept { return _path; };
    [[nodiscard]] constexpr const pms_utils::atom::Category &category() const noexcept { return _category; };
};

class Pkg {};

} // namespace vdb
} // namespace pms_utils
