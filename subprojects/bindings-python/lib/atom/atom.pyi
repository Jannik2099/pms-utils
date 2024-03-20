from enum import Enum, IntEnum
from typing import Optional

class VersionNumber(list[str]):
    pass

VersionLetter = str

class VersionRevision(str):
    pass

class Useflag(str):
    pass

class Category(str):
    pass

class Name(str):
    pass

class VersionSpecifier(Enum):
    lt = 0
    le = 1
    eq = 2
    ea = 3
    td = 4
    ge = 5
    gt = 6

    def __init__(self, string: str) -> None: ...
    def __repr__(self) -> str: ...

class Blocker(Enum):
    weak = 0
    strong = 1

    def __init__(self, string: str) -> None: ...
    def __repr__(self) -> str: ...

class VersionSuffixWord(IntEnum):
    alpha = 0
    beta = 1
    pre = 2
    rc = 3
    p = 4

    def __lt__(self, __value: object) -> bool: ...
    def __le__(self, __value: object) -> bool: ...
    def __eq__(self, __value: object) -> bool: ...
    def __ne__(self, __value: object) -> bool: ...
    def __ge__(self, __value: object) -> bool: ...
    def __gt__(self, __value: object) -> bool: ...
    def __init__(self, string: str) -> None: ...
    def __repr__(self) -> str: ...

class VersionSuffix:
    word: VersionSuffixWord
    number: str

    def __init__(self, string: str) -> None: ...
    def __repr__(self) -> str: ...

class Version:
    numbers: VersionNumber
    letter: Optional[VersionLetter]
    suffixes: list[VersionSuffix]
    revision: Optional[VersionRevision]

    def __lt__(self, __value: object) -> bool: ...
    def __le__(self, __value: object) -> bool: ...
    def __eq__(self, __value: object) -> bool: ...
    def __ne__(self, __value: object) -> bool: ...
    def __ge__(self, __value: object) -> bool: ...
    def __gt__(self, __value: object) -> bool: ...
    def __init__(self, string: str) -> None: ...
    def __repr__(self) -> str: ...

class Slot:
    slot: str
    subslot: str

    def __init__(self, string: str) -> None: ...
    def __repr__(self) -> str: ...

class SlotVariant(Enum):
    none = 0
    star = 1
    equal = 2

class SlotExpr:
    slot_variant: SlotVariant
    slot: Optional[Slot]

    def __init__(self, string: str) -> None: ...
    def __repr__(self) -> str: ...

class UsedepNegate(Enum):
    minus = 0
    plus = 1

    def __repr__(self) -> str: ...

class UsedepSign(Enum):
    plus = 0
    minus = 1

    def __repr__(self) -> str: ...

class UsedepCond(Enum):
    equal = 0
    question = 1

    def __repr__(self) -> str: ...

class Usedep:
    negate: Optional[UsedepNegate]
    useflag: Useflag
    sign: Optional[UsedepSign]
    conditional: Optional[UsedepCond]

    def __init__(self, string: str) -> None: ...
    def __repr__(self) -> str: ...

Usedeps = list[Usedep]

class PackageExpr:
    blocker: Optional[Blocker]
    category: Category
    name: Name
    version_specifier: Optional[VersionSpecifier]
    version: Optional[Version]
    slot_expr: Optional[SlotExpr]
    usedeps: Usedeps

    def __init__(self, string: str) -> None: ...
    def __repr__(self) -> str: ...
