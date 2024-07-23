from docutils.statemachine import StringList
from sphinx.ext.autodoc import ClassDocumenter
from sphinx.util.typing import ExtensionMetadata, restify
from sphinx.application import Sphinx


def setup(app: Sphinx) -> ExtensionMetadata:
    app.setup_extension("sphinx.ext.autodoc")
    app.add_autodocumenter(Autoclass, override=True)
    return {"parallel_read_safe": True}


class Autoclass(ClassDocumenter):
    priority = ClassDocumenter.priority + 1

    def add_directive_header(self, sig: str) -> None:
        super().add_directive_header(sig)
        self._bind_bases()

    def add_content(self, more_content: StringList | None) -> None:
        super().add_content(more_content)
        self._bind_method("__iter__")
        self._bind_method("__getitem__")

    def _bind_bases(self):
        bases = getattr(self.object, "__bases__", None)
        if bases is None:
            return
        pms_bases = list()
        for base in bases:
            if base.__module__.split(".")[0] == "pms_utils":
                pms_bases.append(base)
        if len(pms_bases) > 0:
            if self.config.autodoc_typehints_format == "short":
                pms_bases = [restify(base, "smart") for base in pms_bases]
            else:
                pms_bases = [restify(base) for base in pms_bases]

            sourcename = self.get_sourcename()
            self.add_line("", sourcename)
            self.add_line("   Bases: %s" % ", ".join(pms_bases), sourcename)

    def _bind_method(self, method: str):
        if not hasattr(self.object, method):
            return
        sourcename = self.get_sourcename()
        self.add_line("", sourcename)
        self.add_line(f".. automethod:: {method}", sourcename)
