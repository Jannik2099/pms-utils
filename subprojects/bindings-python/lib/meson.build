lib_bindings_py = []

subdir('atom')
subdir('depend')
subdir('ebuild')
subdir('profile')
subdir('repo')

mod_bindings_py = py_installation.extension_module(
    'pms_utils',
    'pms_utils.cpp',
    link_with: lib_bindings_py,
    dependencies: [pms_utils_dep, pybind11_dep],
    install: true,
)