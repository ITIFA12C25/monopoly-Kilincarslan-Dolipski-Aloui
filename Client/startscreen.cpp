#include "startscreen.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>

StartScreen::StartScreen(QWidget *parent) : QWidget(parent) {
    this->setWindowTitle("Monopoly Launcher");
    this->resize(350, 300);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QLabel *title = new QLabel("MONOPOLY CLIENT");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 20px; font-weight: bold; margin-bottom: 20px;");

    ipInput = new QLineEdit();
    ipInput->setPlaceholderText("z.B. 192.168.178.45");
    ipInput->setText("127.0.0.1"); // Standard für lokalen Test

    guestNameInput = new QLineEdit();
    guestNameInput->setText("Spieler 1");

    btnJoin = new QPushButton("Spiel beitreten");
    btnJoin->setStyleSheet("background-color: #2980b9; color: white; padding: 10px; font-weight: bold;");

    mainLayout->addWidget(title);
    mainLayout->addWidget(new QLabel("Server IP-Adresse:"));
    mainLayout->addWidget(ipInput);
    mainLayout->addWidget(new QLabel("Dein Spielername:"));
    mainLayout->addWidget(guestNameInput);
    mainLayout->addWidget(btnJoin);

    connect(btnJoin, &QPushButton::clicked, this, [this]() {
        emit joinGameClicked(ipInput->text(), guestNameInput->text());
    });
}
