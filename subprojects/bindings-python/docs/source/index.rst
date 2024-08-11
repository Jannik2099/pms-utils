pms-utils
=========

| This library aims to implement various types, structures and algorithms described in the Gentoo `Package Manager Specification <https://wiki.gentoo.org/wiki/Package_Manager_Specification>`_.
| pms-utils is implemented in C++ and has the primary focus on correctness. To this extent, almost all expressions described in PMS are parsed via BNF-style PEG instead of handwritten parsers.
| Most of the API is exposed as a Python library, which this page documents.

.. toctree::
    :hidden:

    genindex

.. toctree::
    :hidden:
    :maxdepth: 1
    :caption: Modules

    atom
    depend
    ebuild
    profile
    repo


.. toctree::
    :hidden:
    :maxdepth: 1
    :caption: Examples

    examples/atom
    examples/depend
    examples/repo
    examples/equery
