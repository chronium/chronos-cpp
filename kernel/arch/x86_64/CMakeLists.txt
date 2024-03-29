add_library(kernel_arch_amd64 OBJECT
    src/exceptions.cpp
    src/exceptions.S
    src/gdt.cpp
    src/gdt.S
    src/idt.cpp
    src/init.cpp
)

set(ARCH_COMPILE_OPTS -mcmodel=kernel -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -fno-omit-frame-pointer CACHE STRING "Architecture specific compile flags")

target_compile_options(kernel_arch_amd64 PRIVATE ${KERNEL_COMPILE_OPTS} ${ARCH_COMPILE_OPTS})

target_include_directories(kernel_arch_amd64 PUBLIC include)
target_include_directories(kernel_arch_amd64 PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../../include)
target_include_directories(kernel_arch_amd64 PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../../src)
target_include_directories(kernel_arch_amd64 PRIVATE src)
target_include_directories(kernel_arch_amd64 PRIVATE ${PLATFORM_INCLUDE_DIR})

set(ARCH_TARGET_NAME "kernel_arch_amd64" CACHE STRING "Architecture target name" FORCE)
set(ARCH_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include CACHE FILEPATH "Architecture include directory" FORCE)
