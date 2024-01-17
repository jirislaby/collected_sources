project('pthread', 'cpp',
  version: '1',
  default_options: [
    'optimization=g',
    'warning_level=2',
    'b_lto=true',
    'b_pie=true',
    'cpp_std=c++17',
  ])

add_project_arguments('-ggdb', language : 'cpp')

deps = []
deps += dependency('threads')

prj_name = meson.project_name()
executable(prj_name, prj_name + '.cpp', dependencies: deps)

