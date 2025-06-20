project(
    'pms-utils',
    'cpp',
    version: files('version'),
    meson_version: '>=1.4.0',
    default_options: ['warning_level=3', 'cpp_std=c++23'],
)

if get_option('only-wheels')
    subproject('bindings-python')
    subdir_done()
endif

fs = import('fs')
pkg = import('pkgconfig')

boost_dep = dependency(
    'boost',
    optional_modules: ['regex'],
    version: '>=1.87',
    include_type: 'system',
)

if get_option('b_coverage')
    add_project_arguments(
        meson.get_compiler('cpp').get_supported_arguments(
            ['-fprofile-update=atomic'],
        ),
        language: 'cpp',
    )
endif

fuzz_args = []
fuzz_link_args = []
if get_option('fuzz')
    if not (meson.get_compiler('cpp').get_id() == 'clang')
        error('fuzzing requires clang')
    endif
    if get_option('b_lundef')
        error('fuzzing requires -Db_lundef=false')
    endif
    fuzz_args += '-fsanitize=fuzzer'
    fuzz_link_args += '-fsanitize=fuzzer-no-link'
endif

subdir('include')

subdir('lib')

if get_option('test')
    subdir('test')
endif

if get_option('bindings').contains('python')
    bindings_python = subproject(
        'bindings-python',
        default_options: [
            'test=' + get_option('test').to_string(),
            'docs=' + get_option('docs').to_string(),
        ],
    )
endif

if get_option('docs')
    subdir('docs')
endif
