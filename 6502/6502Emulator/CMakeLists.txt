project(6502Emulator)

# GUI 前端：olcPixelGameEngine 窗口 + olcPGEX_Sound 音频线程。
# olcImpl.cpp 是唯一定义 OLC_PGE_APPLICATION / OLC_PGEX_SOUND 的 TU。
add_executable(6502Emulator
    "src/olcNes_PPU_Backgrounds.cpp"
    "src/olcImpl.cpp"
)

target_link_libraries(6502Emulator M6502Lib olc_pge)

# 把 ROM 复制到可执行文件旁边，保证从该目录启动即可找到 ./rom/
add_custom_command(TARGET 6502Emulator POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_SOURCE_DIR}/rom $<TARGET_FILE_DIR:6502Emulator>/rom)
