cmake_minimum_required(VERSION 3.25)
project(tango_algorithm)

# <---------- set c++ standard ------------->
# NOTE: you must build redis-plus-plus and your application code with the same standard.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(tango_algorithm main.cpp network.hpp task.hpp base_algorithm.hpp SM_CTP.hpp SM_GRRS.hpp)

# <------------ add hiredis dependency --------------->
find_path(HIREDIS_HEADER hiredis)
target_include_directories(${PROJECT_NAME} PUBLIC ${HIREDIS_HEADER})

find_library(HIREDIS_LIB hiredis)
target_link_libraries(${PROJECT_NAME} ${HIREDIS_LIB})

# <------------ add redis-plus-plus dependency -------------->
# NOTE: this should be *sw* NOT *redis++*
find_path(REDIS_PLUS_PLUS_HEADER sw)
target_include_directories(${PROJECT_NAME} PUBLIC ${REDIS_PLUS_PLUS_HEADER})

find_library(REDIS_PLUS_PLUS_LIB redis++)
target_link_libraries(${PROJECT_NAME} ${REDIS_PLUS_PLUS_LIB})


