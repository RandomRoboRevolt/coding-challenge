include(FetchContent)

## look for test frameworks and get them if not already there
find_package(Catch2 CONFIG 3.5)
if(NOT Catch2_FOUND)
  message("-- Installing Catch2 from git...")
  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.0
    GIT_SHALLOW TRUE)
  FetchContent_GetProperties(Catch2)
  if(NOT catch2_POPULATED)
    FetchContent_Populate(Catch2)
    add_subdirectory(${catch2_SOURCE_DIR} ${catch2_BINARY_DIR} EXCLUDE_FROM_ALL)
  endif()
  set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}"
                        "${CMAKE_BINARY_DIR}/_deps/catch2-src/contrib")
endif()

find_package(trompeloeil CONFIG)
if(NOT trompeloeil_FOUND)
  message("-- Installing Trompeloeil from git...")
  FetchContent_Declare(
    trompeloeil
    GIT_REPOSITORY https://github.com/rollbear/trompeloeil.git
    GIT_TAG v46
    GIT_SHALLOW TRUE)
  FetchContent_GetProperties(trompeloeil)
  if(NOT trompeloeil_POPULATED)
    FetchContent_Populate(trompeloeil)
    add_subdirectory(${trompeloeil_SOURCE_DIR} ${trompeloeil_BINARY_DIR}
                     EXCLUDE_FROM_ALL)
  endif()
endif()

include(Catch)
set(TESTFRAMEWORK_LIBRARIES "Catch2::Catch2WithMain" "trompeloeil")

add_executable(state-machine-utest "")

target_sources(state-machine-utest PRIVATE ${CMAKE_CURRENT_LIST_DIR}/all_test_cases_here.cpp)
target_include_directories(state-machine-utest PRIVATE
                           ${CMAKE_CURRENT_BINARY_DIR}/../motordriver/;${CMAKE_CURRENT_SOURCE_DIR}/../motordriver/include/;${CMAKE_CURRENT_SOURCE_DIR}/../master/include/)
target_link_libraries(state-machine-utest PRIVATE
                     sml::sml
                     Catch2::Catch2WithMain
                     trompeloeil)

catch_discover_tests(state-machine-utest)
