Parsing an Atom expression
==========================

.. doctest::

    >>> atom = pms_utils.atom.Atom("!>=foo/bar-1:0[baz,qux]")
    >>> members = [attr for attr in dir(atom) if not callable(getattr(atom, attr)) and not attr.startswith("_")]
    >>> for m in members:
    ...     print(m + ": " + eval(f"atom.{m}").__repr__())
    blocker: pms_utils.atom.Blocker('!')
    category: pms_utils.atom.Category('foo')
    name: pms_utils.atom.Name('bar')
    slotExpr: pms_utils.atom.SlotExpr(':0')
    usedeps: pms_utils.atom.Usedeps('[baz,qux]')
    version: pms_utils.atom.Version('1')
    verspec: pms_utils.atom.VersionSpecifier('>=')
