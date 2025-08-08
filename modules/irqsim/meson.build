project('irqsim', ['c', 'cpp'],
  version: '1',
  default_options: [
    'optimization=g',
    'warning_level=2',
    'b_lto=true',
    'b_pie=true',
    'c_std=gnu17',
    'cpp_std=c++17',
  ])

add_project_arguments('-ggdb', language : 'c')
add_project_arguments('-ggdb', language : 'cpp')

deps = []

executable('writer', 'writer.cpp', dependencies: deps)

make = find_program('make')
uname_r = run_command('uname', '-r', check: true)
kdir = '/lib/modules/' + uname_r.stdout().strip() + '/build'

custom_target('module',
  input: 'irqsim.c',
  output: 'irqsim.ko',
  build_by_default: true,
  command: [ make,
    '-C', kdir,
    'M=' + meson.current_source_dir(),
    'MO=' + meson.current_build_dir(),
    'modules'
  ],
)
