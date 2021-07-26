add_library(kernel_platform_pc64 OBJECT
    src/boot.cpp
    src/io/serial.cpp
    src/time/cmos.cpp
)

target_include_directories(kernel_platform_pc64 PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../../src)
target_include_directories(kernel_platform_pc64 PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../../include)
target_include_directories(kernel_platform_pc64 PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)
target_include_directories(kernel_platform_pc64 PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include)

target_include_directories(kernel_platform_pc64 PRIVATE ${ARCH_INCLUDE_DIR})

target_compile_options(kernel_platform_pc64 PRIVATE ${KERNEL_COMPILE_OPTS} ${ARCH_COMPILE_OPTS})

set(PLATFORM_TARGET_NAME "kernel_platform_pc64" CACHE STRING "Platform target name" FORCE)
set(PLATFORM_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include CACHE FILEPATH "Platform include directory" FORCE)

set(PLATFORM_KERNEL_EXTENSION ".elf" CACHE STRING "File extension for kernel binary")
set(PLATFORM_LINKER_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/linker.ld CACHE FILEPATH "Platform specific linker script")

install(FILES ${CMAKE_CURRENT_LIST_DIR}/limine.cfg DESTINATION ${SYSROOT_DIR}/boot)
