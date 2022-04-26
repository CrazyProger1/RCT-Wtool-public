cmake_minimum_required(VERSION 3.19)
project(Bot)

set(CMAKE_CXX_STANDARD 20)
set(CURL_LIBRARY ${CMAKE_SOURCE_DIR}/libs/curl-7.79.1-win64-mingw/lib)
set(CURL_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/libs/curl-7.79.1-win64-mingw/include)
set(CMAKE_EXE_LINKER_FLAGS "-static-libstdc++ -static-libgcc ${CMAKE_EXE_LINKER_FLAGS}")
find_package(CURL)


include_directories(${CURL_INCLUDE_DIRS})
link_directories(${CURL_LIBRARIES})


add_executable(${PROJECT_NAME} WIN32 srcs/main.cpp srcs/Config.cpp include/Config.h include/types.h srcs/logging.cpp include/logging.h srcs/Requests.cpp srcs/Response.cpp include/Requests.h include/Response.h srcs/Bot.cpp include/Bot.h srcs/Message.cpp include/Message.h srcs/App.cpp include/App.h)# WIN32
target_link_libraries(${PROJECT_NAME} curl stdc++fs)