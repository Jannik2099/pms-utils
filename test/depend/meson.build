tests = ['depend', 'ast_iterator']

if fs.is_dir('/var/db/repos/gentoo/metadata/md5-cache')
    temp = 'md5-cache'
    test(
        temp,
        executable(
            temp,
            temp + '.cpp',
            dependencies: pms_utils_dep,
            include_directories: inc_test_utils,
        ),
        timeout: 0,
    )
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
