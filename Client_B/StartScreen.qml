import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: startRoot
    anchors.fill: parent

    gradient: Gradient {
        orientation: Gradient.Horizontal
        GradientStop { position: 0.0; color: "#ff0044" }
        GradientStop { position: 0.5; color: "#e91e90" }
        GradientStop { position: 1.0; color: "#8e2de2" }
    }

    // *** BEGINN KI-generiert Label: Prompt 3 ***
    // Kompletter Animationsblock wurde von KI als Deko generiert.
    // Decorative floating circles (background animation)
    Repeater {
        model: 6
        Rectangle {
            width: 60 + Math.random() * 80
            height: width
            radius: width / 2
            opacity: 0.08
            color: "white"
            x: Math.random() * startRoot.width
            y: Math.random() * startRoot.height

            SequentialAnimation on y {
                loops: Animation.Infinite
                NumberAnimation { to: y - 30; duration: 2000 + Math.random() * 2000; easing.type: Easing.InOutSine }
                NumberAnimation { to: y + 30; duration: 2000 + Math.random() * 2000; easing.type: Easing.InOutSine }
            }
        }
    }
    // *** ENDE KI-generiert Label: Prompt 3 ***

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 10

        // Title
        Text {
            text: "MONOPOLY"
            font.pixelSize: 52
            font.bold: true
            font.family: "Arial Black"
            color: "white"
            style: Text.Outline
            styleColor: "#00000044"
            Layout.alignment: Qt.AlignHCenter

            // Subtle scale animation on appear
            scale: 0.8
            Component.onCompleted: scaleAnim.start()
            NumberAnimation on scale {
                id: scaleAnim
                from: 0.8; to: 1.0
                duration: 600
                easing.type: Easing.OutBack
            }
        }

        Text {
            text: "Mobile Edition"
            font.pixelSize: 18
            font.bold: true
            color: "#fde047"
            Layout.alignment: Qt.AlignHCenter
            Layout.bottomMargin: 40
        }

        // SPIELEN Button
        Rectangle {
            id: playBtnOuter
            Layout.alignment: Qt.AlignHCenter
            width: 260
            height: 70
            radius: 35
            color: "transparent"
            border.color: "#f59e0b"
            border.width: 3

            Rectangle {
                id: playBtn
                anchors.centerIn: parent
                width: 240
                height: 56
                radius: 28
                color: "#22c55e"

                gradient: Gradient {
                    GradientStop { position: 0.0; color: "#4ade80" }
                    GradientStop { position: 1.0; color: "#16a34a" }
                }

                Row {
                    anchors.centerIn: parent
                    spacing: 12

                    Text {
                        text: "▶"
                        font.pixelSize: 22
                        color: "white"
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Text {
                        text: "SPIELEN"
                        font.pixelSize: 22
                        font.bold: true
                        color: "white"
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onPressed: playBtn.scale = 0.95
                    onReleased: playBtn.scale = 1.0
                    onClicked: stackView.push(lobbyScreenComponent)
                }

                Behavior on scale { NumberAnimation { duration: 100 } }
            }

            // Pulsing border animation
            SequentialAnimation on border.width {
                loops: Animation.Infinite
                NumberAnimation { to: 5; duration: 800; easing.type: Easing.InOutSine }
                NumberAnimation { to: 3; duration: 800; easing.type: Easing.InOutSine }
            }
        }

        Item { height: 20; width: 1 }

        // Color dots
        Row {
            Layout.alignment: Qt.AlignHCenter
            spacing: 18

            Repeater {
                model: ["#ef4444", "#3b82f6", "#22c55e", "#f59e0b"]

                Rectangle {
                    width: 28
                    height: 28
                    radius: 14
                    color: modelData

                    // Subtle bounce
                    scale: 0.0
                    Component.onCompleted: dotAnim.start()
                    NumberAnimation on scale {
                        id: dotAnim
                        from: 0.0; to: 1.0
                        duration: 400
                        easing.type: Easing.OutBack
                    }
                }
            }
        }
    }
}
