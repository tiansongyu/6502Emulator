project(nes_tests)

# headless 回归 harness：只链接核心库，不需要任何 GUI/音频/图形依赖。
# 它本身就是"核心可作为独立库复用"的持续验证。
add_executable(nes_headless nes_headless.cpp)

find_package(Threads REQUIRED)
target_link_libraries(nes_headless M6502Lib Threads::Threads)
