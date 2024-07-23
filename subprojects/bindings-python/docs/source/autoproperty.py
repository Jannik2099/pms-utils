from sphinx.ext.autodoc import PropertyDocumenter, Options
from sphinx.util import inspect
from sphinx.util.typing import ExtensionMetadata
from sphinx.application import Sphinx
import re


def setup(app: Sphinx) -> ExtensionMetadata:
    app.setup_extension("sphinx.ext.autodoc")
    app.connect("autodoc-skip-member", property_adder)
    app.add_autodocumenter(Autoproperty, override=True)
    return {"parallel_read_safe": True}


def property_adder(
    app: Sphinx, what: str, name: str, obj: object, skip: bool, options: Options
) -> bool:
    if not inspect.isproperty(obj):
        return skip
    if options.undoc_members:
        return False
    getter = getattr(obj, "fget", None)
    if getter is None:
        return skip
    doc = getter.__doc__
    if doc is None:
        return skip
    if doc != "":
        return False
    return skip


class Autoproperty(PropertyDocumenter):
    priority = PropertyDocumenter.priority + 1

    def add_directive_header(self, sig: str) -> None:
        super().add_directive_header(sig)
        getter = self._get_property_getter()
        if getter is None:
            return
        getter_doc = getter.__doc__
        if getter_doc is None:
            return
        matched = re.match(r"^(\(.*\))\s+->\s+(.*)$", getter_doc)
        if matched is None:
            return
        ptype = matched.group(2)
        if ptype is None:
            return
        sourcename = self.get_sourcename()
        self.add_line(f"   :type: {ptype}", sourcename)
