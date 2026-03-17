#include <QApplication>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QClipboard>
#include <QTcpServer>
#include <QTcpSocket>
#include <QNetworkInterface>
#include <QDebug>
#include <QScrollArea>
#include <QTextStream>
#include <QFile>
#include <QDateTime>
#include <vector>
#include <string>

using namespace std;

// --- DATENMODELLE ---
class ServerSpieler {
public:
    int id;
    QString name;
    double geld;
    int position;
    int imGefaengnis; // 0 = frei, >0 = Runden gesperrt
    QTcpSocket* socket;
    ServerSpieler(int id, QTcpSocket* s) : id(id), name("Unbekannt"), geld(1500), position(0), imGefaengnis(0), socket(s) {}
};

class Feld {
public:
    int id;
    string name;
    virtual ~Feld() {}
    Feld(int id, string n) : id(id), name(n) {}
};

class StadtFeld : public Feld {
public:
    double preis, basisMiete, hausPreis;
    int haeuser, besitzerID;

    StadtFeld(int id, string n, double p, double m, double hp)
        : Feld(id, n), preis(p), basisMiete(m), hausPreis(hp), haeuser(0), besitzerID(-1) {}

    double getAktuelleMiete() {
        if (haeuser == 0) return basisMiete;
        if (haeuser == 5) return basisMiete * 10; // Hotel
        return basisMiete * (haeuser + 1) * 1.5; // Häuser
    }
};

class SonderFeld : public Feld {
public:
    string typ;
    SonderFeld(int id, string n, string t) : Feld(id, n), typ(t) {}
};

class IPDialog : public QDialog {
public:
    IPDialog(const QStringList &ips, QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("Monopoly Server");
        setMinimumWidth(400);
        setStyleSheet(
            "QDialog { background-color: #1e1e1e; color: white; }"
            "QLabel#title { color: #2ecc71; font-size: 20px; font-weight: bold; margin-bottom: 10px; }"
            "QLabel#subtitle { color: #bdc3c7; font-size: 12px; margin-bottom: 20px; }"
            "QFrame#ipCard { background-color: #2c3e50; border-radius: 8px; padding: 10px; }"
            "QLineEdit { background: #34495e; border: none; color: #ecf0f1; padding: 8px; border-radius: 4px; font-family: 'Courier New'; }"
            "QPushButton { background-color: #27ae60; color: white; border-radius: 4px; padding: 8px 15px; font-weight: bold; }"
            "QPushButton:hover { background-color: #2ecc71; }"
            "QPushButton#btnClose { background-color: #c0392b; margin-top: 20px; }"
            );

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(30, 30, 30, 30);

        QLabel *title = new QLabel("Monopoly Server");
        title->setObjectName("title");
        layout->addWidget(title);

        QLabel *subtitle = new QLabel("Kopiere diese IP für deine Mitspieler:");
        subtitle->setObjectName("subtitle");
        layout->addWidget(subtitle);

        QScrollArea *scroll = new QScrollArea();
        scroll->setWidgetResizable(true);
        scroll->setStyleSheet("QScrollArea { border: none; background: transparent; }");
        QWidget *scrollWidget = new QWidget();
        QVBoxLayout *scrollLayout = new QVBoxLayout(scrollWidget);

        for (const QString &ip : ips) {
            QFrame *card = new QFrame();
            card->setObjectName("ipCard");
            QHBoxLayout *cardLayout = new QHBoxLayout(card);
            QLineEdit *edit = new QLineEdit(ip);
            edit->setReadOnly(true);
            QPushButton *btnCopy = new QPushButton("Kopieren");
            connect(btnCopy, &QPushButton::clicked, this, [ip]() { QApplication::clipboard()->setText(ip); });
            cardLayout->addWidget(edit);
            cardLayout->addWidget(btnCopy);
            scrollLayout->addWidget(card);
        }

        scroll->setWidget(scrollWidget);
        layout->addWidget(scroll);

        QPushButton *btnClose = new QPushButton("Server starten");
        btnClose->setObjectName("btnClose");
        connect(btnClose, &QPushButton::clicked, this, &QDialog::accept);
        layout->addWidget(btnClose);
    }
};

// --- SERVER LOGIK ---
class MonopolyServer : public QObject {
private:
    QTcpServer* server;
    vector<ServerSpieler*> spieler;
    vector<Feld*> brett;
    int aktuellerSpielerIndex = 0;
    bool spielBeendet = false;

    void setupBrett() {
        for(int i=0; i<40; i++) {
            if (i == 0) brett.push_back(new SonderFeld(i, "LOS", "START"));
            else if (i == 10) brett.push_back(new SonderFeld(i, "GEFÄNGNIS", "BESUCH"));
            else if (i == 20) brett.push_back(new SonderFeld(i, "PARKEN", "FREI"));
            else if (i == 30) brett.push_back(new SonderFeld(i, "POLIZEI", "GEHE INS GEFÄNGNIS"));
            else brett.push_back(new StadtFeld(i, "Feld " + to_string(i), 100 + i*5, 10 + i, 50));
        }
    }

    void broadcast(QString msg) {
        for(auto s : spieler) {
            if(s->socket && s->socket->isOpen()) {
                QTextStream out(s->socket);
                out << msg << "\n";
                out.flush();
            }
        }
    }

    void sendeAnSpieler(int id, QString msg) {
        if(id >= 0 && id < (int)spieler.size() && spieler[id]->socket) {
            QTextStream out(spieler[id]->socket);
            out << msg << "\n";
            out.flush();
        }
    }

    void syncAlle() {
        for(auto s : spieler) {
            if (s->name != "Unbekannt") {
                broadcast("PLAYER_UPDATE:" + QString::number(s->id) + ":" + s->name + ":" + QString::number(s->geld));
                broadcast("UPDATE_POS:" + QString::number(s->id) + ":" + QString::number(s->position));
            }
        }
        if (!spieler.empty() && !spielBeendet) {
            broadcast("MSG:--- " + spieler[aktuellerSpielerIndex]->name + " ist am Zug ---");
        }
    }

    void naechsterSpieler() {
        if (spieler.empty() || spielBeendet) return;
        aktuellerSpielerIndex = (aktuellerSpielerIndex + 1) % (int)spieler.size();
        broadcast("MSG:--- " + spieler[aktuellerSpielerIndex]->name + " ist am Zug ---");
    }

    void spielerBankrott(ServerSpieler* pleiteGeier, ServerSpieler* gewinner) {
        spielBeendet = true;
        broadcast("MSG:💥 BANKROTT! " + pleiteGeier->name + " ist pleite!");
        broadcast("MSG:🏆 " + gewinner->name + " GEWINNT DAS SPIEL!");
        broadcast("GAME_OVER");

        // CSV Export für die Doku
        QFile file("gewinner.csv");
        if(file.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&file);
            out << QDateTime::currentDateTime().toString("dd.MM.yyyy;HH:mm:ss") << ";"
                << gewinner->name << ";"
                << gewinner->geld << "\n";
            file.close();
            qDebug() << "Sieger in gewinner.csv gespeichert.";
        }
    }

public:
    MonopolyServer(QObject *parent = nullptr) : QObject(parent) {
        srand(time(0));
        setupBrett();
        server = new QTcpServer(this);
        connect(server, &QTcpServer::newConnection, this, &MonopolyServer::neueVerbindung);

        if(server->listen(QHostAddress::Any, 1234)) {
            QStringList foundIps;
            for (const QHostAddress &address: QNetworkInterface::allAddresses()) {
                if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress::LocalHost) {
                    foundIps << address.toString();
                }
            }
            IPDialog dialog(foundIps);
            dialog.exec();
            qDebug() << "Server aktiv auf Port 1234...";
        }
    }

private:
    void neueVerbindung() {
        QTcpSocket* clientSocket = server->nextPendingConnection();
        int newId = spieler.size();
        ServerSpieler* neuerSpieler = new ServerSpieler(newId, clientSocket);
        spieler.push_back(neuerSpieler);

        connect(clientSocket, &QTcpSocket::readyRead, this, [this, neuerSpieler]() { datenEmpfangen(neuerSpieler); });
        // WICHTIG: Client seine ID mitteilen
        sendeAnSpieler(newId, "ASSIGN_ID:" + QString::number(newId));
    }

    void datenEmpfangen(ServerSpieler* sender) {
        if(spielBeendet) return; // Nichts mehr annehmen, wenn das Spiel vorbei ist

        QByteArray data = sender->socket->readAll();
        QStringList lines = QString(data).split("\n", Qt::SkipEmptyParts);

        for(QString msg : lines) {
            QStringList parts = msg.split(":");
            QString cmd = parts[0];

            if(cmd == "NAME") {
                sender->name = parts[1];
                broadcast("MSG:" + sender->name + " ist beigetreten.");
                syncAlle();
            } else if(cmd == "CMD_ROLL") {
                if(sender->id != aktuellerSpielerIndex) {
                    sendeAnSpieler(sender->id, "MSG:Du bist nicht dran!");
                    return;
                }

                // GEFÄNGNIS LOGIK
                if (sender->imGefaengnis > 0) {
                    sender->imGefaengnis--;
                    broadcast("MSG:⛓️ " + sender->name + " sitzt im Gefängnis und muss aussetzen. (Noch " + QString::number(sender->imGefaengnis) + " Runden)");
                    naechsterSpieler();
                    return;
                }

                int a = (rand()%6+1) + (rand()%6+1);
                broadcast("MSG:" + sender->name + " wuerfelt " + QString::number(a));

                int altePos = sender->position;
                sender->position = (sender->position + a) % 40;

                if (sender->position < altePos) {
                    sender->geld += 200;
                    broadcast("UPDATE_MONEY:" + QString::number(sender->id) + ":" + QString::number(sender->geld));
                }
                broadcast("UPDATE_POS:" + QString::number(sender->id) + ":" + QString::number(sender->position));

                // POLIZEI LOGIK
                if (sender->position == 30) {
                    broadcast("MSG:🚨 " + sender->name + " wurde von der Polizei erwischt und geht ins Gefängnis!");
                    sender->position = 10;
                    sender->imGefaengnis = 2;
                    broadcast("UPDATE_POS:" + QString::number(sender->id) + ":10");
                    naechsterSpieler();
                } else {
                    StadtFeld* stadt = dynamic_cast<StadtFeld*>(brett[sender->position]);
                    if(stadt) {
                        if(stadt->besitzerID == -1) {
                            sendeAnSpieler(sender->id, "ASK_BUY:" + QString::number(stadt->id) + ":" + QString::number(stadt->preis));
                        } else if(stadt->besitzerID != sender->id) {
                            // MIETE ZAHLEN UND BANKROTT PRÜFEN
                            double miete = stadt->getAktuelleMiete();
                            sender->geld -= miete;
                            spieler[stadt->besitzerID]->geld += miete;

                            broadcast("MSG:" + sender->name + " zahlt " + QString::number(miete) + "$ Miete.");
                            broadcast("UPDATE_MONEY:" + QString::number(sender->id) + ":" + QString::number(sender->geld));
                            broadcast("UPDATE_MONEY:" + QString::number(stadt->besitzerID) + ":" + QString::number(spieler[stadt->besitzerID]->geld));

                            if (sender->geld < 0) {
                                spielerBankrott(sender, spieler[stadt->besitzerID]);
                            } else {
                                naechsterSpieler();
                            }
                        } else {
                            naechsterSpieler();
                        }
                    } else {
                        naechsterSpieler();
                    }
                }
            } else if(cmd == "CMD_BUY") {
                int feldId = parts[1].toInt();
                StadtFeld* stadt = dynamic_cast<StadtFeld*>(brett[feldId]);
                if(stadt && stadt->besitzerID == -1 && sender->geld >= stadt->preis) {
                    sender->geld -= stadt->preis;
                    stadt->besitzerID = sender->id;
                    broadcast("UPDATE_MONEY:" + QString::number(sender->id) + ":" + QString::number(sender->geld));
                    broadcast("UPDATE_OWNER:" + QString::number(feldId) + ":" + QString::number(sender->id));
                    broadcast("MSG:" + sender->name + " hat das Feld gekauft.");
                }
                naechsterSpieler();
            } else if(cmd == "CMD_NOBUY") {
                naechsterSpieler();
            } else if(cmd == "CMD_BUILD") {
                // HAUSBAU LOGIK
                int feldId = parts[1].toInt();
                StadtFeld* stadt = dynamic_cast<StadtFeld*>(brett[feldId]);
                if(stadt && stadt->besitzerID == sender->id && stadt->haeuser < 5 && sender->geld >= stadt->hausPreis) {
                    sender->geld -= stadt->hausPreis;
                    stadt->haeuser++;
                    broadcast("UPDATE_MONEY:" + QString::number(sender->id) + ":" + QString::number(sender->geld));
                    broadcast("UPDATE_HOUSES:" + QString::number(feldId) + ":" + QString::number(stadt->haeuser));
                    QString gebaeude = (stadt->haeuser == 5) ? "ein Hotel 🏨" : "ein Haus 🏠";
                    broadcast("MSG:" + sender->name + " baut " + gebaeude + " auf Feld " + QString::number(feldId) + ".");
                }
            }
        }
    }
};

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MonopolyServer server;
    return a.exec();
}
