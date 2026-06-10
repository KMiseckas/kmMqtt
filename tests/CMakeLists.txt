cmake_minimum_required (VERSION 3.5)

project ("kmMqttTests" LANGUAGES CXX)

file(GLOB_RECURSE test_source_files "*.cpp" "*.h")
add_executable(${PROJECT_NAME} ${test_source_files})

include(FetchContent)
FetchContent_Declare(
	Doctest
	GIT_REPOSITORY https://github.com/doctest/doctest.git
	GIT_TAG 6804767ee637789db8a5cb281381cae98dc36906 #v2.5.2
)
FetchContent_MakeAvailable(Doctest)

target_include_directories(${PROJECT_NAME} PRIVATE ${doctest_SOURCE_DIR}/doctest)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_ROOT}/include/private)
target_link_libraries(${PROJECT_NAME} PRIVATE kmMqtt)

# Include standard C++ headers (originally added to fix warnings due to specilizations of std::tuple)
target_compile_definitions(${PROJECT_NAME} PRIVATE
    DOCTEST_CONFIG_USE_STD_HEADERS
)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
	target_compile_options(${PROJECT_NAME} PRIVATE -Wno-error=switch)
endif()

#Sanitizer link options added.
target_link_options(${PROJECT_NAME} PRIVATE ${KMMQTT_SANITIZER_FLAGS})
set_target_properties(${PROJECT_NAME} PROPERTIES ENVIRONMENT "MSAN_OPTIONS=suppressions=${CMAKE_CURRENT_SOURCE_DIR}/tests/msan_suppressions.txt:halt_on_error=false")

add_test(
	NAME ${PROJECT_NAME}
	COMMAND $<TARGET_FILE:${PROJECT_NAME}>
)