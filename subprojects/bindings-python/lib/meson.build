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
    dependencies: [pms_utils_dep, nanobind_dep],
    install: true,
    gnu_symbol_visibility: 'inlineshidden',
)

py_installation.install_sources(
    [
        'py.typed',
    ],
    subdir: 'pms_utils',
)