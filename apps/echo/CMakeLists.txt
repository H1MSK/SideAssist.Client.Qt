project(SideAssist.Client.Echo)

set(EXECUTABLE_NAME echo)

file(GLOB PUBLIC_HEADERS include/*)
file(GLOB SOURCES src/*)

add_executable(${EXECUTABLE_NAME}
    ${SOURCES}
    ${PUBLIC_HEADERS})

target_link_libraries(${EXECUTABLE_NAME} SideAssist.Client.Qt.Lib)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        find_program(TOOL_WINDEPLOYQT NAMES windeployqt.debug.bat)
    else()
        find_program(TOOL_WINDEPLOYQT NAMES windeployqt)
    endif()

    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND ${TOOL_WINDEPLOYQT}
                $<TARGET_FILE:${EXECUTABLE_NAME}>
        COMMENT "Running ${TOOL_WINDEPLOYQT}..."
    )
endif()
