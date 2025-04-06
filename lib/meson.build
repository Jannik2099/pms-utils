libpms_utils_src = []

subdir('atom')
subdir('depend')
subdir('ebuild')
subdir('repo')
subdir('profile')

libpms_utils = library(
    'pms-utils',
    libpms_utils_src,
    dependencies: boost_dep,
    include_directories: inc_lib,
    install: true,
    gnu_symbol_visibility: 'inlineshidden',
    cpp_pch: pch_header.full_path(),
)

pms_utils_dep = declare_dependency(
    link_with: libpms_utils,
    include_directories: inc_lib,
    variables: {
        'pch_header': pch_header.full_path(),
    },
)

meson.override_dependency('pms-utils', pms_utils_dep)

pkg.generate(
    libpms_utils,
    variables: {
        'pch_header': '${includedir}/pms-utils.hpp',
    },
    uninstalled_variables: {
        'pch_header': pch_header.full_path(),
    },
)
