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
    // Signale, um dem Hauptprogramm zu sagen: "Starte als Host" oder "Starte als Gast"
    void hostGameClicked(QString name);
    void joinGameClicked(QString ip, QString name);

private slots:
    void onHost();
    void onJoin();

private:
    QLineEdit *hostNameInput;
    QLineEdit *guestNameInput;
    QLineEdit *ipInput;

    QPushButton *btnHost;
    QPushButton *btnJoin;

    void setupUi();
    void setupStyle();
};

#endif // STARTSCREEN_H
