from collections.abc import Sequence
import os
import pathlib

import pms_utils


class Filters:
    @property
    def masked(self) -> bool: ...

    @property
    def use(self) -> set[str]: ...

    @property
    def use_mask(self) -> set[str]: ...

    @property
    def use_force(self) -> set[str]: ...

    @property
    def use_stable_mask(self) -> set[str]: ...

    @property
    def use_stable_force(self) -> set[str]: ...

    def __eq__(self, arg: Filters, /) -> bool: ...

    def __ne__(self, arg: Filters, /) -> bool: ...

    def __hash__(self) -> int: ...

class PortageProfile(Profile):
    def __init__(self, arg: str | os.PathLike, /) -> None: ...

    def __eq__(self, arg: PortageProfile, /) -> bool: ...

    def __ne__(self, arg: PortageProfile, /) -> bool: ...

    def __hash__(self) -> int: ...

class Profile:
    def __init__(self, arg: str | os.PathLike, /) -> None: ...

    @property
    def path(self) -> pathlib.Path: ...

    @property
    def name(self) -> str: ...

    @property
    def parents(self) -> list[Profile]: ...

    @property
    def EAPI(self) -> str: ...

    @property
    def deprecated(self) -> bool: ...

    @property
    def make_defaults(self) -> dict[str, str]: ...

    @property
    def make_defaults_unevaluated(self) -> list[tuple[str, str]]: ...

    @property
    def packages(self) -> set[pms_utils.atom.Atom]: ...

    @property
    def use_mask(self) -> set[str]: ...

    @property
    def use_force(self) -> set[str]: ...

    @property
    def use_stable_mask(self) -> set[str]: ...

    @property
    def use_stable_force(self) -> set[str]: ...

    @property
    def USE(self) -> set[pms_utils.atom.Useflag]: ...

    @property
    def USE_EXPAND(self) -> set[str]: ...

    @property
    def USE_EXPAND_HIDDEN(self) -> set[str]: ...

    @property
    def CONFIG_PROTECT(self) -> set[pathlib.Path]: ...

    @property
    def CONFIG_PROTECT_MASK(self) -> set[pathlib.Path]: ...

    @property
    def IUSE_IMPLICIT(self) -> set[pms_utils.atom.Useflag]: ...

    @property
    def USE_EXPAND_IMPLICIT(self) -> set[str]: ...

    @property
    def USE_EXPAND_UNPREFIXED(self) -> set[str]: ...

    @property
    def ENV_UNSET(self) -> set[str]: ...

    @property
    def ARCH(self) -> str: ...

    @property
    def filters(self) -> dict[str, Filters]: ...

    def __eq__(self, arg: Profile, /) -> bool: ...

    def __ne__(self, arg: Profile, /) -> bool: ...

    def __hash__(self) -> int: ...

    def effective_useflags(self, arg: pms_utils.atom.Atom, /) -> set[pms_utils.atom.Useflag]: ...

def expand_package_expr(arg0: str, arg1: Sequence[pms_utils.repo.Repository], /) -> list[tuple[pms_utils.atom.Atom, int]]: ...