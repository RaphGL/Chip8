project(
  'chip8', 'c',
  default_options: ['c_std=c11', 'b_ndebug=if-release'],
  version: '1.2'
)

cc = meson.get_compiler('c')

deps = [
  dependency('sdl2'),
  cc.find_library('m')
]

executable(
  'chip8',
  ['src/main.c', 'src/cpu.c', 'src/opcode.c', 'src/io.c'],
  dependencies: deps
)
