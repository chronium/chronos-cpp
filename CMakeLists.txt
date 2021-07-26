cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
project(chronos VERSION 0.1 LANGUAGES ASM C CXX)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODUKE_PATH}")

include(ExternalProject)

set(SYSROOT_DIR ${CMAKE_CURRENT_BINARY_DIR}/../sysroot CACHE FILEPATH "Sysroot directory" FORCE)
message("Sysroot at ${SYSROOT_DIR}")

set(CMAKE_INSTALL_PREFIX ${SYSROOT_DIR})
set(CMAKE_INSTALL_INCLUDEDIR "usr/include")

option(BUILD_KERNEL "Build the kernel" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_VERBOSE_MAKEFILE ON)

add_compile_options(-Wall -Wno-format -Wmissing-declarations -Wformat=2 -fdiagnostics-color=always -ftls-model=initial-exec -Wundef -Wcast-qual -Wwrite-strings)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options(-Werror -Wimplicit-fallthrough -Wno-deprecated-copy -Wno-address-of-packed-member -Wno-expansion-to-defined -Wno-undef)
add_compile_options(-Wno-unused-private-field)
endif()

add_compile_options(-g1 -fstack-protector-strong -Wundef -Wcast-qual -Wwrite-strings)

if(BUILD_KERNEL)
    add_subdirectory(kernel)
endif()

set(DISK_IMAGE ${CMAKE_BINARY_DIR}/${PROJECT_NAME}-${KERNEL_ARCH}-${KERNEL_PLATFORM}.iso CACHE FILEPATH "Bootable ISO" FORCE)
message("Image at ${DISK_IMAGE}")

add_custom_target(image
    DEPENDS install
    COMMAND ${CMAKE_COMMAND} -E env "OUTPUT=${DISK_IMAGE}" "OUT=${CMAKE_BINARY_DIR}" "BASE=${CMAKE_SOURCE_DIR}" ${CMAKE_SOURCE_DIR}/iso.sh
    BYPRODUCTS ${DISK_IMAGE}
    USES_TERMINAL
)

add_custom_target(run
    DEPENDS image
    COMMAND ${CMAKE_COMMAND} -E env "IMAGE=${DISK_IMAGE}" "BASE=${CMAKE_SOURCE_DIR}" "HOST=${KERNEL_ARCH}" ${CMAKE_SOURCE_DIR}/qemu.sh
    USES_TERMINAL
)
