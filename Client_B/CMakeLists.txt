cmake_minimum_required(VERSION 3.16)

project(Client_B VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Core Network)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appClient_B
    main.cpp
)

qt_add_qml_module(appClient_B
    URI Client_B
    QML_FILES
        Main.qml
        StartScreen.qml
        LobbyScreen.qml
        GameScreen.qml
        PlayerHeader.qml
        GameBoard.qml
        PlayerList.qml
        FieldInfoPopup.qml
        BuyPopup.qml
        BuildPopup.qml
        GameOverPopup.qml
        DiceButton.qml
    SOURCES
        monopolyclient.h monopolyclient.cpp
)

set_target_properties(appClient_B PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appClient_B
    PRIVATE Qt6::Quick Qt6::Core Qt6::Network
)

include(GNUInstallDirs)
install(TARGETS appClient_B
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
