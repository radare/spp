project('spp', 'c')
spp_version = '1.0'

platform_inc = ['.']

configure_file(input: 'config.def.h',
  output: 'config.h',
  copy: true)

spp_files = [
  'spp.c',
  's_api.c'
]

spp_inc = include_directories(platform_inc)

libspp = both_libraries('spp', spp_files,
  include_directories: spp_inc,
  implicit_include_directories: false,
  install: true,
  soversion: spp_version
)

spp_static_dep = declare_dependency(
  link_with: libspp.get_static_lib(),
  include_directories: spp_inc
)

spp_dep = declare_dependency(
  link_with: libspp.get_shared_lib(),
  include_directories: spp_inc
)

pkgconfig_mod = import('pkgconfig')
pkgconfig_mod.generate(
  libraries: [libspp.get_shared_lib()],
  version: spp_version,
  name: 'spp',
  subdirs: ['.'],
  filebase: 'spp',
  description: 'spp library'
)

if not meson.is_subproject()
  install_headers('spp.h')

  spp_exe = executable('spp', ['main.c'],
    dependencies: spp_dep,
    install: true
  )
endif
