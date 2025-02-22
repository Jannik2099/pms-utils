inc_test_utils = include_directories('utils')
tests = []

subdir('atom')
subdir('depend')
subdir('ebuild')
subdir('repo')
subdir('profile')

is_asan = get_option('b_sanitize').contains('address')
test_env = environment()

if is_asan and meson.get_compiler('cpp').get_id() == 'gcc'
    # e.g.
    # SUMMARY: AddressSanitizer: odr-violation: global '__incb_edges' at /usr/lib/gcc/x86_64-pc-linux-gnu/14/include/g++-v14/bits/unicode-data.h:383:29
    # seems like a false positive, since that symbol is declared inline?
    test_env.set('ASAN_OPTIONS', 'detect_odr_violation=0')
endif

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
        env: test_env,
    )
endforeach