import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Popup {
    id: fieldInfoRoot
    anchors.centerIn: parent
    width: Math.min(parent.width - 40, 320)
    height: infoCol.implicitHeight + 60
    modal: true

    property string fieldName: ""
    property int price: 0
    property string owner: "Bank"
    property int houses: 0

    background: Rectangle {
        radius: 20
        color: "#1e293b"
        border.color: "#818cf8"
        border.width: 2
    }

    ColumnLayout {
        id: infoCol
        anchors.fill: parent
        anchors.margins: 24
        spacing: 14

        Text {
            text: "📋 Feld Info"
            font.pixelSize: 20
            font.bold: true
            color: "white"
            Layout.alignment: Qt.AlignHCenter
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#334155"
        }

        Text {
            text: "<b>" + fieldInfoRoot.fieldName + "</b>"
            font.pixelSize: 18
            color: "#e2e8f0"
            textFormat: Text.RichText
            Layout.alignment: Qt.AlignHCenter
        }

        Column {
            Layout.fillWidth: true
            spacing: 6
            visible: fieldInfoRoot.price > 0

            Text {
                text: "Kaufpreis: " + fieldInfoRoot.price + " €"
                font.pixelSize: 14
                color: "#94a3b8"
            }

            Text {
                text: "Besitzer: " + fieldInfoRoot.owner
                font.pixelSize: 14
                color: "#94a3b8"
            }

            Text {
                text: "Häuser: " + fieldInfoRoot.houses
                font.pixelSize: 14
                color: "#94a3b8"
            }
        }

        Rectangle {
            Layout.fillWidth: true
            height: 44
            radius: 12
            color: "#6366f1"

            Text {
                anchors.centerIn: parent
                text: "Schließen"
                font.pixelSize: 15
                font.bold: true
                color: "white"
            }

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: fieldInfoRoot.close()
            }
        }
    }

    Overlay.modal: Rectangle {
        color: "#00000088"
    }
}
