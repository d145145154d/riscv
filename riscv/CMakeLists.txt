cmake_minimum_required(VERSION 3.29)
project(riscv C)

set(CMAKE_C_STANDARD 11)

include_directories(include)

add_executable(riscv
        main.c
        src/scrambler.c
        src/qpsk_mod.c
        src/rate_matching.c
)

target_link_libraries(riscv m)
