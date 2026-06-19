include_guard()

function(apply_3rd_party_libs_settings target_name)
    if (NOT TARGET ${target_name})
        message(WARNING "apply_3rd_party_libs_settings: target '${target_name}' does not exist")
        return()
    endif()

    if (MSVC)
        target_compile_options(${target_name} PRIVATE
            /W0
        )
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(${target_name} PRIVATE
            -w
        )
    endif()
endfunction()