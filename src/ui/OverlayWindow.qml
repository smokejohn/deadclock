import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Shapes 1.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts

Window {
    id: overlay_window
    title: "Overlay"
    objectName: "overlay_window"
    visible: false
    width: 100
    height: 100
    color: "transparent"
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
    x: Qt.application.screens[0].virtualX + Qt.application.screens[0].width / 2 - width / 2
    y: Qt.application.screens[0].virtualY

    Material.theme: Material.Dark
    Material.accent: Material.Blue

    property bool window_locked: false

    Connections {
        target: window_controller

        function onOverlay_locked(locked) {
            window_locked = locked;
        }
    }

    Item {
        id: manipulation_helpers
        anchors.fill: parent
        visible: !window_locked

        Rectangle {
            id: overlay_border
            anchors.fill: parent
            // border.width: 1
            // border.color: "white"
            color: "transparent"
        }

        Rectangle {
            id: overlay_center_dot
            anchors.centerIn: parent
            width: 5
            height: 5
            radius: width / 2
            color: Qt.rgba(1.0, 1.0, 1.0, 0.25)
        }

        Rectangle {
            anchors.top: parent.top
            width: parent.width
            height: 15
            color: Qt.rgba(1.0, 1.0, 1.0, 0.1)

            MouseArea {
                id: vertical_drag_area
                anchors.fill: parent
                cursorShape: Qt.SizeVerCursor

                property real start_y

                onPressed: mouse => {
                    start_y = mouse.y
                }
                onMouseYChanged: mouse => {
                    if (mouse.buttons & Qt.LeftButton)
                        overlay_window.y += mouse.y - start_y
                }
                onDoubleClicked: mouse => {
                    overlay_window.y = Qt.application.screens[0].virtualY
                }
            }
        }
        Rectangle {
            anchors.bottom: parent.bottom
            width: parent.width
            height: 15
            color: Qt.rgba(1.0, 1.0, 1.0, 0.1)

            MouseArea {
                id: horizontal_drag_area
                anchors.fill: parent
                cursorShape: Qt.SizeHorCursor

                property real start_x

                onPressed: mouse => {
                    start_x = mouse.x
                }
                onMouseYChanged: mouse => {
                    if (mouse.buttons & Qt.LeftButton)
                        overlay_window.x += mouse.x - start_x
                }
                onDoubleClicked: mouse => {
                    overlay_window.x = Qt.application.screens[0].virtualX + Qt.application.screens[0].width / 2 - width / 2
                }
            }
        }
    }
    Text {
        anchors.centerIn: parent
        text: timer_controller.display_time
        font.pixelSize: 22
        color: "white"
        Layout.alignment: Qt.AlignCenter
    }
}
