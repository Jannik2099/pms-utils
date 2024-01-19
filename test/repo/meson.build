if fs.is_dir('/var/db/repos/gentoo')
    temp = 'iter'
    test(
        temp,
        executable(
            temp,
            temp + '.cpp',
            link_with: [lib_repo],
            include_directories: [inc_test_utils, inc_lib],
        ),
        timeout: 0,
    )
endif
