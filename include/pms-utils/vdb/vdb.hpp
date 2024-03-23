#pragma once

#include <cstddef>
#include <filesystem>
#include <vector>

namespace [[gnu::visibility("default")]] pms_utils {
namespace vdb {

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

class Category {};

} // namespace vdb
} // namespace pms_utils
