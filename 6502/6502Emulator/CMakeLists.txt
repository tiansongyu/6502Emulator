cmake_minimum_required(VERSION 3.7)

project( 6502Emulator )

set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DM6502_DEBUG" )	#so we can add the DEBUG preprocessor define and other flags to stay in debug mode - see https://cmake.org/Wiki/CMake_Useful_Variables#Compilers_and_Tools
set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -DM6502_DEBUG" )
set(CMAKE_CXX_STANDARD 17)

if(MSVC)
	add_compile_options(/MP)				#Use multiple processors when building
	add_compile_options(/W4 /wd4201 )	#Warning level 4, all warnings are errors
else()
	add_compile_options(-W -Wall ) #All Warnings, all warnings are errors
endif()

set  (M6502_SOURCES
		"src/olcNes_PPU_Backgrounds.cpp"
		)
		
source_group("src" FILES ${M6502_SOURCES})
		
add_executable( 6502Emulator ${M6502_SOURCES} 	)
add_dependencies( 6502Emulator M6502Lib )
target_link_libraries(6502Emulator M6502Lib)

if (${APPLE})

#########################################################
# FIND CARBON
#########################################################
FIND_LIBRARY(CARBON_LIBRARY Carbon)
target_link_libraries(6502Emulator ${CARBON_LIBRARY})

#########################################################
# FIND PNG
#########################################################
find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIRS})
link_directories(${PNG_LIBRARY_DIRS})
add_definitions(${PNG_DEFINITIONS})
if(NOT PNG_FOUND)
    message(ERROR " PNG not found!")
endif(NOT PNG_FOUND)
target_link_libraries(6502Emulator ${PNG_LIBRARIES})

#########################################################
# FIND GLUT
#########################################################
find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIRS})
link_directories(${GLUT_LIBRARY_DIRS})
add_definitions(${GLUT_DEFINITIONS})
if(NOT GLUT_FOUND)
    message(ERROR " GLUT not found!")
endif(NOT GLUT_FOUND)
target_link_libraries(6502Emulator ${GLUT_LIBRARIES})

#########################################################
# FIND OPENGL
#########################################################
find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})
if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif(NOT OPENGL_FOUND)
target_link_libraries(6502Emulator ${OPENGL_LIBRARIES})
else()
target_LINK_LIBRARIES(6502Emulator -lasound -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++17)
endif()

