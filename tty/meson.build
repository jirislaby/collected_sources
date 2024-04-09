project('tty', 'c',
  version: '1',
  default_options: [
    'optimization=g',
    'warning_level=2',
    #'b_lto=true',
    'b_pie=true',
  ])

add_project_arguments('-ggdb', language : 'c')

progs = [
  'csi_P', 'kdgkb',
  'ld', 'ld1', 'ld2',
  'n_gsm', 'pty', 'serial',
  'tcset2',
  'tiocm', 'tiocsetd',
  'tty1',
  'tty_double', 'tty_double_r',
  'tty-loop-hangup', 'tty-loop-open', 'tty-loop-rw', 'tty-loop-tiocsctty', 'tty-loop-tiocsetd',
  'tty_name', 'tty_printk',
  'vcs_test'
]

foreach p : progs
  executable(p, p + '.c')
endforeach

progs_spec = {
  'pty_multiwrite' : { 'link_args' : '-pthread' },
  'tty_unimap' : { 'link_args' : '-pthread' }
}

foreach p, kwargs : progs_spec
  executable(p, p + '.c', kwargs: kwargs)
endforeach
