bindings_py_dir = py_installation.get_install_dir() + 'pms_utils-stubs'

lib_bindings_py = []
bindings_py_pyi = [files('__init__.pyi')]

subdir('atom')
subdir('depend')
subdir('repo')

mod_bindings_py = py_installation.extension_module(
    'pms_utils',
    'pms_utils.cpp',
    link_with: lib_bindings_py,
    dependencies: [python_dep, pybind11_dep],
    install: true,
)


install_data(
    bindings_py_pyi,
    install_dir: bindings_py_dir,
)
