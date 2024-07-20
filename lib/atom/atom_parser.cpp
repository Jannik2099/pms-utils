#include "pms-utils/atom/atom_parser.hpp"

#include "pms-utils/atom/atom.hpp"

#include <boost/fusion/container/deque/deque.hpp>
#include <boost/optional/optional.hpp>
#include <boost/spirit/home/x3.hpp> // IWYU pragma: keep
#include <boost/spirit/home/x3/char/char.hpp>
#include <boost/spirit/home/x3/char/char_class.hpp>
#include <boost/spirit/home/x3/directive/matches.hpp>
#include <boost/spirit/home/x3/string/literal_string.hpp>
#include <boost/spirit/home/x3/string/symbols.hpp>
#include <boost/variant/get.hpp>
#include <boost/variant/variant.hpp>
#include <stdexcept>

// comment to prevent reordering
#include "../x3_utils.hpp"

namespace [[gnu::visibility("default")]] pms_utils {
namespace parsers::atom {

namespace x3 = boost::spirit::x3;

namespace {

struct VerSpec final : x3::symbols<pms_utils::atom::VersionSpecifier> {
    // This intentionally lacks VersionSpecifier::ea because we cannot match against it directly
    // clang-format off
    VerSpec() {
        using enum pms_utils::atom::VersionSpecifier;
        add
            ("<", lt)
            ("<=", le)
            ("=", eq)
            ("~", td)
            (">=", ge)
            (">", gt)
            ;
    }
    // clang-format on
};

struct Blocker final : x3::symbols<pms_utils::atom::Blocker> {
    // clang-format off
    Blocker() {
        add
            ("!", pms_utils::atom::Blocker::weak)
            ("!!", pms_utils::atom::Blocker::strong)
            ;
    }
    // clang-format on
};

struct VersionSuffixWord final : x3::symbols<pms_utils::atom::VersionSuffixWord> {
    // clang-format off
    VersionSuffixWord() {
        using enum pms_utils::atom::VersionSuffixWord;
        add
            ("_alpha", alpha)
            ("_beta", beta)
            ("_pre", pre)
            ("_rc", rc)
            ("_p", p)
            ;
    }
    // clang-format on
};

struct UsedepNegate final : x3::symbols<pms_utils::atom::UsedepNegate> {
    // clang-format off
    UsedepNegate() {
        add
            ("-", pms_utils::atom::UsedepNegate::minus)
            ("!", pms_utils::atom::UsedepNegate::exclamation)
            ;
    }
    // clang-format on
};
struct UsedepSign final : x3::symbols<pms_utils::atom::UsedepSign> {
    // clang-format off
    UsedepSign() {
        add
            ("+", pms_utils::atom::UsedepSign::plus)
            ("-", pms_utils::atom::UsedepSign::minus)
            ;
    }
    // clang-format on
};

struct UsedepConditional final : x3::symbols<pms_utils::atom::UsedepCond> {
    // clang-format off
    UsedepConditional() {
        add
            ("=", pms_utils::atom::UsedepCond::eqal)
            ("?", pms_utils::atom::UsedepCond::question)
            ;
    }
    // clang-format on
};

constexpr inline auto slot_expr_helper = [](auto &ctx) {
    boost::variant<char, boost::fusion::deque<boost::optional<pms_utils::atom::Slot>, bool>> &attr =
        x3::_attr(ctx);
    pms_utils::atom::SlotExpr &val = x3::_val(ctx);
    using boost::fusion::at_c;

    switch (attr.which()) {
    case 0:
        val.slotVariant = pms_utils::atom::SlotVariant::star;
        break;
    case 1: {
        auto &var2 = boost::get<boost::fusion::deque<boost::optional<pms_utils::atom::Slot>, bool>>(attr);
        const boost::optional<pms_utils::atom::Slot> &slot = at_c<0>(var2);
        const bool equal = at_c<1>(var2);

        val.slot = slot;
        val.slotVariant = equal ? pms_utils::atom::SlotVariant::equal : pms_utils::atom::SlotVariant::none;

        if (!equal && !slot.has_value()) {
            // TODO
            throw std::runtime_error("TODO");
        }
        break;
    }

    default:
        // TODO
        throw std::runtime_error("TODO");
        break;
    }
};

constexpr inline auto package_dep_helper = [](auto &ctx, bool requireVerSpec) {
    // Parser semantic actions are a pathway to many abilities some consider to be unnatural
    boost::fusion::deque<
        boost::optional<pms_utils::atom::Blocker>, boost::optional<pms_utils::atom::VersionSpecifier>,
        pms_utils::atom::Category, pms_utils::atom::Name,
        boost::optional<boost::fusion::deque<pms_utils::atom::Version, bool>>,
        boost::optional<pms_utils::atom::SlotExpr>, boost::optional<pms_utils::atom::Usedeps>> &attr =
        x3::_attr(ctx);
    pms_utils::atom::PackageExpr &val = x3::_val(ctx);
    using boost::fusion::at_c;

    val.blocker = at_c<0>(attr);
    boost::optional<pms_utils::atom::VersionSpecifier> &version_specifier = at_c<1>(attr);
    val.category = at_c<2>(attr);
    val.name = at_c<3>(attr);
    boost::optional<boost::fusion::deque<pms_utils::atom::Version, bool>> &versionPart = at_c<4>(attr);
    val.slotExpr = at_c<5>(attr);
    val.usedeps = at_c<6>(attr).value_or(pms_utils::atom::Usedeps{});

    if (versionPart.has_value()) {
        const pms_utils::atom::Version &version = at_c<0>(versionPart.value());
        const bool asterisk = at_c<1>(versionPart.value());

        if (!version_specifier.has_value() && requireVerSpec) {
            // TODO
            x3::_pass(ctx) = false;
        }
        if (asterisk) {
            if (version_specifier == pms_utils::atom::VersionSpecifier::eq) {
                version_specifier = pms_utils::atom::VersionSpecifier::ea;
            } else {
                // TODO
                x3::_pass(ctx) = false;
            }
        }
        val.version = version;
        val.verspec = version_specifier;
    }
};

} // namespace

PARSER_DEFINE(version_specifier, VerSpec());
PARSER_DEFINE(blocker, Blocker());
PARSER_DEFINE(slot_no_subslot,
              (x3::ascii::alnum | x3::char_("_")) >>
                  *(x3::ascii::alnum | x3::char_("_") | x3::char_("-") | x3::char_("+") | x3::char_(".")));
PARSER_DEFINE(slot, slot_no_subslot() >> -(x3::lit("/") >> slot_no_subslot()));
PARSER_DEFINE(slot_expr, x3::lit(":") >> (x3::char_("*") | (-slot() >> x3::matches["="]))[slot_expr_helper]);

PARSER_DEFINE(ver_num, +x3::digit % '.');
PARSER_DEFINE(ver_letter, x3::ascii::lower);
PARSER_DEFINE(ver_suffix_word, VersionSuffixWord());
PARSER_DEFINE(ver_suffix, ver_suffix_word() >> *x3::digit);
PARSER_DEFINE(ver_rev, x3::lit("-r") >> +x3::digit);
PARSER_DEFINE(package_version, ver_num() >> -ver_letter() >> *ver_suffix() >> -ver_rev());

PARSER_DEFINE(category,
              (x3::ascii::alnum | x3::char_("_")) >>
                  *(x3::ascii::alnum | x3::char_("_") | x3::char_("-") | x3::char_("+") | x3::char_(".")));
// this basically means "name not followed by ( -version which itself is followed by legal atom name charset )
// Otherwise e.g. name(foo-1-1) would match fully
PARSER_DEFINE(name, (x3::ascii::alnum | x3::char_("_")) >>
                        *(x3::ascii::alnum | x3::char_("_") | x3::char_("+") |
                          (x3::char_("-") -
                           (x3::lit("-") >> package_version() >>
                            !(x3::ascii::alnum | x3::char_("_") | x3::char_("-") | x3::char_("+"))))));

PARSER_DEFINE(useflag, x3::ascii::alnum >> *(x3::ascii::alnum | x3::char_("_") | x3::char_("-") |
                                             x3::char_("+") | x3::char_("@")));
// TODO: reinstate "UsedepCond needs !, not -"
PARSER_DEFINE(use_dep, -UsedepNegate() >> useflag() >> -(x3::lit("(") >> UsedepSign() >> x3::lit(")")) >>
                           -UsedepConditional())
PARSER_DEFINE(use_deps, x3::lit("[") >> use_dep() % "," >> x3::lit("]"));

// unsure about how to handle the "duplicate rules for requireVerSpec"
namespace {
const auto atom_helper = [](bool requireVerSpec) {
    return (-blocker() >> -version_specifier() >> category() >> x3::lit("/") >> name() >>
            -(x3::lit("-") >> package_version() >> x3::matches["*"]) >> -slot_expr() >>
            -use_deps())[([requireVerSpec](auto &ctx) { package_dep_helper(ctx, requireVerSpec); })];
};
} // namespace

// this way it's still two types, can I pass an arg to the x3 rule perhaps?
PARSER_DEFINE(package_dep, atom_helper(true));
PARSER_DEFINE(atom, atom_helper(false));
} // namespace parsers::atom
} // namespace pms_utils
