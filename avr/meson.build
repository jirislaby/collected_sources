project('avr', 'c',
  version: '1',
  default_options: [
    'optimization=s',
    'warning_level=2',
    'b_lto=false',
    'b_pie=false',
  ])

add_project_arguments('-ggdb', language : 'c')

objcopy = find_program('objcopy')
avrdude = find_program('avrdude')

subdir('leds')
subdir('i2c')
subdir('spi')
