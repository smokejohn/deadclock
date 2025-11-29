import QtQuick 2.15

Rectangle {
    id: title_bar
    height: 32
    color: Qt.color("#222222")
    width: parent.width

    MouseArea {
        id: titlebar_mousearea
        anchors.fill: parent
        hoverEnabled: true

        property variant click_pos: "1,1"

        onPressed: mouse => {
            click_pos = Qt.point(mouse.x, mouse.y);
        }

        onPositionChanged: mouse => {
            if (!mouse.buttons & Qt.LeftButton) {
                return;
            }
            var delta = Qt.point(mouse.x - click_pos.x, mouse.y - click_pos.y);
            main_window.x += delta.x;
            main_window.y += delta.y;
        }
    }

    Text {
        anchors.centerIn: parent
        text: "Deadclock"
        font.pointSize: 11
        font.bold: true
        color: titlebar_mousearea.containsMouse ? Qt.rgba(1.0, 1.0, 1.0, 0.95) : Qt.rgba(1.0, 1.0, 1.0, 0.75)
    }

    Row {
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter

        Rectangle {
            id: minimize_container
            width: 32
            height: 32
            color: minimize_mousearea.containsMouse ? Qt.rgba(1.0, 1.0, 1.0, 0.2) : Qt.color("transparent")

            MouseArea {
                id: minimize_mousearea
                anchors.fill: parent
                onClicked: main_window.showMinimized()
                cursorShape: Qt.PointingHandCursor
                hoverEnabled: true
            }
            Text {
                text: "–"
                anchors.centerIn: parent
                color: Qt.color("white")
            }
        }
        Rectangle {
            id: close_container
            width: 32
            height: 32
            color: close_mousearea.containsMouse ? Qt.color("#a62f34") : Qt.color("transparent")

            MouseArea {
                id: close_mousearea
                anchors.fill: parent
                onClicked: Qt.quit()
                cursorShape: Qt.PointingHandCursor
                hoverEnabled: true
            }
            Text {
                text: "✕"
                anchors.centerIn: parent
                color: Qt.color("white")
            }
        }
    }
}
