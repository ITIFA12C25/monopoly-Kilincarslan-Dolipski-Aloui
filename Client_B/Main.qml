import QtQuick
import QtQuick.Controls

ApplicationWindow {
    id: appWindow
    visible: true
    width: 400
    height: 750
    title: "Monopoly Mobile"
    color: "#0f0a1e"

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: startScreenComponent
    }

    Component {
        id: startScreenComponent
        StartScreen {}
    }

    Component {
        id: lobbyScreenComponent
        LobbyScreen {}
    }

    Component {
        id: gameScreenComponent
        GameScreen {}
    }
}
