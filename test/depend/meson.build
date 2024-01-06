tests = ['depend', 'ast_iterator']

foreach t : tests
    test(
        t,
        executable(
            t,
            t + '.cpp',
            link_with: [lib_depend],
            include_directories: [inc_lib],
        ),
    )
endforeach
