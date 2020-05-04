cmake_minimum_required(VERSION 3.15)
project(TIN_Tablica_ogloszen)

set(CMAKE_CXX_STANDARD 14)

find_path(libpqxx_INCLUDE_DIR /pqxx/pqxx)
include(SelectLibraryConfigurations)
find_library(libpqxx_LIBRARY_RELEASE NAMES libpqxx)
find_library(libpqxx_LIBRARY_DEBUG NAMES libpqxxd)
select_library_configurations(libpqxx)

add_executable(client client.h client.cpp)
target_link_libraries(client -lpqxx)
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}  -Wall -pthread -lboost_system")
