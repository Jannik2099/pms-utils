stubgen_exe = nanobind_dep.get_variable(internal: 'stubgen_file')
stub_install_dir = py_installation.get_install_dir() / 'pms_utils'

stubgen_tgts = []
foreach module : ['atom', 'depend', 'ebuild', 'profile', 'repo']
    stubgen_tgts += custom_target(
        'stubgen.pms_utils.' + module,
        command: [
            stubgen_exe,
            '-m', 'pms_utils.' + module,
            '-i', fs.parent(mod_bindings_py.full_path()),
            '-o', '@OUTPUT@',
        ],
        depends: mod_bindings_py,
        output: module + '.pyi',
        install: true,
        install_dir: stub_install_dir,
        install_tag: 'python-runtime',
    )
endforeach

stubgen_tgts += custom_target(
    'stubgen.pms_utils',
    command: [
        stubgen_exe,
        '-m', 'pms_utils',
        '-i', fs.parent(mod_bindings_py.full_path()),
        '-o', '@OUTPUT@',
    ],
    depends: mod_bindings_py,
    output: '__init__.pyi',
    install: true,
    install_dir: stub_install_dir,
    install_tag: 'python-runtime',
)