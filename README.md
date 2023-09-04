# conan-matlab-opengl-avi-player

# This branch provides an OpenGL Matlab AVI player based on YUV format.

# Prerequisite (Window or Unix):
* Conan
* Matlab (2023 or above)
* Visual Studio 2022 (Win)

# Build (Window):
* Run **run_conan_opengl_avi_player_debug.bat** or **run_conan_opengl_avi_player_release.bat**
* It will spontaneously open up an example from MATLAB.
* *Run*.

# Build (Unix):
```Matlab
conan install . -c tools.system.package_manager:mode=install -c tools.system.package_manager:sudo=True --output-folder=build --build=missing --settings=build_type=Debug
cd build
cmake .. -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=./build/build/Debug/generators/conan_toolchain.cmake -DCMAKE_POLICY_DEFAULT_CMP0091=NEW -DCMAKE_BUILD_TYPE=Debug
cmake --build . --config Debug
./matlab-imgui-plot-conan (just for testing)
```

# What you need:
**openglAviMex**
