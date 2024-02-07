#include "pms-utils/atom/atom_parser.hpp"

#include "pms-utils/atom/atom.hpp"
#include "pms-utils/misc/x3_utils.hpp"

#include <boost/spirit/home/x3.hpp>
#include <stdexcept>

// comment to prevent reordering
#include "../x3_utils.hpp"

namespace [[gnu::visibility("default")]] pms_utils {
namespace parsers {

namespace {

struct VerSpec final : x3::symbols<atom::VersionSpecifier> {
    // This intentionally lacks VersionSpecifier::ea because we cannot match against it directly
    // clang-format off
    VerSpec() {
        using enum atom::VersionSpecifier;
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

struct Blocker final : x3::symbols<atom::Blocker> {
    // clang-format off
    Blocker() {
        add
            ("!", atom::Blocker::weak)
            ("!!", atom::Blocker::strong)
            ;
    }
    // clang-format on
};

struct VersionSuffixWord final : x3::symbols<atom::VersionSuffixWord> {
    // clang-format off
    VersionSuffixWord() {
        using enum atom::VersionSuffixWord;
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

struct UsedepSign final : x3::symbols<atom::UsedepSign> {
    // clang-format off
    UsedepSign() {
        add
            ("+", atom::UsedepSign::plus)
            ("-", atom::UsedepSign::minus)
            ;
    }
    // clang-format on
};

struct UsedepConditional final : x3::symbols<atom::UsedepCond> {
    // clang-format off
    UsedepConditional() {
        add
            ("=", atom::UsedepCond::eqal)
            ("?", atom::UsedepCond::question)
            ;
    }
    // clang-format on
};

constexpr inline auto slot_expr_helper = [](auto &ctx) {
    boost::variant<char, boost::fusion::deque<boost::optional<atom::Slot>, bool>> &attr = x3::_attr(ctx);
    atom::SlotExpr &val = x3::_val(ctx);
    using boost::fusion::at_c;

    switch (attr.which()) {
    case 0:
        val.slotVariant = atom::SlotVariant::star;
        break;
    case 1: {
        auto &var2 = boost::get<boost::fusion::deque<boost::optional<atom::Slot>, bool>>(attr);
        const boost::optional<atom::Slot> &slot = at_c<0>(var2);
        bool equal = at_c<1>(var2);

        val.slot = slot;
        val.slotVariant = equal ? atom::SlotVariant::equal : atom::SlotVariant::none;

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
    boost::fusion::deque<boost::optional<atom::Blocker>, boost::optional<atom::VersionSpecifier>,
                         atom::Category, atom::Name,
                         boost::optional<boost::fusion::deque<atom::Version, bool>>,
                         boost::optional<atom::SlotExpr>, boost::optional<std::vector<atom::Usedep>>> &attr =
        x3::_attr(ctx);
    atom::PackageExpr &val = x3::_val(ctx);
    using boost::fusion::at_c;

    val.blocker = at_c<0>(attr);
    boost::optional<atom::VersionSpecifier> &version_specifier = at_c<1>(attr);
    val.category = at_c<2>(attr);
    val.name = at_c<3>(attr);
    boost::optional<boost::fusion::deque<atom::Version, bool>> &versionPart = at_c<4>(attr);
    val.slotExpr = at_c<5>(attr);
    val.usedeps = at_c<6>(attr).value_or(std::vector<atom::Usedep>{});

    if (versionPart.has_value()) {
        const atom::Version &version = at_c<0>(versionPart.value());
        bool asterisk = at_c<1>(versionPart.value());

        if (!version_specifier.has_value() && requireVerSpec) {
            // TODO
            x3::_pass(ctx) = false;
        }
        if (asterisk) {
            if (version_specifier == atom::VersionSpecifier::eq) {
                version_specifier = atom::VersionSpecifier::ea;
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
// this basically means "name not followed by a -version which itself is followed by what comes after version
// in an Atom, or end of input". Otherwise e.g. name(foo-1-1) would match fully
PARSER_DEFINE(name, (x3::ascii::alnum | x3::char_("_")) >>
                        *((x3::ascii::alnum | x3::char_("_") | x3::char_("-") | x3::char_("+")) -
                          (x3::lit("-") >> package_version() >>
                           !(x3::ascii::alnum | x3::char_("_") | x3::char_("-") | x3::char_("+")))));

PARSER_DEFINE(useflag, x3::ascii::alnum >> *(x3::ascii::alnum | x3::char_("_") | x3::char_("-") |
                                             x3::char_("+") | x3::char_("@")));
// this looks a bit redundant, but this way we can enforce "UsedepCond needs !, not -" without further
// semantic action
PARSER_DEFINE(use_dep,
              (x3::matches["!"] >> useflag() >> -(x3::lit("(") >> UsedepSign() >> x3::lit(")")) >>
               -UsedepConditional()) |
                  (x3::matches["-"] >> useflag() >> -(x3::lit("(") >> UsedepSign() >> x3::lit(")")) >>
                   x3::attr(boost::optional<atom::UsedepCond>{})));
PARSER_DEFINE(use_deps, x3::lit("[") >> use_dep() % "," >> x3::lit("]"));

// unsure about how to handle the "duplicate rules for requireVerSpec"
namespace {
const auto atom_helper = [](bool requireVerSpec) {
    return (-blocker() >> -version_specifier() >> category() >> x3::lit("/") >> name() >>
            -(x3::lit("-") >> package_version() >> x3::matches["*"]) >> -slot_expr() >>
            -use_deps())[([requireVerSpec](auto &ctx) { package_dep_helper(ctx, requireVerSpec); })];
};
}

// this way it's still two types, can I pass an arg to the x3 rule perhaps?
PARSER_DEFINE(package_dep, atom_helper(true));
PARSER_DEFINE(atom, atom_helper(false));
} // namespace parsers
} // namespace pms_utils
