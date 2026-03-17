import QtQuick
import QtQuick.Controls
import Client_B 1.0

Rectangle {
    id: gameRoot
    anchors.fill: parent
    color: "#0f0a1e"

    // Connections to MonopolyClient signals
    Connections {
        target: MonopolyClient
        function onAskBuyField(fieldId, price) {
            buyPopup.fieldId = fieldId
            buyPopup.price = price
            buyPopup.fieldName = MonopolyClient.fieldName(fieldId)
            buyPopup.open()
        }
        function onGameOver(message) {
            gameOverPopup.message = message
            gameOverPopup.open()
        }
    }

    Flickable {
        anchors.fill: parent
        contentHeight: mainColumn.implicitHeight + 20
        clip: true
        boundsBehavior: Flickable.StopAtBounds

        Column {
            id: mainColumn
            width: parent.width
            spacing: 8

            // Player Header bar
            PlayerHeader {
                width: parent.width
            }

            // Game Board
            GameBoard {
                width: parent.width
                anchors.horizontalCenter: parent.horizontalCenter
            }

            // Dice / Roll button
            DiceButton {
                width: parent.width - 24
                anchors.horizontalCenter: parent.horizontalCenter
            }

            // Player List
            PlayerList {
                width: parent.width - 24
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }

    // Popups
    BuyPopup { id: buyPopup }
    FieldInfoPopup { id: fieldInfoPopup }
    BuildPopup { id: buildPopup }
    GameOverPopup { id: gameOverPopup }
}
