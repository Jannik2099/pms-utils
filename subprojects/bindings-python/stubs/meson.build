# TODO: autogenerate from stubgen
# GroupHeaderOp does not get correctly located, bug in my Enum binding?
# Manual fixup required for now
bindings_py_pyi = files(
    'pms_utils.pyi',
    'atom/__init__.pyi',
    'depend/__init__.pyi',
    'ebuild/__init__.pyi',
    'profile/__init__.pyi',
    'repo/__init__.pyi',
)

install_data(
    bindings_py_pyi,
    install_dir: py_installation.get_install_dir() + 'pms_utils-stubs',
)