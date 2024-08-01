DependExpr
==========
.. currentmodule:: pms_utils.depend

| This class represents a package dependency specification as per `PMS 8.2 <https://projects.gentoo.org/pms/8/pms.html#x1-730008.2>`_.
| It is implemented as a recursive tree, where each node may either be another :py:class:`DependExpr`, or an :py:class:`~pms_utils.atom.Atom`.
| A missing ``conditional`` represents an *all-of* dependency group.
| Note: ``conditional`` is always ``None`` for the root node.

.. autoclass:: DependExpr
