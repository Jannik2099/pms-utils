Atom
====
.. currentmodule:: pms_utils.atom

| The Atom class represents a PMS Atom / package expression. It can be constructed from any valid Atom string, be it a simple ``foo/bar`` or a more restricted ``!>=foo/bar-1:0[baz]``.
| Unless explicitly stated otherwise, this class is used as the universal Atom representation in ``pms_utils``, for example in ebuilds, metadata values, or configuration files.
| Locations where only a subset of the Atom syntax is valid will accordingly only return Atom objects with the non-allowed members being empty or ``None``, and will only accept Atom objects with said members set accordingly unless explicitly stated otherwise.

.. autoclass:: Atom
