import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Client_B 1.0

Popup {
    id: buyPopupRoot
    anchors.centerIn: parent
    width: Math.min(parent.width - 40, 340)
    height: contentCol.implicitHeight + 60
    modal: true
    closePolicy: Popup.NoAutoClose

    property int fieldId: -1
    property int price: 0
    property string fieldName: ""

    background: Rectangle {
        radius: 20
        color: "#1e293b"
        border.color: "#6366f1"
        border.width: 2
    }

    ColumnLayout {
        id: contentCol
        anchors.fill: parent
        anchors.margins: 24
        spacing: 16

        Text {
            text: "🏠 Kaufen?"
            font.pixelSize: 22
            font.bold: true
            color: "white"
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: "Möchtest du <b>" + buyPopupRoot.fieldName + "</b> kaufen?"
            font.pixelSize: 15
            color: "#e2e8f0"
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            textFormat: Text.RichText
        }

        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            width: 120
            height: 36
            radius: 18
            color: "#fde047"

            Text {
                anchors.centerIn: parent
                text: "💰 " + buyPopupRoot.price + " €"
                font.pixelSize: 16
                font.bold: true
                color: "#1e293b"
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            // Reject
            Rectangle {
                Layout.fillWidth: true
                height: 44
                radius: 12
                color: "#ef4444"

                Text {
                    anchors.centerIn: parent
                    text: "Nein"
                    font.pixelSize: 16
                    font.bold: true
                    color: "white"
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        MonopolyClient.rejectBuy()
                        buyPopupRoot.close()
                    }
                }
            }

            // Accept
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
                    text: "Kaufen!"
                    font.pixelSize: 16
                    font.bold: true
                    color: "white"
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        MonopolyClient.buyField(buyPopupRoot.fieldId)
                        buyPopupRoot.close()
                    }
                }
            }
        }
    }

    Overlay.modal: Rectangle {
        color: "#00000088"
    }
}
