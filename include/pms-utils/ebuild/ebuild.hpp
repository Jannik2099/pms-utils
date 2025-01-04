#pragma once

#include "pms-utils/atom/atom.hpp"
#include "pms-utils/depend/depend.hpp"
#include "pms-utils/misc/meta.hpp"

#include <boost/describe/class.hpp>
#include <boost/describe/enum.hpp>
#include <boost/mp11/list.hpp> // IWYU pragma: keep
#include <cstdint>
#include <filesystem>
#include <iosfwd>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace [[gnu::visibility("default")]] pms_utils {
namespace ebuild {

struct URI : public std::string {};

struct uri_elem {
private:
    std::ostream &ostream_impl(std::ostream &out) const;

public:
    std::variant<URI, std::filesystem::path> uri;
    std::optional<std::filesystem::path> filename;

    [[nodiscard]] explicit operator std::string() const;
    friend std::ostream &operator<<(std::ostream &out, const uri_elem &uri_elem) {
        return uri_elem.ostream_impl(out);
    }
};

struct src_uri : public depend::GroupExpr<uri_elem, src_uri> {
    using Base = depend::GroupExpr<uri_elem, src_uri>;
};

struct restrict_elem {
private:
    std::ostream &ostream_impl(std::ostream &out) const;

public:
    std::string string;
    enum class Type : std::uint8_t { UNKNOWN, mirror, fetch, strip, userpriv, test };
    Type type = restrict_elem::Type::UNKNOWN;

    [[nodiscard]] explicit operator std::string() const;
    friend std::ostream &operator<<(std::ostream &out, const restrict_elem &restrict_elem) {
        return restrict_elem.ostream_impl(out);
    }
};
struct restrict : public depend::GroupExpr<restrict_elem, restrict> {
    using Base = depend::GroupExpr<restrict_elem, restrict>;
};

struct homepage : public depend::GroupExpr<URI, homepage> {
    using Base = depend::GroupExpr<URI, homepage>;
};

struct license_elem : public std::string {};
struct license : public depend::GroupExpr<license_elem, license> {
    using Base = depend::GroupExpr<license_elem, license>;
};

struct keyword : public std::string {};
struct keywords : public std::vector<keyword> {
private:
    std::ostream &ostream_impl(std::ostream &out) const;

public:
    [[nodiscard]] explicit operator std::string() const;
    friend std::ostream &operator<<(std::ostream &out, const keywords &keywords) {
        return keywords.ostream_impl(out);
    }
};

struct inherited_elem : public std::string {};
struct inherited : public std::vector<inherited_elem> {
private:
    std::ostream &ostream_impl(std::ostream &out) const;

public:
    [[nodiscard]] explicit operator std::string() const;
    friend std::ostream &operator<<(std::ostream &out, const inherited &inherited) {
        return inherited.ostream_impl(out);
    }
};

struct iuse_elem {
private:
    std::ostream &ostream_impl(std::ostream &out) const;

public:
    bool default_enabled = false;
    atom::Useflag useflag;

    [[nodiscard]] explicit operator std::string() const;
    friend std::ostream &operator<<(std::ostream &out, const iuse_elem &iuse_elem) {
        return iuse_elem.ostream_impl(out);
    }
};
struct iuse : public std::vector<iuse_elem> {
private:
    std::ostream &ostream_impl(std::ostream &out) const;

public:
    [[nodiscard]] explicit operator std::string() const;
    friend std::ostream &operator<<(std::ostream &out, const iuse &iuse) { return iuse.ostream_impl(out); }
};

struct required_use : public depend::GroupExpr<atom::Usedep, required_use> {
    using Base = depend::GroupExpr<atom::Usedep, required_use>;
};

struct eapi : public std::string {};

struct properties_elem {
private:
    std::ostream &ostream_impl(std::ostream &out) const;

public:
    std::string string;
    enum class Type : std::uint8_t { UNKNOWN, interactive, live, test_network };
    Type type = properties_elem::Type::UNKNOWN;

    [[nodiscard]] explicit operator std::string() const;
    friend std::ostream &operator<<(std::ostream &out, const properties_elem &properties_elem) {
        return properties_elem.ostream_impl(out);
    }
};
struct properties : public depend::GroupExpr<properties_elem, properties> {
    using Base = depend::GroupExpr<properties_elem, properties>;
};

BOOST_DEFINE_FIXED_ENUM_CLASS(phases, std::uint8_t, pretend, setup, unpack, prepare, configure, compile, test,
                              install, preinst, postinst, prerm, postrm, config, info, nofetch);
struct defined_phases : public std::vector<phases> {
private:
    std::ostream &ostream_impl(std::ostream &out) const;

public:
    [[nodiscard]] explicit operator std::string() const;
    friend std::ostream &operator<<(std::ostream &out, const defined_phases &defined_phases) {
        return defined_phases.ostream_impl(out);
    }
};

struct Metadata {
    depend::DependExpr DEPEND;
    depend::DependExpr RDEPEND;
    atom::Slot SLOT;
    src_uri SRC_URI;
    restrict RESTRICT;
    homepage HOMEPAGE;
    license LICENSE;
    std::string DESCRIPTION;
    keywords KEYWORDS;
    inherited INHERITED;
    iuse IUSE;
    required_use REQUIRED_USE;
    depend::DependExpr PDEPEND;
    depend::DependExpr BDEPEND;
    eapi EAPI;
    properties PROPERTIES;
    defined_phases DEFINED_PHASES;
    depend::DependExpr IDEPEND;
};

// BEGIN DESCRIBE

BOOST_DESCRIBE_STRUCT(URI, (), ());

BOOST_DESCRIBE_STRUCT(uri_elem, (), (uri, filename));
BOOST_DESCRIBE_STRUCT(src_uri, (src_uri::Base), ());

BOOST_DESCRIBE_ENUM(restrict_elem::Type, UNKNOWN, mirror, fetch, strip, userpriv, test);
BOOST_DESCRIBE_STRUCT(restrict_elem, (), (string, type));
BOOST_DESCRIBE_STRUCT(restrict, (restrict ::Base), ());

BOOST_DESCRIBE_STRUCT(homepage, (homepage::Base), ());

BOOST_DESCRIBE_STRUCT(license_elem, (), ());
BOOST_DESCRIBE_STRUCT(license, (license::Base), ());

BOOST_DESCRIBE_STRUCT(keyword, (), ());
BOOST_DESCRIBE_STRUCT(keywords, (), ());

BOOST_DESCRIBE_STRUCT(inherited_elem, (), ());
BOOST_DESCRIBE_STRUCT(inherited, (), ());

BOOST_DESCRIBE_STRUCT(iuse_elem, (), (default_enabled, useflag));
BOOST_DESCRIBE_STRUCT(iuse, (), ());

BOOST_DESCRIBE_STRUCT(required_use, (required_use::Base), ());

BOOST_DESCRIBE_STRUCT(eapi, (), ());

BOOST_DESCRIBE_ENUM(properties_elem::Type, UNKNOWN, interactive, live, test_network);
BOOST_DESCRIBE_STRUCT(properties_elem, (), (string, type));
BOOST_DESCRIBE_STRUCT(properties, (properties::Base), ());

BOOST_DESCRIBE_STRUCT(defined_phases, (), ());

BOOST_DESCRIBE_STRUCT(Metadata, (),
                      (DEPEND, RDEPEND, SLOT, SRC_URI, RESTRICT, HOMEPAGE, LICENSE, DESCRIPTION, KEYWORDS,
                       INHERITED, IUSE, REQUIRED_USE, PDEPEND, BDEPEND, EAPI, PROPERTIES, DEFINED_PHASES,
                       IDEPEND));

namespace meta {

using all = boost::mp11::mp_list<URI, uri_elem, src_uri, restrict_elem::Type, restrict_elem, restrict,
                                 homepage, license_elem, license, keyword, keywords, inherited_elem,
                                 inherited, iuse_elem, iuse, required_use, eapi, properties_elem::Type,
                                 properties_elem, properties, defined_phases, Metadata>;

} // namespace meta

// END DESCRIBE

// BEGIN IO

[[nodiscard]] std::string to_string(restrict_elem::Type type);
std::ostream &operator<<(std::ostream &out, restrict_elem::Type type);

[[nodiscard]] std::string to_string(properties_elem::Type type);
std::ostream &operator<<(std::ostream &out, properties_elem::Type type);

[[nodiscard]] std::string to_string(phases _phases);
std::ostream &operator<<(std::ostream &out, phases _phases);

// END IO

} // namespace ebuild
} // namespace pms_utils

PMS_UTILS_FOOTER(ebuild);
