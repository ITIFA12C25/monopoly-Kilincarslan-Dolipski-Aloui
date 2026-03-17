import QtQuick
import QtQuick.Layouts
import Client_B 1.0

Rectangle {
    id: headerRoot
    height: 60
    color: "#1e293b"
    border.color: "#f59e0b"
    border.width: 2
    radius: 0

    property var playersList: MonopolyClient.players
    property int myId: MonopolyClient.myId

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 16
        anchors.rightMargin: 16
        spacing: 12

        // Player icon
        Rectangle {
            width: 40
            height: 40
            radius: 20
            color: "#ef4444"
            border.color: "#fca5a5"
            border.width: 2

            Text {
                anchors.centerIn: parent
                text: "🎯"
                font.pixelSize: 18
            }
        }

        Column {
            Layout.fillWidth: true
            spacing: 2

            Text {
                text: {
                    if (playersList.length > 0 && myId >= 0 && myId < playersList.length)
                        return playersList[myId].name
                    return MonopolyClient.myName
                }
                font.pixelSize: 18
                font.bold: true
                color: "white"
            }

            Text {
                text: "Runde " + (playersList.length > 0 ? "●" : "...")
                font.pixelSize: 12
                color: "#f59e0b"
                font.bold: true
            }
        }

        // Trophy icon
        Text {
            text: "🏆"
            font.pixelSize: 26
        }
    }
}
