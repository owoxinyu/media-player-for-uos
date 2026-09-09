set(APP_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    playbar.cpp
    playbar.h
    playbar.ui
    playlist.cpp
    playlist.h
    playlist.ui
    setting.cpp
    setting.h
    setting.ui
    shortcutkeyedititem.cpp
    shortcutkeyedititem.h
    shortcutkeyedititem.ui
    modelchat.cpp
    modelchat.h
    modelchat.ui
    grableprogressbar.cpp
    grableprogressbar.h
    mytimeformater.cpp
    mytimeformater.h
    ../resources/qss.qrc
)

if(WIN32)
    list(APPEND APP_SOURCES ../resources/icons/icon.rc)
endif()

qt_add_executable(MediaPlayerForUOS
    MANUAL_FINALIZATION
    ${APP_SOURCES}
)

target_include_directories(MediaPlayerForUOS PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(MediaPlayerForUOS PRIVATE
    Qt6::Widgets
    Qt6::MultimediaWidgets
    Qt6::WebSockets
)

set_target_properties(MediaPlayerForUOS PROPERTIES
    OUTPUT_NAME media-player-for-uos
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS MediaPlayerForUOS
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(MediaPlayerForUOS)
