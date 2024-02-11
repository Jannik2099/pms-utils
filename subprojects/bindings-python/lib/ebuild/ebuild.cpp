
#include "ebuild.hpp"

#include "../common.hpp"
#include "pms-utils/ebuild/ebuild.hpp"
#include "pms-utils/ebuild/ebuild_parser.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

using namespace pms_utils::ebuild;

namespace PYBIND11_NAMESPACE {
namespace detail {

template <typename T> struct type_caster<boost::optional<T>> : optional_caster<boost::optional<T>> {};

template <typename... Ts>
struct type_caster<boost::variant<Ts...>> : variant_caster<boost::variant<Ts...>> {};

template <> struct visit_helper<boost::variant> {
    template <typename... Args> static auto call(Args &&...args) -> decltype(boost::apply_visitor(args...)) {
        return boost::apply_visitor(std::forward<Args>(args)...);
    }
};

} // namespace detail
} // namespace PYBIND11_NAMESPACE

namespace pms_utils::bindings::python::ebuild {

void _register(py::module &_module) {

    py::module ebuild = _module.def_submodule("ebuild");

    auto py_URI = create_bindings<URI>(ebuild, parsers::uri);

    auto py_uri_elem = create_bindings<uri_elem>(ebuild, parsers::uri_elem);
    auto py_SRC_URI = create_bindings<SRC_URI>(ebuild, parsers::SRC_URI);

    auto py_restrict_elem = create_bindings<restrict_elem>(ebuild, parsers::restrict_elem);
    auto py_restrict_elem_type = create_bindings<restrict_elem::Type>(py_restrict_elem);
    auto py_RESTRICT = create_bindings<RESTRICT>(ebuild, parsers::RESTRICT);

    auto py_HOMEPAGE = create_bindings<HOMEPAGE>(ebuild, parsers::HOMEPAGE);

    auto py_license_elem = create_bindings<license_elem>(ebuild, parsers::license_elem);
    auto py_LICENSE = create_bindings<LICENSE>(ebuild, parsers::LICENSE);

    auto py_keyword = create_bindings<keyword>(ebuild, parsers::keyword);
    auto py_KEYWORDS = create_bindings<KEYWORDS>(ebuild, parsers::KEYWORDS);

    auto py_inherited_elem = create_bindings<inherited_elem>(ebuild, parsers::inherited_elem);
    auto py_INHERITED = create_bindings<INHERITED>(ebuild, parsers::INHERITED);

    auto py_iuse_elem = create_bindings<iuse_elem>(ebuild, parsers::iuse_elem);
    auto py_IUSE = create_bindings<IUSE>(ebuild, parsers::IUSE);

    auto py_REQUIRED_USE = create_bindings<REQUIRED_USE>(ebuild, parsers::REQUIRED_USE);

    auto py_EAPI = create_bindings<EAPI>(ebuild, parsers::EAPI);

    auto py_properties_elem = create_bindings<properties_elem>(ebuild, parsers::properties_elem);
    auto py_properties_elem_type = create_bindings<properties_elem::Type>(py_properties_elem);
    auto py_PROPERTIES = create_bindings<PROPERTIES>(ebuild, parsers::PROPERTIES);

    auto py_phases = create_bindings<phases>(ebuild);
    auto py_DEFINED_PHASES = create_bindings<DEFINED_PHASES>(ebuild, parsers::DEFINED_PHASES);
}

} // namespace pms_utils::bindings::python::ebuild
