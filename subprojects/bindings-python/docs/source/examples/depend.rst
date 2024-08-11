
Iterating over a DEPEND expression
==================================

There are two ways of iterating over a dependency-style expression:

* A shallow iteration over all top-level nodes via the ``DependExpr.nodes`` member.
* A deep iteration over all nodes via the ``DependExpr`` object itself.

.. doctest::

    >>> expr = pms_utils.depend.DependExpr('''
    ...     foo/bar1
    ...     use1? (
    ...         foo/bar2
    ...     )
    ...     || (
    ...         foo/bar3
    ...         foo/bar4
    ...     )
    ...     (
    ...         foo/bar5
    ...     )
    ...     ''')
    >>> for elem in expr.nodes:
    ...     if isinstance(elem, pms_utils.atom.Atom):
    ...         print(f"top-level atom {elem}")
    ...     else:
    ...         print(f"sub-expression of size {len(elem.nodes)} with conditional {elem.conditional}")
    top-level atom foo/bar1
    sub-expression of size 1 with conditional use1?
    sub-expression of size 2 with conditional ||
    sub-expression of size 1 with conditional None
    >>> for elem in expr:
    ...     print(type(elem))
    <class 'pms_utils.atom.Atom'>
    <class 'pms_utils.depend.DependExpr'>
    <class 'pms_utils.atom.Atom'>
    <class 'pms_utils.depend.DependExpr'>
    <class 'pms_utils.atom.Atom'>
    <class 'pms_utils.atom.Atom'>
    <class 'pms_utils.depend.DependExpr'>
    <class 'pms_utils.atom.Atom'>
