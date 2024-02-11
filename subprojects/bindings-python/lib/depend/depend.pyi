from enum import Enum
from typing import Optional, Union
from collections.abc import Iterable

from pms_utils import atom

class UseConditional:
    negate: bool
    useflag: atom.Useflag

    def __init__(self, string: str) -> None: ...
    def __repr__(self) -> str: ...

class GroupHeaderOp(Enum):
    any_of = 0
    at_most_one_of = 1
    exactly_one_of = 2

    def __repr__(self) -> str: ...

GroupHeader = Union[UseConditional, GroupHeaderOp]

class DependExpr(Iterable[DependExpr]):
    Node = Union[atom.PackageExpr, DependExpr]
    conditional: Optional[GroupHeader]
    nodes: list[Node]

    def __init__(self, string: str) -> None: ...
    def __repr__(self) -> str: ...
