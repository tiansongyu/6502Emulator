cmake_minimum_required(VERSION 3.7)

project( M6502Lib )

set(CMAKE_CXX_STANDARD 17)

if(MSVC)
	add_compile_options(/MP)				#Use multiple processors when building
	add_compile_options(/W4 /wd4201)	#Warning level 4, all warnings are errors
else()
	add_compile_options(-W -Wall) #All Warnings, all warnings are errors
endif()

set  (M6502_SOURCES
	"src/public/Bus.h"
	"src/public/Bus.cpp"
	"src/public/Cartridge.h"
	"src/public/Cartridge.cpp"
	"src/public/Mapper.h"
	"src/public/Mapper.cpp"
	"src/public/Mapper_000.h"
	"src/public/Mapper_000.cpp"
	"src/public/Mapper_002.h"
	"src/public/Mapper_002.cpp"
	"src/public/Mapper_003.h"
	"src/public/Mapper_003.cpp"
	"src/public/Mapper_066.h"
	"src/public/Mapper_066.cpp"
	"src/public/olc2C02.h"
	"src/public/olc2C02.cpp"
	"src/public/olc6502.h"
	"src/public/olc6502.cpp"
 	"src/public/olcPGEX_Sound.h"
	"src/public/olcPixelGameEngine.h"
	"src/public/olc2A03.cpp"
	"src/public/olc2A03.h"
	)
		
source_group("src" FILES ${M6502_SOURCES})
		
add_library( M6502Lib ${M6502_SOURCES} )

target_include_directories ( M6502Lib PUBLIC "${PROJECT_SOURCE_DIR}/src/public")
target_include_directories ( M6502Lib PRIVATE "${PROJECT_SOURCE_DIR}/src/private")

