# LIVE++ Utils
# OBSOLETE: This is obsolete and will be removed in the future. Use the new export_directory function instead.
function(ns_export_broker_output TARGET_NAME)
    add_custom_command(TARGET ${TARGET_NAME}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Broker/LPP_Broker.exe $<TARGET_FILE_DIR:${TARGET_NAME}>
        WORKING_DIRECTORY ${CMAKE_CURRENT_FUNCTION_LIST_DIR}
    )
endfunction()
function(ns_export_directory TARGET_NAME)
    add_custom_command(TARGET ${TARGET_NAME}
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${CMAKE_CURRENT_FUNCTION_LIST_DIR} $<TARGET_FILE_DIR:${TARGET_NAME}>/LivePP
        WORKING_DIRECTORY ${CMAKE_CURRENT_FUNCTION_LIST_DIR}
    )
endfunction()

function(ns_target_link_livepp TARGET_NAME)
    target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/API/x64)
    target_compile_definitions(${TARGET_NAME} PRIVATE LIVEPP_ENABLED=1)
    #ns_export_broker_output(${TARGET_NAME})
    ns_export_directory(${TARGET_NAME})
endfunction()

