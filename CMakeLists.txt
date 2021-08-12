cmake_minimum_required(VERSION 3.7)

project( 6502 )

# Turn on the ability to create folders to organize projects (.vcproj)
# It creates "CMakePredefinedTargets" folder by default and adds CMake
# defined projects like INSTALL.vcproj and ZERO_CHECK.vcproj
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Sub-directories where more CMakeLists.txt exist
#add_subdirectory(6502/6502Test)
add_subdirectory(6502/6502Lib)
add_subdirectory(6502/6502Emulator)

file(COPY ./nestest.nes DESTINATION ./6502/6502Emulator/)

if (${APPLE})

#########################################################
# FIND CARBON
#########################################################
FIND_LIBRARY(CARBON_LIBRARY Carbon)
link_libraries( ${CARBON_LIBRARY})

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
link_libraries( ${PNG_LIBRARIES})

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
link_libraries( ${GLUT_LIBRARIES})

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
link_libraries( ${OPENGL_LIBRARIES})

else()
LINK_LIBRARIES(main -lasound -lX11 -lGL -lpthread -lpng -lstdc++fs)
endif()

add_definitions(-std=c++17)