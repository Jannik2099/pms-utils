from enum import Enum
from typing import Optional, Union
from pathlib import Path

from pms_utils import atom, depend

class URI(str):
    def __init__(self, string: str) -> None: ...

class uri_elem:
    uri: Union[URI, Path]
    filename: Optional[Path]

    def __init__(self, string: str) -> None: ...
    def __repr__(self) -> str: ...

class SRC_URI:
    Node = Union[uri_elem, SRC_URI]
    conditional: Optional[depend.GroupHeader]
    nodes: list[Node]

    def __init__(self, string: str) -> None: ...
    def __repr__(self) -> str: ...

class restrict_elem:
    string: str

    class Type(Enum):
        UNKNOWN = 0
        mirror = 1
        fetch = 2
        strip = 3
        userpriv = 4
        test = 5

        def __repr__(self) -> str: ...

    type: Type

    def __init__(self, string: str) -> None: ...
    def __repr__(self) -> str: ...

class RESTRICT:
    Node = Union[restrict_elem, RESTRICT]
    conditional: Optional[depend.GroupHeader]
    nodes: list[Node]

    def __init__(self, string: str) -> None: ...
    def __repr__(self) -> str: ...

class HOMEPAGE:
    Node = Union[URI, HOMEPAGE]
    conditional: Optional[depend.GroupHeader]
    nodes: list[Node]

    def __init__(self, string: str) -> None: ...
    def __repr__(self) -> str: ...

class license_elem(str):
    def __init__(self, string: str) -> None: ...

class LICENSE:
    Node = Union[license_elem, LICENSE]
    conditional: Optional[depend.GroupHeader]
    nodes: list[Node]

    def __init__(self, string: str) -> None: ...
    def __repr__(self) -> str: ...

class keyword(str):
    def __init__(self, string: str) -> None: ...

class KEYWORDS:
    Node = Union[keyword, KEYWORDS]
    conditional: Optional[depend.GroupHeader]
    nodes: list[Node]

    def __init__(self, string: str) -> None: ...
    def __repr__(self) -> str: ...

class inherited_elem(str):
    def __init__(self, string: str) -> None: ...

class INHERITED:
    Node = Union[inherited_elem, INHERITED]
    conditional: Optional[depend.GroupHeader]
    nodes: list[Node]

    def __init__(self, string: str) -> None: ...
    def __repr__(self) -> str: ...

class iuse_elem:
    default_enabled: bool
    useflag: atom.Useflag

    def __init__(self, string: str) -> None: ...
    def __repr__(self) -> str: ...

class IUSE:
    Node = Union[iuse_elem, IUSE]
    conditional: Optional[depend.GroupHeader]
    nodes: list[Node]

    def __init__(self, string: str) -> None: ...
    def __repr__(self) -> str: ...

class REQUIRED_USE:
    Node = Union[atom.Usedep, REQUIRED_USE]
    conditional: Optional[depend.GroupHeader]
    nodes: list[Node]

    def __init__(self, string: str) -> None: ...
    def __repr__(self) -> str: ...

class EAPI(str):
    def __init__(self, string: str) -> None: ...

class properties_elem:
    string: str

    class Type(Enum):
        UNKNOWN = 0
        interactive = 1
        live = 2
        test_network = 3

        def __init__(self, string: str) -> None: ...
        def __repr__(self) -> str: ...

    type: Type

    def __init__(self, string: str) -> None: ...
    def __repr__(self) -> str: ...

class PROPERTIES:
    Node = Union[properties_elem, PROPERTIES]
    conditional: Optional[depend.GroupHeader]
    nodes: list[Node]

    def __init__(self, string: str) -> None: ...
    def __repr__(self) -> str: ...

class phases(Enum):
    pretend = 0
    setup = 1
    unpack = 2
    prepare = 3
    configure = 4
    compile = 5
    test = 6
    install = 7
    preinst = 8
    postinst = 9
    prerm = 10
    postrm = 11
    config = 12
    info = 13
    nofetch = 14

    def __repr__(self) -> str: ...

class DEFINED_PHASES:
    Node = Union[phases, DEFINED_PHASES]
    conditional: Optional[depend.GroupHeader]
    nodes: list[Node]

    def __init__(self, string: str) -> None: ...
    def __repr__(self) -> str: ...

class Metadata:
    DEPEND: depend.DependExpr
    RDEPEND: depend.DependExpr
    SLOT: atom.Slot
    SRC_URI: SRC_URI
    RESTRICT: RESTRICT
    HOMEPAGE: HOMEPAGE
    LICENSE: LICENSE
    DESCRIPTION: str
    KEYWORDS: KEYWORDS
    INHERITED: INHERITED
    IUSE: IUSE
    REQUIRED_USE: REQUIRED_USE
    PDEPEND: depend.DependExpr
    BDEPEND: depend.DependExpr
    EAPI: EAPI
    PROPERTIES: PROPERTIES
    DEFINED_PHASES: DEFINED_PHASES
    IDEPEND: depend.DependExpr
