lib_bindings_py_src = files('pms_utils.cpp')

subdir('atom')
subdir('depend')
subdir('ebuild')
subdir('profile')
subdir('repo')
subdir('vdb')

mod_bindings_py = py_installation.extension_module(
    'pms_utils',
    lib_bindings_py_src,
    dependencies: [pms_utils_dep, boost_dep, python_dep, nanobind_dep],
    install: true,
    gnu_symbol_visibility: 'inlineshidden',
)

py_installation.install_sources(['py.typed'], subdir: 'pms_utils')
