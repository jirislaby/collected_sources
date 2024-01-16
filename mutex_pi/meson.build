project('mutex_pi', 'c',
  version: '1',
  default_options: [
    'optimization=g',
    'warning_level=2',
    'b_lto=true',
    'b_pie=true',
    'cpp_std=c++17',
  ])

add_project_arguments('-ggdb', language : 'c')

prj_name = meson.project_name()
executable(prj_name, prj_name + '.c')

