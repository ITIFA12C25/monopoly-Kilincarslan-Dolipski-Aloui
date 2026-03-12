import QtQuick
import QtQuick.Controls
import Client_B

import QtQuick.Layouts

ApplicationWindow {
    id: window
    width: 390
    height: 844
    visible: true
    title: "Monopoly "

    StackView {
        id: stack
        anchors.fill: parent
        initialItem: connectScreen
    }

    /* =======================
       CONNECT / LOBBY SCREEN
       ======================= */
    Component {
        id: connectScreen

        Page {
            ColumnLayout {
                anchors.fill: parent
                spacing: 16
                anchors.margins: 20

                Label {
                    text: "Online-Spiel"
                    font.pixelSize: 28
                    Layout.alignment: Qt.AlignHCenter
                }

                TextField {
                    placeholderText: "Server IP"
                    Layout.fillWidth: true
                }

                TextField {
                    placeholderText: "Port"
                    inputMethodHints: Qt.ImhDigitsOnly
                    Layout.fillWidth: true
                }

                TextField {
                    placeholderText: "Spielername"
                    Layout.fillWidth: true
                }

                Button {
                    text: "Verbinden"
                    Layout.fillWidth: true
                    onClicked: stack.push(gameScreen)
                }
            }
        }
    }

    /* =======================
       GAME SCREEN
       ======================= */
    Component {
        id: gameScreen

        Page {
            ColumnLayout {
                anchors.fill: parent
                spacing: 8
                anchors.margins: 8

                /* --- HEADER / STATUS --- */
                Rectangle {
                    color: "#222"
                    radius: 8
                    Layout.fillWidth: true
                    Layout.preferredHeight: 80

                    Column {
                        anchors.centerIn: parent
                        spacing: 4

                        Text {
                            text: "Aktiver Spieler: Spieler 1"
                            color: "white"
                            font.pixelSize: 18
                        }

                        Text {
                            text: "Runde 3"
                            color: "#cccccc"
                            font.pixelSize: 14
                        }
                    }
                }

                /* --- SPIELFELD --- */
                Rectangle {
                    color: "#444"
                    radius: 10
                    Layout.fillWidth: true
                    Layout.preferredHeight: 350

                    Text {
                        anchors.centerIn: parent
                        text: "Spielfeld\n(Platzhalter)"
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter
                    }
                }

                /* --- SPIELER INFO --- */
                Rectangle {
                    color: "#333"
                    radius: 8
                    Layout.fillWidth: true
                    Layout.preferredHeight: 120

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 8
                        spacing: 12

                        Column {
                            Text { text: "Spieler 1"; color: "white" }
                            Text { text: "Geld: 1500 €"; color: "#aaa" }
                        }

                        Column {
                            Text { text: "Spieler 2"; color: "white" }
                            Text { text: "Geld: 1400 €"; color: "#aaa" }
                        }
                    }
                }

                /* --- AKTIONEN --- */
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 10

                    Button {
                        text: "🎲 Würfeln"
                        Layout.fillWidth: true
                    }

                    Button {
                        text: "Zug beenden"
                        Layout.fillWidth: true
                    }
                }

                /* --- EVENT LOG --- */
                Rectangle {
                    color: "#222"
                    radius: 8
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    ListView {
                        anchors.fill: parent
                        model: [
                            "Spiel gestartet",
                            "Spieler 1 würfelt 4",
                            "Spieler 1 zieht"
                        ]

                        delegate: Text {
                            text: modelData
                            color: "white"
                            padding: 6
                        }
                    }
                }
            }
        }
    }
}
