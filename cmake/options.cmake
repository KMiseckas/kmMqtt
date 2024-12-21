include_guard()

option(BUILD_SHARED_LIBS "Build shared library instead of static library?" OFF)
option(BUILD_TESTS "Should tests be built?" ON)
option(ENABLE_TESTS "Should tests be ran after building?" ON)
option(ENABLE_CODE_COVERAGE "Should enable code coverage to run?" ON)
option(BUILD_EXAMPLES "Should examples be built?" ON)
option(ENABLE_WARNINGS_AS_ERRORS "Should enable max warnings level as errors?" ON)
option(ENABLE_LOGS "Should logging be enabled?" ON)
option(ENABLE_THROW_ON_FATAL_LOG "Should throw exception on fatal log level be enabled?" ON)

#MQTT Encoding
option(FORCE_ADD_PROPERTIES "Should properties for packets be added as part of encoding regardless 
whether they are empty to force properties to always have a value when they reach the mqtt broker. 
May be useful if broker for some reason doesnt follow MQTT spec correctly and requires property 
to not be empty, however ideally this should be toggled to `OFF` and broker fixed to follow specs." OFF)

message(STATUS "Mqtt Library Options:")
message(STATUS "  BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS}")
message(STATUS "  BUILD_UNIT_TESTS: ${BUILD_UNIT_TESTS}")
message(STATUS "  ENABLE_TESTS: ${ENABLE_TESTS}")
message(STATUS "  ENABLE_CODE_COVERAGE: ${ENABLE_CODE_COVERAGE}")
message(STATUS "  BUILD_EXAMPLES: ${BUILD_EXAMPLES}")
message(STATUS "  ENABLE_WARNINGS_AS_ERRORS: ${ENABLE_WARNINGS_AS_ERRORS}")
message(STATUS "  FORCE_ADD_PROPERTIES: ${FORCE_ADD_PROPERTIES}")
message(STATUS "  ENABLE_LOGS: ${ENABLE_LOGS}")
message(STATUS "  ENABLE_THROW_ON_FATAL_LOG: ${ENABLE_THROW_ON_FATAL_LOG}")