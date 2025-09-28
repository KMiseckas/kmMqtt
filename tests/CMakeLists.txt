cmake_minimum_required (VERSION 3.14)

project ("cleanMqttTests" LANGUAGES CXX)

file(GLOB_RECURSE test_source_files "*.cpp")
add_executable(${PROJECT_NAME} ${test_source_files})

include(FetchContent)
FetchContent_Declare(
	Doctest
	GIT_REPOSITORY https://github.com/doctest/doctest.git
	GIT_TAG ae7a13539fb71f270b87eb2e874fbac80bc8dda2 #v2.4.11
)
FetchContent_MakeAvailable(Doctest)

target_include_directories(${PROJECT_NAME} PRIVATE ${doctest_SOURCE_DIR}/doctest)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_ROOT}/include/private)
target_link_libraries(${PROJECT_NAME} PRIVATE cleanMqtt)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})