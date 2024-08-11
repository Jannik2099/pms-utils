Implementing equery hasuse
==========================

The command ``equery hasuse <useflag>`` from ``app-portage/gentoolkit`` finds all packages that contain the given useflag.

In pms-utils, this would look like:

.. doctest::

    >>> def hasuse(useflag: pms_utils.atom.Useflag) -> list[pms_utils.repo.Package]:
    ...     ret = []
    ...     repo = pms_utils.repo.Repository("/var/db/repos/gentoo")
    ...     def check_package(package: pms_utils.repo.Package) -> bool:
    ...         for ebuild in package:
    ...             for iuse_elem in ebuild.metadata.IUSE:
    ...                 if iuse_elem.useflag == useflag:
    ...                     return True
    ...         return False
    ...     for category in repo:
    ...         for package in category:
    ...             if check_package(package):
    ...                 ret.append(package)
    ...     return ret
