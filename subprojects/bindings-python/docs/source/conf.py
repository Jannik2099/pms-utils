import pathlib
import sys

sys.path.insert(0, str(pathlib.Path(__file__).parent.resolve()))

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = "pms-utils"
copyright = "2024, Jannik Glückert"
author = "Jannik Glückert"

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

autoclass_content = "both"
autodoc_default_options = {"members": True, "undoc-members": True}
doctest_global_setup = "import pms_utils"

extensions = [
    "sphinx.ext.autodoc",
    "sphinx.ext.doctest",
    "enum_tools.autoenum",
    "autoclass",
    "autoproperty",
]

templates_path = ["_templates"]
exclude_patterns = []


# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = "sphinx_rtd_theme"
html_static_path = ["_static"]
html_css_files = ["custom.css"]
