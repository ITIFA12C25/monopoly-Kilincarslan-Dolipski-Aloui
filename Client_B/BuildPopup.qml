import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Client_B 1.0

Popup {
    id: buildPopupRoot
    anchors.centerIn: parent
    width: Math.min(parent.width - 40, 340)
    height: buildCol.implicitHeight + 60
    modal: true

    property int fieldId: -1
    property string fieldName: ""
    property int currentHouses: 0

    background: Rectangle {
        radius: 20
        color: "#1e293b"
        border.color: "#22c55e"
        border.width: 2
    }

    ColumnLayout {
        id: buildCol
        anchors.fill: parent
        anchors.margins: 24
        spacing: 16

        Text {
            text: currentHouses === 4 ? "🏨 Hotel bauen?" : "🏠 Haus bauen?"
            font.pixelSize: 22
            font.bold: true
            color: "white"
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: "<b>" + buildPopupRoot.fieldName + "</b><br>Aktuelle Häuser: " + buildPopupRoot.currentHouses + "<br>Kosten: 50 €"
            font.pixelSize: 14
            color: "#e2e8f0"
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            textFormat: Text.RichText
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            Rectangle {
                Layout.fillWidth: true
                height: 44
                radius: 12
                color: "#475569"

                Text {
                    anchors.centerIn: parent
                    text: "Abbrechen"
                    font.pixelSize: 15
                    font.bold: true
                    color: "white"
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: buildPopupRoot.close()
                }
            }

            Rectangle {
                Layout.fillWidth: true
                height: 44
                radius: 12

                gradient: Gradient {
                    GradientStop { position: 0.0; color: "#4ade80" }
                    GradientStop { position: 1.0; color: "#16a34a" }
                }

                Text {
                    anchors.centerIn: parent
                    text: "Bauen!"
                    font.pixelSize: 15
                    font.bold: true
                    color: "white"
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        MonopolyClient.buildHouse(buildPopupRoot.fieldId)
                        buildPopupRoot.close()
                    }
                }
            }
        }
    }

    Overlay.modal: Rectangle {
        color: "#00000088"
    }
}
