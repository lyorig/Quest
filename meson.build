project('HalodaQuest', 'cpp', default_options : ['cpp_std=c++23'])

sources = files(
  'src/scene/console.cpp',
  'src/scene/main_menu.cpp',
  'src/scene/manager.cpp',
  'src/game.cpp',
  'src/helpers.cpp',
  'src/main.cpp',
  'src/sprite.cpp',
  'src/field.cpp'
)

executable('HalodaQuest', sources,
  include_directories: 'include',
  dependencies : subproject('halcyon').get_variable('halcyon_dep'),
)
