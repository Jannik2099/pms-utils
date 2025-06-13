lib_bindings_py = []

subdir('atom')
subdir('depend')
subdir('ebuild')
subdir('profile')
subdir('repo')
subdir('vdb')

mod_bindings_py = py_installation.extension_module(
    'pms_utils',
    'pms_utils.cpp',
    link_with: lib_bindings_py,
    dependencies: [pms_utils_dep, nanobind_dep],
    install: true,
    gnu_symbol_visibility: 'inlineshidden',
    limited_api: '3.' + py_installation.version().split('.')[1],
)

py_installation.install_sources(['py.typed'], subdir: 'pms_utils')
