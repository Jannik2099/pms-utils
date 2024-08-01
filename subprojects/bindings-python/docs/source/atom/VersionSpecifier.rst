VersionSpecifier
================
.. currentmodule:: pms_utils.atom

This enum represents the version dependency operators as per `PMS 8.3.1 <https://projects.gentoo.org/pms/8/pms.html#x1-800008.3.1>`_.

..
    a normal | block won't work here for *reasons*

.. role:: raw-html(raw)
    :format: html

.. autoenum:: VersionSpecifier

    Note: :py:enum:mem:`~VersionSpecifier.ea` is the :code:`=foo/bar-1*` specifier, and thus cannot be constructed through the :code:`VersionSpecifier(expr)` parser interface.
    Use :py:enum:mem:`~VersionSpecifier.ea` directly.
    :raw-html:`<br/>`
