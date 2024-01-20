project(
    'pms-utils',
    'cpp',
    version: '0.1.0',
    default_options: ['warning_level=3', 'cpp_std=c++20'],
)
add_global_arguments(['-fvisibility=hidden', '-fvisibility-inlines-hidden'], language: 'cpp')
add_global_arguments('-DBOOST_ALL_DYN_LINK=1', language: 'cpp')

fs = import('fs')

boost_dep = dependency('boost')
pybind11_dep = dependency('pybind11')

pymod = import('python')
py_installation = pymod.find_installation()
python_dep = py_installation.dependency()

subdir('lib')

subdir('bindings')

subdir('test')
