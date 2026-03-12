#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <string>
#include <qobject.h>

class GameState
{
public:
    GameState();

private:
    QString m_currentPlayer;

    QString m_round;
    QString m_diceResult;


};

#endif // GAMESTATE_H
