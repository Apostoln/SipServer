cmake_minimum_required(VERSION 3.4)
project(SipServer)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_STANDARD 14)
set(GCC_COVERAGE_COMPILE_FLAGS "-DASIO_STANDALONE -pthread ")

add_library(argparse INTERFACE )
target_include_directories(argparse INTERFACE lib/argparse )

add_library(asio INTERFACE )
target_include_directories(asio INTERFACE lib/asio)

add_library(easyloggingpp lib/easyloggingpp/src/easylogging++.cc)
target_include_directories(easyloggingpp INTERFACE lib/easyloggingpp/src)

include_directories(src)

set(SOURCE_FILES main.cpp src/SipServer.hpp src/SipServer.cpp src/Builder.cpp src/Builder.hpp src/utils.hpp src/ErrorCode.hpp src/ExitException.cpp src/ExitException.hpp src/Registrar.cpp src/Registrar.hpp src/SipUser.hpp src/constants.hpp src/SipUser.cpp src/AuthManager.hpp src/AuthManager.cpp src/SipAccount.hpp src/SipAccount.cpp src/Db.hpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

include_directories(/usr/include)
link_directories(/usr/lib)

target_link_libraries(${PROJECT_NAME} stdc++ asio argparse easyloggingpp pthread resip rutil dum sqlite3)