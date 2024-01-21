from enum import Enum
from typing import Optional
from collections.abc import Iterable

from pms_utils import atom

class Ebuild:
    path: str
    name: atom.Name
    version: atom.Version

class Package(Iterable[Ebuild]):
    path: str
    name: atom.Name

class Category(Iterable[Package]):
    path: str
    name: atom.Category

class Repository(Iterable[Category]):
    path: str
    name: str

    def __init__(self, path: str, name: str) -> None: ...
