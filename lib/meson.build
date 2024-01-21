subdir('atom')
subdir('depend')
subdir('repo')

libpms_utils = library(
    'pms-utils',
    link_whole: [lib_atom, lib_depend, lib_repo],
    install: true,
    include_directories: inc_lib,
)

pms_utils_dep = declare_dependency(
    link_with: libpms_utils,
    include_directories: inc_lib,
)

meson.override_dependency('pms-utils', pms_utils_dep)

pkg.generate(libpms_utils)
