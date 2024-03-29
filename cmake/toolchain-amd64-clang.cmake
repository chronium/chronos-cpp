set(CMAKE_SYSTEM_NAME amd64-chronos)
set(CMAKE_SYSTEM_PROCESSOR "x86_64")

set(TARGET_TRIPLE "amd64-pc-unknown-elf")

SET(ARCH_FLAGS "-march=x86-64")

SET(CMAKE_SYSROOT ~/chronos/sysroot)
get_filename_component(CMAKE_SYSROOT ${CMAKE_SYSROOT} ABSOLUTE)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SYSROOT}/../cmake/)

set(CMAKE_ASM_COMPILER "clang")

set(CMAKE_C_COMPILER clang)
set(CMAKE_C_COMPILER_TARGET ${triple})
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_COMPILER_TARGET ${triple})

set(CMAKE_ASM_NASM_COMPILER "nasm")

set(CMAKE_ASM_NASM_FLAGS "" CACHE STRING "" FORCE)
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDEX> -o <OBJECT> <SOURCE>")

set(CMAKE_ASM_FLAGS "${ASM_FLAGS} -target ${TARGET_TRIPLE} ${ARCH_FLAGS}" CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS "${C_CXX_FLAGS} ${ARCH_FLAGS}" CACHE STRING "" FORCE)
SET(CMAKE_CXX_FLAGS "${C_CXX_FLAGS} ${ARCH_FLAGS}" CACHE STRING "" FORCE)
SET(CMAKE_EXE_LINKER_FLAGS_INIT "-fuse-ld=lld")

set(CMAKE_ASM_COMPILER_WORKS 1)
set(CMAKE_ASM_NASM_COMPILER_WORKS 1)
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)
