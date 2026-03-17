import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Client_B 1.0

Rectangle {
    id: lobbyRoot
    anchors.fill: parent

    gradient: Gradient {
        GradientStop { position: 0.0; color: "#6366f1" }
        GradientStop { position: 1.0; color: "#7c3aed" }
    }

    Connections {
        target: MonopolyClient
        function onConnectedChanged() {
            if (MonopolyClient.connected) {
                stackView.push(gameScreenComponent)
            }
        }
        function onConnectionFailed() {
            errorText.visible = true
            errorTimer.start()
        }
    }

    Timer {
        id: errorTimer
        interval: 3000
        onTriggered: errorText.visible = false
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 20

        // Back button
        RowLayout {
            Layout.fillWidth: true

            Text {
                text: "← Zurück"
                font.pixelSize: 16
                font.bold: true
                color: "white"
                opacity: 0.9

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: stackView.pop()
                }
            }
            Item { Layout.fillWidth: true }
        }

        // Title
        Text {
            text: "Lobby"
            font.pixelSize: 42
            font.bold: true
            color: "white"
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: "Trete einem Spiel bei oder erstelle ein neues"
            font.pixelSize: 13
            color: "#e2e8f0"
            Layout.alignment: Qt.AlignHCenter
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
        }

        Item { height: 10; width: 1 }

        // Spiel Beitreten Card
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: joinCol.implicitHeight + 40
            radius: 16
            color: "#ffffff18"
            border.color: "#ffffff22"
            border.width: 1

            ColumnLayout {
                id: joinCol
                anchors.fill: parent
                anchors.margins: 20
                spacing: 14

                Row {
                    spacing: 10
                    Text { text: "🖥️"; font.pixelSize: 22 }
                    Text {
                        text: "Spiel Beitreten"
                        font.pixelSize: 20
                        font.bold: true
                        color: "white"
                    }
                }

                // Name input
                TextField {
                    id: nameInput
                    Layout.fillWidth: true
                    placeholderText: "Dein Spielername"
                    text: "Spieler 1"
                    font.pixelSize: 14
                    color: "#1e293b"
                    placeholderTextColor: "#94a3b8"

                    background: Rectangle {
                        radius: 10
                        color: "white"
                        border.color: nameInput.activeFocus ? "#818cf8" : "#e2e8f0"
                        border.width: nameInput.activeFocus ? 2 : 1
                    }

                    leftPadding: 14
                    topPadding: 12
                    bottomPadding: 12
                }

                // IP input
                TextField {
                    id: ipInput
                    Layout.fillWidth: true
                    placeholderText: "IP-Adresse eingeben (z.B. 192.168.1.100)"
                    text: "127.0.0.1"
                    font.pixelSize: 14
                    color: "#1e293b"
                    placeholderTextColor: "#94a3b8"

                    background: Rectangle {
                        radius: 10
                        color: "white"
                        border.color: ipInput.activeFocus ? "#818cf8" : "#e2e8f0"
                        border.width: ipInput.activeFocus ? 2 : 1
                    }

                    leftPadding: 14
                    topPadding: 12
                    bottomPadding: 12
                }

                // Error message
                Text {
                    id: errorText
                    text: "❌ Server nicht erreichbar!"
                    color: "#fca5a5"
                    font.pixelSize: 13
                    font.bold: true
                    visible: false
                    Layout.alignment: Qt.AlignHCenter
                }

                // Join button
                Rectangle {
                    id: joinBtn
                    Layout.fillWidth: true
                    height: 48
                    radius: 12
                    color: "#7c3aed"

                    gradient: Gradient {
                        GradientStop { position: 0.0; color: "#818cf8" }
                        GradientStop { position: 1.0; color: "#6366f1" }
                    }

                    Text {
                        anchors.centerIn: parent
                        text: "Spiel Beitreten"
                        font.pixelSize: 16
                        font.bold: true
                        color: "white"
                    }

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onPressed: joinBtn.scale = 0.97
                        onReleased: joinBtn.scale = 1.0
                        onClicked: {
                            MonopolyClient.connectToServer(ipInput.text, nameInput.text)
                        }
                    }

                    Behavior on scale { NumberAnimation { duration: 80 } }
                }
            }
        }

        // Divider
        Row {
            Layout.fillWidth: true
            Layout.topMargin: 5
            Layout.bottomMargin: 5
            spacing: 10

            Rectangle { height: 1; Layout.fillWidth: true; color: "#ffffff33"; anchors.verticalCenter: parent.verticalCenter; width: 1 }
            Text { text: "ODER"; font.pixelSize: 12; color: "#ffffff66"; font.bold: true }
            Rectangle { height: 1; Layout.fillWidth: true; color: "#ffffff33"; anchors.verticalCenter: parent.verticalCenter; width: 1 }
        }

        // Neues Spiel Card (placeholder for future server hosting)
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: newCol.implicitHeight + 40
            radius: 16
            color: "#ffffff18"
            border.color: "#ffffff22"
            border.width: 1

            ColumnLayout {
                id: newCol
                anchors.fill: parent
                anchors.margins: 20
                spacing: 14

                Row {
                    spacing: 10
                    Text { text: "🎮"; font.pixelSize: 22 }
                    Text {
                        text: "Neues Spiel Erstellen"
                        font.pixelSize: 20
                        font.bold: true
                        color: "white"
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 48
                    radius: 12

                    gradient: Gradient {
                        GradientStop { position: 0.0; color: "#4ade80" }
                        GradientStop { position: 1.0; color: "#16a34a" }
                    }

                    Text {
                        anchors.centerIn: parent
                        text: "Neues Spiel Erstellen"
                        font.pixelSize: 16
                        font.bold: true
                        color: "white"
                    }

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            // Server muss extern gestartet werden
                        }
                    }
                }
            }
        }

        Item { Layout.fillHeight: true }
    }
}
