#ifndef GAMESCREENVIEWMODEL_H
#define GAMESCREENVIEWMODEL_H

#include <QObject>
#include <QQmlEngine>

class GameScreenViewModel
{
    QML_ELEMENT
public:
    GameScreenViewModel();

private:
    QString m_title;
    QString m_currentPlayer;

};

#endif // GAMESCREENVIEWMODEL_H

