QT       += core gui network widgets

CONFIG += c++17

# Backslashes (\) sind wichtig!
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    startscreen.cpp

HEADERS += \
    mainwindow.h \
    startscreen.h

FORMS += \
    mainwindow.ui

# Standard Deployment Regeln
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
