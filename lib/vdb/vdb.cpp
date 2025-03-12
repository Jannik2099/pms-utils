#include "pms-utils/vdb/vdb.hpp"

#include "pms-utils/atom/atom.hpp"
#include "pms-utils/atom/atom_parser.hpp"
#include "pms-utils/depend/depend_parser.hpp"
#include "pms-utils/ebuild/ebuild.hpp"
#include "pms-utils/misc/try_parse.hpp"
#include "pms-utils/vdb/vdb_parser.hpp"
#include "util/optional.hpp"
#include "util/readfile.hpp"

#include <cstring>
#include <expected>
#include <filesystem>
#include <format>
#include <iterator>
#include <optional>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

using pms_utils::misc::try_parse;
using pms_utils::misc::_internal::try_readfile;

struct IoError {};

struct ParseError {
    std::string unconsumed;
};

using Error = std::variant<IoError, ParseError>;

template <> struct std::formatter<IoError> {
    constexpr static auto parse(std::format_parse_context &context) { return context.begin(); }

    constexpr static auto format(const IoError & /**/, std::format_context &context) {
        return std::format_to(context.out(), "IO Error");
    }
};

template <> struct std::formatter<ParseError> {
    constexpr static auto parse(std::format_parse_context &context) { return context.begin(); }

    constexpr static auto format(const ParseError &err, std::format_context &context) {
        return std::format_to(context.out(), "Parser Error: '{}'", err.unconsumed);
    }
};

template <> struct std::formatter<Error> {
    constexpr static auto parse(std::format_parse_context &context) { return context.begin(); }

    constexpr static auto format(const Error &err, std::format_context &context) {
        return std::visit([&context](auto &&arg) -> auto { return std::format_to(context.out(), "{}", arg); },
                          err);
    }
};

namespace [[gnu::visibility("default")]] pms_utils {
namespace vdb {

Obj::operator std::string() const { return std::format("obj {} {} {}", path.c_str(), md5, size); };

std::ostream &Obj::ostream_impl(std::ostream &out) const {
    out << std::string{*this};
    return out;
};

Dir::operator std::string() const { return std::format("dir {}", this->c_str()); };

std::ostream &Dir::ostream_impl(std::ostream &out) const {
    out << std::string{*this};
    return out;
};

Sym::operator std::string() const { return std::format("sym {} -> {} {}", src.c_str(), dest.c_str(), size); };

std::ostream &Sym::ostream_impl(std::ostream &out) const {
    out << std::string{*this};
    return out;
};

Content::operator std::string() const {
    return std::visit([](const auto &content) { return std::string{content}; }, *this);
};

std::ostream &Content::ostream_impl(std::ostream &out) const {
    std::visit([&out](const auto &content) { out << content; }, *this);
    return out;
};

Package::operator std::string() const {
    return std::format("{}-{}", std::string{name}, std::string{version});
};

std::ostream &Package::ostream_impl(std::ostream &out) const {
    out << std::string{*this};
    return out;
};

Entry::Entry(std::filesystem::path path) : path_(std::move(path)) {
    {
        auto stem = path_.stem().string();

        package_ = unwrap(
            try_parse(stem, pms_utils::parsers::vdb::package, true).transform_error([&stem](const auto err) {
                return Error{ParseError{std::string(stem.cbegin(), err)}};
            }),
            std::format("failed to parse package name: {}", stem));
    }

    {
        for (const auto kind :
             {std::string_view("DEPEND"), std::string_view("BDEPEND"), std::string_view("RDEPEND")}) {
            auto depend_path = path_ / kind;

            if (std::filesystem::exists(depend_path)) {
                const auto input = unwrap(ok_or(try_readfile(depend_path), Error{IoError{}}),
                                          std::format("failed to load {}", kind));

                auto depend = unwrap(try_parse(input, pms_utils::parsers::depend::nodes, true)
                                         .transform_error([&input](const auto err) {
                                             return Error{ParseError{std::string(input.cbegin(), err)}};
                                         }),
                                     std::format("failed to parse {}", kind));

                if (kind == "DEPEND") {
                    depend_ = std::move(depend);
                } else if (kind == "BDEPEND") {
                    bdepend_ = std::move(depend);
                } else if (kind == "RDEPEND") {
                    rdepend_ = std::move(depend);
                }
            }
        }
    }

    {
        const auto keywords_path = path_ / "KEYWORDS";

        if (std::filesystem::exists(keywords_path)) {
            keywords_ = value_or_else(
                unwrap(ok_or(try_readfile(keywords_path), Error{IoError{}}).and_then([](const auto &input) {
                    return try_parse(input, pms_utils::parsers::vdb::_internal::VDB_KEYWORDS, true)
                        .transform_error([&input](const auto err) {
                            return Error{ParseError{std::string(input.cbegin(), err)}};
                        });
                }),
                       "failed to read KEYWORDS"),
                []() { return pms_utils::ebuild::keywords{}; });
        }
    }

    {
        const auto iuse_path = path_ / "IUSE";

        if (std::filesystem::exists(iuse_path)) {
            iuse_ = value_or_else(
                unwrap(ok_or(try_readfile(iuse_path), Error{IoError{}}).and_then([](const auto &input) {
                    return try_parse(input, pms_utils::parsers::vdb::_internal::VDB_IUSE, true)
                        .transform_error([&input](const auto err) {
                            return Error{ParseError{std::string(input.cbegin(), err)}};
                        });
                }),
                       "failed to read KEYWORDS"),
                []() { return pms_utils::ebuild::iuse{}; });
        }

        {
            const auto use_path = path_ / "USE";

            if (std::filesystem::exists(use_path)) {
                use_ = value_or_else(
                    unwrap(ok_or(try_readfile(use_path), Error{IoError{}}).and_then([](const auto &input) {
                        return try_parse(input, pms_utils::parsers::vdb::_internal::use_flags, true)
                            .transform_error([&input](const auto err) {
                                return Error{ParseError{std::string(input.cbegin(), err)}};
                            });
                    }),
                           "failed to read KEYWORDS"),
                    []() { return std::vector<pms_utils::atom::Useflag>{}; });
            }
        }

        {
            const auto contents_path = path_ / "CONTENTS";

            contents_ = value_or_else(
                unwrap(ok_or(try_readfile(contents_path), Error{IoError{}}).and_then([](const auto &input) {
                    return try_parse(input, pms_utils::parsers::vdb::contents, true)
                        .transform_error([&input](const auto err) {
                            return Error{ParseError{std::string(input.cbegin(), err)}};
                        });
                }),
                       "failed to read KEYWORDS"),
                []() { return std::vector<pms_utils::vdb::Content>{}; });
        }

        repository_ =
            unwrap(ok_or(try_readfile(path_ / "repository"), Error{IoError{}}), "failed to load repository");

        size_ = unwrap(ok_or(try_readfile(path_ / "SIZE"), Error{IoError{}}).and_then([](const auto &input) {
            return try_parse(input, pms_utils::parsers::vdb::_internal::size, true)
                .transform_error(
                    [&input](const auto err) { return Error{ParseError{std::string(input.cbegin(), err)}}; });
        }),
                       "failed to load SIZE");
    }
}

Vdb::Vdb(std::filesystem::path path) : path_(std::move(path)) {};

Vdb::Iterator Vdb::begin() const {
    auto iter = std::filesystem::directory_iterator(path_);
    Category category{*(iter)};
    return Iterator{iter, category};
}

Vdb::Iterator Vdb::cbegin() const { return begin(); };

Vdb::Iterator Vdb::end() const {
    auto end = std::filesystem::end(std::filesystem::directory_iterator(path_));
    return Iterator{end, std::nullopt};
}

Vdb::Iterator Vdb::cend() const { return end(); }

Category::Category(std::filesystem::path path) : path_(std::move(path)) {
    auto stem = path_.stem().string();

    category_ = unwrap(
        try_parse(stem, pms_utils::parsers::atom::category, true).transform_error([&stem](const auto err) {
            return Error{ParseError{std::string(stem.cbegin(), err)}};
        }),
        std::format("invalid category: {}", stem));
};

Category::Iterator Category::begin() const {
    auto iter = std::filesystem::directory_iterator{path_};
    Entry entry{*(iter)};
    return Iterator{iter, entry};
}

Category::Iterator Category::cbegin() const { return begin(); }

Category::Iterator Category::end() const {
    auto end = std::filesystem::end(std::filesystem::directory_iterator(path_));
    return Iterator{end, std::nullopt};
}

Category::Iterator Category::cend() const { return end(); }

Vdb::Iterator::Iterator(std::filesystem::directory_iterator iter, std::optional<Category> category)
    : it(std::move(iter)), category(std::move(category)) {};

bool Vdb::Iterator::operator==(const Vdb::Iterator &other) const { return this->it == other.it; }

Vdb::Iterator::reference Vdb::Iterator::operator*() const { return category.value(); }

Vdb::Iterator::pointer Vdb::Iterator::operator->() const { return &category.value(); }

Vdb::Iterator &Vdb::Iterator::operator++() {
    if (it != std::filesystem::end(it)) {
        it++;
    }

    if (it != std::filesystem::end(it)) {
        category = Category{(*it).path()};
    }

    return *this;
}

Vdb::Iterator &Vdb::Iterator::operator++(int amt) {
    std::ranges::advance(*this, amt);
    return *this;
}

Category::Iterator::Iterator(std::filesystem::directory_iterator iter, std::optional<Entry> entry)
    : it(std::move(iter)), entry(std::move(entry)) {}

bool Category::Iterator::operator==(const Category::Iterator &other) const { return it == other.it; }

Category::Iterator::reference Category::Iterator::operator*() const { return entry.value(); }

Category::Iterator::pointer Category::Iterator::operator->() const { return &entry.value(); }

Category::Iterator &Category::Iterator::operator++() {
    if (it != std::filesystem::end(it)) {
        it++;
    }

    if (it != std::filesystem::end(it)) {
        entry = Entry{(*it).path()};
    }

    return *this;
}

Category::Iterator &Category::Iterator::operator++(int amt) {
    std::ranges::advance(*this, amt);
    return *this;
}

} // namespace vdb
} // namespace pms_utils
