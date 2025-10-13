import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Shapes 1.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts

Item {
    id: root

    property bool overlay_locked: false

    Connections {
        target: application

        function onOverlay_locked(locked) {
            overlay_locked = locked;
        }
    }

    Window {
        id: timer_overlay
        title: "Timer Overlay"
        objectName: "timer_overlay"
        visible: false
        width: 100
        height: 100
        color: "transparent"
        flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
        x: Qt.application.screens[0].virtualX + Qt.application.screens[0].width / 2 - width / 2
        y: Qt.application.screens[0].virtualY

        Material.theme: Material.Dark
        Material.accent: Material.Blue

        Item {
            id: timer_overlay_manipulators
            anchors.fill: parent
            visible: !root.overlay_locked

            Rectangle {
                id: timer_overlay_border
                anchors.fill: parent
                border.width: 1
                border.color: "white"
                color: "transparent"
            }

            Rectangle {
                anchors.top: parent.top
                width: parent.width
                height: 15
                color: Qt.rgba(1.0, 1.0, 1.0, 0.1)

                MouseArea {
                    id: timer_overlay_vertical_drag
                    anchors.fill: parent
                    cursorShape: Qt.SizeVerCursor

                    property real start_y

                    onPressed: mouse => {
                        start_y = mouse.y;
                    }
                    onMouseYChanged: mouse => {
                        if (mouse.buttons & Qt.LeftButton)
                            timer_overlay.y += mouse.y - start_y;
                    }
                    onDoubleClicked: mouse => {
                        timer_overlay.y = Qt.application.screens[0].virtualY;
                    }
                }
            }
            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width
                height: 15
                color: Qt.rgba(1.0, 1.0, 1.0, 0.1)

                MouseArea {
                    id: timer_overlay_horizontal_drag
                    anchors.fill: parent
                    cursorShape: Qt.SizeHorCursor

                    property real start_x

                    onPressed: mouse => {
                        start_x = mouse.x;
                    }
                    onMouseYChanged: mouse => {
                        if (mouse.buttons & Qt.LeftButton)
                            timer_overlay.x += mouse.x - start_x;
                    }
                    onDoubleClicked: mouse => {
                        timer_overlay.x = Qt.application.screens[0].virtualX + Qt.application.screens[0].width / 2 - width / 2;
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

    Window {
        id: notification_overlay
        title: "Notification Overlay"
        objectName: "notification_overlay"
        visible: false
        width: 400
        height: 200
        color: "transparent"
        flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
        x: Qt.application.screens[0].virtualX + Qt.application.screens[0].width / 2 - width / 2
        y: Qt.application.screens[0].virtualY + 200

        Material.theme: Material.Dark
        Material.accent: Material.Blue

        Item {
            id: notification_overlay_manipulators
            anchors.fill: parent
            visible: !root.overlay_locked

            Rectangle {
                id: notification_overlay_border
                anchors.fill: parent
                border.width: 1
                border.color: "white"
                color: "transparent"
            }

            Rectangle {
                anchors.top: parent.top
                width: parent.width
                height: 15
                color: Qt.rgba(1.0, 1.0, 1.0, 0.1)

                MouseArea {
                    id: notification_vertical_drag_area
                    anchors.fill: parent
                    cursorShape: Qt.SizeVerCursor

                    property real start_y

                    onPressed: mouse => {
                        start_y = mouse.y;
                    }
                    onMouseYChanged: mouse => {
                        if (mouse.buttons & Qt.LeftButton)
                            notification_overlay.y += mouse.y - start_y;
                    }
                    onDoubleClicked: mouse => {
                        notification_overlay.y = Qt.application.screens[0].virtualY;
                    }
                }
            }
            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width
                height: 15
                color: Qt.rgba(1.0, 1.0, 1.0, 0.1)

                MouseArea {
                    id: notification_horizontal_drag_area
                    anchors.fill: parent
                    cursorShape: Qt.SizeHorCursor

                    property real start_x

                    onPressed: mouse => {
                        start_x = mouse.x;
                    }
                    onMouseYChanged: mouse => {
                        if (mouse.buttons & Qt.LeftButton)
                            notification_overlay.x += mouse.x - start_x;
                    }
                    onDoubleClicked: mouse => {
                        notification_overlay.x = Qt.application.screens[0].virtualX + Qt.application.screens[0].width / 2 - width / 2;
                    }
                }
            }
        }
        Item {
            id: notification_container
            anchors.fill: parent
            opacity: 0.0

            Connections {
                target: notification
                function onMessage_changed(message) {
                    fade_in.start();
                    hide_timer.restart();
                }
            }

            Timer {
                id: hide_timer
                interval: 5000
                running: false
                repeat: false
                onTriggered: fade_out.start()
            }

            OpacityAnimator {
                id: fade_in
                target: notification_container
                from: 0.0
                to: 1.0
                duration: 1000
            }

            OpacityAnimator {
                id: fade_out
                target: notification_container
                from: 1.0
                to: 0.0
                duration: 1000
            }

            Rectangle {
                id: notification_frame

                property int padding: 16
                property int max_width: 400

                radius: padding
                color: "#aa9b7858"

                anchors.centerIn: parent

                width: Math.min(notification_text.implicitWidth + padding * 2, max_width)
                height: notification_text.implicitHeight + padding * 2

                Text {
                    id: notification_text
                    text: notification.message
                    wrapMode: Text.Wrap
                    anchors.top: parent.top
                    anchors.topMargin: notification_frame.padding
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: notification_frame.width - 2 * notification_frame.padding
                    font.pixelSize: 16
                    color: "white"
                    maximumLineCount: -1
                }
            }
        }
    }
}
