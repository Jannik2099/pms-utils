project(
    'pms-utils',
    'cpp',
    version: files('version'),
    default_options: ['warning_level=3', 'cpp_std=c++20'],
)
add_project_arguments(
    ['-fvisibility=hidden', '-fvisibility-inlines-hidden'],
    language: 'cpp',
)
add_global_arguments('-DBOOST_ALL_DYN_LINK=1', language: 'cpp')

cpp = meson.get_compiler('cpp')
# Boost and nanobind are naughty
wno_args = [
    '-Wno-gnu-zero-variadic-macro-arguments',
    '-Wno-zero-length-array',
    '-Wno-nested-anon-types',
    '-Wno-pedantic',
]

foreach arg : wno_args
    if cpp.has_argument(arg)
        add_global_arguments(
            arg,
            language: 'cpp',
        )
    endif
endforeach

fs = import('fs')
pkg = import('pkgconfig')

boost_dep = dependency('boost', modules: ['regex'])

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