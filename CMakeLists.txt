cmake_minimum_required(VERSION 3.10)
project(HW2)

set(CMAKE_CXX_STANDARD 17)

add_executable(adduser
	src/add_user_main.cpp
	src/CryptoEval.cpp
	src/User.cpp)

add_executable(Client
	src/client_main.cpp
	src/Client.cpp
	src/Session.cpp
	src/Logger.cpp
	src/User.cpp
	src/IP.cpp
	src/CryptoEval.cpp)

add_executable(Server
	src/server_main.cpp
	src/Server.cpp
	src/Session.cpp
	src/Logger.cpp
	src/User.cpp
	src/IP.cpp
	src/CryptoEval.cpp)

target_include_directories(adduser PRIVATE include)

target_include_directories(Client PRIVATE include)

target_include_directories(Server PRIVATE include)

TARGET_LINK_LIBRARIES(Client pthread argon2)
TARGET_LINK_LIBRARIES(Server pthread argon2)
TARGET_LINK_LIBRARIES(adduser pthread argon2)
