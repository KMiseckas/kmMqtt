cmake_minimum_required (VERSION 3.14)

project ("cleanMqttTests" LANGUAGES CXX)

file(GLOB_RECURSE test_source_files "*.cpp" "*.h")
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

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
	target_compile_options(${PROJECT_NAME} PRIVATE -Wno-error=switch)
endif()

#Sanitizer link options added.
target_link_options(${PROJECT_NAME} PRIVATE ${CLEANMQTT_SANITIZER_FLAGS})
set_target_properties(${PROJECT_NAME} PROPERTIES ENVIRONMENT "MSAN_OPTIONS=halt_on_error=false")