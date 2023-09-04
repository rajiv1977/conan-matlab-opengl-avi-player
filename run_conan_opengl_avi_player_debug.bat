@ECHO ON

set BASEDIR=%~dp0
PUSHD %BASEDIR%

RMDIR /Q /S build


conan install . -c tools.system.package_manager:mode=install -c tools.system.package_manager:sudo=True --output-folder=build --build=missing --settings=build_type=Debug
cd build
cmake .. -G "Visual Studio 17 2022" -DCMAKE_TOOLCHAIN_FILE=./build/generators/conan_toolchain.cmake -DCMAKE_POLICY_DEFAULT_CMP0091=NEW 
cmake --build . --config Debug
robocopy ../Test Debug TestOpenGlAvi.m /z
robocopy ../Test Debug file_example_AVI_1920_2_3MG.avi /z
cd Debug
matlab -nosplash -noFigureWindows -r "try; cd('Debug'); TestOpenGlAvi(); catch; end;"