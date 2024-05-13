import os
import pms_utils.atom
import pms_utils.ebuild
from typing import Iterator, overload

class Category:
    def __init__(self, *args, **kwargs) -> None: ...
    def __getitem__(self, arg0: str) -> Package | None: ...
    def __iter__(self) -> Iterator[Package]: ...

class Ebuild:
    def __init__(self, *args, **kwargs) -> None: ...
    @property
    def metadata(self) -> pms_utils.ebuild.Metadata: ...
    @property
    def name(self) -> pms_utils.atom.Name: ...
    @property
    def path(self) -> os.PathLike: ...
    @property
    def version(self) -> pms_utils.atom.Version: ...

class Package:
    def __init__(self, *args, **kwargs) -> None: ...
    @overload
    def __getitem__(self, arg0: pms_utils.atom.Version) -> Ebuild | None: ...
    @overload
    def __getitem__(self, arg0: str) -> Ebuild | None: ...
    def __iter__(self) -> Iterator[Ebuild]: ...

class Repository:
    def __init__(self, arg0: os.PathLike) -> None: ...
    def __getitem__(self, arg0: str) -> Category | None: ...
    def __iter__(self) -> Iterator[Category]: ...