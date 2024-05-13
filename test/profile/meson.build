tests = ['make_defaults', 'package_use', 'wildcard_atom', 'shlex_single']
if fs.is_dir('/var/db/repos/gentoo')
    tests += 'expand_wildcard'
endif

foreach t : tests
    test(
        t,
        executable(
            t,
            t + '.cpp',
            dependencies: [boost_dep, pms_utils_dep],
            include_directories: inc_test_utils,
        ),
    )
endforeach