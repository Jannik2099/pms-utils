atom
====
.. module:: pms_utils.atom

| This module provides a strongly typed representation of the various components of a PMS Atom.
| All types may only be constructed from valid expression strings and are immutable.
| The primary class is :py:class:`Atom` which represents a full Atom / package expression, including versioning constraints, blockers, useflags, use dependencies and constraints, and slots and subslots.

.. toctree::
    :maxdepth: 1

    atom/VersionSpecifier
    atom/VersionNumber
    atom/VersionSuffixWord
    atom/VersionSuffix
    atom/VersionRevision
    atom/Version
    atom/Blocker
    atom/Slot
    atom/SlotVariant
    atom/SlotExpr
    atom/Category
    atom/Name
    atom/Useflag
    atom/UsedepNegate
    atom/UsedepSign
    atom/UsedepCond
    atom/Usedep
    atom/Usedeps
    atom/Atom
