include_guard()

option(BUILD_SHARED_LIBS "Build shared library instead of static library?" OFF)
option(BUILD_UNIT_TESTS "Should unit tests be built?" ON)
option(BUILD_BENCHMARKING "Should benchmarking project be built?" ON)
option(ENABLE_UNIT_TESTS "Should unit tests be ran after building?" ON)
option(ENABLE_CODE_COVERAGE "Should enable code coverage to run?" ON)
option(BUILD_EXAMPLES "Should examples be built?" ON)
option(ENABLE_WARNINGS_AS_ERRORS "Should enable max warnings level as errors?" ON)
option(ENABLE_LOGS "Should logging be enabled?" ON)
option(ENABLE_ASAN "Should AddressSanitizer be enabled?" OFF)
option(ENABLE_UBSAN "Should UndefinedBehaviorSanitizer be enabled?" OFF)
option(ENABLE_TSAN "Should ThreadSanitizer be enabled?" OFF)
option(ENABLE_MSAN "Should MemorySanitizer be enabled?" OFF)

#SBO
option(ENABLE_BYTEBUFFER_SBO "Enable the SBO for ByteBuffer class. Buffer SBO size set to 128 (BYTEBUFFER_SBO_MAX_SIZE) bytes as default." ON)
option(ENABLE_UNIQUEFUNCTION_SBO "Enable the SBO for UniqueFunction class. Buffer SBO size set to 32 (UNIQUEFUNCTION_SBO_MAX_SIZE) bytes as default." ON)

#MQTT Encoding
option(FORCE_ADD_PROPERTIES "Should properties for packets be added as part of encoding regardless 
whether they are empty to force properties to always have a value when they reach the mqtt broker. 
May be useful if broker for some reason doesnt follow MQTT spec correctly and requires property 
to not be empty, however ideally this should be toggled to `OFF` and broker fixed to follow specs." OFF)

message(STATUS "Mqtt Library Options:")
message(STATUS "  BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS}")
message(STATUS "  BUILD_UNIT_TESTS: ${BUILD_UNIT_TESTS}")
message(STATUS "  ENABLE_UNIT_TESTS: ${ENABLE_TESTS}")
message(STATUS "  ENABLE_CODE_COVERAGE: ${ENABLE_CODE_COVERAGE}")
message(STATUS "  BUILD_EXAMPLES: ${BUILD_EXAMPLES}")
message(STATUS "  ENABLE_WARNINGS_AS_ERRORS: ${ENABLE_WARNINGS_AS_ERRORS}")
message(STATUS "  FORCE_ADD_PROPERTIES: ${FORCE_ADD_PROPERTIES}")
message(STATUS "  ENABLE_LOGS: ${ENABLE_LOGS}")
message(STATUS "  ENABLE_BYTEBUFFER_SBO: ${ENABLE_BYTEBUFFER_SBO}")
message(STATUS "  ENABLE_UNIQUEFUNCTION_SBO: ${ENABLE_UNIQUEFUNCTION_SBO}")
message(STATUS "  ENABLE_ASAN: ${ENABLE_ASAN}")
message(STATUS "  ENABLE_UBSAN: ${ENABLE_UBSAN}")
message(STATUS "  ENABLE_TSAN: ${ENABLE_TSAN}")
message(STATUS "  ENABLE_MSAN: ${ENABLE_MSAN}")


# Sets
set(LOG_BUFFER_SIZE 2048 CACHE STRING "Fixed log buffer size in bytes")
set(BYTEBUFFER_SBO_MAX_SIZE 128 CACHE STRING "Fixed buffer on stack max size for ByteBuffer class (Used by packets on receive and send)")
set(UNIQUEFUNCTION_SBO_MAX_SIZE 32 CACHE STRING "Fixed buffer on stack max size for UniqueFunction class")