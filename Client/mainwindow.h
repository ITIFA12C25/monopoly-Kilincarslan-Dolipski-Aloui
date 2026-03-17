#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <vector>
#include <QLabel>
#include <QFrame>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct UI_Spieler {
    QString name;
    double geld;
    int position;
};

struct UI_Feld {
    int besitzerID = -1;
    int haeuser = 0;
    int preis = 0;
    QString name;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QString playerName, QString targetIp, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnWuerfeln_clicked();
    void datenEmpfangen();

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;
    QString meinName;
    int meineID;

    std::vector<UI_Spieler> spielerUI;
    std::vector<UI_Feld> brettUI;

    std::vector<QFrame*> feldFrames;
    std::vector<QLabel*> feldLabels;
    std::vector<QLabel*> spielerLabels;
    std::vector<QLabel*> preisLabels;
    std::vector<QLabel*> hausLabels;

    QLabel* statsLabel;

    void setupBrettUI();
    void updateBrettUI();
    void log(QString text);
    void schreibeCsvLog(QString aktion);
    void getKoordinaten(int id, int &r, int &c);

    QString getFeldFarbe(int id);
    QString getFeldIcon(int id);
    QString getFeldName(int id);
    int getFeldPreis(int id);
};

#endif
