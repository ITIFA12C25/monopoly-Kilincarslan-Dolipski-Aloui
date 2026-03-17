import QtQuick
import QtQuick.Layouts
import Client_B 1.0

Item {
    id: playerListRoot
    height: playerGrid.implicitHeight + 16

    property var playersList: MonopolyClient.players
    property int myId: MonopolyClient.myId

    property var playerColors: ["#ef4444", "#3b82f6", "#22c55e", "#f59e0b"]

    Grid {
        id: playerGrid
        anchors.fill: parent
        anchors.margins: 4
        columns: 2
        spacing: 6

        Repeater {
            model: playersList.length

            Rectangle {
                width: (playerGrid.width - 6) / 2
                height: 52
                radius: 10
                color: index === myId ? "#1e293bdd" : "#1e293b99"
                border.color: index === myId ? playerColors[index % 4] : "#334155"
                border.width: index === myId ? 2 : 1

                Row {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 10

                    // Color dot
                    Rectangle {
                        width: 14
                        height: 14
                        radius: 7
                        color: playerColors[index % 4]
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Column {
                        anchors.verticalCenter: parent.verticalCenter
                        spacing: 2

                        Text {
                            text: playersList[index].name
                            font.pixelSize: 13
                            font.bold: true
                            color: "white"
                        }

                        Text {
                            text: "💰 " + Math.floor(playersList[index].money) + "€"
                            font.pixelSize: 11
                            color: "#fde047"
                        }
                    }
                }
            }
        }
    }
}
