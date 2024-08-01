restrict_elem
=============
.. currentmodule:: pms_utils.ebuild

| This class represents a single ebuild restrict token as per `PMS 7.3.5 <https://projects.gentoo.org/pms/8/pms.html#x1-670007.3.5>`_.
| It combines an enum of type :py:enum:`properties_elem_type` to discern between the properties defined by PMS, and the raw string representation, for use when the property not defined in PMS. The string representation is initialized for known property types aswell.

.. autoclass:: restrict_elem
