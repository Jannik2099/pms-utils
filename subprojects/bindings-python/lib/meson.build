src_bindings_py = []

subdir('atom')
subdir('depend')
subdir('ebuild')
subdir('profile')
subdir('repo')

mod_bindings_py = py_installation.extension_module(
    'pms_utils',
    src_bindings_py + 'pms_utils.cpp',
    dependencies: [pms_utils_dep, boost_dep, python_dep, nanobind_dep],
    install: true,
    gnu_symbol_visibility: 'inlineshidden',
    cpp_pch: pms_utils_dep.get_variable('pch_header'),
)

py_installation.install_sources(['py.typed'], subdir: 'pms_utils')
