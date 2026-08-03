project('finit', 'cpp',
  version: '1',
  meson_version : '>=1.10.0',
  default_options: [
    'optimization=g',
    'warning_level=2',
    'b_lto=true',
    'b_pie=true',
    'cpp_std=c++20',
  ])

num_modules = get_option('num_modules')
kbytes_payload = get_option('kbytes_payload')

add_project_arguments('-ggdb', language : 'cpp')
add_project_arguments('-DNUM_MODULES=' + num_modules.to_string(), language : 'cpp')

deps = []
deps += dependency('slhelpers++')

prj_name = meson.project_name()
executable(prj_name, prj_name + '.cpp', dependencies: deps)

kdir = '/lib/modules/' + run_command('uname', '-r', check: true).stdout().strip() + '/build'
sh = find_program('sh')
rand_data = ''
foreach _ : range(0, kbytes_payload * 1024 / 10)
  rand_data += '0123456789'
endforeach

inputs = []
mods = []
kbuild_lines = []

foreach i : range(0, num_modules)
  i_str = i.to_string()
  mod_i = 'mod' + i_str
  inputs += configure_file(
    input : 'mod.c.in',
    output : mod_i + '.c',
    configuration : {
      'DATA' : rand_data,
    }
  )

  kbuild_lines += 'obj-m += ' + mod_i + '.o'
  mods += mod_i + '.ko'
endforeach

kbuild = configure_file(
  input : 'Kbuild.in',
  output : 'Kbuild',
  configuration : {
    'MODS' : '\n'.join(kbuild_lines),
  }
  )

custom_target('finit_modules',
  input : [ inputs, kbuild ],
  output : mods,
  command : ['make', '-j', '-C', kdir, 'M=' + meson.current_build_dir(), 'modules'],
  build_by_default : true,
)

