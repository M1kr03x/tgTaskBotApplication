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
   # src/userInterface.cpp
    src/taskManager.cpp
    src/userManager.cpp
)
#psgres
set(PostgreSQL_ROOT "G:/PostgreSQL/17")
find_package(PostgreSQL REQUIRED)
find_library(PQXX_LIBRARY NAMES libpqxx pqxx)

#tg
set(VCPKG_PATH "C:/Users/M1kr0/vcpkg/installed/x64-windows")
find_path(TGBOT_INCLUDE NAMES tgbot/tgbot.h PATHS ${VCPKG_PATH}/include NO_DEFAULT_PATH)
find_library(TGBOT_LIB NAMES tgbot PATHS ${VCPKG_PATH}/lib NO_DEFAULT_PATH)
#json
find_package(nlohmann_json CONFIG REQUIRED)

if(TGBOT_INCLUDE AND TGBOT_LIB)
    include_directories(${TGBOT_INCLUDE})
endif()


target_link_libraries(tgBot
    ${PostgreSQL_LIBRARIES}
    ${PQXX_LIBRARY}
    ${TGBOT_LIB}
)
target_link_libraries(tgBot
    ${PostgreSQL_LIBRARIES}
    ${PQXX_LIBRARY}
    ${TGBOT_LIB}
    nlohmann_json::nlohmann_json 
)