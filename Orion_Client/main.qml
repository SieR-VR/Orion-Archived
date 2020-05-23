import QtQuick 2.12
import QtQuick.Controls 2.12

ApplicationWindow {
    id: window
    width: screen.width * 1/2
    height: screen.height * 1/2

    flags: Qt.Window | Qt.FramelessWindowHint
    color: "white"

    menuBar: Rectangle {
        id: titlebar
        width: parent.width
        height: 48
        color: "#AA000000"
        property variant clickPos: "1,1"

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton
            property variant clickPos: "1,1"

            onPressed: {
                clickPos = Qt.point(mouse.x,mouse.y)

            }
            onPositionChanged: {
                var delta = Qt.point(Math.max(Math.min(mouse.x-clickPos.x, 1920), 0),Math.max(Math.min(mouse.y-clickPos.y, 1080), 0))
                window.x = window.x + delta.x
                window.y = window.y + delta.y
                console.log(delta.x,delta.y)
            }
        }
    }

    Rectangle {
        id: rectangle
        x: 0
        y: 0
        width: 874
        height: 388
        color: "#ffffff"
    }
}
