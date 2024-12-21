from collections.abc import Iterator
import os
import pathlib
from typing import overload

import pms_utils


class Category:
    @property
    def path(self) -> pathlib.Path: ...

    @property
    def name(self) -> pms_utils.atom.Category: ...

    def __iter__(self) -> Iterator[Package]: ...

    def __eq__(self, arg: Category, /) -> bool: ...

    def __ne__(self, arg: Category, /) -> bool: ...

    def __hash__(self) -> int: ...

    def __getitem__(self, arg: str, /) -> Package | None: ...

class Ebuild:
    @property
    def path(self) -> pathlib.Path: ...

    @property
    def name(self) -> pms_utils.atom.Name: ...

    @property
    def version(self) -> pms_utils.atom.Version: ...

    def __eq__(self, arg: Ebuild, /) -> bool: ...

    def __ne__(self, arg: Ebuild, /) -> bool: ...

    def __hash__(self) -> int: ...

    @property
    def metadata(self) -> pms_utils.ebuild.Metadata: ...

class Package:
    @property
    def path(self) -> pathlib.Path: ...

    @property
    def name(self) -> pms_utils.atom.Name: ...

    def __iter__(self) -> Iterator[Ebuild]: ...

    def __eq__(self, arg: Package, /) -> bool: ...

    def __ne__(self, arg: Package, /) -> bool: ...

    def __hash__(self) -> int: ...

    @overload
    def __getitem__(self, arg: pms_utils.atom.Version, /) -> Ebuild | None: ...

    @overload
    def __getitem__(self, arg: str, /) -> Ebuild | None: ...

class Repository:
    def __init__(self, arg: str | os.PathLike, /) -> None: ...

    @property
    def path(self) -> pathlib.Path: ...

    @property
    def name(self) -> str: ...

    def __iter__(self) -> Iterator[Category]: ...

    def __eq__(self, arg: Repository, /) -> bool: ...

    def __ne__(self, arg: Repository, /) -> bool: ...

    def __hash__(self) -> int: ...

    def __getitem__(self, arg: str, /) -> Category | None: ...
