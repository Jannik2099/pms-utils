from collections.abc import Iterator
import enum


class Atom:
    def __init__(self, expr: str) -> None:
        """
        Constructs a new Atom object from the input expression.

        :raises ValueError: The expression is invalid.
        """

    @property
    def blocker(self) -> Blocker | None: ...

    @property
    def category(self) -> Category: ...

    @property
    def name(self) -> Name: ...

    @property
    def verspec(self) -> VersionSpecifier | None: ...

    @property
    def version(self) -> Version | None: ...

    @property
    def slotExpr(self) -> SlotExpr | None: ...

    @property
    def usedeps(self) -> Usedeps: ...

    def __str__(self) -> str: ...

    def __repr__(self) -> str: ...

    def __eq__(self, arg: Atom, /) -> bool: ...

    def __ne__(self, arg: Atom, /) -> bool: ...

    def __hash__(self) -> int: ...

class Blocker(enum.Enum):
    """
    Constructs a new Blocker object from the input expression.

    :raises ValueError: The expression is invalid.
    """

    def __str__(self) -> str: ...

    def __repr__(self) -> str: ...

    weak = 0

    strong = 1

class Category:
    def __init__(self, expr: str) -> None:
        """
        Constructs a new Category object from the input expression.

        :raises ValueError: The expression is invalid.
        """

    def __str__(self) -> str: ...

    def __repr__(self) -> str: ...

    def __eq__(self, arg: Category, /) -> bool: ...

    def __ne__(self, arg: Category, /) -> bool: ...

    def __hash__(self) -> int: ...

class Name:
    def __init__(self, expr: str) -> None:
        """
        Constructs a new Name object from the input expression.

        :raises ValueError: The expression is invalid.
        """

    def __str__(self) -> str: ...

    def __repr__(self) -> str: ...

    def __eq__(self, arg: Name, /) -> bool: ...

    def __ne__(self, arg: Name, /) -> bool: ...

    def __hash__(self) -> int: ...

class Slot:
    def __init__(self, expr: str) -> None:
        """
        Constructs a new Slot object from the input expression.

        :raises ValueError: The expression is invalid.
        """

    @property
    def slot(self) -> str: ...

    @property
    def subslot(self) -> str: ...

    def __str__(self) -> str: ...

    def __repr__(self) -> str: ...

    def __eq__(self, arg: Slot, /) -> bool: ...

    def __ne__(self, arg: Slot, /) -> bool: ...

    def __hash__(self) -> int: ...

class SlotExpr:
    def __init__(self, expr: str) -> None:
        """
        Constructs a new SlotExpr object from the input expression.

        :raises ValueError: The expression is invalid.
        """

    @property
    def slotVariant(self) -> SlotVariant: ...

    @property
    def slot(self) -> Slot | None: ...

    def __str__(self) -> str: ...

    def __repr__(self) -> str: ...

    def __eq__(self, arg: SlotExpr, /) -> bool: ...

    def __ne__(self, arg: SlotExpr, /) -> bool: ...

    def __hash__(self) -> int: ...

class SlotVariant(enum.Enum):
    """
    Constructs a new SlotVariant object from the input expression.

    :raises ValueError: The expression is invalid.
    """

    none = 0

    star = 1

    equal = 2

class Usedep:
    def __init__(self, expr: str) -> None:
        """
        Constructs a new Usedep object from the input expression.

        :raises ValueError: The expression is invalid.
        """

    @property
    def negate(self) -> UsedepNegate | None: ...

    @property
    def useflag(self) -> Useflag: ...

    @property
    def sign(self) -> UsedepSign | None: ...

    @property
    def conditional(self) -> UsedepCond | None: ...

    def __str__(self) -> str: ...

    def __repr__(self) -> str: ...

    def __eq__(self, arg: Usedep, /) -> bool: ...

    def __ne__(self, arg: Usedep, /) -> bool: ...

    def __hash__(self) -> int: ...

class UsedepCond(enum.Enum):
    """
    Constructs a new UsedepCond object from the input expression.

    :raises ValueError: The expression is invalid.
    """

    def __str__(self) -> str: ...

    eqal = 0

    question = 1

class UsedepNegate(enum.Enum):
    """
    Constructs a new UsedepNegate object from the input expression.

    :raises ValueError: The expression is invalid.
    """

    def __str__(self) -> str: ...

    minus = 0

    exclamation = 1

class UsedepSign(enum.Enum):
    """
    Constructs a new UsedepSign object from the input expression.

    :raises ValueError: The expression is invalid.
    """

    def __str__(self) -> str: ...

    plus = 0

    minus = 1

class Usedeps:
    def __init__(self, expr: str) -> None:
        """
        Constructs a new Usedeps object from the input expression.

        :raises ValueError: The expression is invalid.
        """

    def __str__(self) -> str: ...

    def __repr__(self) -> str: ...

    def __iter__(self) -> Iterator[Usedep]: ...

    def __eq__(self, arg: Usedeps, /) -> bool: ...

    def __ne__(self, arg: Usedeps, /) -> bool: ...

    def __hash__(self) -> int: ...

class Useflag:
    def __init__(self, expr: str) -> None:
        """
        Constructs a new Useflag object from the input expression.

        :raises ValueError: The expression is invalid.
        """

    def __str__(self) -> str: ...

    def __repr__(self) -> str: ...

    def __eq__(self, arg: Useflag, /) -> bool: ...

    def __ne__(self, arg: Useflag, /) -> bool: ...

    def __hash__(self) -> int: ...

class Version:
    def __init__(self, expr: str) -> None:
        """
        Constructs a new Version object from the input expression.

        :raises ValueError: The expression is invalid.
        """

    @property
    def numbers(self) -> VersionNumber: ...

    @property
    def letter(self) -> str | None: ...

    @property
    def suffixes(self) -> list[VersionSuffix]: ...

    @property
    def revision(self) -> VersionRevision | None: ...

    def __str__(self) -> str: ...

    def __repr__(self) -> str: ...

    def __eq__(self, arg: Version, /) -> bool: ...

    def __ne__(self, arg: Version, /) -> bool: ...

    def __hash__(self) -> int: ...

    def __lt__(self, arg: Version, /) -> bool: ...

    def __le__(self, arg: Version, /) -> bool: ...

    def __gt__(self, arg: Version, /) -> bool: ...

    def __ge__(self, arg: Version, /) -> bool: ...

class VersionNumber:
    def __str__(self) -> str: ...

    def __iter__(self) -> Iterator[str]: ...

    def __eq__(self, arg: VersionNumber, /) -> bool: ...

    def __ne__(self, arg: VersionNumber, /) -> bool: ...

    def __hash__(self) -> int: ...

class VersionRevision:
    def __str__(self) -> str: ...

    def __eq__(self, arg: VersionRevision, /) -> bool: ...

    def __ne__(self, arg: VersionRevision, /) -> bool: ...

    def __hash__(self) -> int: ...

class VersionSpecifier(enum.Enum):
    """
    Constructs a new VersionSpecifier object from the input expression.

    :raises ValueError: The expression is invalid.
    """

    def __str__(self) -> str: ...

    def __repr__(self) -> str: ...

    lt = 0

    le = 1

    eq = 2

    ea = 3

    td = 4

    ge = 5

    gt = 6

class VersionSuffix:
    def __init__(self, expr: str) -> None:
        """
        Constructs a new VersionSuffix object from the input expression.

        :raises ValueError: The expression is invalid.
        """

    @property
    def word(self) -> VersionSuffixWord: ...

    @property
    def number(self) -> str: ...

    def __str__(self) -> str: ...

    def __repr__(self) -> str: ...

    def __eq__(self, arg: VersionSuffix, /) -> bool: ...

    def __ne__(self, arg: VersionSuffix, /) -> bool: ...

    def __hash__(self) -> int: ...

class VersionSuffixWord(enum.IntEnum):
    """
    Constructs a new VersionSuffixWord object from the input expression.

    :raises ValueError: The expression is invalid.
    """

    def __str__(self) -> str: ...

    def __repr__(self) -> str: ...

    alpha = 0

    beta = 1

    pre = 2

    rc = 3

    p = 4
