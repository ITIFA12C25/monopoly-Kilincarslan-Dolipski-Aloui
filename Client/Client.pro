QT += core gui network widgets

CONFIG += c++17 cmdline

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    startscreen.cpp

HEADERS += \
    mainwindow.h \
    startscreen.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
