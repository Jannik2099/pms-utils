properties_elem
===============
.. currentmodule:: pms_utils.ebuild

| This class represents a single ebuild property as per `PMS 7.3.6 <https://projects.gentoo.org/pms/8/pms.html#x1-680007.3.6>`_.
| It combines an enum of type :py:enum:`restrict_elem_type` to discern between the tokens defined by PMS, and the raw string representation, for use when the token not defined in PMS. The string representation is initialized for known tokens aswell.

.. autoclass:: properties_elem
