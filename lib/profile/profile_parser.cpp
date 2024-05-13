#include "pms-utils/profile/profile_parser.hpp"

#include "pms-utils/atom/atom.hpp"
#include "pms-utils/atom/atom_parser.hpp"
#include "pms-utils/misc/x3_utils.hpp"
#include "pms-utils/profile/profile.hpp"

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/auxiliary/eoi.hpp>
#include <boost/spirit/home/x3/char/any_char.hpp>
#include <boost/spirit/home/x3/char/char_class.hpp>
#include <boost/spirit/home/x3/core/action.hpp>
#include <boost/spirit/home/x3/core/call.hpp>
#include <boost/spirit/home/x3/support/unused.hpp>

// comment to prevent reordering
#include "../x3_utils.hpp"

namespace [[gnu::visibility("default")]] pms_utils {
namespace parsers::profile {

namespace x3 = boost::spirit::x3;

namespace {

template <std::size_t N, typename... Ts>
using NthTypeOf = typename std::tuple_element<N, std::tuple<Ts...>>::type;

template <int N, typename... Ts> auto &get(boost::variant<Ts...> &val) {
    using target = NthTypeOf<N, Ts...>;
    return boost::get<target>(val);
}

const auto package_use_inserter = [](auto &ctx) {
    boost::fusion::deque<boost::optional<char>, boost::variant<char, pms_utils::atom::Useflag>> &attr =
        x3::_attr(ctx);
    pms_utils::profile::_internal::unordered_str_set<pms_utils::atom::Useflag> &val = x3::_val(ctx);
    using boost::fusion::at_c;

    const bool negate = at_c<0>(attr).has_value();
    if (boost::variant<char, pms_utils::atom::Useflag> &use_elem = at_c<1>(attr); use_elem.which() == 0) {
        // if it's a wildcard, just reset the set
        val.emplace("-*");
        if (!negate) {
            val.emplace("*");
        }
    } else {
        auto &useflag = boost::get<pms_utils::atom::Useflag>(use_elem);
        if (negate) {
            val.erase(useflag);
        } else {
            val.emplace(std::move(useflag));
        }
    }
};

// why is this necessary???
const auto shlex_helper = [](auto &ctx) {
    boost::fusion::deque<std::string, bool> &attr = x3::_attr(ctx);
    std::tuple<std::string, bool> &val = x3::_val(ctx);
    using boost::fusion::at_c;

    std::get<0>(val) = std::move(at_c<0>(attr));
    std::get<1>(val) = at_c<1>(attr);
};

const auto make_defaults_use_inserter = [](auto &ctx) {
    auto &attr = x3::_attr(ctx);
    auto &val = x3::_val(ctx);
    using boost::fusion::at_c;

    const bool negate = at_c<0>(attr).has_value();
    if (auto &elem = at_c<1>(attr); elem.which() == 0) {
        if (!negate) {
            // * is not allowed in these make.defaults keys
            x3::_pass(ctx) = false;
            return;
        }
        val.clear();
        val.emplace("*");
    } else {
        auto &elem2 = get<1>(elem);
        val.emplace(std::move(elem2));
    }
};

// why is this necessary???
const auto pair_helper = [](auto &ctx) {
    boost::fusion::deque<std::string, std::string> &attr = x3::_attr(ctx);
    std::tuple<std::string, std::string> &val = x3::_val(ctx);
    using boost::fusion::at_c;
    std::get<0>(val) = std::move(at_c<0>(attr));
    std::get<1>(val) = std::move(at_c<1>(attr));
};

// once again we get to write a huge semantic action because x3 hates us
const auto atom_helper = [](auto &ctx) {
    boost::fusion::deque<
        boost::optional<pms_utils::atom::Blocker>, boost::optional<pms_utils::atom::VersionSpecifier>,
        std::string, std::string,
        boost::optional<boost::fusion::deque<boost::variant<pms_utils::atom::Version, std::string>, bool>>,
        boost::optional<pms_utils::atom::Slot>, boost::optional<std::string>> &attr = x3::_attr(ctx);
    pms_utils::profile::_internal::WildcardAtom &val = x3::_val(ctx);
    using boost::fusion::at_c;

    val.blocker = std::move(at_c<0>(attr));
    boost::optional<pms_utils::atom::VersionSpecifier> version_specifier = at_c<1>(attr);
    val.category = std::move(at_c<2>(attr));
    val.name = std::move(at_c<3>(attr));
    boost::optional<boost::fusion::deque<boost::variant<pms_utils::atom::Version, std::string>, bool>>
        &versionPart = at_c<4>(attr);
    val.slot = std::move(at_c<5>(attr));
    val.repo = std::move(at_c<6>(attr));

    if (versionPart.has_value() != version_specifier.has_value()) {
        x3::_pass(ctx) = false;
    }
    if (versionPart.has_value()) {
        val.version = std::move(at_c<0>(versionPart.value()));
        const bool asterisk = at_c<1>(versionPart.value());

        if (version_specifier.value() != pms_utils::atom::VersionSpecifier::eq &&
            val.version->type() == typeid(std::string)) {
            // wildcard versions are only allowed on =
            x3::_pass(ctx) = false;
        }

        if (asterisk) {
            if (version_specifier.value() == pms_utils::atom::VersionSpecifier::eq) {
                version_specifier = pms_utils::atom::VersionSpecifier::ea;
            } else {
                x3::_pass(ctx) = false;
            }
        }
        val.version_specifier = version_specifier.value();
    }
};

} // namespace

namespace [[gnu::visibility("hidden")]] _internal {

PARSER_RULE_T(make_defaults_key, std::string);
PARSER_DEFINE(make_defaults_key, x3::char_("a-zA-Z") >> *x3::char_("a-zA-Z0-9_"));

PARSER_RULE_T(make_defaults_value, std::string);
PARSER_DEFINE(make_defaults_value, *((x3::ascii::graph | x3::ascii::space) - x3::char_("\"")));

PARSER_RULE_T(make_defaults_value_charset, char);
PARSER_DEFINE(make_defaults_value_charset,
              x3::ascii::graph - (x3::char_("\\") | x3::char_("\"") | x3::char_("'")));

PARSER_RULE_T(make_defaults_normal_value, std::string);
PARSER_DEFINE(make_defaults_normal_value,
              +(make_defaults_value_charset() | x3::string("\\\n") |
                x3::char_("'") >> +(make_defaults_value_charset() | x3::char_("\n") | x3::char_(" ")) >>
                    x3::char_("'")));

PARSER_RULE_T(make_defaults_variable, std::string);
PARSER_DEFINE(make_defaults_variable,
              x3::lit("$") >> (x3::lit("{") >> make_defaults_key() >> x3::lit("}") | make_defaults_key()));

PARSER_RULE_T(shlex_variable, (std::tuple<std::string, bool>));
PARSER_DEFINE(shlex_variable, (make_defaults_variable() >> x3::attr(true))[shlex_helper]);

PARSER_RULE_T(shlex_value, (std::tuple<std::string, bool>));
PARSER_DEFINE(shlex_value, (make_defaults_normal_value() >> x3::attr(false))[shlex_helper]);

PARSER_RULE_T(wildcard, char);
PARSER_DEFINE(wildcard, x3::char_("*") >> !x3::char_("*"));

PARSER_RULE_T(wildcard_version, std::string);
PARSER_DEFINE(wildcard_version, x3::char_("*") >> +x3::char_("a-zA-Z0-9_.") >> x3::char_("*"));

// TODO: probably useful in some common header
PARSER_RULE_T(newline_or_comment, x3::unused_type);
PARSER_DEFINE(newline_or_comment, x3::omit[x3::ascii::space | x3::char_("#") >> *(x3::char_ - x3::eol)]);

} // namespace _internal

// -x3::lit("-") folds to Unused ?!?, so we can't just use a bool attr
PARSER_DEFINE(package_use,
              (-x3::char_("-") >> (x3::char_("*") | atom::useflag()))[package_use_inserter] % " ");

// variable before value to resolve $ ambiguity
PARSER_DEFINE(make_defaults_shlex, (_internal::shlex_variable() | _internal::shlex_value()) %
                                       +(x3::char_(" ") | x3::string("\\\n")));

PARSER_DEFINE(make_defaults_elem, (_internal::make_defaults_key() >> "=\"" >>
                                   _internal::make_defaults_value() >> "\"")[pair_helper]);
PARSER_DEFINE(make_defaults, *_internal::newline_or_comment() >>
                                 -(make_defaults_elem() % +_internal::newline_or_comment()) >>
                                 *_internal::newline_or_comment());

PARSER_DEFINE(wildcard_category, (x3::ascii::alnum | x3::char_("_") | _internal::wildcard()) >>
                                     *(x3::ascii::alnum | x3::char_("_") | x3::char_("-") | x3::char_("+") |
                                       x3::char_(".") | _internal::wildcard()));
PARSER_DEFINE(wildcard_name_ver, (x3::ascii::alnum | x3::char_("_") | _internal::wildcard()) >>
                                     *((x3::ascii::alnum | x3::char_("_") | x3::char_("-") | x3::char_("+") |
                                        _internal::wildcard())));
// this is essentially parsers::name with wildcard added to the character set
PARSER_DEFINE(wildcard_name,
              (x3::ascii::alnum | x3::char_("_") | _internal::wildcard()) >>
                  *((x3::ascii::alnum | x3::char_("_") | x3::char_("-") | x3::char_("+") |
                     _internal::wildcard()) -
                    (x3::lit("-") >> wildcard_version() >> (x3::char_(":") | x3::char_("*") | x3::eoi))));

PARSER_DEFINE(wildcard_version, atom::package_version() | _internal::wildcard_version());

PARSER_DEFINE(wildcard_atom,
              (-atom::blocker() >> -atom::version_specifier() >> wildcard_category() >> x3::lit("/") >>
               wildcard_name() >> -(x3::lit("-") >> wildcard_version() >> x3::matches["*"]) >>
               -(x3::lit(":") >> atom::slot()) >> -(x3::lit("::") >> +x3::ascii::alpha))[atom_helper]);

PARSER_DEFINE(USE, (-x3::char_("-") >> (x3::char_("*") | atom::useflag()))[make_defaults_use_inserter] % " ");
PARSER_DEFINE(USE_EXPAND,
              (-x3::char_("-") >> (x3::char_("*") | atom::useflag()))[make_defaults_use_inserter] % " ");
PARSER_DEFINE(USE_EXPAND_HIDDEN, USE_EXPAND());
PARSER_DEFINE(CONFIG_PROTECT,
              (-x3::char_("-") >> (x3::char_("*") | +x3::ascii::graph))[make_defaults_use_inserter] % " ");
PARSER_DEFINE(CONFIG_PROTECT_MASK, CONFIG_PROTECT());
PARSER_DEFINE(IUSE_IMPLICIT, USE());
PARSER_DEFINE(USE_EXPAND_IMPLICIT, USE_EXPAND());
PARSER_DEFINE(USE_EXPAND_UNPREFIXED, USE_EXPAND());
PARSER_DEFINE(ENV_UNSET,
              (-x3::char_("-") >> (x3::char_("*") | +x3::ascii::graph))[make_defaults_use_inserter] % " ");

} // namespace parsers::profile
} // namespace pms_utils
