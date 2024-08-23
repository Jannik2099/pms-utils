import os
import pms_utils.atom
import pms_utils.ebuild
from typing import Iterator, overload

class Category:
    def __init__(self, *args, **kwargs) -> None:
        """Initialize self.  See help(type(self)) for accurate signature."""
    def __eq__(self, arg0: Category) -> bool:
        """__eq__(self: pms_utils.repo.Category, arg0: pms_utils.repo.Category) -> bool"""
    def __getitem__(self, arg0: str) -> Package | None:
        """__getitem__(self: pms_utils.repo.Category, arg0: str) -> Optional[pms_utils.repo.Package]"""
    def __hash__(self) -> int:
        """__hash__(self: pms_utils.repo.Category) -> int"""
    def __iter__(self) -> Iterator[Package]:
        """__iter__(self: pms_utils.repo.Category) -> Iterator[pms_utils.repo.Package]"""
    def __ne__(self, arg0: Category) -> bool:
        """__ne__(self: pms_utils.repo.Category, arg0: pms_utils.repo.Category) -> bool"""
    @property
    def name(self) -> pms_utils.atom.Category: ...
    @property
    def path(self) -> os.PathLike: ...

class Ebuild:
    def __init__(self, *args, **kwargs) -> None:
        """Initialize self.  See help(type(self)) for accurate signature."""
    def __eq__(self, arg0: Ebuild) -> bool:
        """__eq__(self: pms_utils.repo.Ebuild, arg0: pms_utils.repo.Ebuild) -> bool"""
    def __hash__(self) -> int:
        """__hash__(self: pms_utils.repo.Ebuild) -> int"""
    def __ne__(self, arg0: Ebuild) -> bool:
        """__ne__(self: pms_utils.repo.Ebuild, arg0: pms_utils.repo.Ebuild) -> bool"""
    @property
    def metadata(self) -> pms_utils.ebuild.Metadata: ...
    @property
    def name(self) -> pms_utils.atom.Name: ...
    @property
    def path(self) -> os.PathLike: ...
    @property
    def version(self) -> pms_utils.atom.Version: ...

class Package:
    def __init__(self, *args, **kwargs) -> None:
        """Initialize self.  See help(type(self)) for accurate signature."""
    def __eq__(self, arg0: Package) -> bool:
        """__eq__(self: pms_utils.repo.Package, arg0: pms_utils.repo.Package) -> bool"""
    @overload
    def __getitem__(self, arg0: pms_utils.atom.Version) -> Ebuild | None:
        """__getitem__(*args, **kwargs)
        Overloaded function.

        1. __getitem__(self: pms_utils.repo.Package, arg0: pms_utils.atom.Version) -> Optional[pms_utils.repo.Ebuild]

        2. __getitem__(self: pms_utils.repo.Package, arg0: str) -> Optional[pms_utils.repo.Ebuild]
        """
    @overload
    def __getitem__(self, arg0: str) -> Ebuild | None:
        """__getitem__(*args, **kwargs)
        Overloaded function.

        1. __getitem__(self: pms_utils.repo.Package, arg0: pms_utils.atom.Version) -> Optional[pms_utils.repo.Ebuild]

        2. __getitem__(self: pms_utils.repo.Package, arg0: str) -> Optional[pms_utils.repo.Ebuild]
        """
    def __hash__(self) -> int:
        """__hash__(self: pms_utils.repo.Package) -> int"""
    def __iter__(self) -> Iterator[Ebuild]:
        """__iter__(self: pms_utils.repo.Package) -> Iterator[pms_utils.repo.Ebuild]"""
    def __ne__(self, arg0: Package) -> bool:
        """__ne__(self: pms_utils.repo.Package, arg0: pms_utils.repo.Package) -> bool"""
    @property
    def name(self) -> pms_utils.atom.Name: ...
    @property
    def path(self) -> os.PathLike: ...

class Repository:
    def __init__(self, arg0: os.PathLike) -> None:
        """__init__(self: pms_utils.repo.Repository, arg0: os.PathLike) -> None"""
    def __eq__(self, arg0: Repository) -> bool:
        """__eq__(self: pms_utils.repo.Repository, arg0: pms_utils.repo.Repository) -> bool"""
    def __getitem__(self, arg0: str) -> Category | None:
        """__getitem__(self: pms_utils.repo.Repository, arg0: str) -> Optional[pms_utils.repo.Category]"""
    def __hash__(self) -> int:
        """__hash__(self: pms_utils.repo.Repository) -> int"""
    def __iter__(self) -> Iterator[Category]:
        """__iter__(self: pms_utils.repo.Repository) -> Iterator[pms_utils.repo.Category]"""
    def __ne__(self, arg0: Repository) -> bool:
        """__ne__(self: pms_utils.repo.Repository, arg0: pms_utils.repo.Repository) -> bool"""
    @property
    def name(self) -> str: ...
    @property
    def path(self) -> os.PathLike: ...
