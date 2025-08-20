![Effects SDK logo](Assets/Logo.png "Logo")

# Audio Effect SDK Desktop Sample

There is source code of the desktop sample application of the [Audio Effects SDK](https://github.com/EffectsSDK/audio-effects-sdk-cpp-win-mac-linux).  
This sample demonstrates how to use the **Audio Effects SDK**.

## Requirements

Download a release of [Audio Effects SDK](https://github.com/EffectsSDK/audio-effects-sdk-cpp-win-mac-linux/releases)

### Windows

* Visual Studio 2022
* CMake 3.29
* Qt 6.8

## How To Build

### Windows

1. Configure the project
```bat
cmake -G "Visual Studio 17 2022" -A x64 -S . -B build ^
 -DAUDIO_EFFECTS_SDK_DIR=%CD%/audio-effects-sdk/x64 ^ 
 -DCMAKE_INSTALL_PREFIX=%CD%/output_app
```

2. Build the project
```bat
cmake --build build --config Release
```

3. Install
```bat 
cmake --install build --config Release
```
_Don't use `--prefix`, use `-DCMAKE_INSTALL_PREFIX` in the configure step instead_

The built application must be inside `%CD%/output_app`. Run `AudioEffectsSDKDemo.exe` file.