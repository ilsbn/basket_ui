function(basket_deploy_qt_platform_plugins target)
    if(NOT WIN32)
        return()
    endif()

    # vcpkg deploys linked DLLs, but Qt's dynamically loaded platform plugins
    # must also be present next to the executable.
    foreach(plugin IN ITEMS Qt6::QWindowsIntegrationPlugin Qt6::QMinimalIntegrationPlugin)
        if(TARGET "${plugin}")
            get_target_property(plugin_type "${plugin}" TYPE)
            if(plugin_type STREQUAL "MODULE_LIBRARY")
                add_custom_command(TARGET ${target} POST_BUILD
                    COMMAND "${CMAKE_COMMAND}" -E make_directory
                        "$<TARGET_FILE_DIR:${target}>/platforms"
                    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                        "$<TARGET_FILE:${plugin}>"
                        "$<TARGET_FILE_DIR:${target}>/platforms"
                    VERBATIM
                )
            endif()
        endif()
    endforeach()
endfunction()
