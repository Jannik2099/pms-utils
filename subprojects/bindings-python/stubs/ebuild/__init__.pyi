from collections.abc import Iterator
import enum
import pathlib

import pms_utils


class Metadata:
    @property
    def DEPEND(self) -> pms_utils.depend.DependExpr: ...

    @property
    def RDEPEND(self) -> pms_utils.depend.DependExpr: ...

    @property
    def SLOT(self) -> pms_utils.atom.Slot: ...

    @property
    def SRC_URI(self) -> src_uri: ...

    @property
    def RESTRICT(self) -> restrict: ...

    @property
    def HOMEPAGE(self) -> homepage: ...

    @property
    def LICENSE(self) -> license: ...

    @property
    def DESCRIPTION(self) -> str: ...

    @property
    def KEYWORDS(self) -> keywords: ...

    @property
    def INHERITED(self) -> inherited: ...

    @property
    def IUSE(self) -> iuse: ...

    @property
    def REQUIRED_USE(self) -> required_use: ...

    @property
    def PDEPEND(self) -> pms_utils.depend.DependExpr: ...

    @property
    def BDEPEND(self) -> pms_utils.depend.DependExpr: ...

    @property
    def EAPI(self) -> eapi: ...

    @property
    def PROPERTIES(self) -> properties: ...

    @property
    def DEFINED_PHASES(self) -> defined_phases: ...

    @property
    def IDEPEND(self) -> pms_utils.depend.DependExpr: ...

    def __eq__(self, arg: Metadata, /) -> bool: ...

    def __ne__(self, arg: Metadata, /) -> bool: ...

    def __hash__(self) -> int: ...

class URI:
    def __init__(self, expr: str) -> None:
        """
        Constructs a new URI object from the input expression.

        :raises ValueError: The expression is invalid.
        """

    def __str__(self) -> str: ...

    def __repr__(self) -> str: ...

    def __eq__(self, arg: URI, /) -> bool: ...

    def __ne__(self, arg: URI, /) -> bool: ...

    def __hash__(self) -> int: ...

class defined_phases:
    def __init__(self, expr: str) -> None:
        """
        Constructs a new defined_phases object from the input expression.

        :raises ValueError: The expression is invalid.
        """

    def __str__(self) -> str: ...

    def __repr__(self) -> str: ...

    def __iter__(self) -> Iterator[phases]: ...

    def __eq__(self, arg: defined_phases, /) -> bool: ...

    def __ne__(self, arg: defined_phases, /) -> bool: ...

    def __hash__(self) -> int: ...

class eapi:
    def __init__(self, expr: str) -> None:
        """
        Constructs a new eapi object from the input expression.

        :raises ValueError: The expression is invalid.
        """

    def __str__(self) -> str: ...

    def __repr__(self) -> str: ...

    def __eq__(self, arg: eapi, /) -> bool: ...

    def __ne__(self, arg: eapi, /) -> bool: ...

    def __hash__(self) -> int: ...

class homepage:
    def __init__(self, expr: str) -> None:
        """
        Constructs a new homepage object from the input expression.

        :raises ValueError: The expression is invalid.
        """

    @property
    def conditional(self) -> pms_utils.depend.UseConditional | pms_utils.depend.GroupHeaderOp | None: ...

    @property
    def nodes(self) -> list[URI | homepage]: ...

    def __str__(self) -> str: ...

    def __repr__(self) -> str: ...

    def __iter__(self) -> Iterator[URI | homepage]: ...

    def __eq__(self, arg: homepage, /) -> bool: ...

    def __ne__(self, arg: homepage, /) -> bool: ...

    def __hash__(self) -> int: ...

class inherited:
    def __init__(self, expr: str) -> None:
        """
        Constructs a new inherited object from the input expression.

        :raises ValueError: The expression is invalid.
        """

    def __str__(self) -> str: ...

    def __repr__(self) -> str: ...

    def __iter__(self) -> Iterator[inherited_elem]: ...

    def __eq__(self, arg: inherited, /) -> bool: ...

    def __ne__(self, arg: inherited, /) -> bool: ...

    def __hash__(self) -> int: ...

class inherited_elem:
    def __init__(self, expr: str) -> None:
        """
        Constructs a new inherited_elem object from the input expression.

        :raises ValueError: The expression is invalid.
        """

    def __str__(self) -> str: ...

    def __repr__(self) -> str: ...

    def __eq__(self, arg: inherited_elem, /) -> bool: ...

    def __ne__(self, arg: inherited_elem, /) -> bool: ...

    def __hash__(self) -> int: ...

class iuse:
    def __init__(self, expr: str) -> None:
        """
        Constructs a new iuse object from the input expression.

        :raises ValueError: The expression is invalid.
        """

    def __str__(self) -> str: ...

    def __repr__(self) -> str: ...

    def __iter__(self) -> Iterator[iuse_elem]: ...

    def __eq__(self, arg: iuse, /) -> bool: ...

    def __ne__(self, arg: iuse, /) -> bool: ...

    def __hash__(self) -> int: ...

class iuse_elem:
    def __init__(self, expr: str) -> None:
        """
        Constructs a new iuse_elem object from the input expression.

        :raises ValueError: The expression is invalid.
        """

    @property
    def default_enabled(self) -> bool: ...

    @property
    def useflag(self) -> pms_utils.atom.Useflag: ...

    def __str__(self) -> str: ...

    def __repr__(self) -> str: ...

    def __eq__(self, arg: iuse_elem, /) -> bool: ...

    def __ne__(self, arg: iuse_elem, /) -> bool: ...

    def __hash__(self) -> int: ...

class keyword:
    def __init__(self, expr: str) -> None:
        """
        Constructs a new keyword object from the input expression.

        :raises ValueError: The expression is invalid.
        """

    def __str__(self) -> str: ...

    def __repr__(self) -> str: ...

    def __eq__(self, arg: keyword, /) -> bool: ...

    def __ne__(self, arg: keyword, /) -> bool: ...

    def __hash__(self) -> int: ...

class keywords:
    def __init__(self, expr: str) -> None:
        """
        Constructs a new keywords object from the input expression.

        :raises ValueError: The expression is invalid.
        """

    def __str__(self) -> str: ...

    def __repr__(self) -> str: ...

    def __iter__(self) -> Iterator[keyword]: ...

    def __eq__(self, arg: keywords, /) -> bool: ...

    def __ne__(self, arg: keywords, /) -> bool: ...

    def __hash__(self) -> int: ...

class license:
    def __init__(self, expr: str) -> None:
        """
        Constructs a new license object from the input expression.

        :raises ValueError: The expression is invalid.
        """

    @property
    def conditional(self) -> pms_utils.depend.UseConditional | pms_utils.depend.GroupHeaderOp | None: ...

    @property
    def nodes(self) -> list[license_elem | license]: ...

    def __str__(self) -> str: ...

    def __repr__(self) -> str: ...

    def __iter__(self) -> Iterator[license_elem | license]: ...

    def __eq__(self, arg: license, /) -> bool: ...

    def __ne__(self, arg: license, /) -> bool: ...

    def __hash__(self) -> int: ...

class license_elem:
    def __init__(self, expr: str) -> None:
        """
        Constructs a new license_elem object from the input expression.

        :raises ValueError: The expression is invalid.
        """

    def __str__(self) -> str: ...

    def __repr__(self) -> str: ...

    def __eq__(self, arg: license_elem, /) -> bool: ...

    def __ne__(self, arg: license_elem, /) -> bool: ...

    def __hash__(self) -> int: ...

class phases(enum.Enum):
    """
    Constructs a new phases object from the input expression.

    :raises ValueError: The expression is invalid.
    """

    def __str__(self) -> str: ...

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

class properties:
    def __init__(self, expr: str) -> None:
        """
        Constructs a new properties object from the input expression.

        :raises ValueError: The expression is invalid.
        """

    @property
    def conditional(self) -> pms_utils.depend.UseConditional | pms_utils.depend.GroupHeaderOp | None: ...

    @property
    def nodes(self) -> list[properties_elem | properties]: ...

    def __str__(self) -> str: ...

    def __repr__(self) -> str: ...

    def __iter__(self) -> Iterator[properties_elem | properties]: ...

    def __eq__(self, arg: properties, /) -> bool: ...

    def __ne__(self, arg: properties, /) -> bool: ...

    def __hash__(self) -> int: ...

class properties_elem:
    def __init__(self, expr: str) -> None:
        """
        Constructs a new properties_elem object from the input expression.

        :raises ValueError: The expression is invalid.
        """

    @property
    def string(self) -> str: ...

    @property
    def type(self) -> properties_elem_type: ...

    def __str__(self) -> str: ...

    def __repr__(self) -> str: ...

    def __eq__(self, arg: properties_elem, /) -> bool: ...

    def __ne__(self, arg: properties_elem, /) -> bool: ...

    def __hash__(self) -> int: ...

class properties_elem_type(enum.Enum):
    """
    Constructs a new properties_elem_type object from the input expression.

    :raises ValueError: The expression is invalid.
    """

    def __str__(self) -> str: ...

    UNKNOWN = 0

    interactive = 1

    live = 2

    test_network = 3

class required_use:
    def __init__(self, expr: str) -> None:
        """
        Constructs a new required_use object from the input expression.

        :raises ValueError: The expression is invalid.
        """

    @property
    def conditional(self) -> pms_utils.depend.UseConditional | pms_utils.depend.GroupHeaderOp | None: ...

    @property
    def nodes(self) -> list[pms_utils.atom.Usedep | required_use]: ...

    def __str__(self) -> str: ...

    def __repr__(self) -> str: ...

    def __iter__(self) -> Iterator[pms_utils.atom.Usedep | required_use]: ...

    def __eq__(self, arg: required_use, /) -> bool: ...

    def __ne__(self, arg: required_use, /) -> bool: ...

    def __hash__(self) -> int: ...

class restrict:
    def __init__(self, expr: str) -> None:
        """
        Constructs a new restrict object from the input expression.

        :raises ValueError: The expression is invalid.
        """

    @property
    def conditional(self) -> pms_utils.depend.UseConditional | pms_utils.depend.GroupHeaderOp | None: ...

    @property
    def nodes(self) -> list[restrict_elem | restrict]: ...

    def __str__(self) -> str: ...

    def __repr__(self) -> str: ...

    def __iter__(self) -> Iterator[restrict_elem | restrict]: ...

    def __eq__(self, arg: restrict, /) -> bool: ...

    def __ne__(self, arg: restrict, /) -> bool: ...

    def __hash__(self) -> int: ...

class restrict_elem:
    def __init__(self, expr: str) -> None:
        """
        Constructs a new restrict_elem object from the input expression.

        :raises ValueError: The expression is invalid.
        """

    @property
    def string(self) -> str: ...

    @property
    def type(self) -> restrict_elem_type: ...

    def __str__(self) -> str: ...

    def __repr__(self) -> str: ...

    def __eq__(self, arg: restrict_elem, /) -> bool: ...

    def __ne__(self, arg: restrict_elem, /) -> bool: ...

    def __hash__(self) -> int: ...

class restrict_elem_type(enum.Enum):
    """
    Constructs a new restrict_elem_type object from the input expression.

    :raises ValueError: The expression is invalid.
    """

    def __str__(self) -> str: ...

    UNKNOWN = 0

    mirror = 1

    fetch = 2

    strip = 3

    userpriv = 4

    test = 5

class src_uri:
    def __init__(self, expr: str) -> None:
        """
        Constructs a new src_uri object from the input expression.

        :raises ValueError: The expression is invalid.
        """

    @property
    def conditional(self) -> pms_utils.depend.UseConditional | pms_utils.depend.GroupHeaderOp | None: ...

    @property
    def nodes(self) -> list[uri_elem | src_uri]: ...

    def __str__(self) -> str: ...

    def __repr__(self) -> str: ...

    def __iter__(self) -> Iterator[uri_elem | src_uri]: ...

    def __eq__(self, arg: src_uri, /) -> bool: ...

    def __ne__(self, arg: src_uri, /) -> bool: ...

    def __hash__(self) -> int: ...

class uri_elem:
    def __init__(self, expr: str) -> None:
        """
        Constructs a new uri_elem object from the input expression.

        :raises ValueError: The expression is invalid.
        """

    @property
    def uri(self) -> URI | pathlib.Path: ...

    @property
    def filename(self) -> pathlib.Path | None: ...

    def __str__(self) -> str: ...

    def __repr__(self) -> str: ...

    def __eq__(self, arg: uri_elem, /) -> bool: ...

    def __ne__(self, arg: uri_elem, /) -> bool: ...

    def __hash__(self) -> int: ...
