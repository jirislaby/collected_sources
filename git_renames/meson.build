project('git_renames', 'cpp',
  version: '1',
  meson_version: '>= 1.1',
  default_options: [
    'optimization=3',
    'warning_level=2',
    'b_lto=true',
    'b_pie=true',
    'cpp_std=c++20',
  ])

deps = []
deps += dependency('cxxopts')
git = dependency('slgit++')
deps += git
hlp = dependency('slhelpers++')
deps += hlp
deps += dependency('slkerncvs++')
deps += dependency('slsqlite++')

prj_name = meson.project_name()
executable(prj_name,
  [ prj_name + '.cpp' ],
  dependencies: deps,
  install: true,
)

executable('log-raw',
  [ 'log-raw.cpp' ],
  dependencies: [ git, hlp ],
  install: true,
)
