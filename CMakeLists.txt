cmake_minimum_required(VERSION 3.14)

project(6502 CXX)

# 全局策略只在这里定义一次：C++17 + 警告级别。
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
  add_compile_options(/MP /W4 /wd4201)
else()
  add_compile_options(-Wall -Wextra)
endif()

# Turn on the ability to create folders to organize projects (.vcproj)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(6502/6502Lib)      # 模拟器核心（无 GUI 依赖）
add_subdirectory(6502/6502Emulator) # GUI 前端（SDL2）
add_subdirectory(tests)             # headless 回归测试（核心可独立复用的证明）
