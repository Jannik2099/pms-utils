tests = []
if fs.is_dir('/var/db/repos/gentoo')
    tests = ['iter', 'metadata']
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
        timeout: 0,
    )
endforeach
