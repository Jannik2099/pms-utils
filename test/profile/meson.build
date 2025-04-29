tests += files(
    'make_defaults.cpp',
    'package_use.cpp',
    'shlex_single.cpp',
    'wildcard_atom.cpp',
)
if fs.is_dir('/var/db/repos/gentoo')
    tests += files('expand_wildcard.cpp')
    if get_option('coverage_expensive') or not get_option('b_coverage')
        tests += files('parse_profiles.cpp')
    endif
endif
