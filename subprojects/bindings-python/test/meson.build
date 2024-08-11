temp = environment()
temp.set('PYTHONPATH', fs.parent(mod_bindings_py.full_path()))

# to support separate core + bindings builds without installing core inbetween
if pms_utils_dep.type_name() == 'pkgconfig'
    temp.append('LD_LIBRARY_PATH', pms_utils_dep.get_variable('prefix') + '/lib')
endif

tests = []

tests += 'import'

if fs.is_dir('/var/db/repos/gentoo')
    tests += 'issue_26'
    tests += 'issue_36'
endif

foreach t : tests
    test(t, files(t + '.py'), depends: mod_bindings_py, env: temp, timeout: 0)
endforeach