#include "mainwindow.h"
#include "startscreen.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 1. Start Screen anzeigen
    StartScreen s;
    s.show();

    // Variable für das Hauptfenster (Pointer, damit es existiert wenn Lambda fertig ist)
    MainWindow *w = nullptr;

    // 2. Verbindung: Wenn "Host" geklickt wird
    QObject::connect(&s, &StartScreen::hostGameClicked, [&](QString name){
        s.close();
        // Starte als Host (true), IP ist leer
        w = new MainWindow(true, name, "");
        w->show();
    });

    // 3. Verbindung: Wenn "Join" geklickt wird
    QObject::connect(&s, &StartScreen::joinGameClicked, [&](QString ip, QString name){
        s.close();
        // Starte als Gast (false), IP wird übergeben
        w = new MainWindow(false, name, ip);
        w->show();
    });

    return a.exec();
}
