QT += core gui network qml quick

CONFIG += c++17 qmltypes

QML_IMPORT_NAME = Client_B
QML_IMPORT_MAJOR_VERSION = 1

SOURCES += \
    main.cpp \
    monopolyclient.cpp

HEADERS += \
    monopolyclient.h

RESOURCES += qml.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
