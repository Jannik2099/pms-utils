tests = ['depend', 'ast_iterator']

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
