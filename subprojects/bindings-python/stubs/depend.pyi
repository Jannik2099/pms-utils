import enum
import pms_utils.atom
from typing import Callable, ClassVar, Iterator

class DependExpr:
    def __init__(self, expr: str) -> None:
        """__init__(self: pms_utils.depend.DependExpr, expr: str) -> None


                    Constructs a new DependExpr object from the input expression.

                    :raises ValueError: The expression is invalid.

        """
    def __eq__(self, arg0: DependExpr) -> bool:
        """__eq__(self: pms_utils.depend.DependExpr, arg0: pms_utils.depend.DependExpr) -> bool"""
    def __hash__(self) -> int:
        """__hash__(self: pms_utils.depend.DependExpr) -> int"""
    def __iter__(self) -> Iterator[pms_utils.atom.Atom | DependExpr]:
        """__iter__(self: pms_utils.depend.DependExpr) -> Iterator[Union[pms_utils.atom.Atom, pms_utils.depend.DependExpr]]"""
    def __ne__(self, arg0: DependExpr) -> bool:
        """__ne__(self: pms_utils.depend.DependExpr, arg0: pms_utils.depend.DependExpr) -> bool"""
    @property
    def conditional(self) -> UseConditional | GroupHeaderOp | None: ...
    @property
    def nodes(self) -> list[pms_utils.atom.Atom | DependExpr]: ...

class GroupHeaderOp(enum.Enum):
    __new__: ClassVar[Callable] = ...
    _generate_next_value_: ClassVar[Callable] = ...
    _member_map_: ClassVar[dict] = ...
    _member_names_: ClassVar[list] = ...
    _member_type_: ClassVar[type[object]] = ...
    _unhashable_values_: ClassVar[list] = ...
    _use_args_: ClassVar[bool] = ...
    _value2member_map_: ClassVar[dict] = ...
    _value_repr_: ClassVar[None] = ...
    any_of: ClassVar[GroupHeaderOp] = ...
    at_most_one_of: ClassVar[GroupHeaderOp] = ...
    exactly_one_of: ClassVar[GroupHeaderOp] = ...

class UseConditional:
    def __init__(self, expr: str) -> None:
        """__init__(self: pms_utils.depend.UseConditional, expr: str) -> None


                    Constructs a new UseConditional object from the input expression.

                    :raises ValueError: The expression is invalid.

        """
    def __eq__(self, arg0: UseConditional) -> bool:
        """__eq__(self: pms_utils.depend.UseConditional, arg0: pms_utils.depend.UseConditional) -> bool"""
    def __hash__(self) -> int:
        """__hash__(self: pms_utils.depend.UseConditional) -> int"""
    def __ne__(self, arg0: UseConditional) -> bool:
        """__ne__(self: pms_utils.depend.UseConditional, arg0: pms_utils.depend.UseConditional) -> bool"""
    @property
    def negate(self) -> bool: ...
    @property
    def useflag(self) -> pms_utils.atom.Useflag: ...
