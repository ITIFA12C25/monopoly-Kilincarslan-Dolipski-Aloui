import QtQuick
import QtQuick.Layouts
import Client_B 1.0

Rectangle {
    id: boardRoot
    height: width  // Square board
    color: "transparent"
    clip: true

    property var fieldsList: MonopolyClient.fields
    property var playersList: MonopolyClient.players
    property int myId: MonopolyClient.myId

    // We lay out 40 fields around the edge of an 11x11 grid
    // The center area shows the Monopoly logo
    // Field mapping: bottom row (0-10), left col (11-20), top row (21-30), right col (31-39)

    // *** BEGINN KI-generiert Label: Prompt 2 ***
    // Idee zur Berechnung der Kantenkoordinaten von ChatGPT, 
    // dann händisch in QML Javascript umgesetzt.
    function getRow(id) {
        if (id <= 10) return 10
        if (id <= 20) return 10 - (id - 10)
        if (id <= 30) return 0
        return id - 30
    }

    function getCol(id) {
        if (id <= 10) return 10 - id
        if (id <= 20) return 0
        if (id <= 30) return id - 20
        return 10
    }
    // *** ENDE KI-generiert Label: Prompt 2 ***

    // Board border
    Rectangle {
        anchors.fill: parent
        anchors.margins: 2
        color: "transparent"
        border.color: "#f59e0b"
        border.width: 2
        radius: 4
    }

    // Grid of fields
    Grid {
        id: boardGrid
        anchors.fill: parent
        anchors.margins: 4
        rows: 11
        columns: 11

        Repeater {
            model: 121  // 11x11

            Item {
                id: cellItem
                width: boardGrid.width / 11
                height: boardGrid.height / 11

                property int gridRow: Math.floor(index / 11)
                property int gridCol: index % 11

                // Find which field ID maps to this cell (if any)
                property int fieldId: {
                    for (var i = 0; i < 40; i++) {
                        if (getRow(i) === gridRow && getCol(i) === gridCol)
                            return i
                    }
                    return -1
                }

                // Is this the center area?
                property bool isCenter: gridRow > 0 && gridRow < 10 && gridCol > 0 && gridCol < 10

                // Field tile
                Loader {
                    anchors.fill: parent
                    active: cellItem.fieldId >= 0
                    sourceComponent: fieldTile
                }

                // Center logo area (only render once in the middle cell)
                Loader {
                    anchors.fill: parent
                    active: cellItem.isCenter && gridRow === 5 && gridCol === 5
                    sourceComponent: centerLogo
                }
            }
        }
    }

    Component {
        id: centerLogo

        Rectangle {
            // Spans the full center area
            x: -4 * (boardGrid.width / 11)
            y: -4 * (boardGrid.height / 11)
            width: 9 * (boardGrid.width / 11)
            height: 9 * (boardGrid.height / 11)

            gradient: Gradient {
                GradientStop { position: 0.0; color: "#ff0044" }
                GradientStop { position: 1.0; color: "#e91e63" }
            }

            radius: 4

            Column {
                anchors.centerIn: parent
                spacing: 4

                Text {
                    text: "MONOPOLY"
                    font.pixelSize: Math.max(10, boardRoot.width * 0.06)
                    font.bold: true
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Text {
                    text: "Mobile Edition"
                    font.pixelSize: Math.max(7, boardRoot.width * 0.03)
                    color: "#fde047"
                    horizontalAlignment: Text.AlignHCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }
    }

    Component {
        id: fieldTile

        Rectangle {
            id: tileRect
            anchors.fill: parent
            anchors.margins: 0.5

            property int fId: cellItem.fieldId
            property var fData: (fId >= 0 && fId < fieldsList.length) ? fieldsList[fId] : null
            property string fColor: MonopolyClient.fieldColor(fId)
            property bool isOwned: fData ? fData.ownerID >= 0 : false
            property int ownerID: fData ? fData.ownerID : -1

            color: {
                if (isOwned) {
                    if (ownerID === 0) return "#2a1520"
                    if (ownerID === 1) return "#152030"
                    if (ownerID === 2) return "#152a18"
                    if (ownerID === 3) return "#2a2515"
                    return "#1e1e30"
                }
                return "#1a1530"
            }

            border.color: {
                if (isOwned) {
                    var colors = ["#ef4444", "#3b82f6", "#22c55e", "#f59e0b"]
                    if (ownerID >= 0 && ownerID < 4) return colors[ownerID]
                }
                return "#333355"
            }
            border.width: isOwned ? 1.5 : 0.5
            radius: 2

            // Color bar at top
            Rectangle {
                width: parent.width
                height: 4
                color: fColor
                visible: fColor !== "transparent"
                anchors.top: parent.top
            }

            Column {
                anchors.fill: parent
                anchors.topMargin: fColor !== "transparent" ? 5 : 2
                anchors.margins: 1
                spacing: 0

                // Field name
                Text {
                    width: parent.width
                    text: fData ? fData.name : ""
                    font.pixelSize: Math.max(5, boardRoot.width * 0.018)
                    color: "#e2e8f0"
                    horizontalAlignment: Text.AlignHCenter
                    elide: Text.ElideRight
                    maximumLineCount: 2
                    wrapMode: Text.Wrap
                    font.bold: true
                }

                // Price
                Text {
                    width: parent.width
                    text: (fData && fData.price > 0) ? fData.price + "€" : ""
                    font.pixelSize: Math.max(4, boardRoot.width * 0.014)
                    color: "#94a3b8"
                    horizontalAlignment: Text.AlignHCenter
                    visible: !isOwned && fData && fData.price > 0
                }

                // Houses
                Text {
                    width: parent.width
                    text: {
                        if (!fData || fData.houses === 0) return ""
                        if (fData.houses === 5) return "H"
                        var h = ""
                        for (var i = 0; i < fData.houses; i++) h += "■"
                        return h
                    }
                    font.pixelSize: Math.max(4, boardRoot.width * 0.014)
                    color: fData && fData.houses === 5 ? "#f59e0b" : "#22c55e"
                    horizontalAlignment: Text.AlignHCenter
                    font.bold: true
                }

                // Player markers
                Row {
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 1

                    Repeater {
                        model: playersList.length
                        Rectangle {
                            width: Math.max(4, boardRoot.width * 0.015)
                            height: width
                            radius: width / 2
                            visible: playersList[index] && playersList[index].position === fId
                            color: {
                                var colors = ["#ef4444", "#3b82f6", "#22c55e", "#f59e0b"]
                                return index < 4 ? colors[index] : "white"
                            }
                            border.color: "white"
                            border.width: 0.5
                        }
                    }
                }
            }

            // Click handler
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (fId < 0) return
                    // If I own this field, offer to build
                    if (fData && fData.ownerID === myId && fData.houses < 5 && fData.price > 0) {
                        buildPopup.fieldId = fId
                        buildPopup.fieldName = fData.name
                        buildPopup.currentHouses = fData.houses
                        buildPopup.open()
                    } else {
                        // Show field info
                        var ownerName = "Bank"
                        if (fData.ownerID >= 0 && fData.ownerID < playersList.length) {
                            ownerName = playersList[fData.ownerID].name
                        }
                        fieldInfoPopup.fieldName = fData.name
                        fieldInfoPopup.price = fData.price
                        fieldInfoPopup.owner = ownerName
                        fieldInfoPopup.houses = fData.houses
                        fieldInfoPopup.open()
                    }
                }
            }
        }
    }
}
