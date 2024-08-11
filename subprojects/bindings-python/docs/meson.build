sphinx_build_exe = find_program('sphinx-build')

bindings_python_docs = custom_target(
    'bindings_python_docs',
    command: [
        sphinx_build_exe,
        '-W',
        '-E',
        '-a',
        '-b', 'html',
        '@CURRENT_SOURCE_DIR@' + '/source',
        '@OUTDIR@',
    ],
    output: 'index.html',
    depends: mod_bindings_py,
    build_always_stale: true,
    install: false,
    env: {'PYTHONPATH': fs.parent(mod_bindings_py.full_path())},
)

test(
    'bindings_python_docs_test',
    sphinx_build_exe,
    args: [
        '-W',
        '-E',
        '-a',
        '-b', 'doctest',
        meson.current_source_dir() + '/source',
        meson.current_build_dir() + '/doctest',
    ],
    depends: mod_bindings_py,
    env: {'PYTHONPATH': fs.parent(mod_bindings_py.full_path())},
)