cmake_minimum_required(VERSION 3.20)
project(CTD_Server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ==================================
# Server Source
# ==================================
file(GLOB_RECURSE SERVER_SOURCES
    src/*.cpp
)

add_executable(
    CTD_Server
    ${SERVER_SOURCES}
)

target_include_directories(
    CTD_Server
    PRIVATE
        include
        ThirdParty/asio-1.38.2/include
)

target_compile_definitions(
    CTD_Server
    PRIVATE
        ASIO_STANDALONE
        _WIN32_WINNT=0x0A00
)

find_package(unofficial-mysql-connector-cpp CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)

target_link_libraries(
    CTD_Server
    PRIVATE
        ws2_32
        mswsock
        unofficial::mysql-connector-cpp::connector
        OpenSSL::Crypto
)