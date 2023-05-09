project(
  'chip8', 'c',
  default_options: ['c_std=c11', 'b_ndebug=if-release'],
  version: '1.2'
)

deps = [
  dependency('sdl2'),
  dependency('SDL2_ttf')
]

executable(
  'chip8',
  ['src/main.c', 'src/cpu.c', 'src/opcode.c', 'src/io.c'],
  dependencies: deps
)
