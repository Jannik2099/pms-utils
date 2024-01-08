tests = ['depend', 'ast_iterator']

if fs.is_dir('/var/db/repos/gentoo/metadata/md5-cache')
    temp = 'md5-cache'
    test(
        temp,
        executable(
            temp,
            temp + '.cpp',
            link_with: [lib_depend],
            include_directories: [inc_test_utils, inc_lib],
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
            link_with: [lib_depend],
            include_directories: [inc_test_utils, inc_lib],
        ),
    )
endforeach
