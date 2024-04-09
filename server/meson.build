project('server', 'c',
  version: '1',
  default_options: [
    'optimization=g',
    'warning_level=2',
    #'b_lto=true',
    'b_pie=true',
  ])

add_project_arguments('-ggdb', language : 'c')

progs = [
  'ser', 'cl', 'kl',
]

foreach p : progs
  executable(p, p + '.c', link_args: [ '-pthread' ] )
endforeach
