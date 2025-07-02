# Minimum CMake version
cmake_minimum_required(VERSION 3.10)

# Project Name and C++ Standard
project(MyProject LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Explicitly specify Clang as the compiler
set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")

# Use ccache if available
set(CMAKE_C_COMPILER_LAUNCHER ccache)
set(CMAKE_CXX_COMPILER_LAUNCHER ccache)

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-w)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftime-trace -g -ggdb")
#    add_compile_options(-Wno-c++11-narrowing)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-w)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -ggdb")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/W0)
endif()

# Output Directories
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/src)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include directories for header files
include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

# Collect all source files
file(GLOB_RECURSE SOURCES "src/*.cpp" "include/*.h")

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")
list(FILTER SOURCES EXCLUDE REGEX "${CMAKE_BINARY_DIR}/.*")  # Exclude build directory

# Add Executable
#add_executable(MyProject ${SOURCES})
add_executable(MyProject src/main.cpp include/FileDialogs/dialogs.cpp include/Process/process.cpp)  # Explicitly list files or use proper globs

# Build FileDialogs CLI
execute_process(COMMAND make -C ${CMAKE_CURRENT_SOURCE_DIR}/include/FileDialogs)

### Libraries ###

# 3. Vulkan
find_package(Vulkan REQUIRED)
if(Vulkan_FOUND)
    include_directories(${Vulkan_INCLUDE_DIRS})
    target_link_libraries(MyProject ${Vulkan_LIBRARIES})
else()
    message(FATAL_ERROR "Vulkan library not found.")
endif()

# 4. SDL3
find_package(SDL3 REQUIRED)
if(SDL3_FOUND)
    include_directories(${SDL3_INCLUDE_DIRS})
    target_link_libraries(MyProject ${SDL3_LIBRARIES})
else()
    message(FATAL_ERROR "SDL3 library not found.")
endif()

# 5. Process
if(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    target_link_libraries(MyProject elf kvm pthread)
elseif(CMAKE_SYSTEM_NAME STREQUAL "DragonFly")
    target_link_libraries(MyProject kvm pthread)
elseif(CMAKE_SYSTEM_NAME STREQUAL "NetBSD")
    target_link_libraries(MyProject kvm pthread)
elseif(CMAKE_SYSTEM_NAME STREQUAL "OpenBSD")
    target_link_libraries(MyProject kvm pthread)
elseif(CMAKE_SYSTEM_NAME STREQUAL "SunOS")
    target_link_libraries(MyProject kvm proc)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(MyProject ntdll)
endif()

# Post Configuration Message
message(STATUS "Configured MyProject with the following libraries:")
message(STATUS "  - Vulkan")
message(STATUS "  - SDL3")
