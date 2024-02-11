
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
    auto py_src_uri = create_bindings<src_uri>(ebuild, parsers::SRC_URI);

    auto py_restrict_elem = create_bindings<restrict_elem>(ebuild, parsers::restrict_elem);
    auto py_restrict_elem_type = create_bindings<restrict_elem::Type>(py_restrict_elem);
    auto py_restrict = create_bindings<restrict>(ebuild, parsers::RESTRICT);

    auto py_homepage = create_bindings<homepage>(ebuild, parsers::HOMEPAGE);

    auto py_license_elem = create_bindings<license_elem>(ebuild, parsers::license_elem);
    auto py_license = create_bindings<license>(ebuild, parsers::LICENSE);

    auto py_keyword = create_bindings<keyword>(ebuild, parsers::keyword);
    auto py_keywords = create_bindings<keywords>(ebuild, parsers::KEYWORDS);

    auto py_inherited_elem = create_bindings<inherited_elem>(ebuild, parsers::inherited_elem);
    auto py_inherited = create_bindings<inherited>(ebuild, parsers::INHERITED);

    auto py_iuse_elem = create_bindings<iuse_elem>(ebuild, parsers::iuse_elem);
    auto py_iuse = create_bindings<iuse>(ebuild, parsers::IUSE);

    auto py_required_use = create_bindings<required_use>(ebuild, parsers::REQUIRED_USE);

    auto py_eapi = create_bindings<eapi>(ebuild, parsers::EAPI);

    auto py_properties_elem = create_bindings<properties_elem>(ebuild, parsers::properties_elem);
    auto py_properties_elem_type = create_bindings<properties_elem::Type>(py_properties_elem);
    auto py_properties = create_bindings<properties>(ebuild, parsers::PROPERTIES);

    auto py_phases = create_bindings<phases>(ebuild);
    auto py_defined_phases = create_bindings<defined_phases>(ebuild, parsers::DEFINED_PHASES);

    auto py_Metadata = create_bindings<Metadata>(ebuild);
}

} // namespace pms_utils::bindings::python::ebuild
