from collections.abc import Iterator
import enum

import pms_utils


class DependExpr:
    def __init__(self, expr: str) -> None:
        """
        Constructs a new DependExpr object from the input expression.

        :raises ValueError: The expression is invalid.
        """

    @property
    def conditional(self) -> UseConditional | GroupHeaderOp | None: ...

    @property
    def nodes(self) -> list[pms_utils.atom.Atom | DependExpr]: ...

    def __str__(self) -> str: ...

    def __repr__(self) -> str: ...

    def __iter__(self) -> Iterator[pms_utils.atom.Atom | DependExpr]: ...

    def __eq__(self, arg: DependExpr, /) -> bool: ...

    def __ne__(self, arg: DependExpr, /) -> bool: ...

    def __hash__(self) -> int: ...

class GroupHeaderOp(enum.Enum):
    """
    Constructs a new GroupHeaderOp object from the input expression.

    :raises ValueError: The expression is invalid.
    """

    def __str__(self) -> str: ...

    any_of = 0

    exactly_one_of = 1

    at_most_one_of = 2

class UseConditional:
    def __init__(self, expr: str) -> None:
        """
        Constructs a new UseConditional object from the input expression.

        :raises ValueError: The expression is invalid.
        """

    @property
    def negate(self) -> bool: ...

    @property
    def useflag(self) -> pms_utils.atom.Useflag: ...

    def __str__(self) -> str: ...

    def __repr__(self) -> str: ...

    def __eq__(self, arg: UseConditional, /) -> bool: ...

    def __ne__(self, arg: UseConditional, /) -> bool: ...

    def __hash__(self) -> int: ...
