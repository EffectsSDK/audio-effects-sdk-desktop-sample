cmake_minimum_required(VERSION 3.19)

set(TARGET_APP AudioEffectsSDKDemo)

cmake_policy(SET CMP0087 NEW)

project(${TARGET_APP})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTORCC ON) # TODO

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_PREFIX_PATH ${Qt6_DIR})
find_package(Qt6 COMPONENTS Widgets Multimedia REQUIRED)
qt_standard_project_setup()

if(AUDIO_EFFECTS_SDK_DIR)
    set(sdk_lib_target audio_effects_sdk)
    add_library(${sdk_lib_target} SHARED IMPORTED)
    add_library(effects_sdk::AudioEffectsSDK ALIAS ${sdk_lib_target})
    file(TO_CMAKE_PATH ${AUDIO_EFFECTS_SDK_DIR} AUDIO_EFFECTS_SDK_DIR)
    if(WIN32)
        set_target_properties(
            ${sdk_lib_target} PROPERTIES
            IMPORTED_LOCATION ${AUDIO_EFFECTS_SDK_DIR}/bin/AudioEffectsSDK.dll
            IMPORTED_IMPLIB ${AUDIO_EFFECTS_SDK_DIR}/lib/AudioEffectsSDK.lib
        )
    elseif(APPLE)
        set_target_properties(
            ${sdk_lib_target} PROPERTIES
            IMPORTED_LOCATION ${AUDIO_EFFECTS_SDK_DIR}/lib/libAudioEffectsSDK.dylib
        )
    endif()
    target_include_directories(${sdk_lib_target} INTERFACE ${AUDIO_EFFECTS_SDK_DIR}/include)
endif()

set(HEADER_FILES
    ui/main_window.h
    core/abstract_audio_source.h
    core/audio_buffer_qiodevice.h
    core/audio_playback.h
    core/audio_qiodevice_proxy.h
    core/core.h
    core/file_audio_source.h
    core/mic_audio_source.h
    core/mic_enumerator.h
    core/sdk_factory.h
    core/sdk_helpers.h
    core/sdk_notification_handler.h
)

set(SOURCE_FILES
    main.cpp
    ui/main_window.cpp
    core/abstract_audio_source.cpp
    core/audio_buffer_qiodevice.cpp
    core/audio_playback.cpp
    core/audio_qiodevice_proxy.cpp
    core/core.cpp
    core/file_audio_source.cpp
    core/mic_audio_source.cpp
    core/sdk_factory.cpp
    core/sdk_helpers.cpp
    core/sdk_notification_handler.cpp
)

qt6_add_resources(
    RESOURCE_FILES 
    ui/resources/resources.qrc
)

if(WIN32)
    set(EXE_TYPE WIN32)
endif()
if(APPLE)
    set(EXE_TYPE MACOSX_BUNDLE)
endif()

add_executable(${TARGET_APP}
    ${EXE_TYPE}
    ${HEADER_FILES}
    ${SOURCE_FILES}
    ${RESOURCE_FILES}
)
target_link_libraries(${TARGET_APP} 
    Qt6::Widgets 
    Qt6::Multimedia
    effects_sdk::AudioEffectsSDK
)

set_target_properties(${TARGET_APP}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

if(WIN32)
    add_custom_command(
        TARGET ${TARGET_APP} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different 
            "$<TARGET_FILE:effects_sdk::AudioEffectsSDK>" 
            "$<TARGET_FILE_DIR:${TARGET_APP}>/$<TARGET_FILE_NAME:effects_sdk::AudioEffectsSDK>"
        VERBATIM
    )
endif()

get_target_property(qmake_executable Qt6::qmake IMPORTED_LOCATION)
get_filename_component(qt_bin_dir "${qmake_executable}" DIRECTORY)

if(WIN32)
    find_program(windeployqt_bin windeployqt HINTS "${qt_bin_dir}")

    add_custom_target(
        deploy_qt 
        COMMAND "${windeployqt_bin}" --no-translations --no-quick-import  "$<TARGET_FILE:${TARGET_APP}>"
        DEPENDS ${TARGET_APP}
        VERBATIM
    )

    install(
        TARGETS ${TARGET_APP}
        RUNTIME DESTINATION .
    )

    install(CODE "
        set(ENV{Path} \"\")
        set(ENV{PATH} \"\")
        execute_process(COMMAND \"${windeployqt_bin}\" --no-translations --no-quick-import  \"${CMAKE_INSTALL_PREFIX}/$<TARGET_FILE_NAME:${TARGET_APP}>\")
    ")

    set(CMAKE_INSTALL_UCRT_LIBRARIES FALSE)
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION .)
    include(InstallRequiredSystemLibraries)

    install(PROGRAMS "$<TARGET_FILE:effects_sdk::AudioEffectsSDK>" DESTINATION .)
endif()

if (APPLE)
    set_target_properties(
        ${TARGET_APP} PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER com.damiko.audio-effects-sdk.demo.desktop
        MACOSX_BUNDLE_BUNDLE_NAME "Audio Effects SDK Demo"
        MACOSX_BUNDLE_BUNDLE_VERSION "${PRODUCT_VERSION}"
    )

    add_custom_command(
        TARGET ${TARGET_APP} POST_BUILD
        COMMAND plutil -replace NSMicrophoneUsageDescription -string "Demo mic"
            "$<TARGET_BUNDLE_DIR:${TARGET_APP}>/Contents/Info.plist"
        COMMENT "Patching Info.plist"
        VERBOSE
    )

    find_program(macdeployqt_bin macdeployqt HINTS "${qt_bin_dir}")
    add_custom_target(
        deploy
        COMMAND "${macdeployqt_bin}" "$<TARGET_BUNDLE_DIR:${TARGET_APP}>"
        DEPENDS ${TARGET_APP}
        VERBATIM
    )
endif()
