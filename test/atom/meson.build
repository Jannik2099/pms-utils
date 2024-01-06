tests = ['atom', 'usedeps', 'useflag', 'slot', 'slot_no_subslot', 'blocker', 'version_specifier']

foreach t : tests
    test(
        t,
        executable(
            t,
            t + '.cpp',
            link_with: [lib_atom],
            include_directories: [inc_lib],
        ),
    )
endforeach
