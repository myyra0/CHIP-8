# CHIP-8 Emulator

A simple CHIP-8 emulator written in C23 with SDL3. It loads `.ch8` ROM files, executes their instructions, and displays the result in an SDL window.

I made this project to have a stronger grasp on the C language and an intro to emulation. This is not a professional project.

If you want to implement CHIP-8 yourself, I recommend [this guided tutorial](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/#fx0a-get-key). If you prefer a less guided reference, see the [CHIP-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM).

## AI Use Disclosure

AI assistance was used to implement the SDL audio and keyboard input. The display implementation was written manually. I spent significant time exploring SDL display functionality, which was a bad rabbit hole to jump into, and learning SDL was not the main goal of this project. The focus was learning C and gaining an introduction to this project anyways so i used AI when it came time for the other stuff.

## TODO

- Add some error handling. There is literally none.
- Stack Overflow (ifykyk)

## Requirements

- CMake 4.3 or later
- A C23-compatible compiler
- SDL3 development libraries

## Build and Run

Configure and build the debug target:

```sh
cmake --preset debug
cmake --build --preset debug
```

Run the emulator with a ROM. On Linux and macOS:

```sh
./cmake-build-debug/CHIP_8 roms/<enter rom name>
```

On Windows:

```powershell
cmake-build-debug\CHIP_8.exe roms\<enter rom name>
```

The keyboard uses the standard CHIP-8 layout:

```text
1 2 3 4       1 2 3 C
Q W E R       4 5 6 D
A S D F       7 8 9 E
Z X C V       A 0 B F
```

Use `-super` after the ROM path to enable SUPER-CHIP mode. Semi-implemented, only the instructions that have their behaviour changed are implemented. No scrolling support/larger display. Maybe later.

## Portable Release

The release package contains the executable, SDL3's runtime library, and the bundled ROMs:

```sh
cmake --preset release
cmake --build --preset release
cmake --install cmake-build-release --prefix cmake-build-release/install
cmake --build cmake-build-release --target package
```

The generated archive is placed in `cmake-build-release`.

You can also download the latest portable package from the project's GitHub Releases page. The package includes the emulator, SDL3 runtime library, and bundled ROMs, so no separate SDL3 installation is required.
