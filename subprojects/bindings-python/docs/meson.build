bindings_python_docs = custom_target(
    'bindings_python_docs',
    command: [
        'sphinx-build',
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
    env: {'PYTHONPATH': fs.parent(mod_bindings_py.full_path())},
)