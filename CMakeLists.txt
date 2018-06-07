cmake_minimum_required(VERSION 2.8.7)

project(app-monero)

option(ANDROID "Build for android")
option(IOS "Build for ios")


if (IOS)
    INCLUDE(CMakeLists_IOS.txt)
    set(CMAKE_OSX_SYSROOT "${CMAKE_IOS_SDK_ROOT}")
    message("${CMAKE_OSX_SYSROOT}")
endif()
set(ARCH_FLAG "-march=armv8 -arch arm64")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ${ARCH_FLAG} -isysroot ${CMAKE_OSX_SYSROOT}")
set(MONERO_ROOT ${PROJECT_SOURCE_DIR}/../monero)
set(SRC main.cpp)

include_directories(${MONERO_ROOT})
include_directories(${MONERO_ROOT}/external/easylogging++)

set(libs
    wallet_merged
    epee
    easylogging
    lmdb
    boost_serialization
    boost_thread
    boost_system
    boost_date_time
    boost_filesystem
    boost_chrono
    boost_program_options
    boost_regex
    unbound
    ssl
    crypto
    z)

if (ANDROID)
    link_directories(
        /opt/android/monero/armeabi-v7a/
        /opt/android/lib/armeabi-v7a/
        /opt/android/lib/armeabi-v7a/boost
    )
elseif(IOS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I ${BOOST_ROOT}")
    link_directories(${PROJECT_SOURCE_DIR}/../monero-libs/ios-arm64)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

    set(libs ${libs} dl device pcsclite)

    link_directories(
        /usr/lib/x86_64-linux-gnu/
        ${MONERO_ROOT}/x86/lib
    )
endif()

add_executable(app-monero ${SRC})
target_link_libraries(app-monero ${libs})
