# Lunatic Vibes F

Lunatic Vibes F (LVF) is a BMS client - a rhythm game that plays community-made charts in BMS format.

The project is still in development stage. Please do not expect a bug-free experience. Feel free to open issues.

It is a fork of [Lunatic Vibes](https://github.com/yaasdf/lunaticvibes), a clone of Lunatic Rave 2 (beta3 100201).
After using LVF, profiles will not be compatible with Lunatic Vibes anymore, so make sure to have backups before moving.
Replays set in LVF will not work in Lunatic Vibes either.

## Features

- Multi-threaded update procedure
- Async input handling
- Full Unicode support
- Built-in difficulty table support
- Mixed skin resolution support (SD, HD, FHD, UHD)
- ARENA Mode over LAN / VLAN

For LR2 feature compatibility list, check out [the wiki](https://github.com/chown2/lunaticvibesf/wiki/LR2-Features-Compatibility).

## Requirements

- **Do NOT use this application to load unauthorized copyrighted contents (e.g. charts, skins).**
- Supported OS: Windows 7+

## Quick Start

- Install [Microsoft Visual C++ Redistributable 2015+ x64](https://aka.ms/vs/17/release/vc_redist.x64.exe)
- Download the latest release from [here](https://github.com/chown2/lunaticvibesf/releases)
- Copy LR2files folder from LR2 (must include default theme; a fresh copy right from LR2 release is recommended)

## Build

### Windows with Visual Studio

Open the project's directory in Visual Studio, it should pick up CMake and install dependencies with vcpkg
automatically.

### Linux

```sh
# export VCPKG_ROOT=/path/to/vcpkg
cmake --preset linux-vcpkg -B ./build
cmake --build ./build --config=Debug
ls build/bin/Debug
```

To enable additional warnings when compiling with GCC change preset above to `linux-vcpkg-gcc`.
Similarly for clang use `linux-vcpkg-clang`.

## License

- MIT License
