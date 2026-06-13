project(6502Emulator)

# GUI 前端：SDL2 窗口/渲染/输入/音频。核心库（M6502Lib）完全不依赖它。
# 单线程 + 音频队列起搏，详见 src/sdl2_nes.cpp。
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)

add_executable(6502Emulator "src/sdl2_nes.cpp")

target_link_libraries(6502Emulator M6502Lib PkgConfig::SDL2)

# 把 ROM 复制到可执行文件旁边，保证从该目录启动即可找到 ./rom/
add_custom_command(TARGET 6502Emulator POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_SOURCE_DIR}/rom $<TARGET_FILE_DIR:6502Emulator>/rom)
