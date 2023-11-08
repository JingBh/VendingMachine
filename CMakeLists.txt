cmake_minimum_required(VERSION 3.26)
project(VendingMachine)

set(CMAKE_CXX_STANDARD 20)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG=1)
endif ()

add_executable(VendingMachine src/main.cpp
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
)
