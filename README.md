# Chip8
A Chip8 Emulator written in C11.

Note: This emulator implements all of Chip8's spec. While it can display video, play audio and generally works, it has not been tested and polished enough, so bugs should be expected on some roms.

## Installation
This project uses Meson as it's build system. In case you don't have it installed make sure to install meson (and ninja if it's not installed by your package manager).

```sh
$ meson configure build -Dbuildtype
$ meson compile -C build
```

## Usage
```sh
$ chip8 <path_to_rom>
```
