project('collected_sources', 'c',
  version: '1',
  default_options: [
    'optimization=g',
    'warning_level=2',
    #'b_lto=true',
    'b_pie=true',
  ])

add_project_arguments('-ggdb', language : 'c')

progs = [
  'algif', 'af_unix', 'check_mem', 'fadvise', 'fcntl', 'fork_leak', 'gai', 'ipv6', 'malloc_pages',
  'mmap', 'msgrcv', 'open_loop', 'personality', 'pgid_sid', 'poll_sel', 'pps', 'remote_tuner',
  'recv_udp', 'root', 'seq_write', 'sig', 'snd', 'tun', 'usb_killer', 'vhci',
]

foreach p : progs
  executable(p, p + '.c')
endforeach

progs_spec = {
  'fp1'           : { 'link_args' : '-lm', 'c_args' : '-O0' },
  'cond_wait'     : { 'link_args' : '-pthread' },
  'pthread_once'  : { 'link_args' : '-pthread' },
}

foreach p, kwargs : progs_spec
  executable(p, p + '.c', kwargs: kwargs)
endforeach
