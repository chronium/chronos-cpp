set(KERNEL_COMPILE_OPTS -static -ffreestanding -nodefaultlibs -nostdlib -fno-exceptions -fno-rtti -fno-pie -nostdlib)

set(KERNEL_ARCH "x86_64" CACHE STRING "Kernel CPU Architecture")
set_property(CACHE KERNEL_ARCH PROPERTY STRINGS x86_64)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/arch/${KERNEL_ARCH})

set(KERNEL_PLATFORM "pc64" CACHE STRING "Kernel system platform")
set_property(CACHE KERNEL_PLATFORM PROPERTY STRINGS pc64)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/platform/${KERNEL_PLATFORM})

add_executable(kernel
    src/init.cpp
    src/runtime/log.cpp
    src/c/stack_guard.c
    src/c/printf.cpp
    src/c/string.cpp
)

target_include_directories(kernel PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include)
target_include_directories(kernel PRIVATE ${ARCH_INCLUDE_DIR})
target_include_directories(kernel PRIVATE ${PLATFORM_INCLUDE_DIR})
target_include_directories(kernel PRIVATE src)

target_link_options(kernel PRIVATE -nostartfiles)
target_link_libraries(kernel PRIVATE ${ARCH_TARGET_NAME})
target_link_libraries(kernel PRIVATE ${PLATFORM_TARGET_NAME})

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_link_libraries(kernel PRIVATE gcc)
endif()

set_target_properties(kernel PROPERTIES OUTPUT_NAME "kernel-${KERNEL_ARCH}-${KERNEL_PLATFORM}")
set_target_properties(kernel PROPERTIES SUFFIX "${PLATFORM_KERNEL_EXTENSION}")

target_compile_options(kernel PRIVATE ${KERNEL_COMPILE_OPTS} ${ARCH_COMPILE_OPTS})
#target_compile_options(kernel PRIVATE -Os)
target_link_options(kernel PRIVATE ${KERNEL_COMPILE_OPTS} -fno-pie --static -nostartfiles)

message("Platform linker script: ${PLATFORM_LINKER_SCRIPT}")
set_target_properties(kernel PROPERTIES LINK_DEPENDS ${PLATFORM_LINKER_SCRIPT})
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_link_options(kernel PRIVATE "-Wl,-T${PLATFORM_LINKER_SCRIPT}")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_link_options(kernel PRIVATE "-T${PLATFORM_LINKER_SCRIPT}")
endif()

# install the kernel to the boot directory
install(TARGETS kernel RUNTIME DESTINATION ${SYSROOT_DIR}/boot)
