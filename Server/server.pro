QT += core gui network widgets

# Wir entfernen "QT -= gui", damit grafische Pop-ups (Widgets) funktionieren
CONFIG += c++17 console
CONFIG -= app_bundle

SOURCES += \
    main.cpp
