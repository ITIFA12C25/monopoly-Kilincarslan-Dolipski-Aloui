#include "startscreen.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QGraphicsDropShadowEffect>

StartScreen::StartScreen(QWidget *parent) : QWidget(parent) {
    setupUi();
    setupStyle();
}

void StartScreen::setupUi() {
    this->setWindowTitle("Monopoly Launcher");
    this->resize(900, 650);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);

    QFrame *card = new QFrame(this);
    card->setObjectName("card");
    card->setFixedSize(420, 580);

    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setSpacing(15);
    cardLayout->setContentsMargins(45, 45, 45, 45);

    QLabel *title = new QLabel("MONOPOLY");
    title->setObjectName("title");
    title->setAlignment(Qt::AlignCenter);
    QLabel *subtitle = new QLabel("QT EDITION");
    subtitle->setObjectName("subtitle");
    subtitle->setAlignment(Qt::AlignCenter);

    // HOST
    QLabel *lblHostName = new QLabel("Dein Name (Host)");
    hostNameInput = new QLineEdit();
    hostNameInput->setPlaceholderText("Namen eingeben...");
    hostNameInput->setText("Spieler 1");

    btnHost = new QPushButton("Spiel erstellen");
    btnHost->setCursor(Qt::PointingHandCursor);
    btnHost->setObjectName("btnHost");

    // TRENNER
    QHBoxLayout *lineLayout = new QHBoxLayout();
    QFrame *line1 = new QFrame(); line1->setObjectName("line"); line1->setFrameShape(QFrame::HLine);
    QFrame *line2 = new QFrame(); line2->setObjectName("line"); line2->setFrameShape(QFrame::HLine);
    QLabel *lblOr = new QLabel(" ODER BEITRETEN ");
    lblOr->setObjectName("orLabel");
    lineLayout->addWidget(line1);
    lineLayout->addWidget(lblOr);
    lineLayout->addWidget(line2);

    // GAST
    QLabel *lblIp = new QLabel("Server IP");
    ipInput = new QLineEdit();
    ipInput->setPlaceholderText("z.B. 127.0.0.1");
    ipInput->setText("127.0.0.1");

    QLabel *lblGuestName = new QLabel("Dein Name (Gast)");
    guestNameInput = new QLineEdit();
    guestNameInput->setPlaceholderText("Namen eingeben...");
    guestNameInput->setText("Spieler 2");

    btnJoin = new QPushButton("Beitreten");
    btnJoin->setCursor(Qt::PointingHandCursor);
    btnJoin->setObjectName("btnJoin");

    cardLayout->addWidget(title);
    cardLayout->addWidget(subtitle);
    cardLayout->addSpacing(10);
    cardLayout->addWidget(lblHostName);
    cardLayout->addWidget(hostNameInput);
    cardLayout->addWidget(btnHost);
    cardLayout->addSpacing(5);
    cardLayout->addLayout(lineLayout);
    cardLayout->addSpacing(5);
    cardLayout->addWidget(lblIp);
    cardLayout->addWidget(ipInput);
    cardLayout->addWidget(lblGuestName);
    cardLayout->addWidget(guestNameInput);
    cardLayout->addWidget(btnJoin);
    cardLayout->addStretch();

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(30);
    shadow->setColor(QColor(0, 0, 0, 60));
    shadow->setOffset(0, 10);
    card->setGraphicsEffect(shadow);

    mainLayout->addWidget(card);

    connect(btnHost, &QPushButton::clicked, this, &StartScreen::onHost);
    connect(btnJoin, &QPushButton::clicked, this, &StartScreen::onJoin);
}

void StartScreen::setupStyle() {
    this->setStyleSheet(
        "StartScreen { background-color: #2c3e50; }"
        "#card { background-color: white; border-radius: 20px; }"
        "#title { font-family: 'Arial Black'; font-size: 32px; color: #2c3e50; font-weight: 900; }"
        "#subtitle { font-size: 14px; color: #7f8c8d; font-weight: bold; letter-spacing: 4px; margin-bottom: 10px; }"
        "QLabel { color: #95a5a6; font-size: 11px; font-weight: bold; margin-bottom: 2px; }"
        "#orLabel { color: #bdc3c7; font-size: 10px; }"
        "QLineEdit { padding: 10px; border: 2px solid #ecf0f1; border-radius: 6px; font-size: 13px; background-color: #fcfcfc; }"
        "QLineEdit:focus { border: 2px solid #3498db; background-color: white; }"
        "QPushButton { padding: 12px; border-radius: 6px; font-weight: bold; font-size: 13px; color: white; border: none; }"
        "#btnHost { background-color: #27ae60; margin-top: 5px; }"
        "#btnHost:hover { background-color: #2ecc71; }"
        "#btnJoin { background-color: #2980b9; margin-top: 5px; }"
        "#btnJoin:hover { background-color: #3498db; }"
        "#line { color: #ecf0f1; background-color: #ecf0f1; border: none; height: 1px; }"
        );
}

void StartScreen::onHost() { emit hostGameClicked(hostNameInput->text()); }
void StartScreen::onJoin() { emit joinGameClicked(ipInput->text(), guestNameInput->text()); }
