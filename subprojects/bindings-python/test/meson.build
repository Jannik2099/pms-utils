temp = environment()
temp.set('PYTHONPATH', fs.parent(mod_bindings_py.full_path()))

test(
    'import',
    files('import.py'),
    depends: mod_bindings_py,
    env: temp,
)
