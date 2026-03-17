#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>
#include "monopolyclient.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setOrganizationName("Monopoly");
    app.setApplicationName("Monopoly Mobile");

    QQmlApplicationEngine engine;

    // Register singleton instance for QML
    MonopolyClient *client = new MonopolyClient(&app);
    qmlRegisterSingletonInstance("Client_B", 1, 0, "MonopolyClient", client);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    // Works with both CMake (qt_add_qml_module) and qmake (QML_FILES)
    engine.load(QUrl(QStringLiteral("qrc:/qt/qml/Client_B/Main.qml")));

    return app.exec();
}

