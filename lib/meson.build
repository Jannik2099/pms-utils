inc_lib = include_directories('.')

subdir('atom')
subdir('depend')
subdir('repo')

libpms_utils = static_library(
    'pms-utils',
    link_whole: [lib_atom, lib_depend, lib_repo],
)

pms_utils_dep = declare_dependency(
    include_directories: inc_lib,
    link_with: libpms_utils,
)

meson.override_dependency('pms-utils', pms_utils_dep)
