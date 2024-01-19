lib_bindings_py += static_library(
    'atom',
    'atom.cpp',
    include_directories: [inc_lib],
    dependencies: [python_dep, pybind11_dep],
    link_with: [lib_atom],
)

bindings_py_pyi += files('atom.pyi')
