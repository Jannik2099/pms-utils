tests = [
    'atom',
    'usedeps',
    'useflag',
    'slot',
    'slot_no_subslot',
    'blocker',
    'version_specifier',
    'name_like_ver',
]

foreach t : tests
    test(
        t,
        executable(
            t,
            t + '.cpp',
            link_with: [lib_atom],
            include_directories: [inc_test_utils, inc_lib],
        ),
    )
endforeach
