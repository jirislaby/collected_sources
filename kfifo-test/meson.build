project('kfifo-test', 'c',
  version: '1',
  default_options: [
    'optimization=g',
    'warning_level=2',
    'b_lto=true',
    'b_pie=true',
  ])

add_project_arguments('-ggdb', language : 'c')

prj_name = meson.project_name()
executable(prj_name, prj_name + '.c')

