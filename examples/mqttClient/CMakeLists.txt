cmake_minimum_required (VERSION 3.14)

project ("mqttClient" LANGUAGES CXX)

file(GLOB_RECURSE client_src_files "src/*.cpp")

add_executable(${PROJECT_NAME} ${client_src_files})

target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} PRIVATE examples_shared_lib)

#Sanitizer link options added.
target_link_options(${PROJECT_NAME} PRIVATE ${CLEANMQTT_SANITIZER_FLAGS})