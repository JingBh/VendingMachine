cmake_minimum_required(VERSION 3.20)
project(VendingMachine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

# Enable UTF-8 support for MSVC
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG=1)
endif ()

# Install JSON library
add_subdirectory(lib/nlohmann/json)

# Install HTTP library
set(HTTPLIB_INSTALL OFF)
add_subdirectory(lib/yhirose/cpp-httplib)

# Declare project targets

add_library(VendingMachineCore STATIC
        src/core/contract/HasPersistence.cpp
        src/core/contract/HasPersistence.h
        src/core/contract/HasQuantity.cpp
        src/core/contract/HasQuantity.h
        src/core/error/NotSufficientError.h
        src/core/error/ValueError.h
        src/core/Cash.cpp
        src/core/Cash.h
        src/core/Good.cpp
        src/core/Good.h
        src/core/Money.cpp
        src/core/Money.h
        src/core/VendingMachine.cpp
        src/core/VendingMachine.h
)

target_link_libraries(VendingMachineCore PUBLIC nlohmann_json::nlohmann_json)

add_executable(VendingMachine src/main.cpp
        src/ui/TUI.cpp
        src/ui/TUI.h
        src/ui/WebUI.cpp
        src/ui/WebUI.h
)

# Decouple the web assets
set(WEB_ASSETS_HEADER_DIR "web/dist/include")
if(EXISTS "${CMAKE_SOURCE_DIR}/${WEB_ASSETS_HEADER_DIR}")
    include_directories(${WEB_ASSETS_HEADER_DIR})
else()
    add_definitions(-DVENDINGMACHINE_WEBUI_NOASSET=1)
endif()

target_link_libraries(VendingMachine PRIVATE VendingMachineCore)
target_link_libraries(VendingMachine PRIVATE httplib::httplib)

# Below are the configurations for testing
# requires the Catch2 library to be checked out

option(VENDINGMACHINE_BUILD_TESTS "Build tests" ON)
if (NOT EXISTS "${CMAKE_SOURCE_DIR}/lib/catchorg/Catch2/CMakeLists.txt")
    set(VENDINGMACHINE_BUILD_TESTS OFF)
endif()
if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND VENDINGMACHINE_BUILD_TESTS)
    add_subdirectory(lib/catchorg/Catch2)

    add_executable(VendingMachineTests
            tests/Cash.cpp
            tests/Good.cpp
            tests/Money.cpp
    )

    target_link_libraries(VendingMachineTests PRIVATE VendingMachineCore)
    target_link_libraries(VendingMachineTests PRIVATE Catch2::Catch2WithMain)

    include(CTest)
    include(Catch)
    catch_discover_tests(VendingMachineTests)
endif()
