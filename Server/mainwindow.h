#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <vector>
#include <string>
#include <QLabel>
#include <QFrame>

using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// --- DATEN KLASSEN ---

class Spieler {
public:
    int id;
    string name;
    double geld;
    int position;
    bool gefangen;
    string farbe;

    Spieler(int id, string n, string c)
        : id(id), name(n), geld(1500), position(0), gefangen(false), farbe(c) {}
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
    double preis;
    double basisMiete;
    double hausPreis;
    int haeuser;
    int besitzerID;

    StadtFeld(int id, string n, double p, double m, double hp)
        : Feld(id, n), preis(p), basisMiete(m), hausPreis(hp), haeuser(0), besitzerID(-1) {}

    double getAktuelleMiete() {
        if (haeuser == 0) return basisMiete;
        if (haeuser == 5) return basisMiete * 10;
        return basisMiete * (haeuser + 1) * 1.5;
    }
};

class SonderFeld : public Feld {
public:
    string typ;
    SonderFeld(int id, string n, string t) : Feld(id, n), typ(t) {}
};

// --- HAUPTFENSTER ---

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(bool isHost, QString playerName, QString targetIp = "", QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnWuerfeln_clicked();

    // Netzwerk Slots
    void neuerClientVerbunden();
    void datenEmpfangen();
    void verbindungGetrennt();

private:
    Ui::MainWindow *ui;

    vector<Spieler*> spielerListe;
    vector<Feld*> brett;
    int aktuellerSpielerIndex;

    QTcpServer *server;
    QTcpSocket *socket;
    bool binIchServer;
    QString meinName;

    vector<QFrame*> feldFrames;
    vector<QLabel*> feldLabels;
    vector<QLabel*> spielerLabels;
    vector<QLabel*> hausLabels;
    vector<QLabel*> preisLabels; // <--- NEU: Damit wir den Preis ausblenden können

    // NEU: Ein eigenes Label für die Geldanzeige rechts
    QLabel* statsLabel;

    void setupDaten();
    void setupBrettUI();
    void updateBrettUI();

    void verarbeiteFeldLogik(Spieler* s);
    void checkGameOver();
    void sendeNachricht(QString text);
    void log(QString text);
    void getKoordinaten(int feldIndex, int &row, int &col);

    void startHost();
    void connectToHost(QString ip);
};

#endif // MAINWINDOW_H
