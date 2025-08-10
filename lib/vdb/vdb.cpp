#include "pms-utils/vdb/vdb.hpp"

#include "pms-utils/atom/atom.hpp"
#include "pms-utils/atom/atom_parser.hpp"
#include "pms-utils/depend/depend_parser.hpp"
#include "pms-utils/misc/try_parse.hpp"
#include "pms-utils/vdb/vdb_parser.hpp"
#include "util/optional.hpp"
#include "util/readfile.hpp"

#include <boost/mp11/algorithm.hpp>
#include <expected>
#include <filesystem>
#include <format>
#include <optional>
#include <ostream>
#include <ranges>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <variant>

using pms_utils::misc::try_parse;
using pms_utils::misc::_internal::try_readfile;
using namespace std::literals::string_view_literals;

namespace {

struct IoError {};

struct ParseError {
    std::string unconsumed;
};

using Error = std::variant<IoError, ParseError>;

} // namespace

template <> struct std::formatter<IoError> {
    [[nodiscard]] constexpr static auto parse(const std::format_parse_context &context) {
        return context.begin();
    }

    constexpr static auto format(const IoError & /**/, std::format_context &context) {
        return std::format_to(context.out(), "IO Error");
    }
};

template <> struct std::formatter<ParseError> {
    [[nodiscard]] constexpr static auto parse(const std::format_parse_context &context) {
        return context.begin();
    }

    constexpr static auto format(const ParseError &err, std::format_context &context) {
        return std::format_to(context.out(), "Parser Error: '{}'", err.unconsumed);
    }
};

template <> struct std::formatter<Error> {
    [[nodiscard]] constexpr static auto parse(const std::format_parse_context &context) {
        return context.begin();
    }

    constexpr static auto format(const Error &err, std::format_context &context) {
        return std::visit(
            [&context](const auto &arg) -> auto { return std::format_to(context.out(), "{}", arg); }, err);
    }
};

namespace [[gnu::visibility("default")]] pms_utils {
namespace vdb {

Obj::operator std::string() const { return std::format("obj {} {} {}", path.native(), md5, size); };

std::ostream &Obj::ostream_impl(std::ostream &out) const {
    out << std::string{*this};
    return out;
};

Dir::operator std::string() const { return std::format("dir {}", native()); };

std::ostream &Dir::ostream_impl(std::ostream &out) const {
    out << std::string{*this};
    return out;
};

Sym::operator std::string() const {
    return std::format("sym {} -> {} {}", src.native(), dest.native(), size);
};

std::ostream &Sym::ostream_impl(std::ostream &out) const {
    out << std::string{*this};
    return out;
};

Content::operator std::string() const {
    return std::visit([](const auto &content) { return std::string{content}; }, data);
};

std::ostream &Content::ostream_impl(std::ostream &out) const {
    std::visit([&out](const auto &content) { out << content; }, data);
    return out;
};

Package::operator std::string() const {
    return std::format("{}-{}", std::string{name}, std::string{version});
};

std::ostream &Package::ostream_impl(std::ostream &out) const {
    out << std::string{*this};
    return out;
};

Entry::Entry(std::filesystem::path path) : path_{std::move(path)} {
    {
        const std::string dirname = path_.filename().string();

        package_ = unwrap(try_parse(dirname, pms_utils::parsers::vdb::package, true)
                              .transform_error([&dirname](const auto &err) {
                                  return Error{ParseError{std::string{dirname.cbegin(), err}}};
                              }),
                          std::format("failed to parse package name: {}", dirname));
    }

    {
        const auto kinds = {"DEPEND"sv, "BDEPEND"sv, "RDEPEND"sv};
        const auto targets = {&depend_, &bdepend_, &rdepend_};
        for (const auto &[kind, target] : std::views::zip(kinds, targets)) {
            const std::filesystem::path depend_path = path_ / kind;

            if (std::filesystem::exists(depend_path)) {
                const std::string input =
                    unwrap(ok_or(try_readfile(depend_path), Error{IoError{}}),
                           std::format("failed to read {} file {}", kind, depend_path.native()));

                *target = unwrap(try_parse(input, pms_utils::parsers::depend::nodes, true)
                                     .transform_error([&input](const auto &err) {
                                         return Error{ParseError{std::string{input.cbegin(), err}}};
                                     }),
                                 std::format("failed to parse {} file {}", kind, depend_path.native()));
            }
        }
    }

    {
        for (std::string_view file : {"CONTENTS"sv, "SIZE"sv}) {
            if (const std::filesystem::path file_path = path_ / file; !std::filesystem::exists(file_path)) {
                throw std::runtime_error{std::format("{} file not found at {}", file, file_path.native())};
            }
        }
    }

    {
        const std::tuple files{"KEYWORDS"sv, "IUSE"sv, "USE"sv, "CONTENTS"sv, "SIZE"sv};
        const std::tuple targets{&keywords_, &iuse_, &use_, &contents_, &size_};
        const std::tuple parsers{pms_utils::parsers::vdb::_internal::VDB_KEYWORDS,
                                 pms_utils::parsers::vdb::_internal::VDB_IUSE,
                                 pms_utils::parsers::vdb::_internal::use_flags,
                                 pms_utils::parsers::vdb::contents, pms_utils::parsers::vdb::_internal::size};

        const auto init_fn = [this](std::string_view file, auto *target, const auto &parser) {
            const std::filesystem::path file_path = path_ / file;

            if (std::filesystem::exists(file_path)) {
                *target = unwrap(
                    ok_or(try_readfile(file_path), Error{IoError{}})
                        .and_then([&parser](const std::string &input) {
                            return try_parse(input, parser, true).transform_error([&input](const auto &err) {
                                return Error{ParseError{{input.cbegin(), err}}};
                            });
                        }),
                    std::format("failed to read {} file {}", file, file_path.native()));
            }
        };

        boost::mp11::mp_for_each<boost::mp11::mp_iota_c<std::tuple_size_v<decltype(files)>>>(
            [&init_fn, &files, &targets, &parsers](auto idx) {
                init_fn(std::get<idx>(files), std::get<idx>(targets), std::get<idx>(parsers));
            });
    }

    {
        const std::filesystem::path repository_file = path_ / "repository";
        repository_ = unwrap(ok_or(try_readfile(repository_file), Error{IoError{}}),
                             std::format("failed to read repository file {}", repository_file.native()));
    }
}

Vdb::Vdb(std::filesystem::path path) : path_{std::move(path)} {};

Vdb::Iterator Vdb::begin() const {
    auto iter = std::filesystem::directory_iterator{path_};
    if (iter == std::filesystem::end(iter)) {
        return end();
    }
    Category category{*iter};
    return Iterator{iter, category};
}

Vdb::Iterator Vdb::cbegin() const { return begin(); };

Vdb::Iterator Vdb::end() const {
    auto end = std::filesystem::end(std::filesystem::directory_iterator{path_});
    return Iterator{end, std::nullopt};
}

Vdb::Iterator Vdb::cend() const { return end(); }

Category::Category(std::filesystem::path path) : path_{std::move(path)} {
    const std::string dirname = path_.filename().string();

    category_ = unwrap(try_parse(dirname, pms_utils::parsers::atom::category, true)
                           .transform_error([&dirname](const auto &err) {
                               return Error{ParseError{{dirname.cbegin(), err}}};
                           }),
                       std::format("invalid category: {}", dirname));
};

Category::Iterator Category::begin() const {
    auto iter = std::filesystem::directory_iterator{path_};
    if (iter == std::filesystem::end(iter)) {
        return end();
    }
    Entry entry{*iter};
    return Iterator{iter, entry};
}

Category::Iterator Category::cbegin() const { return begin(); }

Category::Iterator Category::end() const {
    auto end = std::filesystem::end(std::filesystem::directory_iterator{path_});
    return Iterator{end, std::nullopt};
}

Category::Iterator Category::cend() const { return end(); }

Vdb::Iterator::Iterator(std::filesystem::directory_iterator iter, std::optional<Category> category)
    : it{std::move(iter)}, category{std::move(category)} {};

bool Vdb::Iterator::operator==(const Vdb::Iterator &other) const { return it == other.it; }

Vdb::Iterator::reference Vdb::Iterator::operator*() const { return category.value(); }

Vdb::Iterator::pointer Vdb::Iterator::operator->() const { return &category.value(); }

Vdb::Iterator &Vdb::Iterator::operator++() {
    if (it != std::filesystem::end(it)) {
        it++;
    }

    if (it != std::filesystem::end(it)) {
        category = Category{it->path()};
    } else {
        category.reset();
    }

    return *this;
}

Vdb::Iterator Vdb::Iterator::operator++(int) {
    Iterator ret = *this;
    operator++();
    return ret;
}

Category::Iterator::Iterator(std::filesystem::directory_iterator iter, std::optional<value_type> entry)
    : it{std::move(iter)}, entry{std::move(entry)} {}

bool Category::Iterator::operator==(const Category::Iterator &other) const { return it == other.it; }

Category::Iterator::reference Category::Iterator::operator*() const { return entry.value(); }

Category::Iterator::pointer Category::Iterator::operator->() const { return &entry.value(); }

Category::Iterator &Category::Iterator::operator++() {
    if (it != std::filesystem::end(it)) {
        it++;
    }

    if (it != std::filesystem::end(it)) {
        entry = Entry{it->path()};
    } else {
        entry.reset();
    }

    return *this;
}

Category::Iterator Category::Iterator::operator++(int) {
    Iterator ret = *this;
    operator++();
    return ret;
}

} // namespace vdb
} // namespace pms_utils
