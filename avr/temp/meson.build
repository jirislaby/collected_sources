exe = executable('temp', 'temp.c')

hex = custom_target(exe.name() + '.hex',
  input: exe,
  output: exe.name() + '.hex',
  build_by_default: true,
  command: [ objcopy, '-j', '.text', '-j', '.data', '-O', 'ihex', '@INPUT@', '@OUTPUT@' ])

run_target('upload_' + exe.name(),
  depends: hex,
  command: [ avrdude, '-c', 'usbasp-clone', '-p', 'm16a', '-U', 'flash:w:' + hex.full_path() ])
