cmake_minimum_required(VERSION 3.15)
project(TIN_Tablica_ogloszen)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -Wall -pthread -lboost_system")
add_definitions(-DBOOST_ERROR_CODE_HEADER_ONLY)

find_package(spdlog 0.16.2)
find_package(Boost COMPONENTS unit_test_framework REQUIRED)

#server
add_executable(server server.cpp RequestHandler.cpp RequestHandler.h)

#tests
add_executable(RequestHandlerTest Tests/RequestHandlerTest.cpp)
target_link_libraries(RequestHandlerTest ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_test(boostUnitTestsRun RequestHandlerTest)

enable_testing()

# indicates the include paths
#target_include_directories(RequestHandlerTest PRIVATE ${Boost_INCLUDE_DIRS})
# indicates the shared library variant
#target_compile_definitions(RequestHandlerTest PRIVATE "BOOST_TEST_DYN_LINK=1")
# indicates the link paths
#target_link_libraries(RequestHandlerTest ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
