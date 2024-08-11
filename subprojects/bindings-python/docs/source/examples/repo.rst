Iterating over and accessing an ebuild repository
=================================================

Repository objects can be iterated over idiomatically:

.. doctest::

    >>> repo = pms_utils.repo.Repository("/var/db/repos/gentoo")
    >>> for category in repo: # sys-apps
    ...     for package in category: # sys-apps/portage
    ...         for ebuild in package: # sys-apps/portage-3.0.65
    ...             pass

A repository can also be indexed directly:

.. doctest::

    >>> repo = pms_utils.repo.Repository("/var/db/repos/gentoo")
    >>> portage_pkg = repo["sys-apps"]["portage"] # you can also further index by version
