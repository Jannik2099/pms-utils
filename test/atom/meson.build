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
            dependencies: [boost_dep, pms_utils_dep],
            include_directories: inc_test_utils,
        ),
    )
endforeach
