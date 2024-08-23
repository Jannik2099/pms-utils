import enum
import os
import pms_utils.atom
import pms_utils.depend
from typing import Callable, ClassVar, Iterator

class Metadata:
    def __init__(self, *args, **kwargs) -> None:
        """Initialize self.  See help(type(self)) for accurate signature."""
    def __eq__(self, arg0: Metadata) -> bool:
        """__eq__(self: pms_utils.ebuild.Metadata, arg0: pms_utils.ebuild.Metadata) -> bool"""
    def __hash__(self) -> int:
        """__hash__(self: pms_utils.ebuild.Metadata) -> int"""
    def __ne__(self, arg0: Metadata) -> bool:
        """__ne__(self: pms_utils.ebuild.Metadata, arg0: pms_utils.ebuild.Metadata) -> bool"""
    @property
    def BDEPEND(self) -> pms_utils.depend.DependExpr: ...
    @property
    def DEFINED_PHASES(self) -> defined_phases: ...
    @property
    def DEPEND(self) -> pms_utils.depend.DependExpr: ...
    @property
    def DESCRIPTION(self) -> str: ...
    @property
    def EAPI(self) -> eapi: ...
    @property
    def HOMEPAGE(self) -> homepage: ...
    @property
    def IDEPEND(self) -> pms_utils.depend.DependExpr: ...
    @property
    def INHERITED(self) -> inherited: ...
    @property
    def IUSE(self) -> iuse: ...
    @property
    def KEYWORDS(self) -> keywords: ...
    @property
    def LICENSE(self) -> license: ...
    @property
    def PDEPEND(self) -> pms_utils.depend.DependExpr: ...
    @property
    def PROPERTIES(self) -> properties: ...
    @property
    def RDEPEND(self) -> pms_utils.depend.DependExpr: ...
    @property
    def REQUIRED_USE(self) -> required_use: ...
    @property
    def RESTRICT(self) -> restrict: ...
    @property
    def SLOT(self) -> pms_utils.atom.Slot: ...
    @property
    def SRC_URI(self) -> src_uri: ...

class URI:
    def __init__(self, expr: str) -> None:
        """__init__(self: pms_utils.ebuild.URI, expr: str) -> None


                    Constructs a new URI object from the input expression.

                    :raises ValueError: The expression is invalid.

        """
    def __eq__(self, arg0: URI) -> bool:
        """__eq__(self: pms_utils.ebuild.URI, arg0: pms_utils.ebuild.URI) -> bool"""
    def __hash__(self) -> int:
        """__hash__(self: pms_utils.ebuild.URI) -> int"""
    def __ne__(self, arg0: URI) -> bool:
        """__ne__(self: pms_utils.ebuild.URI, arg0: pms_utils.ebuild.URI) -> bool"""

class defined_phases:
    def __init__(self, expr: str) -> None:
        """__init__(self: pms_utils.ebuild.defined_phases, expr: str) -> None


                    Constructs a new defined_phases object from the input expression.

                    :raises ValueError: The expression is invalid.

        """
    def __eq__(self, arg0: defined_phases) -> bool:
        """__eq__(self: pms_utils.ebuild.defined_phases, arg0: pms_utils.ebuild.defined_phases) -> bool"""
    def __hash__(self) -> int:
        """__hash__(self: pms_utils.ebuild.defined_phases) -> int"""
    def __iter__(self) -> Iterator[phases]:
        """__iter__(self: pms_utils.ebuild.defined_phases) -> Iterator[pms_utils.ebuild.phases]"""
    def __ne__(self, arg0: defined_phases) -> bool:
        """__ne__(self: pms_utils.ebuild.defined_phases, arg0: pms_utils.ebuild.defined_phases) -> bool"""

class eapi:
    def __init__(self, expr: str) -> None:
        """__init__(self: pms_utils.ebuild.eapi, expr: str) -> None


                    Constructs a new eapi object from the input expression.

                    :raises ValueError: The expression is invalid.

        """
    def __eq__(self, arg0: eapi) -> bool:
        """__eq__(self: pms_utils.ebuild.eapi, arg0: pms_utils.ebuild.eapi) -> bool"""
    def __hash__(self) -> int:
        """__hash__(self: pms_utils.ebuild.eapi) -> int"""
    def __ne__(self, arg0: eapi) -> bool:
        """__ne__(self: pms_utils.ebuild.eapi, arg0: pms_utils.ebuild.eapi) -> bool"""

class homepage:
    def __init__(self, expr: str) -> None:
        """__init__(self: pms_utils.ebuild.homepage, expr: str) -> None


                    Constructs a new homepage object from the input expression.

                    :raises ValueError: The expression is invalid.

        """
    def __eq__(self, arg0: homepage) -> bool:
        """__eq__(self: pms_utils.ebuild.homepage, arg0: pms_utils.ebuild.homepage) -> bool"""
    def __hash__(self) -> int:
        """__hash__(self: pms_utils.ebuild.homepage) -> int"""
    def __iter__(self) -> Iterator[URI | homepage]:
        """__iter__(self: pms_utils.ebuild.homepage) -> Iterator[Union[pms_utils.ebuild.URI, pms_utils.ebuild.homepage]]"""
    def __ne__(self, arg0: homepage) -> bool:
        """__ne__(self: pms_utils.ebuild.homepage, arg0: pms_utils.ebuild.homepage) -> bool"""
    @property
    def conditional(self) -> pms_utils.depend.UseConditional | pms_utils.depend.GroupHeaderOp | None: ...
    @property
    def nodes(self) -> list[URI | homepage]: ...

class inherited:
    def __init__(self, expr: str) -> None:
        """__init__(self: pms_utils.ebuild.inherited, expr: str) -> None


                    Constructs a new inherited object from the input expression.

                    :raises ValueError: The expression is invalid.

        """
    def __eq__(self, arg0: inherited) -> bool:
        """__eq__(self: pms_utils.ebuild.inherited, arg0: pms_utils.ebuild.inherited) -> bool"""
    def __hash__(self) -> int:
        """__hash__(self: pms_utils.ebuild.inherited) -> int"""
    def __iter__(self) -> Iterator[inherited_elem]:
        """__iter__(self: pms_utils.ebuild.inherited) -> Iterator[pms_utils.ebuild.inherited_elem]"""
    def __ne__(self, arg0: inherited) -> bool:
        """__ne__(self: pms_utils.ebuild.inherited, arg0: pms_utils.ebuild.inherited) -> bool"""

class inherited_elem:
    def __init__(self, expr: str) -> None:
        """__init__(self: pms_utils.ebuild.inherited_elem, expr: str) -> None


                    Constructs a new inherited_elem object from the input expression.

                    :raises ValueError: The expression is invalid.

        """
    def __eq__(self, arg0: inherited_elem) -> bool:
        """__eq__(self: pms_utils.ebuild.inherited_elem, arg0: pms_utils.ebuild.inherited_elem) -> bool"""
    def __hash__(self) -> int:
        """__hash__(self: pms_utils.ebuild.inherited_elem) -> int"""
    def __ne__(self, arg0: inherited_elem) -> bool:
        """__ne__(self: pms_utils.ebuild.inherited_elem, arg0: pms_utils.ebuild.inherited_elem) -> bool"""

class iuse:
    def __init__(self, expr: str) -> None:
        """__init__(self: pms_utils.ebuild.iuse, expr: str) -> None


                    Constructs a new iuse object from the input expression.

                    :raises ValueError: The expression is invalid.

        """
    def __eq__(self, arg0: iuse) -> bool:
        """__eq__(self: pms_utils.ebuild.iuse, arg0: pms_utils.ebuild.iuse) -> bool"""
    def __hash__(self) -> int:
        """__hash__(self: pms_utils.ebuild.iuse) -> int"""
    def __iter__(self) -> Iterator[iuse_elem]:
        """__iter__(self: pms_utils.ebuild.iuse) -> Iterator[pms_utils.ebuild.iuse_elem]"""
    def __ne__(self, arg0: iuse) -> bool:
        """__ne__(self: pms_utils.ebuild.iuse, arg0: pms_utils.ebuild.iuse) -> bool"""

class iuse_elem:
    def __init__(self, expr: str) -> None:
        """__init__(self: pms_utils.ebuild.iuse_elem, expr: str) -> None


                    Constructs a new iuse_elem object from the input expression.

                    :raises ValueError: The expression is invalid.

        """
    def __eq__(self, arg0: iuse_elem) -> bool:
        """__eq__(self: pms_utils.ebuild.iuse_elem, arg0: pms_utils.ebuild.iuse_elem) -> bool"""
    def __hash__(self) -> int:
        """__hash__(self: pms_utils.ebuild.iuse_elem) -> int"""
    def __ne__(self, arg0: iuse_elem) -> bool:
        """__ne__(self: pms_utils.ebuild.iuse_elem, arg0: pms_utils.ebuild.iuse_elem) -> bool"""
    @property
    def default_enabled(self) -> bool: ...
    @property
    def useflag(self) -> pms_utils.atom.Useflag: ...

class keyword:
    def __init__(self, expr: str) -> None:
        """__init__(self: pms_utils.ebuild.keyword, expr: str) -> None


                    Constructs a new keyword object from the input expression.

                    :raises ValueError: The expression is invalid.

        """
    def __eq__(self, arg0: keyword) -> bool:
        """__eq__(self: pms_utils.ebuild.keyword, arg0: pms_utils.ebuild.keyword) -> bool"""
    def __hash__(self) -> int:
        """__hash__(self: pms_utils.ebuild.keyword) -> int"""
    def __ne__(self, arg0: keyword) -> bool:
        """__ne__(self: pms_utils.ebuild.keyword, arg0: pms_utils.ebuild.keyword) -> bool"""

class keywords:
    def __init__(self, expr: str) -> None:
        """__init__(self: pms_utils.ebuild.keywords, expr: str) -> None


                    Constructs a new keywords object from the input expression.

                    :raises ValueError: The expression is invalid.

        """
    def __eq__(self, arg0: keywords) -> bool:
        """__eq__(self: pms_utils.ebuild.keywords, arg0: pms_utils.ebuild.keywords) -> bool"""
    def __hash__(self) -> int:
        """__hash__(self: pms_utils.ebuild.keywords) -> int"""
    def __iter__(self) -> Iterator[keyword]:
        """__iter__(self: pms_utils.ebuild.keywords) -> Iterator[pms_utils.ebuild.keyword]"""
    def __ne__(self, arg0: keywords) -> bool:
        """__ne__(self: pms_utils.ebuild.keywords, arg0: pms_utils.ebuild.keywords) -> bool"""

class license:
    def __init__(self, expr: str) -> None:
        """__init__(self: pms_utils.ebuild.license, expr: str) -> None


                    Constructs a new license object from the input expression.

                    :raises ValueError: The expression is invalid.

        """
    def __eq__(self, arg0: license) -> bool:
        """__eq__(self: pms_utils.ebuild.license, arg0: pms_utils.ebuild.license) -> bool"""
    def __hash__(self) -> int:
        """__hash__(self: pms_utils.ebuild.license) -> int"""
    def __iter__(self) -> Iterator[license_elem | license]:
        """__iter__(self: pms_utils.ebuild.license) -> Iterator[Union[pms_utils.ebuild.license_elem, pms_utils.ebuild.license]]"""
    def __ne__(self, arg0: license) -> bool:
        """__ne__(self: pms_utils.ebuild.license, arg0: pms_utils.ebuild.license) -> bool"""
    @property
    def conditional(self) -> pms_utils.depend.UseConditional | pms_utils.depend.GroupHeaderOp | None: ...
    @property
    def nodes(self) -> list[license_elem | license]: ...

class license_elem:
    def __init__(self, expr: str) -> None:
        """__init__(self: pms_utils.ebuild.license_elem, expr: str) -> None


                    Constructs a new license_elem object from the input expression.

                    :raises ValueError: The expression is invalid.

        """
    def __eq__(self, arg0: license_elem) -> bool:
        """__eq__(self: pms_utils.ebuild.license_elem, arg0: pms_utils.ebuild.license_elem) -> bool"""
    def __hash__(self) -> int:
        """__hash__(self: pms_utils.ebuild.license_elem) -> int"""
    def __ne__(self, arg0: license_elem) -> bool:
        """__ne__(self: pms_utils.ebuild.license_elem, arg0: pms_utils.ebuild.license_elem) -> bool"""

class phases(enum.Enum):
    __new__: ClassVar[Callable] = ...
    _generate_next_value_: ClassVar[Callable] = ...
    _member_map_: ClassVar[dict] = ...
    _member_names_: ClassVar[list] = ...
    _member_type_: ClassVar[type[object]] = ...
    _unhashable_values_: ClassVar[list] = ...
    _use_args_: ClassVar[bool] = ...
    _value2member_map_: ClassVar[dict] = ...
    _value_repr_: ClassVar[None] = ...
    compile: ClassVar[phases] = ...
    config: ClassVar[phases] = ...
    configure: ClassVar[phases] = ...
    info: ClassVar[phases] = ...
    install: ClassVar[phases] = ...
    nofetch: ClassVar[phases] = ...
    postinst: ClassVar[phases] = ...
    postrm: ClassVar[phases] = ...
    preinst: ClassVar[phases] = ...
    prepare: ClassVar[phases] = ...
    prerm: ClassVar[phases] = ...
    pretend: ClassVar[phases] = ...
    setup: ClassVar[phases] = ...
    test: ClassVar[phases] = ...
    unpack: ClassVar[phases] = ...

class properties:
    def __init__(self, expr: str) -> None:
        """__init__(self: pms_utils.ebuild.properties, expr: str) -> None


                    Constructs a new properties object from the input expression.

                    :raises ValueError: The expression is invalid.

        """
    def __eq__(self, arg0: properties) -> bool:
        """__eq__(self: pms_utils.ebuild.properties, arg0: pms_utils.ebuild.properties) -> bool"""
    def __hash__(self) -> int:
        """__hash__(self: pms_utils.ebuild.properties) -> int"""
    def __iter__(self) -> Iterator[properties_elem | properties]:
        """__iter__(self: pms_utils.ebuild.properties) -> Iterator[Union[pms_utils.ebuild.properties_elem, pms_utils.ebuild.properties]]"""
    def __ne__(self, arg0: properties) -> bool:
        """__ne__(self: pms_utils.ebuild.properties, arg0: pms_utils.ebuild.properties) -> bool"""
    @property
    def conditional(self) -> pms_utils.depend.UseConditional | pms_utils.depend.GroupHeaderOp | None: ...
    @property
    def nodes(self) -> list[properties_elem | properties]: ...

class properties_elem:
    def __init__(self, expr: str) -> None:
        """__init__(self: pms_utils.ebuild.properties_elem, expr: str) -> None


                    Constructs a new properties_elem object from the input expression.

                    :raises ValueError: The expression is invalid.

        """
    def __eq__(self, arg0: properties_elem) -> bool:
        """__eq__(self: pms_utils.ebuild.properties_elem, arg0: pms_utils.ebuild.properties_elem) -> bool"""
    def __hash__(self) -> int:
        """__hash__(self: pms_utils.ebuild.properties_elem) -> int"""
    def __ne__(self, arg0: properties_elem) -> bool:
        """__ne__(self: pms_utils.ebuild.properties_elem, arg0: pms_utils.ebuild.properties_elem) -> bool"""
    @property
    def string(self) -> str: ...
    @property
    def type(self) -> properties_elem_type: ...

class properties_elem_type(enum.Enum):
    __new__: ClassVar[Callable] = ...
    UNKNOWN: ClassVar[properties_elem_type] = ...
    _generate_next_value_: ClassVar[Callable] = ...
    _member_map_: ClassVar[dict] = ...
    _member_names_: ClassVar[list] = ...
    _member_type_: ClassVar[type[object]] = ...
    _unhashable_values_: ClassVar[list] = ...
    _use_args_: ClassVar[bool] = ...
    _value2member_map_: ClassVar[dict] = ...
    _value_repr_: ClassVar[None] = ...
    interactive: ClassVar[properties_elem_type] = ...
    live: ClassVar[properties_elem_type] = ...
    test_network: ClassVar[properties_elem_type] = ...

class required_use:
    def __init__(self, expr: str) -> None:
        """__init__(self: pms_utils.ebuild.required_use, expr: str) -> None


                    Constructs a new required_use object from the input expression.

                    :raises ValueError: The expression is invalid.

        """
    def __eq__(self, arg0: required_use) -> bool:
        """__eq__(self: pms_utils.ebuild.required_use, arg0: pms_utils.ebuild.required_use) -> bool"""
    def __hash__(self) -> int:
        """__hash__(self: pms_utils.ebuild.required_use) -> int"""
    def __iter__(self) -> Iterator[pms_utils.atom.Usedep | required_use]:
        """__iter__(self: pms_utils.ebuild.required_use) -> Iterator[Union[pms_utils.atom.Usedep, pms_utils.ebuild.required_use]]"""
    def __ne__(self, arg0: required_use) -> bool:
        """__ne__(self: pms_utils.ebuild.required_use, arg0: pms_utils.ebuild.required_use) -> bool"""
    @property
    def conditional(self) -> pms_utils.depend.UseConditional | pms_utils.depend.GroupHeaderOp | None: ...
    @property
    def nodes(self) -> list[pms_utils.atom.Usedep | required_use]: ...

class restrict:
    def __init__(self, expr: str) -> None:
        """__init__(self: pms_utils.ebuild.restrict, expr: str) -> None


                    Constructs a new restrict object from the input expression.

                    :raises ValueError: The expression is invalid.

        """
    def __eq__(self, arg0: restrict) -> bool:
        """__eq__(self: pms_utils.ebuild.restrict, arg0: pms_utils.ebuild.restrict) -> bool"""
    def __hash__(self) -> int:
        """__hash__(self: pms_utils.ebuild.restrict) -> int"""
    def __iter__(self) -> Iterator[restrict_elem | restrict]:
        """__iter__(self: pms_utils.ebuild.restrict) -> Iterator[Union[pms_utils.ebuild.restrict_elem, pms_utils.ebuild.restrict]]"""
    def __ne__(self, arg0: restrict) -> bool:
        """__ne__(self: pms_utils.ebuild.restrict, arg0: pms_utils.ebuild.restrict) -> bool"""
    @property
    def conditional(self) -> pms_utils.depend.UseConditional | pms_utils.depend.GroupHeaderOp | None: ...
    @property
    def nodes(self) -> list[restrict_elem | restrict]: ...

class restrict_elem:
    def __init__(self, expr: str) -> None:
        """__init__(self: pms_utils.ebuild.restrict_elem, expr: str) -> None


                    Constructs a new restrict_elem object from the input expression.

                    :raises ValueError: The expression is invalid.

        """
    def __eq__(self, arg0: restrict_elem) -> bool:
        """__eq__(self: pms_utils.ebuild.restrict_elem, arg0: pms_utils.ebuild.restrict_elem) -> bool"""
    def __hash__(self) -> int:
        """__hash__(self: pms_utils.ebuild.restrict_elem) -> int"""
    def __ne__(self, arg0: restrict_elem) -> bool:
        """__ne__(self: pms_utils.ebuild.restrict_elem, arg0: pms_utils.ebuild.restrict_elem) -> bool"""
    @property
    def string(self) -> str: ...
    @property
    def type(self) -> restrict_elem_type: ...

class restrict_elem_type(enum.Enum):
    __new__: ClassVar[Callable] = ...
    UNKNOWN: ClassVar[restrict_elem_type] = ...
    _generate_next_value_: ClassVar[Callable] = ...
    _member_map_: ClassVar[dict] = ...
    _member_names_: ClassVar[list] = ...
    _member_type_: ClassVar[type[object]] = ...
    _unhashable_values_: ClassVar[list] = ...
    _use_args_: ClassVar[bool] = ...
    _value2member_map_: ClassVar[dict] = ...
    _value_repr_: ClassVar[None] = ...
    fetch: ClassVar[restrict_elem_type] = ...
    mirror: ClassVar[restrict_elem_type] = ...
    strip: ClassVar[restrict_elem_type] = ...
    test: ClassVar[restrict_elem_type] = ...
    userpriv: ClassVar[restrict_elem_type] = ...

class src_uri:
    def __init__(self, expr: str) -> None:
        """__init__(self: pms_utils.ebuild.src_uri, expr: str) -> None


                    Constructs a new src_uri object from the input expression.

                    :raises ValueError: The expression is invalid.

        """
    def __eq__(self, arg0: src_uri) -> bool:
        """__eq__(self: pms_utils.ebuild.src_uri, arg0: pms_utils.ebuild.src_uri) -> bool"""
    def __hash__(self) -> int:
        """__hash__(self: pms_utils.ebuild.src_uri) -> int"""
    def __iter__(self) -> Iterator[uri_elem | src_uri]:
        """__iter__(self: pms_utils.ebuild.src_uri) -> Iterator[Union[pms_utils.ebuild.uri_elem, pms_utils.ebuild.src_uri]]"""
    def __ne__(self, arg0: src_uri) -> bool:
        """__ne__(self: pms_utils.ebuild.src_uri, arg0: pms_utils.ebuild.src_uri) -> bool"""
    @property
    def conditional(self) -> pms_utils.depend.UseConditional | pms_utils.depend.GroupHeaderOp | None: ...
    @property
    def nodes(self) -> list[uri_elem | src_uri]: ...

class uri_elem:
    def __init__(self, expr: str) -> None:
        """__init__(self: pms_utils.ebuild.uri_elem, expr: str) -> None


                    Constructs a new uri_elem object from the input expression.

                    :raises ValueError: The expression is invalid.

        """
    def __eq__(self, arg0: uri_elem) -> bool:
        """__eq__(self: pms_utils.ebuild.uri_elem, arg0: pms_utils.ebuild.uri_elem) -> bool"""
    def __hash__(self) -> int:
        """__hash__(self: pms_utils.ebuild.uri_elem) -> int"""
    def __ne__(self, arg0: uri_elem) -> bool:
        """__ne__(self: pms_utils.ebuild.uri_elem, arg0: pms_utils.ebuild.uri_elem) -> bool"""
    @property
    def filename(self) -> os.PathLike | None: ...
    @property
    def uri(self) -> URI | os.PathLike: ...
