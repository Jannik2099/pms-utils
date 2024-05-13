# TODO: autogenerate from stubgen once pybind11-2.12 is in wrapdb
# GroupHeaderOp does not get correctly located, bug in my Enum binding?
# Manual fixup required for now
bindings_py_pyi = files(
    '__init__.pyi',
    'atom.pyi',
    'depend.pyi',
    'ebuild.pyi',
    'profile.pyi',
    'repo.pyi',
)

install_data(
    bindings_py_pyi,
    install_dir: py_installation.get_install_dir() + 'pms_utils-stubs',
)