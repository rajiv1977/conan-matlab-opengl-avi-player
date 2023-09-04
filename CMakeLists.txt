cmake_minimum_required(VERSION 3.27.0)

project(conan-opengl-avi-player LANGUAGES CXX C VERSION 0.0.1)
set(CMAKE_CXX_STANDARD 20)

find_package(ffmpeg REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glew REQUIRED)

if(WIN32) 
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /STACK:100000000")
find_package(freeglut REQUIRED)
find_package(zlib REQUIRED)
endif(WIN32) 

# find Matlab's matrix and mex libraries
find_package(Matlab COMPONENTS MAT_LIBRARY MX_LIBRARY)
if (NOT Matlab_FOUND)
    message(FATAL_ERROR "Matlab dependencies not found. Is the MATLAB_PATH environment variable set?")
endif()

# build the mex
matlab_add_mex(
    NAME openglAviMex
    SHARED 
	SRC source/Decoder.cpp
		source/openglAviMex.cpp
		source/RoxluPlayer.cpp
		include/Decoder.h
		include/Player.h
		include/TinyLib.h
		include/RoxluPlayer.h
    LINK_TO ffmpeg::ffmpeg Freetype::Freetype glu::glu opengl::opengl glfw GLEW::GLEW ZLIB::ZLIB
)
target_include_directories(openglAviMex PRIVATE ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/source)

set(CPACK_NSIS_CONTACT "rajiv.sithiravel@gmail.com")

include(CTest) 
# CTest sets the BUILD_TESTING variable to ON
if (BUILD_TESTING)

add_executable( conan-opengl-avi-player
				include/Decoder.h
                include/TinyLib.h
				include/RoxluPlayer.h
				source/Decoder.cpp
				Test/TestOpenGLAviPlayer.cpp
				source/RoxluPlayer.cpp)

if(WIN32) 				
target_compile_options(${PROJECT_NAME} PRIVATE "/Od")   
endif(WIN32)
				
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(conan-opengl-avi-player ffmpeg::ffmpeg Freetype::Freetype glu::glu opengl::opengl glfw GLEW::GLEW ZLIB::ZLIB)

endif()

