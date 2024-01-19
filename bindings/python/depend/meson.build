lib_bindings_py += static_library(
    'depend',
    'depend.cpp',
    include_directories: [inc_lib],
    dependencies: [python_dep, pybind11_dep],
    link_with: [lib_depend],
)

bindings_py_pyi += files('depend.pyi')
