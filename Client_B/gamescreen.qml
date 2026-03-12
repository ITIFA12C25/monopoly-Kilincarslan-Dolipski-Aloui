import QtQuick
import QtQuick.Controls

Item {
    id: root
    property var viewModel

    anchors.fill: parent

    Column {
        anchors.centerIn: parent
        spacing: 20

        Text {
            text: viewModel.title
            font.pixelSize: 28
        }

        Text {
            text: "Aktiver Spieler: " + viewModel.currentPlayer
            font.pixelSize: 20
        }

        Rectangle {
            width: 400
            height: 200
            color: "#444"
            radius: 10

            Text {
                anchors.centerIn: parent
                text: "Spielfeld (Platzhalter)"
                color: "white"
            }
        }
    }
}
