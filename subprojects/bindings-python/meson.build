project(
    'bindings-python',
    'cpp',
    version: files('version'),
    meson_version: '>=1.1.0',
    default_options: ['warning_level=3', 'cpp_std=c++20'],
)

fs = import('fs')

pms_utils_dep = dependency('pms-utils')
boost_dep = dependency('boost')
nanobind_dep = dependency('nanobind', static: true, include_type: 'system')

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

subdir('stubs')

if get_option('docs')
    subdir('docs')
endif
