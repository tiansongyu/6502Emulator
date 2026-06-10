# olcPixelGameEngine + olcPGEX_Sound（第三方单头库）。
# INTERFACE target 携带头文件路径与平台链接需求；
# 只有 GUI 前端链接它，核心库完全不依赖。
# 注意：两个头的实现块位于 include guard 之外（由 OLC_PGE_APPLICATION /
# OLC_PGEX_SOUND 宏控制），必须有且仅有一个 TU 定义这两个宏。
add_library(olc_pge INTERFACE)

target_include_directories(olc_pge SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

find_package(Threads REQUIRED)
target_link_libraries(olc_pge INTERFACE Threads::Threads)

if(APPLE)
  find_package(OpenGL REQUIRED)
  find_package(GLUT REQUIRED)
  find_package(PNG REQUIRED)
  find_library(CARBON_LIBRARY Carbon)
  target_link_libraries(olc_pge INTERFACE OpenGL::GL GLUT::GLUT PNG::PNG
                        ${CARBON_LIBRARY})
elseif(UNIX)
  target_link_libraries(olc_pge INTERFACE X11 GL png asound stdc++fs)
endif()
