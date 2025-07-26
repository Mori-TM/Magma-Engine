# Minimum CMake version
if(WIN32)
cmake_minimum_required(VERSION 3.80)
else()
cmake_minimum_required(VERSION 3.10)
endif()

# Project Name and C++ Standard
project(MyProject LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    message(STATUS "Configuring for Windows: Using MSVC")
    set(CMAKE_C_COMPILER "cl.exe")
    set(CMAKE_CXX_COMPILER "cl.exe")
else()
    # Explicitly specify Clang as the compiler
    set(CMAKE_C_COMPILER "/usr/bin/clang")
    set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
endif()

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
add_executable(MyProject ${SOURCES})
#add_executable(MyProject src/main.cpp include/FileDialogs/dialogs.cpp include/Process/process.cpp)  # Explicitly list files or use proper globs

# Copy data folder to build directory
#add_custom_command(
#  TARGET MyProject POST_BUILD
#  COMMAND ${CMAKE_COMMAND} -E copy_directory
#          ${CMAKE_SOURCE_DIR}/src/data
#          $<TARGET_FILE_DIR:MyProject>/data
#  COMMENT "Copying data directory to output dir"
#)

# Remove Unicode definitions (if already defined)
# and add MBCS definition
if(WIN32)
target_compile_definitions(MyProject PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:_MBCS>
)
endif()

#Set VS Working Directory to src/
set_property(TARGET MyProject PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/src")

#target
target_compile_definitions(MyProject PRIVATE SOURCE_DIR="${CMAKE_SOURCE_DIR}")

# Build FileDialogs CLI
#execute_process(COMMAND make -C ${CMAKE_CURRENT_SOURCE_DIR}/include/FileDialogs)

### Libraries ###

# Use FetchContent to automatically download dependencies
include(FetchContent)

# 3. Vulkan
find_package(Vulkan REQUIRED)
if(Vulkan_FOUND)
    include_directories(${Vulkan_INCLUDE_DIRS})
    target_link_libraries(MyProject ${Vulkan_LIBRARIES})
else()
    message(FATAL_ERROR "Vulkan library not found.")
endif()

# 4. SDL3
find_package(SDL3 QUIET)  # Try to find it first
if(NOT SDL3_FOUND)
    message(STATUS "SDL3 not found. Fetching from source...")
    FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.2.4
    )
    FetchContent_MakeAvailable(SDL3)
endif()
if(SDL3_FOUND OR TARGET SDL3::SDL3)
    target_link_libraries(MyProject SDL3::SDL3)
else()
    message(FATAL_ERROR "SDL3 library could not be found or built.")
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
