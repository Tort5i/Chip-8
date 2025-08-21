# Chip-8 install guide

Download latest build from the [release](https://github.com/Tort5i/Chip-8/releases) section or complie your self following the bellow guide.

## Windows 

### Using MinGW and cmake

Download [w64devkit](https://github.com/skeeto/w64devkit/releases/tag/v2.4.0) from here and extract into a sutable place.

Add the bin folder to your system path.

Now u need to install SDL3.

Clone the repo locally using:
`git clone https://github.com/libsdl-org/SDL.git`

Then:
```
cd SDL
mkdir build
cd build
```

now compile and install SDL3
```
cmake -G "MinGW Makefiles" -DCMAKE_MAKE_PROGRAM=make ..
make install
```
finally add SDL3 to your system path (Mine installed in Program Files (x86)) and resart your computer

Now we can install the emulator
Clone the repositories files locally using:
`git clone https://github.com/Tort5i/Chip-8.git`
Or for ImGui version
`git clone --branch ImGui https://github.com/Tort5i/Chip-8.git`

Then:
```
cd Chip-8
mkdir build
cd build
```

Now you can compile the source:
```
cmake -G "MinGW Makefiles" -DCMAKE_MAKE_PROGRAM=make ..
make
```

To run you can navigate to the build folder and run the exe or: `./Chip-8`

If moving the exe ensure that SDL.dll that is generated during compilation is in the same folder as Chip-8.exe.