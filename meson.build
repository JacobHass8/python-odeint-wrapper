project(
  'cheese',
  ['cython', 'cpp'],
  version: '0.1.0',
  default_options: ['cpp_std=c++17']
)

py3 = import('python').find_installation(pure: false)

py3.extension_module('cheese',
  ['cheese.pyx'],
  override_options: ['cython_language=cpp'],
  include_directories: ['.', '/home/jacob/Documents/boost'],
  install: true,
)
