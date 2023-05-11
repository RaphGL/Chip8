# Chip8
A Chip8 Emulator written in C11.

Note: This emulator implements all of Chip8's spec. While it can display video, play audio and generally works, it has not been tested and polished enough, so bugs should be expected on some roms.

## Installation
Using meson (preferred):
```sh
$ meson configure build -Dbuildtype=release
$ meson compile -C build
```

YOLO:
```sh
$ gcc -lm -lSDL2 -std=c11 -O2 src/*.c -o chip8
```


## Usage
```sh
$ chip8 <path_to_rom>
```
