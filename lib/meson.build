libpms_utils_src = []

subdir('atom')
subdir('depend')
subdir('repo')

libpms_utils = library(
    'pms-utils',
    libpms_utils_src,
    dependencies: boost_dep,
    include_directories: inc_lib,
    install: true,
)

pms_utils_dep = declare_dependency(
    link_with: libpms_utils,
    include_directories: inc_lib,
)

meson.override_dependency('pms-utils', pms_utils_dep)

pkg.generate(libpms_utils)
