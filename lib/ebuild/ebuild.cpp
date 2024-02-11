#include "pms-utils/ebuild/ebuild.hpp"

#include <boost/variant/static_visitor.hpp>
#include <format>
#include <ostream>
#include <string>

namespace [[gnu::visibility("default")]] pms_utils {
namespace ebuild {

uri_elem::operator std::string() const {
    struct Visitor : public boost::static_visitor<std::string> {
        std::string operator()(const URI &_uri) const { return _uri; }
        std::string operator()(const std::filesystem::path &path) const { return path.string(); }
    };
    std::string ret = boost::apply_visitor(Visitor(), uri);
    if (filename.has_value()) {
        ret += std::format(" -> {}", filename.value().string());
    }
    return ret;
}

std::ostream &operator<<(std::ostream &out, const uri_elem &uri_elem) { return out << std::string(uri_elem); }

restrict_elem::operator std::string() const { return string; }
std::ostream &operator<<(std::ostream &out, const restrict_elem &restrict_elem) {
    return out << std::string(restrict_elem);
}

[[nodiscard]] std::string to_string(restrict_elem::Type type) {
    using enum restrict_elem::Type;
    switch (type) {
    case UNKNOWN:
        return "UNKNOWN";
    case mirror:
        return "mirror";
    case fetch:
        return "fetch";
    case strip:
        return "strip";
    case userpriv:
        return "userpriv";
    case test:
        return "test";
    default:
        // gcc cannot see that all enum values are covered, sigh
        __builtin_unreachable();
    }
}
std::ostream &operator<<(std::ostream &out, restrict_elem::Type type) { return out << to_string(type); }

KEYWORDS::operator std::string() const {
    std::string ret;
    for (const auto &elem : *this) {
        ret += elem + " ";
    }
    if (!ret.empty()) {
        ret.pop_back();
    }
    return ret;
}
std::ostream &operator<<(std::ostream &out, const KEYWORDS &keywords) { return out << std::string(keywords); }

INHERITED::operator std::string() const {
    std::string ret;
    for (const auto &elem : *this) {
        ret += elem + " ";
    }
    if (!ret.empty()) {
        ret.pop_back();
    }
    return ret;
}
std::ostream &operator<<(std::ostream &out, const INHERITED &inherited) {
    return out << std::string(inherited);
}

iuse_elem::operator std::string() const {
    std::string ret;
    if (default_enabled) {
        ret += '+';
    }
    ret += useflag;
    return ret;
}
std::ostream &operator<<(std::ostream &out, const iuse_elem &iuse_elem) {
    return out << std::string(iuse_elem);
}

IUSE::operator std::string() const {
    std::string ret;
    for (const auto &elem : *this) {
        ret += std::string(elem) + " ";
    }
    if (!ret.empty()) {
        ret.pop_back();
    }
    return ret;
}
std::ostream &operator<<(std::ostream &out, const IUSE &iuse) { return out << std::string(iuse); }

properties_elem::operator std::string() const { return string; }
std::ostream &operator<<(std::ostream &out, const properties_elem &properties_elem) {
    return out << std::string(properties_elem);
}

[[nodiscard]] std::string to_string(properties_elem::Type type) {
    using enum properties_elem::Type;
    switch (type) {
    case UNKNOWN:
        return "UNKNOWN";
    case interactive:
        return "interactive";
    case live:
        return "live";
    case test_network:
        return "test_network";
    default:
        // gcc cannot see that all enum values are covered, sigh
        __builtin_unreachable();
    }
}
std::ostream &operator<<(std::ostream &out, properties_elem::Type type) { return out << to_string(type); }

[[nodiscard]] std::string to_string(phases _phases) {
    using enum phases;
    switch (_phases) {
    case phases::pretend:
        return "pretend";
    case phases::setup:
        return "setup";
    case phases::unpack:
        return "unpack";
    case phases::prepare:
        return "prepare";
    case phases::configure:
        return "configure";
    case phases::compile:
        return "compile";
    case phases::test:
        return "test";
    case phases::install:
        return "install";
    case phases::preinst:
        return "preinst";
    case phases::postinst:
        return "postinst";
    case phases::prerm:
        return "prerm";
    case phases::postrm:
        return "postrm";
    case phases::config:
        return "config";
    case phases::info:
        return "info";
    case phases::nofetch:
        return "nofetch";
    default:
        // gcc cannot see that all enum values are covered, sigh
        __builtin_unreachable();
    }
}
std::ostream &operator<<(std::ostream &out, phases _phases) { return out << to_string(_phases); }

DEFINED_PHASES::operator std::string() const {
    std::string ret;
    for (const auto &elem : *this) {
        ret += to_string(elem) + " ";
    }
    if (ret.empty()) {
        ret = "-";
    } else {
        ret.pop_back();
    }
    return ret;
}
std::ostream &operator<<(std::ostream &out, const DEFINED_PHASES &defined_phases) {
    return out << std::string(defined_phases);
}

} // namespace ebuild
} // namespace pms_utils
