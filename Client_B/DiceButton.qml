import QtQuick
import QtQuick.Layouts
import Client_B 1.0

Item {
    id: diceRoot
    height: 56

    Rectangle {
        id: rollBtn
        anchors.fill: parent
        radius: 14
        opacity: MonopolyClient.canRoll ? 1.0 : 0.5

        gradient: Gradient {
            GradientStop { position: 0.0; color: "#4ade80" }
            GradientStop { position: 1.0; color: "#16a34a" }
        }

        Row {
            anchors.centerIn: parent
            spacing: 12

            Text {
                text: "🎲"
                font.pixelSize: 22
                anchors.verticalCenter: parent.verticalCenter
            }

            Text {
                text: "WÜRFELN"
                font.pixelSize: 18
                font.bold: true
                color: "white"
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        MouseArea {
            anchors.fill: parent
            enabled: MonopolyClient.canRoll
            cursorShape: MonopolyClient.canRoll ? Qt.PointingHandCursor : Qt.ForbiddenCursor
            onPressed: rollBtn.scale = 0.97
            onReleased: rollBtn.scale = 1.0
            onClicked: MonopolyClient.rollDice()
        }

        Behavior on scale { NumberAnimation { duration: 80 } }
        Behavior on opacity { NumberAnimation { duration: 200 } }
    }
}
