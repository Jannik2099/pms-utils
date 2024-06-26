import enum
import os
import pms_utils.atom
import pms_utils.depend
from typing import Callable, ClassVar, Iterator

class Metadata:
    def __init__(self, *args, **kwargs) -> None: ...
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
    def __init__(self, arg0: str) -> None: ...

class defined_phases:
    def __init__(self, arg0: str) -> None: ...
    def __iter__(self) -> Iterator[phases]: ...

class eapi:
    def __init__(self, arg0: str) -> None: ...

class homepage:
    def __init__(self, arg0: str) -> None: ...
    def __iter__(self) -> Iterator[URI | homepage]: ...
    @property
    def conditional(
        self,
    ) -> pms_utils.depend.UseConditional | pms_utils.depend.GroupHeaderOp | None: ...
    @property
    def nodes(self) -> list[URI | homepage]: ...

class inherited:
    def __init__(self, arg0: str) -> None: ...
    def __iter__(self) -> Iterator[inherited_elem]: ...

class inherited_elem:
    def __init__(self, arg0: str) -> None: ...

class iuse:
    def __init__(self, arg0: str) -> None: ...
    def __iter__(self) -> Iterator[iuse_elem]: ...

class iuse_elem:
    def __init__(self, arg0: str) -> None: ...
    @property
    def default_enabled(self) -> bool: ...
    @property
    def useflag(self) -> pms_utils.atom.Useflag: ...

class keyword:
    def __init__(self, arg0: str) -> None: ...

class keywords:
    def __init__(self, arg0: str) -> None: ...
    def __iter__(self) -> Iterator[keyword]: ...

class license:
    def __init__(self, arg0: str) -> None: ...
    def __iter__(self) -> Iterator[license_elem | license]: ...
    @property
    def conditional(
        self,
    ) -> pms_utils.depend.UseConditional | pms_utils.depend.GroupHeaderOp | None: ...
    @property
    def nodes(self) -> list[license_elem | license]: ...

class license_elem:
    def __init__(self, arg0: str) -> None: ...

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
    def __init__(self, arg0: str) -> None: ...
    def __iter__(self) -> Iterator[properties_elem | properties]: ...
    @property
    def conditional(
        self,
    ) -> pms_utils.depend.UseConditional | pms_utils.depend.GroupHeaderOp | None: ...
    @property
    def nodes(self) -> list[properties_elem | properties]: ...

class properties_elem:
    def __init__(self, arg0: str) -> None: ...
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
    def __init__(self, arg0: str) -> None: ...
    def __iter__(self) -> Iterator[pms_utils.atom.Usedep | required_use]: ...
    @property
    def conditional(
        self,
    ) -> pms_utils.depend.UseConditional | pms_utils.depend.GroupHeaderOp | None: ...
    @property
    def nodes(self) -> list[pms_utils.atom.Usedep | required_use]: ...

class restrict:
    def __init__(self, arg0: str) -> None: ...
    def __iter__(self) -> Iterator[restrict_elem | restrict]: ...
    @property
    def conditional(
        self,
    ) -> pms_utils.depend.UseConditional | pms_utils.depend.GroupHeaderOp | None: ...
    @property
    def nodes(self) -> list[restrict_elem | restrict]: ...

class restrict_elem:
    def __init__(self, arg0: str) -> None: ...
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
    def __init__(self, arg0: str) -> None: ...
    def __iter__(self) -> Iterator[uri_elem | src_uri]: ...
    @property
    def conditional(
        self,
    ) -> pms_utils.depend.UseConditional | pms_utils.depend.GroupHeaderOp | None: ...
    @property
    def nodes(self) -> list[uri_elem | src_uri]: ...

class uri_elem:
    def __init__(self, arg0: str) -> None: ...
    @property
    def filename(self) -> os.PathLike | None: ...
    @property
    def uri(self) -> URI | os.PathLike: ...
