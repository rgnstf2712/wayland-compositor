project('wayland-compositor', 'c')
src = ['main.c', 'session.c', 'drm.c']
include = include_directories('include')
deps = [dependency('dbus-1'), dependency('libudev'), dependency('wayland-server')]
executable('main', src, include_directories : include, dependencies : deps)
