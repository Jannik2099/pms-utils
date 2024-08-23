expand_package_expr
===================
.. currentmodule:: pms_utils.profile

This utility function expands a wildcard, slot or not-exactly-versioned package expression into a list of all matching packages, of the form `[pms_utils.atom.Atom(=category/name-version), i]` such that `repos[i]` is the corresponding repository.

.. autofunction:: expand_package_expr
