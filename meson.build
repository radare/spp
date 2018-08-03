project('spp', 'c')

platform_inc = ['.']

spp_files = [
  'spp.c',
  's_api.c'
]

spp_inc = include_directories(platform_inc)

libspp = static_library('spp', spp_files,
  include_directories: spp_inc,
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
