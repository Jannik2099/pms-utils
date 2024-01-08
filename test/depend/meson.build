tests = ['depend', 'ast_iterator']

if fs.is_dir('/var/db/repos/gentoo/metadata/md5-cache')
    tests += 'md5-cache'
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
