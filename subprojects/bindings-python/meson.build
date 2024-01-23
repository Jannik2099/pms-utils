project(
    'bindings-python',
    'cpp',
    version: files('version'),
    default_options: ['warning_level=3', 'cpp_std=c++20'],
)
add_project_arguments(['-fvisibility=hidden', '-fvisibility-inlines-hidden'], language: 'cpp')
add_project_arguments('-DBOOST_ALL_DYN_LINK=1', language: 'cpp')

fs = import('fs')

pms_utils_dep = dependency('pms-utils')
boost_dep = dependency('boost')
pybind11_dep = dependency('pybind11')

pymod = import('python')
py_installation = pymod.find_installation()
python_dep = py_installation.dependency()

subdir('lib')

is_asan = get_option('b_sanitize').contains('address')
is_ubsan = get_option('b_sanitize').contains('undefined')

if get_option('test')
    # due to sanitized dsos requiring LD_PRELOAD
    # need to figure out a reliable way to find the sanitizer dso
    if not (is_asan or is_ubsan)
        subdir('test')
    endif
endif
