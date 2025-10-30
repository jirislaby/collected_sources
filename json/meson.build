project('json', 'cpp',
  version: '1',
  default_options: [
    'optimization=g',
    'warning_level=2',
    'b_lto=true',
    'b_pie=true',
    'c_std=gnu17',
    'cpp_std=c++17',
  ])

add_project_arguments('-ggdb', language : 'cpp')

deps = []
deps += dependency('nlohmann_json')

prj_name = meson.project_name()
executable(prj_name, prj_name + '.cpp', dependencies: deps)
