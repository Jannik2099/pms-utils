# this directory contains previously generated corpuses
# generate by running fuzz tests and then running the "fuzzing_merge_all_corpus" target

run_command(
    [
        'tar',
        '-x',
        '-f',
        meson.current_source_dir() / 'corpus.tar.zst',
        '-C',
        meson.current_build_dir(),
    ],
    check: true,
)
