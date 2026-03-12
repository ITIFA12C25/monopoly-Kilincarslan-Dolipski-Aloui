#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QHostAddress>
#include <cstdlib>
#include <ctime>
#include <QGroupBox>
#include <QMouseEvent>
#include <functional>

// --- HILFSKLASSEN & FUNKTIONEN ---

class ClickableFrame : public QFrame {
public:
    std::function<void()> onClick;
    ClickableFrame(QWidget* parent, std::function<void()> callback) : QFrame(parent), onClick(callback) {}
protected:
    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton && onClick) {
            onClick();
        }
        QFrame::mousePressEvent(event);
    }
};

// Farben für die Straßen (Klassisches Monopoly)
QString getFeldFarbe(int id) {
    if (id == 1 || id == 2) return "#8d6e63"; // Braun
    if (id == 3 || id == 4) return "#81d4fa"; // Hellblau
    if (id >= 6 && id <= 9) return "#f48fb1"; // Rosa
    if (id >= 11 && id <= 14) return "#ffcc80"; // Orange
    if (id >= 16 && id <= 19) return "#ef5350"; // Rot
    if (id >= 21 && id <= 24) return "#fff59d"; // Gelb
    if (id >= 26 && id <= 29) return "#a5d6a7"; // Grün
    if (id >= 31 && id <= 34) return "#90caf9"; // Dunkelblau
    if (id >= 36 && id <= 39) return "#ce93d8"; // Lila

    // Sonderfelder neutral
    return "transparent";
}

// Icons für Sonderfelder (Emoji als Bild-Ersatz)
QString getFeldIcon(int id) {
    if (id == 0) return "🏁"; // LOS
    if (id == 10) return "⛓️"; // Gefängnis
    if (id == 20) return "🅿️"; // Parken
    if (id == 30) return "👮"; // Polizei
    if (id == 5 || id == 15 || id == 25 || id == 35) return "🚂"; // Bahn
    if (id == 12) return "💡"; // E-Werk
    if (id == 28) return "💧"; // Wasser
    return "";
}

// --- KONSTRUKTOR ---
MainWindow::MainWindow(bool isHost, QString playerName, QString targetIp, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , server(nullptr)
    , socket(nullptr)
    , binIchServer(isHost)
    , meinName(playerName)
{
    ui->setupUi(this);
    srand(time(0));

    QList<QGroupBox*> boxes = this->findChildren<QGroupBox*>();
    for(auto box : boxes) {
        if(box->title() == "Netzwerk") box->setVisible(false);
    }

    setupDaten();
    setupBrettUI();

    QGroupBox* statsBox = new QGroupBox("Spieler Status");
    QVBoxLayout* statsLayout = new QVBoxLayout(statsBox);
    statsLabel = new QLabel("Lade...");
    statsLabel->setStyleSheet("font-size: 12px; font-family: Courier;");
    statsLayout->addWidget(statsLabel);

    QGroupBox* guideBox = new QGroupBox("Infos");
    QVBoxLayout* guideLayout = new QVBoxLayout(guideBox);
    QLabel* lFarben = new QLabel("🔴 Host (P1) | 🔵 Gast (P2)");
    lFarben->setStyleSheet("font-weight: bold;");
    QLabel* lHaus = new QLabel("🏠 = Haus | HOTEL = 5 Häuser");
    guideLayout->addWidget(lFarben);
    guideLayout->addWidget(lHaus);

    QVBoxLayout* rightLayout = qobject_cast<QVBoxLayout*>(ui->controlsContainer->layout());
    if(rightLayout) {
        rightLayout->insertWidget(1, statsBox);
        rightLayout->insertWidget(2, guideBox);
    }

    if (binIchServer) {
        startHost();
        this->setWindowTitle("Monopoly - HOST: " + meinName);
    } else {
        connectToHost(targetIp);
        this->setWindowTitle("Monopoly - GAST: " + meinName);
    }

    updateBrettUI();
}

MainWindow::~MainWindow()
{
    for(auto f : brett) delete f;
    for(auto s : spielerListe) delete s;
    delete ui;
}

// --- NETZWERK START ---
void MainWindow::startHost() {
    server = new QTcpServer(this);
    connect(server, &QTcpServer::newConnection, this, &MainWindow::neuerClientVerbunden);
    if(server->listen(QHostAddress::Any, 1234)) log("Warte auf Mitspieler...");
    else log("Fehler: Port belegt!");
}

void MainWindow::connectToHost(QString ip) {
    socket = new QTcpSocket(this);
    socket->connectToHost(ip, 1234);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::datenEmpfangen);
    connect(socket, &QTcpSocket::disconnected, this, &MainWindow::verbindungGetrennt);
    if(socket->waitForConnected(3000)) {
        log("Verbunden!");
        sendeNachricht("NAME:" + meinName);
    } else {
        QMessageBox::critical(this, "Fehler", "Verbindung fehlgeschlagen!");
    }
}

void MainWindow::neuerClientVerbunden() {
    socket = server->nextPendingConnection();
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::datenEmpfangen);
    connect(socket, &QTcpSocket::disconnected, this, &MainWindow::verbindungGetrennt);
    log("Spieler 2 verbunden!");
    sendeNachricht("NAME:" + meinName);
}

// --- SETUP ---
void MainWindow::setupDaten() {
    brett.clear();
    spielerListe.clear();

    for(int i=0; i<40; i++) {
        if (i == 0) brett.push_back(new SonderFeld(i, "LOS", "START"));
        else if (i == 10) brett.push_back(new SonderFeld(i, "GEFÄNGNIS", "BESUCH")); // Umbenannt
        else if (i == 20) brett.push_back(new SonderFeld(i, "PARKEN", "FREI"));
        else if (i == 30) brett.push_back(new SonderFeld(i, "POLIZEI", "GEHE INS GEFÄNGNIS")); // Umbenannt und Logik-String angepasst
        else if (i == 5) brett.push_back(new StadtFeld(i, "Südbahnhof", 200, 25, 0));
        else if (i == 15) brett.push_back(new StadtFeld(i, "Westbahnhof", 200, 25, 0));
        else if (i == 25) brett.push_back(new StadtFeld(i, "Nordbahnhof", 200, 25, 0));
        else if (i == 35) brett.push_back(new StadtFeld(i, "Ostbahnhof", 200, 25, 0));
        else if (i == 12) brett.push_back(new StadtFeld(i, "E-Werk", 150, 10, 0));
        else if (i == 28) brett.push_back(new StadtFeld(i, "Wasserwerk", 150, 10, 0));
        else {
            int preis = 60 + (i * 8);
            int miete = 2 + (i / 2);
            string name = "Strasse " + to_string(i);
            if(i==1) name="Oldenburg"; if(i==2) name="Osnabrück";
            if(i==3) name="Lübeck"; if(i==4) name="Kiel";
            if(i==37) name="Düsseldorf"; if(i==39) name="Berlin";
            brett.push_back(new StadtFeld(i, name, preis, miete, 50 + (i * 2)));
        }
    }

    if(binIchServer) {
        spielerListe.push_back(new Spieler(0, meinName.toStdString(), "red"));
        spielerListe.push_back(new Spieler(1, "Warte...", "blue"));
    } else {
        spielerListe.push_back(new Spieler(0, "Host", "red"));
        spielerListe.push_back(new Spieler(1, meinName.toStdString(), "blue"));
    }
    aktuellerSpielerIndex = 0;
}

void MainWindow::setupBrettUI() {
    QGridLayout* grid = ui->gridLayoutBrett;
    grid->setSpacing(2);
    for(int i=0; i<40; i++) {
        int r, c; getKoordinaten(i, r, c);

        auto clickAction = [this, i]() {
            if(i < 0 || i >= brett.size()) return;
            Feld* f = brett[i];
            QString info = "<b>" + QString::fromStdString(f->name) + "</b>";
            StadtFeld* s = dynamic_cast<StadtFeld*>(f);
            if(s) {
                info += "<br><br>Kaufpreis: " + QString::number(s->preis) + "$";
                info += "<br>Aktuelle Miete: " + QString::number(s->getAktuelleMiete()) + "$";
                info += "<br>Hauskosten: " + QString::number(s->hausPreis) + "$";
                info += "<br>Häuser: " + QString::number(s->haeuser);
                QString besitzer = (s->besitzerID == -1) ? "Bank" : QString::fromStdString(spielerListe[s->besitzerID]->name);
                info += "<br><br>Besitzer: " + besitzer;
            } else {
                SonderFeld* so = dynamic_cast<SonderFeld*>(f);
                if(so) info += "<br><br>Typ: " + QString::fromStdString(so->typ);
            }
            QMessageBox::information(this, "Feld Info", info);
        };

        ClickableFrame* frame = new ClickableFrame(this, clickAction);
        frame->setFrameStyle(QFrame::Box | QFrame::Raised);
        frame->setCursor(Qt::PointingHandCursor);
        QVBoxLayout* vLayout = new QVBoxLayout(frame); vLayout->setContentsMargins(0,0,0,0);
        vLayout->setSpacing(0);

        // --- DESIGN AUFBAU ---

        // 1. Farbstreifen Oben (Nur bei Straßen sichtbar)
        QLabel* lColorBar = new QLabel();
        lColorBar->setFixedHeight(15);
        QString farbe = getFeldFarbe(i);
        if(farbe != "transparent") {
            lColorBar->setStyleSheet("background-color: " + farbe + "; border-bottom: 1px solid #aaa;");
        } else {
            lColorBar->setStyleSheet("background-color: transparent;");
        }

        // 2. Name und Icon
        QString icon = getFeldIcon(i);
        QString displayName = QString::fromStdString(brett[i]->name);
        if(!icon.isEmpty()) displayName = icon + "\n" + displayName;

        QLabel* lName = new QLabel(displayName);
        lName->setAlignment(Qt::AlignCenter);
        lName->setWordWrap(true);
        // Fettere Schrift für Namen
        QFont f = lName->font(); f.setPointSize(6); f.setBold(true); lName->setFont(f);
        lName->setStyleSheet("padding: 2px; background-color: transparent;");

        // 3. Preis / Info
        QLabel* lPreis = new QLabel("");
        lPreis->setAlignment(Qt::AlignCenter);
        QFont fPreis = lPreis->font(); fPreis.setPointSize(6); lPreis->setFont(fPreis);
        StadtFeld* s = dynamic_cast<StadtFeld*>(brett[i]);
        if(s) lPreis->setText(QString::number(s->preis) + "$");

        // 4. Häuser & Spieler Tokens
        QLabel* lHaus = new QLabel(""); lHaus->setAlignment(Qt::AlignCenter);
        lHaus->setStyleSheet("color: darkgreen; font-weight: bold; font-size: 8px;");

        QLabel* lSpieler = new QLabel(""); lSpieler->setAlignment(Qt::AlignCenter);

        vLayout->addWidget(lColorBar); // Farbe oben
        vLayout->addWidget(lName);
        vLayout->addWidget(lPreis);
        vLayout->addWidget(lHaus);
        vLayout->addWidget(lSpieler);

        feldFrames.push_back(frame);
        feldLabels.push_back(lName);
        preisLabels.push_back(lPreis);
        hausLabels.push_back(lHaus);
        spielerLabels.push_back(lSpieler);
        grid->addWidget(frame, r, c);
    }
}

void MainWindow::getKoordinaten(int id, int &r, int &c) {
    if (id <= 10) { r=10; c=10-id; }
    else if (id <= 20) { r=10-(id-10); c=0; }
    else if (id <= 30) { r=0; c=id-20; }
    else { r=id-30; c=10; }
}

void MainWindow::updateBrettUI() {
    for(int i=0; i<40; i++) {
        StadtFeld* s = dynamic_cast<StadtFeld*>(brett[i]);

        // Basis-Style für die Karte (Weiß mit Rand)
        QString frameStyle = "QFrame { background-color: white; border: 1px solid #333; border-radius: 4px; }";

        if(s && s->besitzerID != -1) {
            // Wenn gekauft: Hintergrund leicht einfärben (Besitzerfarbe)
            if(s->besitzerID == 0) frameStyle = "QFrame { background-color: #ffebee; border: 2px solid red; border-radius: 4px; }"; // Host
            else frameStyle = "QFrame { background-color: #e3f2fd; border: 2px solid blue; border-radius: 4px; }"; // Gast

            if(s->haeuser > 0) {
                QString hausSymbol = (s->haeuser == 5) ? "🏨" : QString("🏠").repeated(s->haeuser);
                hausLabels[i]->setText(hausSymbol);
            } else {
                hausLabels[i]->setText("");
            }

            // Preis ausblenden wenn gekauft
            preisLabels[i]->setText("");
        } else {
            // Ungekauft
            preisLabels[i]->setVisible(true);
            if(s) preisLabels[i]->setText(QString::number(s->preis) + "$");

            // Sonderfelder Grau
            if(i%10==0) frameStyle = "QFrame { background-color: #f5f5f5; border: 1px solid #333; }";
        }
        feldFrames[i]->setStyleSheet(frameStyle);
        spielerLabels[i]->setText("");
    }

    // Spieler Token setzen
    for(auto sp : spielerListe) {
        QLabel* lbl = spielerLabels[sp->position];
        QString txt = lbl->text();
        if(!txt.isEmpty()) txt += " ";

        QString nameKurz = QString::fromStdString(sp->name).left(3).toUpper();
        QString farbCode = (sp->farbe == "red") ? "#d32f2f" : "#1976d2";

        QString symbol = (sp->id == 0) ? "🔴" : "🔵";
        lbl->setText(txt + QString("<span style='color:%1; font-weight:bold;'>%2 %3</span>").arg(farbCode).arg(symbol).arg(nameKurz));
    }

    QString statusHTML = "";
    for(auto sp : spielerListe) {
        QString name = QString::fromStdString(sp->name);
        QString geld = QString::number(sp->geld);
        QString farbe = (sp->farbe == "red") ? "red" : "blue";
        QString marker = (sp->id == aktuellerSpielerIndex) ? " <b>(DRAN)</b>" : "";
        statusHTML += QString("<div style='color:%1; font-size:14px;'>%2: <b>%3 $</b>%4</div>").arg(farbe).arg(name).arg(geld).arg(marker);
    }
    statsLabel->setText(statusHTML);
}

void MainWindow::on_btnWuerfeln_clicked() {
    Spieler* s = spielerListe[aktuellerSpielerIndex];
    if (socket && socket->isOpen()) {
        if (binIchServer && s->id != 0) { QMessageBox::warning(this, "Wait", "Gast ist dran!"); return; }
        if (!binIchServer && s->id != 1) { QMessageBox::warning(this, "Wait", "Host ist dran!"); return; }
    }
    int augen = (rand()%6+1) + (rand()%6+1);
    log("Du würfelst: " + QString::number(augen));
    sendeNachricht("MOVE:" + QString::number(s->id) + ":" + QString::number(augen));

    s->position = (s->position + augen) % 40;
    if(s->position < augen) s->geld += 200;
    verarbeiteFeldLogik(s);
    aktuellerSpielerIndex = (aktuellerSpielerIndex + 1) % spielerListe.size();
    updateBrettUI();
    checkGameOver();
}

void MainWindow::datenEmpfangen() {
    QByteArray data = socket->readAll();
    QString msg = QString(data);
    QStringList parts = msg.split(":");

    if(msg.startsWith("NAME:")) {
        QString fremd = parts[1];
        if(binIchServer) spielerListe[1]->name = fremd.toStdString();
        else spielerListe[0]->name = fremd.toStdString();
        log("Gegner ist: " + fremd);
        updateBrettUI();
    }
    else if(msg.startsWith("MOVE:")) {
        int pid = parts[1].toInt(); int augen = parts[2].toInt();
        Spieler* s = spielerListe[pid];

        s->position = (s->position + augen) % 40;

        // Über LOS gegangen beim Gegner
        if(s->position < augen) s->geld += 200;

        StadtFeld* stadt = dynamic_cast<StadtFeld*>(brett[s->position]);
        if(stadt && stadt->besitzerID != -1 && stadt->besitzerID != pid) {
            s->geld -= stadt->getAktuelleMiete();
            spielerListe[stadt->besitzerID]->geld += stadt->getAktuelleMiete();
        }
        aktuellerSpielerIndex = (aktuellerSpielerIndex + 1) % spielerListe.size();
        updateBrettUI();
    }
    else if(msg.startsWith("BUY:")) {
        int fId = parts[1].toInt();
        int pId = parts[2].toInt();
        StadtFeld* stadt = dynamic_cast<StadtFeld*>(brett[fId]);
        Spieler* s = spielerListe[pId];
        if(stadt) {
            s->geld -= stadt->preis;
            stadt->besitzerID = pId;
            log("Gegner kaufte " + QString::fromStdString(stadt->name));
            updateBrettUI();
        }
    }
    else if(msg.startsWith("BUILD:")) {
        int fId = parts[1].toInt();
        int pId = parts[2].toInt();
        StadtFeld* stadt = dynamic_cast<StadtFeld*>(brett[fId]);
        Spieler* s = spielerListe[pId];
        if(stadt) {
            s->geld -= stadt->hausPreis;
            stadt->haeuser++;
            log("Gegner baute auf " + QString::fromStdString(stadt->name));
            updateBrettUI();
        }
    }
}

void MainWindow::verarbeiteFeldLogik(Spieler* s) {
    Feld* f = brett[s->position];
    SonderFeld* sonder = dynamic_cast<SonderFeld*>(f);
    if(sonder && sonder->typ == "GEHE INS GEFÄNGNIS") {
        QMessageBox::warning(this, "Polizei", "Du musst ins Gefängnis!");
        s->position = 10; s->gefangen = true; return;
    }
    StadtFeld* stadt = dynamic_cast<StadtFeld*>(f);
    if(stadt) {
        if(stadt->besitzerID == -1 && s->geld >= stadt->preis) {
            if(QMessageBox::Yes == QMessageBox::question(this, "Kaufen?", "Kaufen für " + QString::number(stadt->preis) + "?")) {
                s->geld -= stadt->preis; stadt->besitzerID = s->id;
                sendeNachricht("BUY:" + QString::number(stadt->id) + ":" + QString::number(s->id));
            }
        }
        else if(stadt->besitzerID != s->id) {
            s->geld -= stadt->getAktuelleMiete();
            spielerListe[stadt->besitzerID]->geld += stadt->getAktuelleMiete();
        }
    }
}

void MainWindow::checkGameOver() {}
void MainWindow::verbindungGetrennt() { log("Disconnected"); }
void MainWindow::sendeNachricht(QString t) { if(socket) { QTextStream s(socket); s << t; } }
void MainWindow::log(QString t) { ui->logAusgabe->append(t); }
