cmake_minimum_required(VERSION 3.15)
project(TIN_Tablica_ogloszen)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -Wall -pthread -lboost_system")
add_definitions(-DBOOST_ERROR_CODE_HEADER_ONLY)

find_package(spdlog 0.16.2)
find_package(Boost COMPONENTS unit_test_framework REQUIRED)

find_path(libpqxx_INCLUDE_DIR /pqxx/pqxx)
include(SelectLibraryConfigurations)
find_library(libpqxx_LIBRARY_RELEASE NAMES libpqxx)
find_library(libpqxx_LIBRARY_DEBUG NAMES libpqxxd)
select_library_configurations(libpqxx)



#server
add_executable(server sockets/server.cpp sockets/communication/RequestHandler.cpp sockets/communication/RequestHandler.h sockets/communication/JSONParser.cpp sockets/communication/JSONParser.h sockets/communication/Message.h sockets/communication/Message.cpp sockets/database/database.h sockets/database/database.cpp)
#tests
add_executable(JSONParserTest Tests/JSONParserTest.cpp sockets/communication/JSONParser.h sockets/communication/JSONParser.cpp)

target_link_libraries(JSONParserTest ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} nlohmann_json::nlohmann_json)

add_test(boostUnitTestsRun JSONParserTest)

#tests

enable_testing()

#JSON parser
include(FetchContent)

FetchContent_Declare(json
        GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent
        GIT_TAG v3.7.3)

FetchContent_GetProperties(json)
if(NOT json_POPULATED)
    FetchContent_Populate(json)
    add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

target_link_libraries(server PRIVATE nlohmann_json::nlohmann_json -lpqxx)

add_executable(client sockets/client.h sockets/client.cpp sockets/communication/JSONParser.cpp sockets/communication/JSONParser.h sockets/communication/RequestHandler.cpp sockets/communication/RequestHandler.h sockets/database/database.h sockets/database/database.cpp)
target_link_libraries(client PRIVATE nlohmann_json::nlohmann_json -lpqxx)
