
#include "ebuild.hpp"

#include "../common.hpp"
#include "../internal.hpp"
#include "pms-utils/ebuild/ebuild.hpp"
#include "pms-utils/ebuild/ebuild_parser.hpp"

#include <nanobind/nanobind.h>
#include <string_view>

namespace nb = nanobind;

using namespace pms_utils::ebuild;

namespace pms_utils::bindings::python {

template <> constexpr std::string_view bound_type_name_override<restrict_elem::Type> = "restrict_elem_type";
template <>
constexpr std::string_view bound_type_name_override<properties_elem::Type> = "properties_elem_type";

namespace ebuild {

void _register(nb::module_ &_module) {

    const nb::module_ ebuild = _module.def_submodule("ebuild");

    auto py_URI = create_bindings<URI>(ebuild, parsers::ebuild::uri);

    auto py_uri_elem = create_bindings<uri_elem>(ebuild, parsers::ebuild::uri_elem);
    auto py_src_uri = create_bindings<src_uri>(ebuild, parsers::ebuild::SRC_URI);

    auto py_restrict_elem = create_bindings<restrict_elem>(ebuild, parsers::ebuild::restrict_elem);
    auto py_restrict_elem_type = create_bindings<restrict_elem::Type>(ebuild);
    auto py_restrict = create_bindings<restrict>(ebuild, parsers::ebuild::RESTRICT);

    auto py_homepage = create_bindings<homepage>(ebuild, parsers::ebuild::HOMEPAGE);

    auto py_license_elem = create_bindings<license_elem>(ebuild, parsers::ebuild::license_elem);
    auto py_license = create_bindings<license>(ebuild, parsers::ebuild::LICENSE);

    auto py_keyword = create_bindings<keyword>(ebuild, parsers::ebuild::keyword);
    auto py_keywords = create_bindings<keywords>(ebuild, parsers::ebuild::KEYWORDS);

    auto py_inherited_elem = create_bindings<inherited_elem>(ebuild, parsers::ebuild::inherited_elem);
    auto py_inherited = create_bindings<inherited>(ebuild, parsers::ebuild::INHERITED);

    auto py_iuse_elem = create_bindings<iuse_elem>(ebuild, parsers::ebuild::iuse_elem);
    auto py_iuse = create_bindings<iuse>(ebuild, parsers::ebuild::IUSE);

    auto py_required_use = create_bindings<required_use>(ebuild, parsers::ebuild::REQUIRED_USE);

    auto py_eapi = create_bindings<eapi>(ebuild, parsers::ebuild::EAPI);

    auto py_properties_elem = create_bindings<properties_elem>(ebuild, parsers::ebuild::properties_elem);
    auto py_properties_elem_type = create_bindings<properties_elem::Type>(ebuild);
    auto py_properties = create_bindings<properties>(ebuild, parsers::ebuild::PROPERTIES);

    auto py_phases = create_bindings<phases>(ebuild);
    auto py_defined_phases = create_bindings<defined_phases>(ebuild, parsers::ebuild::DEFINED_PHASES);

    auto py_Metadata = create_bindings<Metadata>(ebuild);
}

} // namespace ebuild
} // namespace pms_utils::bindings::python
