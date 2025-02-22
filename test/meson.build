inc_test_utils = include_directories('utils')
tests = []

subdir('atom')
subdir('depend')
subdir('ebuild')
subdir('repo')
subdir('profile')

foreach t : tests
    parent = fs.parent(fs.relative_to(t, meson.current_source_dir()))
    test(
        parent + '/' + fs.stem(t),
        executable(
            parent + '_' + fs.stem(t),
            t,
            dependencies: [boost_dep, pms_utils_dep],
            include_directories: inc_test_utils,
            gnu_symbol_visibility: 'inlineshidden',
        ),
        timeout: 0,
    )
endforeach