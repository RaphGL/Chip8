project(
  'chip8', 'c',
  default_options: ['c_std=c11'],
  version: '1.2'
)

deps = [
  dependency('sdl2'),
  dependency('SDL2_ttf')
]

executable(
  'chip8',
  ['src/main.c', 'src/memory.c', 'src/opcode.c'],
  dependencies: deps
)
