project(
    'pms-utils',
    'cpp',
    version: files('version'),
    default_options: ['warning_level=3', 'cpp_std=c++20'],
)
add_global_arguments(['-fvisibility=hidden', '-fvisibility-inlines-hidden'], language: 'cpp')
add_global_arguments('-DBOOST_ALL_DYN_LINK=1', language: 'cpp')

fs = import('fs')
pkg = import('pkgconfig')

boost_dep = dependency('boost')

subdir('include')

subdir('lib')

if get_option('test')
    subdir('test')
endif

if get_option('bindings').contains('python')
    subproject('bindings-python')
endif
