@ECHO ON

set BASEDIR=%~dp0
PUSHD %BASEDIR%

RMDIR /Q /S build

conan install . -c tools.system.package_manager:mode=install -c tools.system.package_manager:sudo=True --build=missing
cd build
cmake .. -G "Visual Studio 17 2022" -DCMAKE_TOOLCHAIN_FILE=./build/generators/conan_toolchain.cmake -DCMAKE_POLICY_DEFAULT_CMP0091=NEW
cmake --build . --config Release
robocopy ../Test Release TestOpenGlAvi.m /z
robocopy ../Test Release file_example_AVI_1920_2_3MG.avi /z
cd Realease
matlab -nosplash -noFigureWindows -r "try; cd('Release'); TestOpenGlAvi(); catch; end;"