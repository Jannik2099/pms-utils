if fs.is_dir('/var/db/repos/gentoo')
    temp = 'iter'
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
