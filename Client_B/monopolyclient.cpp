#include "monopolyclient.h"
#include <QTextStream>

MonopolyClient::MonopolyClient(QObject *parent)
    : QObject(parent), m_socket(new QTcpSocket(this)),
      m_connected(false), m_myId(-1), m_canRoll(false)
{
    initFields();
    connect(m_socket, &QTcpSocket::readyRead, this, &MonopolyClient::onDataReceived);
}

void MonopolyClient::initFields() {
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

    m_fields.resize(40);
    for (int i = 0; i < 40; i++) {
        m_fields[i].name = names[i];
        m_fields[i].price = fieldPrice(i);
        m_fields[i].ownerID = -1;
        m_fields[i].houses = 0;
    }
}

// --- Property Getters ---

bool MonopolyClient::isConnected() const { return m_connected; }
int MonopolyClient::myId() const { return m_myId; }
QString MonopolyClient::myName() const { return m_myName; }
bool MonopolyClient::canRoll() const { return m_canRoll; }

QVariantList MonopolyClient::players() const {
    QVariantList list;
    for (int i = 0; i < m_players.size(); i++) {
        QVariantMap map;
        map["id"] = i;
        map["name"] = m_players[i].name;
        map["money"] = m_players[i].money;
        map["position"] = m_players[i].position;
        list.append(map);
    }
    return list;
}

QVariantList MonopolyClient::fields() const {
    QVariantList list;
    for (int i = 0; i < m_fields.size(); i++) {
        QVariantMap map;
        map["id"] = i;
        map["name"] = m_fields[i].name;
        map["price"] = m_fields[i].price;
        map["ownerID"] = m_fields[i].ownerID;
        map["houses"] = m_fields[i].houses;
        list.append(map);
    }
    return list;
}

QVariantList MonopolyClient::logMessages() const {
    QVariantList list;
    for (const auto &msg : m_logMessages)
        list.append(msg);
    return list;
}

// --- Helper Functions ---

QString MonopolyClient::fieldName(int id) const {
    if (id >= 0 && id < 40) return m_fields[id].name;
    return "Unbekannt";
}

int MonopolyClient::fieldPrice(int i) const {
    if (i % 10 == 0) return 0;
    if (i == 5 || i == 15 || i == 25 || i == 35) return 200;
    if (i == 12 || i == 28) return 150;
    return 60 + (i * 8);
}

QString MonopolyClient::fieldColor(int id) const {
    if (id == 1 || id == 2) return "#8d6e63";
    if (id == 3 || id == 4) return "#81d4fa";
    if (id >= 6 && id <= 9) return "#f48fb1";
    if (id >= 11 && id <= 14) return "#ffcc80";
    if (id >= 16 && id <= 19) return "#ef5350";
    if (id >= 21 && id <= 24) return "#fff59d";
    if (id >= 26 && id <= 29) return "#a5d6a7";
    if (id >= 31 && id <= 34) return "#90caf9";
    if (id >= 36 && id <= 39) return "#ce93d8";
    return "transparent";
}

QString MonopolyClient::fieldIcon(int id) const {
    if (id == 0) return QString::fromUtf8("\xF0\x9F\x8F\x81");       // 🏁
    if (id == 10) return QString::fromUtf8("\xE2\x9B\x93\xEF\xB8\x8F"); // ⛓️
    if (id == 20) return QString::fromUtf8("\xF0\x9F\x85\xBF\xEF\xB8\x8F"); // 🅿️
    if (id == 30) return QString::fromUtf8("\xF0\x9F\x91\xAE");       // 👮
    if (id == 5 || id == 15 || id == 25 || id == 35)
        return QString::fromUtf8("\xF0\x9F\x9A\x82"); // 🚂
    if (id == 12) return QString::fromUtf8("\xF0\x9F\x92\xA1"); // 💡
    if (id == 28) return QString::fromUtf8("\xF0\x9F\x92\xA7"); // 💧
    return "";
}

void MonopolyClient::addLog(const QString &text) {
    m_logMessages.prepend(text);
    if (m_logMessages.size() > 100)
        m_logMessages.removeLast();
    emit logMessagesChanged();
}

void MonopolyClient::updateCanRoll() {
    bool newVal = m_connected && m_players.size() >= 2;
    if (newVal != m_canRoll) {
        m_canRoll = newVal;
        emit canRollChanged();
    }
}

// --- Invokable Methods ---

void MonopolyClient::connectToServer(const QString &ip, const QString &playerName) {
    m_myName = playerName;
    emit myNameChanged();

    m_socket->connectToHost(ip, 1234);
    if (m_socket->waitForConnected(3000)) {
        m_connected = true;
        emit connectedChanged();
        addLog("Verbunden! Warte auf Mitspieler...");

        QTextStream out(m_socket);
        out << "NAME:" << m_myName << "\n";
        out.flush();
    } else {
        m_connected = false;
        emit connectedChanged();
        emit connectionFailed();
        addLog("Server nicht erreichbar!");
    }
}

void MonopolyClient::rollDice() {
    if (m_players.size() < 2) return;
    if (m_socket && m_socket->isOpen()) {
        QTextStream out(m_socket);
        out << "CMD_ROLL\n";
        out.flush();
    }
}

void MonopolyClient::buyField(int fieldId) {
    if (m_socket && m_socket->isOpen()) {
        QTextStream out(m_socket);
        out << "CMD_BUY:" << fieldId << "\n";
        out.flush();
    }
}

void MonopolyClient::rejectBuy() {
    if (m_socket && m_socket->isOpen()) {
        QTextStream out(m_socket);
        out << "CMD_NOBUY\n";
        out.flush();
    }
}

void MonopolyClient::buildHouse(int fieldId) {
    if (m_socket && m_socket->isOpen()) {
        QTextStream out(m_socket);
        out << "CMD_BUILD:" << fieldId << "\n";
        out.flush();
    }
}

// --- Data Receiving (identical protocol to Client) ---

// *** BEGINN KI-generiert Label: Prompt 1 ***
// Ursprünglich von KI generiert, dann an unsere spezifischen Command-Strings angepasst.
void MonopolyClient::onDataReceived() {
    QByteArray data = m_socket->readAll();
    QStringList lines = QString(data).split("\n", Qt::SkipEmptyParts);

    for (const QString &msg : lines) {
        QStringList parts = msg.split(":");
        QString cmd = parts[0];

        if (cmd == "MSG") {
            addLog(parts.mid(1).join(":"));  // rejoin in case message had colons
        }
        else if (cmd == "ASSIGN_ID") {
            m_myId = parts[1].toInt();
            emit myIdChanged();
        }
// *** ENDE KI-generiert Label: Prompt 1 ***
        else if (cmd == "PLAYER_UPDATE") {
            int id = parts[1].toInt();
            if (id >= m_players.size()) m_players.resize(id + 1);
            m_players[id].name = parts[2];
            m_players[id].money = parts[3].toDouble();
            emit playersChanged();
            updateCanRoll();
        }
        else if (cmd == "UPDATE_POS") {
            int id = parts[1].toInt();
            if (id < m_players.size()) {
                m_players[id].position = parts[2].toInt();
                emit playersChanged();
            }
        }
        else if (cmd == "UPDATE_MONEY") {
            int id = parts[1].toInt();
            if (id < m_players.size()) {
                m_players[id].money = parts[2].toDouble();
                emit playersChanged();
            }
        }
        else if (cmd == "UPDATE_OWNER") {
            int fieldId = parts[1].toInt();
            if (fieldId >= 0 && fieldId < 40) {
                m_fields[fieldId].ownerID = parts[2].toInt();
                emit fieldsChanged();
            }
        }
        else if (cmd == "UPDATE_HOUSES") {
            int fieldId = parts[1].toInt();
            if (fieldId >= 0 && fieldId < 40) {
                m_fields[fieldId].houses = parts[2].toInt();
                emit fieldsChanged();
            }
        }
        else if (cmd == "GAME_OVER") {
            m_canRoll = false;
            emit canRollChanged();
            emit gameOver("Das Spiel ist vorbei! Ein Spieler ist bankrott gegangen.");
        }
        else if (cmd == "ASK_BUY") {
            int feldId = parts[1].toInt();
            int price = parts[2].toInt();
            emit askBuyField(feldId, price);
        }
    }
}
