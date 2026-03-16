#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFile>
#include <QDateTime>
#include <QTextStream>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QMouseEvent>
#include <functional>
#include <QIcon>

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

MainWindow::MainWindow(QString playerName, QString targetIp, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), meinName(playerName), meineID(-1)
{
    ui->setupUi(this);
    setupBrettUI();

    statsLabel = new QLabel("Lade...", this);
    statsLabel->setStyleSheet("font-size: 12px; font-family: Courier; background: white; border: 1px solid #aaa; padding: 10px;");

    if (ui->controlsContainer && ui->controlsContainer->layout()) {
        qobject_cast<QVBoxLayout*>(ui->controlsContainer->layout())->insertWidget(1, statsLabel);
    }

    ui->btnWuerfeln->setEnabled(false);

    socket = new QTcpSocket(this);
    socket->connectToHost(targetIp, 1234);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::datenEmpfangen);

    if(socket->waitForConnected(3000)) {
        log("Verbunden! Warte auf Mitspieler...");
        QTextStream out(socket);
        out << "NAME:" << meinName << "\n";
    } else {
        QMessageBox msg(this);
        msg.setWindowTitle("Fehler");
        msg.setWindowIcon(QIcon());
        msg.setIcon(QMessageBox::NoIcon);
        msg.setText("Server nicht erreichbar!");
        msg.exec();
    }
}

MainWindow::~MainWindow() { delete ui; }

QString MainWindow::getFeldName(int id) {
    // Ein komplettes Array für alle 40 Felder (Deutschland Edition)
    QString names[40] = {
        "LOS", "Flensburg", "Kiel", "Lübeck", "Rostock",
        "Südbahnhof", "Bremen", "Hannover", "Wolfsburg", "Osnabrück",
        "GEFÄNGNIS", "Dortmund", "E-Werk", "Bochum", "Essen",
        "Westbahnhof", "Düsseldorf", "Köln", "Bonn", "Aachen",
        "PARKEN", "Frankfurt", "Wiesbaden", "Mainz", "Darmstadt",
        "Nordbahnhof", "Mannheim", "Karlsruhe", "Wasserwerk", "Freiburg",
        "POLIZEI", "Stuttgart", "Ulm", "Augsburg", "München",
        "Hauptbahnhof", "Nürnberg", "Regensburg", "Leipzig", "Berlin"
    };

    if(id >= 0 && id < 40) return names[id];
    return "Unbekannt";
}

int MainWindow::getFeldPreis(int i) {
    if (i % 10 == 0) return 0;
    if (i == 5 || i == 15 || i == 25 || i == 35) return 200;
    if (i == 12 || i == 28) return 150;
    return 60 + (i * 8);
}

QString MainWindow::getFeldFarbe(int id) {
    if (id == 1 || id == 2) return "#8d6e63"; if (id == 3 || id == 4) return "#81d4fa";
    if (id >= 6 && id <= 9) return "#f48fb1"; if (id >= 11 && id <= 14) return "#ffcc80";
    if (id >= 16 && id <= 19) return "#ef5350"; if (id >= 21 && id <= 24) return "#fff59d";
    if (id >= 26 && id <= 29) return "#a5d6a7"; if (id >= 31 && id <= 34) return "#90caf9";
    if (id >= 36 && id <= 39) return "#ce93d8";
    return "transparent";
}

QString MainWindow::getFeldIcon(int id) {
    if (id == 0) return "🏁"; if (id == 10) return "⛓️"; if (id == 20) return "🅿️"; if (id == 30) return "👮";
    if (id == 5 || id == 15 || id == 25 || id == 35) return "🚂";
    if (id == 12) return "💡"; if (id == 28) return "💧";
    return "";
}

void MainWindow::setupBrettUI() {
    QGridLayout* grid = ui->gridLayoutBrett;
    grid->setSpacing(2);
    brettUI.resize(40);

    for(int i=0; i<40; i++) {
        int r, c; getKoordinaten(i, r, c);
        brettUI[i].name = getFeldName(i);
        brettUI[i].preis = getFeldPreis(i);

        auto clickAction = [this, i]() {
            if(i < 0 || i >= (int)brettUI.size()) return;

            QMessageBox msg(this);
            msg.setWindowIcon(QIcon());
            msg.setIcon(QMessageBox::NoIcon);
            msg.setWindowFlags(msg.windowFlags() | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

            // GEHÖRT DAS FELD MIR? -> BAUEN
            if (brettUI[i].besitzerID == meineID && brettUI[i].haeuser < 5 && brettUI[i].preis > 0) {
                msg.setWindowTitle("Bauen");
                QString text = (brettUI[i].haeuser == 4) ? "Hotel bauen?" : "Haus bauen?";
                msg.setText("Möchtest du ein " + text);
                msg.setInformativeText("Kosten: 50 $\nAktuelle Häuser: " + QString::number(brettUI[i].haeuser));
                msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                msg.setDefaultButton(QMessageBox::Yes);

                if(msg.exec() == QMessageBox::Yes) {
                    QTextStream(socket) << "CMD_BUILD:" << i << "\n";
                }
            } else {
                // NORMALES INFO FENSTER
                msg.setWindowTitle("Feld Info");
                QString info = "<b>" + brettUI[i].name + "</b>";
                if(brettUI[i].preis > 0) {
                    info += "<br><br>Kaufpreis: " + QString::number(brettUI[i].preis) + "$";
                    QString besitzer = (brettUI[i].besitzerID == -1) ? "Bank" : spielerUI[brettUI[i].besitzerID].name;
                    info += "<br><br>Besitzer: " + besitzer;
                    info += "<br>Häuser: " + QString::number(brettUI[i].haeuser);
                }
                msg.setText(info);
                msg.exec();
            }
        };

        ClickableFrame* frame = new ClickableFrame(this, clickAction);
        frame->setFrameStyle(QFrame::Box | QFrame::Raised);
        frame->setCursor(Qt::PointingHandCursor);
        QVBoxLayout* vLayout = new QVBoxLayout(frame);
        vLayout->setContentsMargins(0,0,0,0);
        vLayout->setSpacing(0);

        QLabel* lColorBar = new QLabel();
        lColorBar->setFixedHeight(15);
        QString farbe = getFeldFarbe(i);
        lColorBar->setStyleSheet("background-color: " + farbe + "; border-bottom: 1px solid #aaa;");

        QLabel* lName = new QLabel(getFeldIcon(i) + "\n" + brettUI[i].name);
        lName->setAlignment(Qt::AlignCenter);
        lName->setWordWrap(true);
        lName->setStyleSheet("font-size: 6px; font-weight: bold; background: transparent;");

        QLabel* lPreis = new QLabel(brettUI[i].preis > 0 ? QString::number(brettUI[i].preis) + "$" : "");
        lPreis->setAlignment(Qt::AlignCenter);
        lPreis->setStyleSheet("font-size: 6px; background: transparent;");

        // NEU: Haus Label für Emojis (🏠 / 🏨) eingefügt
        QLabel* lHaus = new QLabel("");
        lHaus->setAlignment(Qt::AlignCenter);
        lHaus->setStyleSheet("font-size: 8px; background: transparent;");

        QLabel* lSpieler = new QLabel("");
        lSpieler->setAlignment(Qt::AlignCenter);

        vLayout->addWidget(lColorBar);
        vLayout->addWidget(lName);
        vLayout->addWidget(lPreis);
        vLayout->addWidget(lHaus);
        vLayout->addWidget(lSpieler);

        feldFrames.push_back(frame);
        preisLabels.push_back(lPreis);
        hausLabels.push_back(lHaus);
        spielerLabels.push_back(lSpieler);
        grid->addWidget(frame, r, c);
    }
}

void MainWindow::getKoordinaten(int id, int &r, int &c) {
    if (id <= 10) { r=10; c=10-id; } else if (id <= 20) { r=10-(id-10); c=0; }
    else if (id <= 30) { r=0; c=id-20; } else { r=id-30; c=10; }
}

void MainWindow::updateBrettUI() {
    for(int i=0; i<40; i++) {
        QString style = "QFrame { background-color: white; border: 1px solid #333; border-radius: 4px; }";
        if(brettUI[i].besitzerID == 0) style = "QFrame { background-color: #ffebee; border: 2px solid red; }";
        if(brettUI[i].besitzerID == 1) style = "QFrame { background-color: #e3f2fd; border: 2px solid blue; }";

        if(brettUI[i].besitzerID != -1) preisLabels[i]->setText("");
        feldFrames[i]->setStyleSheet(style);
        spielerLabels[i]->setText("");

        // HÄUSER ANZEIGEN
        if (brettUI[i].haeuser == 0) {
            hausLabels[i]->setText("");
        } else if (brettUI[i].haeuser == 5) {
            hausLabels[i]->setText("🏨");
        } else {
            QString h = "";
            for(int k=0; k<brettUI[i].haeuser; k++) h += "🏠";
            hausLabels[i]->setText(h);
        }
    }

    for(size_t i=0; i<spielerUI.size(); i++) {
        QLabel* lbl = spielerLabels[spielerUI[i].position];
        QString marker = (i == 0) ? "🔴" : "🔵";
        lbl->setText(lbl->text() + " " + marker);
    }

    QString status = "";
    for(auto s : spielerUI) {
        QString farbe = (s.name == meinName) ? "black" : "gray";
        status += QString("<span style='color:%1'>%2: <b>%3 $</b></span><br>").arg(farbe).arg(s.name).arg(s.geld);
    }
    statsLabel->setText(status);

    ui->btnWuerfeln->setEnabled(spielerUI.size() >= 2);
}

void MainWindow::on_btnWuerfeln_clicked() {
    if (spielerUI.size() < 2) {
        QMessageBox msg(this);
        msg.setWindowTitle("Warten");
        msg.setWindowIcon(QIcon());
        msg.setIcon(QMessageBox::NoIcon);
        msg.setText("Warte, bis der zweite Spieler beigetreten ist!");
        msg.exec();
        return;
    }
    if(socket && socket->isOpen()) {
        QTextStream(socket) << "CMD_ROLL\n";
    }
}

void MainWindow::datenEmpfangen() {
    QByteArray data = socket->readAll();
    QStringList lines = QString(data).split("\n", Qt::SkipEmptyParts);

    for(QString msg : lines) {
        QStringList parts = msg.split(":");
        QString cmd = parts[0];

        if(cmd == "MSG") log(parts[1]);
        else if(cmd == "ASSIGN_ID") meineID = parts[1].toInt(); // SEHR WICHTIG FÜR HAUSBAU
        else if(cmd == "PLAYER_UPDATE") {
            int id = parts[1].toInt();
            if(id >= (int)spielerUI.size()) spielerUI.resize(id + 1);
            spielerUI[id].name = parts[2];
            spielerUI[id].geld = parts[3].toDouble();
            updateBrettUI();
        }
        else if(cmd == "UPDATE_POS") {
            spielerUI[parts[1].toInt()].position = parts[2].toInt();
            updateBrettUI();
        }
        else if(cmd == "UPDATE_MONEY") {
            if(parts[1].toInt() < (int)spielerUI.size())
                spielerUI[parts[1].toInt()].geld = parts[2].toDouble();
            updateBrettUI();
        }
        else if(cmd == "UPDATE_OWNER") {
            brettUI[parts[1].toInt()].besitzerID = parts[2].toInt();
            updateBrettUI();
        }
        else if(cmd == "UPDATE_HOUSES") {
            int feldId = parts[1].toInt();
            int haeuser = parts[2].toInt();
            brettUI[feldId].haeuser = haeuser;
            updateBrettUI();
        }
        else if(cmd == "ASK_BUY") {
            QMessageBox msgBox(this);
            msgBox.setWindowIcon(QIcon());
            msgBox.setWindowTitle("Kaufentscheidung");
            msgBox.setIcon(QMessageBox::NoIcon);
            msgBox.setWindowFlags(msgBox.windowFlags() | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

            msgBox.setText("Möchtest du dieses Feld kaufen?");
            msgBox.setInformativeText("Kosten: " + parts[2] + " $");
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::Yes);

            if(msgBox.exec() == QMessageBox::Yes) {
                QTextStream(socket) << "CMD_BUY:" << parts[1] << "\n";
            } else {
                QTextStream(socket) << "CMD_NOBUY\n";
            }
        }
    }
}

void MainWindow::log(QString t) { ui->logAusgabe->append(t); }
