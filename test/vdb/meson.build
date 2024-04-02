vdb = '/var/db/pkg'

tests = []

if fs.is_dir(vdb)
  tests += ['vdb']
endif

foreach test : tests
  exec = executable(test,
                    test + '.cpp',
                    dependencies: [boost_dep, pms_utils_dep],
                    include_directories: inc_test_utils)
  test(test, exec, timeout: 0, args: [vdb])
endforeach
