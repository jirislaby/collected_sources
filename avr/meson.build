project('avr', 'c',
  version: '1',
  default_options: [
    'optimization=s',
    'warning_level=2',
    'b_lto=false',
    'b_pie=false',
  ])

add_project_arguments('-ggdb', language : 'c')

prj_name = meson.project_name()
exe = executable(prj_name, prj_name + '.c')

objcopy = find_program('objcopy')
hex = custom_target(prj_name + '.hex',
  output: prj_name + '.hex',
  depends: exe,
  build_by_default: true,
  command: [ objcopy, '-j', '.text', '-j', '.data', '-O', 'ihex', exe, prj_name + '.hex' ])

avrdude = find_program('avrdude')
run_target('upload',
  depends: hex,
  command: [ avrdude, '-c', 'usbasp-clone', '-p', 'm16a', '-U', 'flash:w:' + hex.full_path() ])
