cmake_minimum_required(VERSION 3.26)
project(VendingMachine)

set(CMAKE_CXX_STANDARD 20)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG=1)
endif ()

add_subdirectory(lib/nlohmann/json)

set(HTTPLIB_INSTALL OFF)
add_subdirectory(lib/yhirose/cpp-httplib)

add_executable(VendingMachine src/main.cpp
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
        src/ui/TUI.cpp
        src/ui/TUI.h
        src/ui/WebUI.cpp
        src/ui/WebUI.h
)

set(WEB_ASSETS_HEADER_DIR "web/dist/include")
if(EXISTS "${CMAKE_SOURCE_DIR}/${WEB_ASSETS_HEADER_DIR}")
    include_directories(${WEB_ASSETS_HEADER_DIR})
else()
    add_definitions(-DVENDINGMACHINE_WEBUI_NOASSET=1)
endif()

target_link_libraries(VendingMachine PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(VendingMachine PRIVATE httplib::httplib)
