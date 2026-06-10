project(M6502Lib)

# 模拟器核心：CPU / PPU / APU / 总线 / 卡带 / Mapper。
# 不依赖任何 GUI 引擎，可作为独立库复用（见 tests/nes_headless）。
set(M6502_SOURCES
    "src/public/Bus.h"
    "src/public/Bus.cpp"
    "src/public/Cartridge.h"
    "src/public/Cartridge.cpp"
    "src/public/Mapper.h"
    "src/public/Mapper.cpp"
    "src/public/Mapper_000.h"
    "src/public/Mapper_000.cpp"
    "src/public/Mapper_001.h"
    "src/public/Mapper_001.cpp"
    "src/public/Mapper_002.h"
    "src/public/Mapper_002.cpp"
    "src/public/Mapper_003.h"
    "src/public/Mapper_003.cpp"
    "src/public/Mapper_004.h"
    "src/public/Mapper_004.cpp"
    "src/public/Mapper_066.h"
    "src/public/Mapper_066.cpp"
    "src/public/Nes2C02.h"
    "src/public/Nes2C02.cpp"
    "src/public/Nes6502.h"
    "src/public/Nes6502.cpp"
    "src/public/Nes2A03.h"
    "src/public/Nes2A03.cpp"
)

source_group("src" FILES ${M6502_SOURCES})

add_library(M6502Lib ${M6502_SOURCES})

target_include_directories(M6502Lib PUBLIC "${PROJECT_SOURCE_DIR}/src/public")
