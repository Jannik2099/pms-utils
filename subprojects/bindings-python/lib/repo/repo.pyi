from enum import Enum
from typing import Optional
from collections.abc import Iterable

from pms_utils import atom, depend, ebuild

class Metadata:
    DEPEND: depend.DependExpr
    RDEPEND: depend.DependExpr
    SLOT: atom.Slot
    SRC_URI: ebuild.SRC_URI
    RESTRICT: ebuild.RESTRICT
    HOMEPAGE: ebuild.HOMEPAGE
    LICENSE: ebuild.LICENSE
    DESCRIPTION: str
    KEYWORDS: ebuild.KEYWORDS
    INHERITED: ebuild.INHERITED
    IUSE: ebuild.IUSE
    REQUIRED_USE: ebuild.REQUIRED_USE
    PDEPEND: depend.DependExpr
    BDEPEND: depend.DependExpr
    EAPI: ebuild.EAPI
    PROPERTIES: ebuild.PROPERTIES
    DEFINED_PHASES: ebuild.DEFINED_PHASES
    IDEPEND: depend.DependExpr

class Ebuild:
    path: str
    name: atom.Name
    version: atom.Version
    metadata: Metadata

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
