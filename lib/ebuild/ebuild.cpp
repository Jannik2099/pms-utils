#include "pms-utils/ebuild/ebuild.hpp"

#include <filesystem>
#include <format>
#include <ostream>
#include <string>
#include <variant>

namespace [[gnu::visibility("default")]] pms_utils {
namespace ebuild {

uri_elem::operator std::string() const {
    class Visitor {
    public:
        std::string operator()(const URI &_uri) const { return _uri; }
        std::string operator()(const std::filesystem::path &path) const { return path.string(); }
    };
    std::string ret = std::visit(Visitor{}, uri);
    if (filename.has_value()) {
        ret += std::format(" -> {}", filename.value().string());
    }
    return ret;
}
std::ostream &uri_elem::ostream_impl(std::ostream &out) const { return out << std::string{*this}; }

restrict_elem::operator std::string() const { return string; }
std::ostream &restrict_elem::ostream_impl(std::ostream &out) const { return out << std::string{*this}; }

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
        throw std::out_of_range{"unknown enum value"};
    }
}
std::ostream &operator<<(std::ostream &out, restrict_elem::Type type) { return out << to_string(type); }

keywords::operator std::string() const {
    std::string ret;
    for (const auto &elem : *this) {
        ret += elem + " ";
    }
    if (!ret.empty()) {
        ret.pop_back();
    }
    return ret;
}
std::ostream &keywords::ostream_impl(std::ostream &out) const { return out << std::string{*this}; }

inherited::operator std::string() const {
    std::string ret;
    for (const auto &elem : *this) {
        ret += elem + " ";
    }
    if (!ret.empty()) {
        ret.pop_back();
    }
    return ret;
}
std::ostream &inherited::ostream_impl(std::ostream &out) const { return out << std::string{*this}; }

iuse_elem::operator std::string() const {
    std::string ret;
    if (default_enabled) {
        ret += '+';
    }
    ret += useflag;
    return ret;
}
std::ostream &iuse_elem::ostream_impl(std::ostream &out) const { return out << std::string{*this}; }

iuse::operator std::string() const {
    std::string ret;
    for (const auto &elem : *this) {
        ret += std::string{elem} + " ";
    }
    if (!ret.empty()) {
        ret.pop_back();
    }
    return ret;
}
std::ostream &iuse::ostream_impl(std::ostream &out) const { return out << std::string{*this}; }

properties_elem::operator std::string() const { return string; }
std::ostream &properties_elem::ostream_impl(std::ostream &out) const { return out << std::string{*this}; }

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
        throw std::out_of_range{"unknown enum value"};
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
        throw std::out_of_range{"unknown enum value"};
    }
}
std::ostream &operator<<(std::ostream &out, phases _phases) { return out << to_string(_phases); }

defined_phases::operator std::string() const {
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
std::ostream &defined_phases::ostream_impl(std::ostream &out) const { return out << std::string{*this}; }

} // namespace ebuild
} // namespace pms_utils
