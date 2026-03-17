#ifndef MONOPOLYCLIENT_H
#define MONOPOLYCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QVariantList>
#include <QVariantMap>

class MonopolyClient : public QObject {
    Q_OBJECT

    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)
    Q_PROPERTY(int myId READ myId NOTIFY myIdChanged)
    Q_PROPERTY(QString myName READ myName NOTIFY myNameChanged)
    Q_PROPERTY(QVariantList players READ players NOTIFY playersChanged)
    Q_PROPERTY(QVariantList fields READ fields NOTIFY fieldsChanged)
    Q_PROPERTY(QVariantList logMessages READ logMessages NOTIFY logMessagesChanged)
    Q_PROPERTY(bool canRoll READ canRoll NOTIFY canRollChanged)

public:
    explicit MonopolyClient(QObject *parent = nullptr);

    bool isConnected() const;
    int myId() const;
    QString myName() const;
    QVariantList players() const;
    QVariantList fields() const;
    QVariantList logMessages() const;
    bool canRoll() const;

    Q_INVOKABLE void connectToServer(const QString &ip, const QString &playerName);
    Q_INVOKABLE void rollDice();
    Q_INVOKABLE void buyField(int fieldId);
    Q_INVOKABLE void rejectBuy();
    Q_INVOKABLE void buildHouse(int fieldId);

    Q_INVOKABLE QString fieldName(int id) const;
    Q_INVOKABLE int fieldPrice(int id) const;
    Q_INVOKABLE QString fieldColor(int id) const;
    Q_INVOKABLE QString fieldIcon(int id) const;

signals:
    void connectedChanged();
    void myIdChanged();
    void myNameChanged();
    void playersChanged();
    void fieldsChanged();
    void logMessagesChanged();
    void canRollChanged();
    void connectionFailed();

    // UI event signals
    void askBuyField(int fieldId, int price);
    void gameOver(QString message);
    void showFieldInfo(int fieldId, QString name, int price, QString owner, int houses, bool canBuild);

private slots:
    void onDataReceived();

private:
    QTcpSocket *m_socket;
    bool m_connected;
    int m_myId;
    QString m_myName;

    struct PlayerData {
        QString name;
        double money = 1500;
        int position = 0;
    };

    struct FieldData {
        QString name;
        int price = 0;
        int ownerID = -1;
        int houses = 0;
    };

    QVector<PlayerData> m_players;
    QVector<FieldData> m_fields;
    QStringList m_logMessages;

    void initFields();
    void addLog(const QString &text);
    void updateCanRoll();
    bool m_canRoll;
};

#endif // MONOPOLYCLIENT_H
