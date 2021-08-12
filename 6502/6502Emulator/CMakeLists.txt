cmake_minimum_required(VERSION 3.7)

project( 6502Emulator )

set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DM6502_DEBUG" )	#so we can add the DEBUG preprocessor define and other flags to stay in debug mode - see https://cmake.org/Wiki/CMake_Useful_Variables#Compilers_and_Tools
set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -DM6502_DEBUG" )

if(MSVC)
	add_compile_options(/MP)				#Use multiple processors when building
	add_compile_options(/W4 /wd4201 )	#Warning level 4, all warnings are errors
else()
	add_compile_options(-W -Wall ) #All Warnings, all warnings are errors
endif()

set  (M6502_SOURCES
		"src/olcNes_Video2_2C02.cpp"
		)
		
source_group("src" FILES ${M6502_SOURCES})
		
add_executable( 6502Emulator ${M6502_SOURCES} 	)
add_dependencies( 6502Emulator M6502Lib )
target_link_libraries(6502Emulator M6502Lib)

TARGET_LINK_LIBRARIES(6502Emulator -lasound -lX11 -lGL -lpthread -lpng -lstdc++fs )
add_definitions(-std=c++14)
