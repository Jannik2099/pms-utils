# PMS-Utils

WARNING: this is merely a proof of concept, don't rely on it for anything.

PMS-Utils is a collection of utilities and algorithms to implement Gentoo's [Package Manager Specification](https://wiki.gentoo.org/wiki/Package_Manager_Specification).

## Features

PMS-Utils can be used to parse DEPEND expressions and any Atom expression.  
As an extension, "weak" Atom expressions are supported when creating an Atom directly - i.e. `foo/bar-3`, which is an invalid DEPEND expression, but more convenient to type than `=foo/bar-3`.  
Weak Atom expressions are not enabled when parsing a full DEPEND expression.

PMS-Utils also implements the version sorting algorithm for Atoms.

## Python bindings

All returned objects are fully typed, no string soup.  
Objects are currently immutable and can only be created via atom.Atom() and depend.DependExpr()

### Atom

```python
from pms_utils.atom import Atom
Atom("foo/bar-1").version < Atom("foo/bar-2").version
True

atom = Atom("!>=foo/bar-1:0[baz,qux]")
members = [attr for attr in dir(atom) if not callable(getattr(atom, attr)) and not attr.startswith("__")]
for m in members:
    print(m + ": " + eval(f"atom.{m}").__repr__())
blocker: !
category: 'foo'
name: 'bar'
slot_expr: :0
usedeps: [baz, qux]
version: 1
version_specifier: >=

for m in members:
    print(f"type({m}): " + str(type(eval(f"atom.{m}"))))
type(blocker): <class 'pms_utils.atom.Blocker'>
type(category): <class 'str'>
type(name): <class 'str'>
type(slot_expr): <class 'pms_utils.atom.SlotExpr'>
type(usedeps): <class 'list'>
type(version): <class 'pms_utils.atom.Version'>
type(version_specifier): <class 'pms_utils.atom.VersionSpecifier'>
```

### DependExpr
```python
from pms_utils.depend import DependExpr
expr = DependExpr("foo/bar foo? ( foo/baz ) || ( foo/baz foo/qux )")

for e in expr:
    print(type(e))
<class 'pms_utils.atom.Atom'>
<class 'pms_utils.depend.DependExpr'>
<class 'pms_utils.atom.Atom'>
<class 'pms_utils.depend.DependExpr'>
<class 'pms_utils.atom.Atom'>
<class 'pms_utils.atom.Atom'>

expr.nodes
[foo/bar, foo? foo/baz, || foo/baz
foo/qux] # nested formatting not quite fleshed out

expr.nodes[2].conditional
||
expr.nodes[2].nodes
[foo/baz, foo/qux]
```
