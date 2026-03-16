#include "mainwindow.h"
#include "startscreen.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 1. Zeige den Startbildschirm an
    StartScreen s;
    s.show();

    MainWindow *w = nullptr;

    // 2. Wenn der Spieler auf "Beitreten" klickt, öffne das Spielfeld
    QObject::connect(&s, &StartScreen::joinGameClicked, [&](QString ip, QString name){
        s.close();
        w = new MainWindow(name, ip);
        w->show();
    });

    return a.exec();
}
