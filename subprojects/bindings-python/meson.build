project(
    'bindings-python',
    'cpp',
    version: files('version'),
    default_options: ['warning_level=3', 'cpp_std=c++20'],
)

pms_utils_dep = dependency('pms-utils')
boost_dep = dependency('boost')
pybind11_dep = dependency('pybind11')

pymod = import('python')
py_installation = pymod.find_installation()
python_dep = py_installation.dependency()

subdir('lib')
