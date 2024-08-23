import os
import pms_utils.atom
import pms_utils.repo

class Filters:
    def __init__(self, *args, **kwargs) -> None:
        """Initialize self.  See help(type(self)) for accurate signature."""

    def __eq__(self, arg0: Filters) -> bool:
        """__eq__(self: pms_utils.profile.Filters, arg0: pms_utils.profile.Filters) -> bool"""

    def __hash__(self) -> int:
        """__hash__(self: pms_utils.profile.Filters) -> int"""

    def __ne__(self, arg0: Filters) -> bool:
        """__ne__(self: pms_utils.profile.Filters, arg0: pms_utils.profile.Filters) -> bool"""

    @property
    def masked(self) -> bool: ...
    @property
    def use(self) -> set[str]: ...
    @property
    def use_force(self) -> set[str]: ...
    @property
    def use_mask(self) -> set[str]: ...
    @property
    def use_stable_force(self) -> set[str]: ...
    @property
    def use_stable_mask(self) -> set[str]: ...

class PortageProfile(Profile):
    def __init__(self, arg0: os.PathLike) -> None:
        """__init__(self: pms_utils.profile.PortageProfile, arg0: os.PathLike) -> None"""

    def __eq__(self, arg0: PortageProfile) -> bool:
        """__eq__(self: pms_utils.profile.PortageProfile, arg0: pms_utils.profile.PortageProfile) -> bool"""

    def __hash__(self) -> int:
        """__hash__(self: pms_utils.profile.PortageProfile) -> int"""

    def __ne__(self, arg0: PortageProfile) -> bool:
        """__ne__(self: pms_utils.profile.PortageProfile, arg0: pms_utils.profile.PortageProfile) -> bool"""

class Profile:
    def __init__(self, arg0: os.PathLike) -> None:
        """__init__(self: pms_utils.profile.Profile, arg0: os.PathLike) -> None"""

    def effective_useflags(
        self, arg0: pms_utils.atom.Atom
    ) -> set[pms_utils.atom.Useflag]:
        """effective_useflags(self: pms_utils.profile.Profile, arg0: pms_utils.atom.Atom) -> set[pms_utils.atom.Useflag]"""

    def __eq__(self, arg0: Profile) -> bool:
        """__eq__(self: pms_utils.profile.Profile, arg0: pms_utils.profile.Profile) -> bool"""

    def __hash__(self) -> int:
        """__hash__(self: pms_utils.profile.Profile) -> int"""

    def __ne__(self, arg0: Profile) -> bool:
        """__ne__(self: pms_utils.profile.Profile, arg0: pms_utils.profile.Profile) -> bool"""

    @property
    def ARCH(self) -> str: ...
    @property
    def CONFIG_PROTECT(self) -> set[os.PathLike]: ...
    @property
    def CONFIG_PROTECT_MASK(self) -> set[os.PathLike]: ...
    @property
    def EAPI(self) -> str: ...
    @property
    def ENV_UNSET(self) -> set[str]: ...
    @property
    def IUSE_IMPLICIT(self) -> set[pms_utils.atom.Useflag]: ...
    @property
    def USE(self) -> set[pms_utils.atom.Useflag]: ...
    @property
    def USE_EXPAND(self) -> set[str]: ...
    @property
    def USE_EXPAND_HIDDEN(self) -> set[str]: ...
    @property
    def USE_EXPAND_IMPLICIT(self) -> set[str]: ...
    @property
    def USE_EXPAND_UNPREFIXED(self) -> set[str]: ...
    @property
    def deprecated(self) -> bool: ...
    @property
    def filters(self) -> dict[str, Filters]: ...
    @property
    def make_defaults(self) -> dict[str, str]: ...
    @property
    def make_defaults_unevaluated(self) -> list[tuple[str, str]]: ...
    @property
    def name(self) -> str: ...
    @property
    def packages(self) -> set[pms_utils.atom.Atom]: ...
    @property
    def parents(self) -> list[Profile]: ...
    @property
    def path(self) -> os.PathLike: ...
    @property
    def use_force(self) -> set[str]: ...
    @property
    def use_mask(self) -> set[str]: ...
    @property
    def use_stable_force(self) -> set[str]: ...
    @property
    def use_stable_mask(self) -> set[str]: ...

def expand_package_expr(
    arg0: str, arg1: list[pms_utils.repo.Repository]
) -> list[tuple[pms_utils.atom.Atom, int]]:
    """expand_package_expr(arg0: str, arg1: list[pms_utils.repo.Repository]) -> list[tuple[pms_utils.atom.Atom, int]]"""
