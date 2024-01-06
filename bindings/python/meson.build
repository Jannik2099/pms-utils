lib_bindings_py = []

subdir('atom')
subdir('depend')

mod_bindings_py = py_installation.extension_module(
    'pms_utils',
    'pms_utils.cpp',
    link_with: lib_bindings_py,
    dependencies: [pybind11_dep],
    install: true,
)
