fakevdb = meson.current_source_dir() + '/fakevdb'

test('vdb', executable('vdb',
                       'vdb.cpp',
                       dependencies: [boost_dep, pms_utils_dep],
                       include_directories: inc_test_utils,),
     args: [fakevdb],
     timeout: 0)
