// *** BEGINN KI-generiert Label: Prompt_09_Client_Start_H ***
#ifndef STARTSCREEN_H
#define STARTSCREEN_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>

class StartScreen : public QWidget {
    Q_OBJECT
public:
    explicit StartScreen(QWidget *parent = nullptr);

signals:
    void joinGameClicked(QString ip, QString name);

private:
    QLineEdit *guestNameInput;
    QLineEdit *ipInput;
    QPushButton *btnJoin;
};

#endif
// *** ENDE KI-generiert Label: Prompt_09_Client_Start_H ***
