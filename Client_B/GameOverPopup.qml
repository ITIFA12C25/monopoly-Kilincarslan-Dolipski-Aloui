import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Popup {
    id: gameOverRoot
    anchors.centerIn: parent
    width: Math.min(parent.width - 40, 320)
    height: goCol.implicitHeight + 60
    modal: true
    closePolicy: Popup.NoAutoClose

    property string message: ""

    background: Rectangle {
        radius: 20
        color: "#1e293b"
        border.color: "#f59e0b"
        border.width: 3
    }

    ColumnLayout {
        id: goCol
        anchors.fill: parent
        anchors.margins: 24
        spacing: 20

        Text {
            text: "🏆 Spielende!"
            font.pixelSize: 26
            font.bold: true
            color: "#fde047"
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: gameOverRoot.message
            font.pixelSize: 15
            color: "#e2e8f0"
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
        }

        Rectangle {
            Layout.fillWidth: true
            height: 48
            radius: 12

            gradient: Gradient {
                GradientStop { position: 0.0; color: "#f59e0b" }
                GradientStop { position: 1.0; color: "#d97706" }
            }

            Text {
                anchors.centerIn: parent
                text: "OK"
                font.pixelSize: 18
                font.bold: true
                color: "white"
            }

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: gameOverRoot.close()
            }
        }
    }

    Overlay.modal: Rectangle {
        color: "#00000088"
    }
}
