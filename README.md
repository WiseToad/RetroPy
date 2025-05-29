The adapter that allows you to run libretro cores written in Python.

#### LIMITATIONS

Adapter is intended to work with [pygame](https://www.pygame.org/)-based Python programs.
Only frame buffer refresh and joypad/keyboard events propagation supported. Sound and other features
provided by `pygame` (like sound, etc) didn't ever planned for now.

Generally, any library that support Python API's [Buffer Protocol](https://docs.python.org/3/c-api/buffer.html)
to provide frame buffer can be used, but that wasn't ever explored.

#### COMPATIBILITY

Developed and tested on Ubuntu 24.04 LTS with Python 3.12 and RetroArch 1.18.0

Also, tested on custom Lakka 6.0 fork, built for Raspberry Pi 5, which includes manually
embedded `pygame` package and it's dependencies, that are not provided there originally.

#### BUILD

- Prepare essentials:
  ```sh
  sudo apt install -y build-essential python3-dev
  ```

- Obtain libretro dev stuff in `libretro-common` dir next to `src` dir of this repo:
  ```sh
  git clone https://github.com/libretro/libretro-common
  ```
  In fact, only `libretro-common/inclide/libretro.h` file used.

- Build:
  ```sh
  make
  ```

- Install:
  Place just build `retropy_libretro.so` library in standard directory for libretro cores.

- Configure:
  Place `RetroPy.conf` file in RetroPy subdir of libretro system directory, then change `pythonLib`
  configuration parameter in this file to actual Python library location for your distro.

#### USAGE

The most smooth approach to develop libretro core in Python is to subclass `RetroCore` class
and use `RetroKey` constants provided in `python` dir of this repo.

See `sample.py` for an example how to do this. Example is able to run both as a libretro core
and as a standalone Python "game".
