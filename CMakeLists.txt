cmake_minimum_required(VERSION 4.2.3)
project(tgBot)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_executable(tgBot
    src/main.cpp
    src/database.cpp
    src/bot.cpp
    src/task.cpp
    src/user.cpp
    src/userInterface.cpp
    src/taskManager.cpp
    src/userManager.cpp
    database.cpp
)
find_package(PostgreSQL REQUIRED)
find_library(PQXX_LIBRARY NAMES libpqxx pqxx)

target_link_libraries(tgBot
    ${PostgreSQL_LIBRARIES}
    ${PQXX_LIBRARY}
)