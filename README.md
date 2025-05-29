The adapter that allows you to run libretro cores written in Python.

#### LIMITATIONS

Adapter is intended to work with [`pygame`](https://www.pygame.org/)-based Python cores.
Only frame buffer refresh and joypad/keyboard events propagation supported. Sound and other features
provided by `pygame` didn't ever planned for now.

Generally, any library that supports Python API's [Buffer Protocol](https://docs.python.org/3/c-api/buffer.html)
can be used to provide rendered frame buffer, but it wasn't ever explored.

#### COMPATIBILITY

Developed and tested on Ubuntu 24.04 LTS with Python 3.12 and RetroArch 1.18.0

Also, tested on custom Lakka 6.0 fork, built for Raspberry Pi 5, which includes manually
embedded `pygame` package and it's dependencies, that are not provided there originally.

#### BUILD

Prepare essentials:
```sh
sudo apt install -y build-essential python3-dev
```

Obtain libretro developer stuff into `libretro-common/` directory next to `src/` directory of this repo:
```sh
git clone https://github.com/libretro/libretro-common
```
In fact, only `libretro-common/inclide/libretro.h` file used.

  ```sh
  make
  ```

Copy `retropy_libretro.so` file from `build/` directory into standard directory containing libretro cores.
You may pick this directory from RetroArch settings, as a hint.

Place `RetroPy.conf` file from this repo into `RetroPy/` subdirectory of libretro system directory (again, look for it in RetroArch settings).
Then change `pythonLib` configuration parameter in this file to actual Python library location for your system.

#### USAGE

The most smooth approach to develop libretro core in Python is to subclass `RetroCore` class
and use `RetroKey` constants provided in `python/` directory of this repo.

See `sample.py` for an example how to do this. Example can be launched both as a libretro core
and as a standalone Python "game".
