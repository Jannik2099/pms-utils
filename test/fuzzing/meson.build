subdir('previous_corpus')

pymod = import('python')
py_installation = pymod.find_installation(
    pure: false,
)
python_dep = py_installation.dependency(
    include_type: 'system',
    embed: true,
)
nanobind_dep = dependency(
    'nanobind',
    static: true,
    include_type: 'system',
)

tests = files('atom.cpp')

parent = 'fuzzing'
nproc = run_command(
    'nproc',
    capture: true,
    check: true,
).stdout().strip()

provided_corpus = ''
if get_option('fuzz-corpus') != ''
    provided_corpus = get_option('fuzz-corpus')
    assert(
        fs.is_dir(provided_corpus),
        'provided corpus directory does not exist',
    )
endif

generator_exe = executable(
    parent + '_generate_corpus_exe',
    'generate_corpus.cpp',
    dependencies: [pms_utils_dep],
    include_directories: inc_test_utils,
    gnu_symbol_visibility: 'inlineshidden',
    link_args: [fuzz_link_args],
)
generated_corpus = meson.current_build_dir() / 'generated_corpus'
generator_tgt = custom_target(
    parent + '_generate_corpus',
    command: [
        'rm',
        '-rf',
        generated_corpus,
        '&&',
        'mkdir',
        '-p',
        generated_corpus,
        '&&',
        generator_exe,
        generated_corpus,
    ],
    output: 'generated_corpus',
)

merged_corpus_dirs = []
merged_corpus_tgts = []

foreach t : tests
    workdir = meson.current_build_dir() / (fs.stem(t) + '_workdir')
    corpus = meson.current_build_dir() / (fs.stem(t) + '_corpus')
    previous_corpus = meson.current_build_dir() / 'previous_corpus' / (fs.stem(t) + '_corpus_merged')

    workdir_tgt = custom_target(
        parent + '_' + fs.stem(t) + '_workdir',
        command: ['rm', '-rf', workdir, '&&', 'mkdir', '-p', workdir],
        output: fs.stem(t) + '_workdir',
        build_always_stale: true,
    )
    corpus_tgt = custom_target(
        parent + '_' + fs.stem(t) + '_corpus',
        command: ['rm', '-rf', corpus, '&&', 'mkdir', '-p', corpus],
        output: fs.stem(t) + '_corpus',
        build_always_stale: true,
    )

    args = [
        '-error_exitcode=1',
        '-use_value_profile=1',
        '-jobs=' + nproc,
        '-workers=' + nproc,
    ]
    if get_option('fuzz-options') != ''
        args += get_option('fuzz-options').split()
    endif
    args += [corpus, previous_corpus, generated_corpus]
    if provided_corpus != ''
        args += provided_corpus
    endif

    fuzz_exe = executable(
        parent + '_' + fs.stem(t),
        t,
        dependencies: [python_dep, nanobind_dep, pms_utils_dep],
        include_directories: inc_test_utils,
        gnu_symbol_visibility: 'inlineshidden',
        # we explicitly do NOT want to compile this with -fsanitize=fuzzer,
        # as the extra instrumentation distracts from the library
        link_args: [fuzz_args],
    )
    test(
        parent + '/' + fs.stem(t),
        fuzz_exe,
        depends: [workdir_tgt, corpus_tgt, generator_tgt],
        args: args,
        timeout: 0,
        is_parallel: false,
        suite: 'fuzz',
        env: {
            'PYTHON_EXE': py_installation.full_path(),
        },
        workdir: workdir,
    )

    corpus_merged = meson.current_build_dir() / (fs.stem(t) + '_corpus_merged')
    merged_corpus_dirs += fs.relative_to(
        corpus_merged,
        meson.current_build_dir(),
    )
    args = [
        'rm',
        '-rf',
        corpus_merged,
        '&&',
        'mkdir',
        '-p',
        corpus_merged,
        '&&',
        fuzz_exe,
        '-merge=1',
        corpus_merged,
        corpus,
        previous_corpus,
        generated_corpus,
    ]
    if provided_corpus != ''
        args += provided_corpus
    endif
    merged_corpus_tgts += custom_target(
        parent + '_' + fs.stem(t) + '_merge',
        command: args,
        env: {
            'PYTHON_EXE': py_installation.full_path(),
        },
        output: fs.stem(t) + '_corpus_merged',
        build_always_stale: true,
    )
endforeach

custom_target(
    parent + '_merge_all_corpus',
    command: [
        'tar',
        '-c',
        '-I',
        'zstd -19 -T0',
        '-f',
        meson.current_build_dir() / 'merged_corpus.tar.zst',
        '-C',
        meson.current_build_dir(),
        merged_corpus_dirs,
    ],
    output: 'merged_corpus.tar.zst',
    depends: merged_corpus_tgts,
)
