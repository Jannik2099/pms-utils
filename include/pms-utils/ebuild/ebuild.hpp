#pragma once

#include "pms-utils/atom/atom.hpp"
#include "pms-utils/depend/depend.hpp"
#include "pms-utils/misc/meta.hpp"

#include <boost/describe/class.hpp>
#include <boost/describe/enum.hpp>
#include <boost/describe/enumerators.hpp>
#include <boost/describe/members.hpp>
#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/list.hpp>
#include <boost/mp11/set.hpp>
#include <boost/optional.hpp>
#include <filesystem>
#include <ostream>
#include <string>
#include <vector>

namespace [[gnu::visibility("default")]] pms_utils {
namespace ebuild {

struct URI : public std::string {};

struct uri_elem {
    boost::variant<URI, std::filesystem::path> uri;
    boost::optional<std::filesystem::path> filename;

    [[nodiscard]] explicit operator std::string() const;
};

struct SRC_URI : public depend::GroupExpr<uri_elem, SRC_URI> {
    using Base = depend::GroupExpr<uri_elem, SRC_URI>;
};

struct restrict_elem {
    std::string string;
    enum class Type { UNKNOWN, mirror, fetch, strip, userpriv, test };
    Type type = restrict_elem::Type::UNKNOWN;

    [[nodiscard]] explicit operator std::string() const;
};
struct RESTRICT : public depend::GroupExpr<restrict_elem, RESTRICT> {
    using Base = depend::GroupExpr<restrict_elem, RESTRICT>;
};

struct HOMEPAGE : public depend::GroupExpr<URI, HOMEPAGE> {
    using Base = depend::GroupExpr<URI, HOMEPAGE>;
};

struct license_elem : public std::string {};
struct LICENSE : public depend::GroupExpr<license_elem, LICENSE> {
    using Base = depend::GroupExpr<license_elem, LICENSE>;
};

struct keyword : public std::string {};
struct KEYWORDS : public std::vector<keyword> {
    [[nodiscard]] explicit operator std::string() const;
};

struct inherited_elem : public std::string {};
struct INHERITED : public std::vector<inherited_elem> {
    [[nodiscard]] explicit operator std::string() const;
};

struct iuse_elem {
    bool default_enabled = false;
    atom::Useflag useflag;

    [[nodiscard]] explicit operator std::string() const;
};
struct IUSE : public std::vector<iuse_elem> {
    [[nodiscard]] explicit operator std::string() const;
};

struct REQUIRED_USE : public depend::GroupExpr<atom::Usedep, REQUIRED_USE> {
    using Base = depend::GroupExpr<atom::Usedep, REQUIRED_USE>;
};

struct EAPI : public std::string {};

struct properties_elem {
    std::string string;
    enum class Type { UNKNOWN, interactive, live, test_network };
    Type type = properties_elem::Type::UNKNOWN;

    [[nodiscard]] explicit operator std::string() const;
};
struct PROPERTIES : public depend::GroupExpr<properties_elem, PROPERTIES> {
    using Base = depend::GroupExpr<properties_elem, PROPERTIES>;
};

BOOST_DEFINE_ENUM_CLASS(phases, pretend, setup, unpack, prepare, configure, compile, test, install, preinst,
                        postinst, prerm, postrm, config, info, nofetch);
struct DEFINED_PHASES : public std::vector<phases> {
    [[nodiscard]] explicit operator std::string() const;
};

struct Metadata {
    depend::DependExpr DEPEND;
    depend::DependExpr RDEPEND;
    atom::Slot SLOT;
    SRC_URI SRC_URI;
    RESTRICT RESTRICT;
    HOMEPAGE HOMEPAGE;
    LICENSE LICENSE;
    std::string DESCRIPTION;
    KEYWORDS KEYWORDS;
    INHERITED INHERITED;
    IUSE IUSE;
    REQUIRED_USE REQUIRED_USE;
    depend::DependExpr PDEPEND;
    depend::DependExpr BDEPEND;
    EAPI EAPI;
    PROPERTIES PROPERTIES;
    DEFINED_PHASES DEFINED_PHASES;
    depend::DependExpr IDEPEND;
};

// BEGIN DESCRIBE

BOOST_DESCRIBE_STRUCT(URI, (std::string), ());

BOOST_DESCRIBE_STRUCT(uri_elem, (), (uri, filename));
BOOST_DESCRIBE_STRUCT(SRC_URI, (SRC_URI::Base), ());

BOOST_DESCRIBE_ENUM(restrict_elem::Type, UNKNOWN, mirror, fetch, strip, userpriv, test);
BOOST_DESCRIBE_STRUCT(restrict_elem, (), (string, type));
BOOST_DESCRIBE_STRUCT(RESTRICT, (RESTRICT::Base), ());

BOOST_DESCRIBE_STRUCT(HOMEPAGE, (HOMEPAGE::Base), ());

BOOST_DESCRIBE_STRUCT(license_elem, (std::string), ());
BOOST_DESCRIBE_STRUCT(LICENSE, (LICENSE::Base), ());

BOOST_DESCRIBE_STRUCT(keyword, (std::string), ());
BOOST_DESCRIBE_STRUCT(KEYWORDS, (std::vector<keyword>), ());

BOOST_DESCRIBE_STRUCT(inherited_elem, (std::string), ());
BOOST_DESCRIBE_STRUCT(INHERITED, (std::vector<inherited_elem>), ());

BOOST_DESCRIBE_STRUCT(iuse_elem, (), (default_enabled, useflag));
BOOST_DESCRIBE_STRUCT(IUSE, (std::vector<iuse_elem>), ());

BOOST_DESCRIBE_STRUCT(REQUIRED_USE, (REQUIRED_USE::Base), ());

BOOST_DESCRIBE_STRUCT(EAPI, (std::string), ());

BOOST_DESCRIBE_ENUM(properties_elem::Type, UNKNOWN, interactive, live, test_network);
BOOST_DESCRIBE_STRUCT(properties_elem, (), (string, type));
BOOST_DESCRIBE_STRUCT(PROPERTIES, (PROPERTIES::Base), ());

BOOST_DESCRIBE_STRUCT(DEFINED_PHASES, (std::vector<phases>), ());

BOOST_DESCRIBE_STRUCT(Metadata, (),
                      (DEPEND, RDEPEND, SLOT, SRC_URI, RESTRICT, HOMEPAGE, LICENSE, DESCRIPTION, KEYWORDS,
                       INHERITED, IUSE, REQUIRED_USE, PDEPEND, BDEPEND, EAPI, PROPERTIES, DEFINED_PHASES,
                       IDEPEND));

namespace meta {

using all = boost::mp11::mp_list<URI, uri_elem, SRC_URI, restrict_elem::Type, restrict_elem, RESTRICT,
                                 HOMEPAGE, license_elem, LICENSE, keyword, KEYWORDS, inherited_elem,
                                 INHERITED, iuse_elem, IUSE, REQUIRED_USE, EAPI, properties_elem::Type,
                                 properties_elem, PROPERTIES, DEFINED_PHASES, Metadata>;
static_assert(boost::mp11::mp_is_set<all>{});
static_assert(boost::mp11::mp_all_of<all, pms_utils::meta::is_described>{});

} // namespace meta

// END DESCRIBE

// BEGIN IO

std::ostream &operator<<(std::ostream &out, const uri_elem &uri_elem);

std::ostream &operator<<(std::ostream &out, const restrict_elem &restrict_elem);

[[nodiscard]] std::string to_string(restrict_elem::Type type);
std::ostream &operator<<(std::ostream &out, restrict_elem::Type type);

std::ostream &operator<<(std::ostream &out, const KEYWORDS &keywords);

std::ostream &operator<<(std::ostream &out, const INHERITED &inherited);

std::ostream &operator<<(std::ostream &out, const iuse_elem &iuse_elem);

std::ostream &operator<<(std::ostream &out, const IUSE &iuse);

std::ostream &operator<<(std::ostream &out, const properties_elem &properties_elem);

[[nodiscard]] std::string to_string(properties_elem::Type type);
std::ostream &operator<<(std::ostream &out, properties_elem::Type type);

[[nodiscard]] std::string to_string(phases _phases);
std::ostream &operator<<(std::ostream &out, phases _phases);

std::ostream &operator<<(std::ostream &out, const DEFINED_PHASES &defined_phases);

// END IO

} // namespace ebuild
} // namespace pms_utils
