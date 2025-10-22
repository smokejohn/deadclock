import QtQuick 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts
import QtQuick.Effects

Item {
    id: root

    property bool overlay_locked: false

    Connections {
        target: application

        function onOverlay_locked(locked) {
            overlay_locked = locked;
        }

        function onHighlight_minimap() {
            minimap_highlight.flash()
        }
    }

    Window {
        id: timer_overlay
        title: "Timer Overlay"
        objectName: "timer_overlay"
        visible: false
        width: 100
        height: 100
        color: Qt.color("transparent")
        flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.Tool
        x: Qt.application.screens[0].virtualX + Qt.application.screens[0].width / 2 - width / 2
        y: Qt.application.screens[0].virtualY

        Component.onCompleted: {
            // Load Settings from disk
            const x_pos = settings.load_setting("timer/window_x_pos");
            if (x_pos !== undefined) {
                timer_overlay.x = x_pos;
            }
            const y_pos = settings.load_setting("timer/window_y_pos");
            if (y_pos !== undefined) {
                timer_overlay.y = y_pos;
            }
        }

        Item {
            id: timer_overlay_manipulators
            anchors.fill: parent
            visible: !root.overlay_locked

            Rectangle {
                id: timer_overlay_border
                anchors.fill: parent
                border.width: 1
                border.color: Qt.color("white")
                color: Qt.color("transparent")
            }

            Rectangle {
                anchors.top: parent.top
                width: parent.width
                height: 16
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
                    onReleased: mouse => {
                        settings.save_setting("timer/window_y_pos", timer_overlay.y);
                    }
                }
            }
            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width
                height: 16
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
                    onReleased: mouse => {
                        settings.save_setting("timer/window_x_pos", timer_overlay.x);
                    }
                }
            }
        }
        Text {
            anchors.centerIn: parent
            text: timer_controller.display_time
            font.pixelSize: 22
            color: Qt.color("white")
            Layout.alignment: Qt.AlignCenter
        }
    }

    Window {
        id: notification_overlay
        title: "Notification Overlay"
        objectName: "notification_overlay"
        visible: false
        width: 600
        height: 200
        color: Qt.color("transparent")
        flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.Tool
        x: Qt.application.screens[0].virtualX + Qt.application.screens[0].width / 2 - width / 2
        y: Qt.application.screens[0].virtualY + 200

        Component.onCompleted: {
            // Load Settings from disk
            const x_pos = settings.load_setting("notification/window_x_pos");
            if (x_pos !== undefined) {
                notification_overlay.x = x_pos;
            }
            const y_pos = settings.load_setting("notification/window_y_pos");
            if (y_pos !== undefined) {
                notification_overlay.y = y_pos;
            }
        }

        Item {
            id: notification_overlay_manipulators
            anchors.fill: parent
            visible: !root.overlay_locked

            Rectangle {
                id: notification_overlay_border
                anchors.fill: parent
                border.width: 1
                border.color: Qt.color("white")
                color: Qt.color("transparent")
            }

            Rectangle {
                anchors.top: parent.top
                width: parent.width
                height: 16
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
                        notification_overlay.y = Qt.application.screens[0].virtualY + 200;
                    }
                    onReleased: mouse => {
                        settings.save_setting("notification/window_y_pos", notification_overlay.y);
                    }
                }
            }
            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width
                height: 16
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
                    onReleased: mouse => {
                        settings.save_setting("notification/window_x_pos", notification_overlay.x);
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
                duration: 100
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

                property int padding: 20
                property int max_width: notification_overlay.width

                radius: padding / 2
                color: Qt.color("#dd9b7858")

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
                    font.pixelSize: 24
                    lineHeight: 1.25
                    lineHeightMode: Text.ProportionalHeight

                    color: Qt.color("white")
                    maximumLineCount: -1
                }
            }
        }
    }

    Window {
        id: minimap_overlay
        title: "Minimap Overlay"
        objectName: "minimap_overlay"
        visible: true
        color: Qt.color("transparent")
        flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.Tool

        property real width_percentage: 0.1981 // 507 out of 2560
        property real x_pos_percentage: 0.7860 // 2012 out of 2560
        property real y_pos_percentage: 0.6063 // 873 out of 1440

        width: Qt.application.screens[0].width * width_percentage
        height: width
        x: Qt.application.screens[0].virtualX + Qt.application.screens[0].width * x_pos_percentage
        y: Qt.application.screens[0].virtualY + Qt.application.screens[0].height * y_pos_percentage

        Rectangle {
            id: minimap_bounds
            anchors.fill: parent
            visible: false
            color: Qt.color("transparent")
            border.color: Qt.color("white")
            border.width: 1
        }

        Image {
            id: minimap_highlight
            anchors.fill: parent
            visible: false
            source: "qrc:/images/minimap_highlight.png"

            SequentialAnimation {
                id: flash_animation
                running: false
                onStopped: minimap_highlight.visible = false

                NumberAnimation { target: minimap_highlight; property: "opacity"; to: 1.0; duration: 150 }
                NumberAnimation { target: minimap_highlight; property: "opacity"; to: 0.0; duration: 150 }
                NumberAnimation { target: minimap_highlight; property: "opacity"; to: 1.0; duration: 150 }
                NumberAnimation { target: minimap_highlight; property: "opacity"; to: 0.0; duration: 150 }
            }

            function flash() {
                minimap_highlight.visible = true
                flash_animation.start()
            }
        }
    }
}
