lib_bindings_py += static_library(
    'repo',
    'repo.cpp',
    include_directories: [inc_lib],
    dependencies: [boost_dep, python_dep, pybind11_dep],
    link_with: [lib_repo],
)

bindings_py_pyi += files('repo.pyi')
