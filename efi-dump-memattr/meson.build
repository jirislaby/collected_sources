project('myefi', 'c',
  version: '1',
  default_options: [
    'optimization=2',
    'warning_level=2',
    'b_lto=false',
    'b_pie=true',
  ])

add_project_arguments('-ggdb', '-DGNU_EFI_USE_MS_ABI', '-DEFI_FUNCTION_WRAPPER',
  '-fmessage-length=0', '-mno-red-zone',
  '-fno-stack-protector', '-fno-strict-aliasing',
  '-fpic', '-fshort-wchar', language: 'c')

inc = include_directories([ '/usr/include/efi', '/usr/include/efi/x86_64' ])

deps = [ ]
deps += meson.get_compiler('c').find_library('gnuefi',
  has_headers : [ 'efi.h', 'efilib.h' ],
  header_include_directories : inc,
  )
deps += meson.get_compiler('c').find_library('efi')

prj_name = meson.project_name()

crt0='/usr/lib64/crt0-efi-x86_64.o'
link_script='/usr/lib64/elf_x86_64_efi.lds'
shared = shared_library(prj_name, [ 'main.c', ],
  objects: [ crt0 ],
  dependencies: deps,
  include_directories: inc,
  link_args: [ '-Wl,-T,' + link_script, '-Wl,-nostdlib', '-Wl,-znocombreloc', '-Wl,-Bsymbolic', ],
  )

sections_params = []
foreach sec : [ '.text', '.sdata', '.data', '.dynamic',
    '.dynsym', '.rel', '.rela', '.rel.*', '.rela.*', '.reloc'
  ]
  sections_params += '-j'
  sections_params += sec
endforeach

objcopy=find_program('objcopy')
custom_target('myefi', output: prj_name + '.efi',
  depends: shared,
  build_by_default: true,
  command: [ objcopy, sections_params, '--target', 'efi-app-x86_64', shared, prj_name + '.efi' ])
