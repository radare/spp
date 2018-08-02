project('spp', 'c')

platform_inc = ['.']
c_args = []

spp_files = [
  'spp.c'
]

if get_option('use_r_util') and get_option('r_util_includes') != ''
  message('Using r_util')
  c_args = ['-DHAVE_R_UTIL']
  platform_inc += [get_option('r_util_includes')]
else
  message('Using internal implementation of r_strbuf functions')
  spp_files += [
    'r_api.c'
  ]
endif

spp_inc = include_directories(platform_inc)

libspp = static_library('spp', spp_files,
  include_directories: spp_inc,
  c_args: c_args,
  implicit_include_directories: false,
  install: true
)

spp_dep = declare_dependency(
  link_with: libspp,
  include_directories: spp_inc
)

if not meson.is_subproject()
  spp_exe = executable('spp', ['main.c'],
    dependencies: spp_dep,
    install: true
  )
endif
