import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

ApplicationWindow {
    id: main_window
    objectName: "main_window"
    visible: true
    width: 300
    minimumHeight: main_column.implicitHeight + margin
    maximumHeight: main_column.implicitHeight + margin
    title: "Deadclock"
    flags: Qt.FramelessWindowHint | Qt.Window

    Material.theme: Material.Dark
    Material.accent: main_window.accent_color

    property int margin: 16
    property int spacing: 16
    property int spacing_small: 8
    property int spacing_tiny: 4
    property var accent_color: Qt.color("#9b7858")

    // Rectangle {
    //     color: "transparent"
    //     anchors.fill: main_column
    //     border.width: 1
    //     border.color: Qt.color("white")
    // }

    Component.onCompleted: {
        // Load Settings from disk
        speech_volume_slider.value = settings.load_setting("speech/volume");
        speech_voice_selection.currentIndex = settings.load_setting("speech/voice");
        lead_time_slider.value = settings.load_setting("timer/lead_time");
    }
    Column {
        id: main_column
        anchors.fill: parent
        spacing: main_window.spacing

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
        Rectangle {
            width: parent.width
            height: timer.height + main_window.margin * 2
            color: Qt.rgba(0.0, 0.0, 0.0, 0.1)

            Text {
                id: timer
                text: timer_controller.display_time
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                font.pointSize: 36
                color: Qt.color("white")
            }

            Rectangle {
                anchors {
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                }
                height: 2
                color: timer_controller.is_running ? main_window.accent_color : Qt.color("transparent")

                SequentialAnimation on opacity {
                    loops: Animation.Infinite
                    running: timer_controller.is_running
                    NumberAnimation { to: 0.0; duration: 2500; easing.type: Easing.InOutSine }
                    NumberAnimation { to: 1.0; duration: 2500; easing.type: Easing.InOutSine }
                }
            }
        }
        Row {
            spacing: main_window.spacing_small
            anchors.horizontalCenter: parent.horizontalCenter
            Button {
                text: timer_controller.is_running ? "Stop" : "Start"
                onClicked: {
                    if (timer_controller.is_running) {
                        timer_controller.pause();
                        return;
                    }
                    timer_controller.start();
                }
            }
            Button {
                text: "Set"
                onClicked: timer_controller.set_time(parseInt(input_minutes.text), parseInt(input_seconds.text))
            }
        }
        Row {
            spacing: main_window.spacing_small
            anchors.horizontalCenter: parent.horizontalCenter
            TextField {
                id: input_minutes
                placeholderText: "Min"
                text: "00"
                width: 70
                onEditingFinished: timer_controller.set_last_set_minutes(parseInt(input_minutes.text))
            }
            TextField {
                id: input_seconds
                placeholderText: "Sec"
                text: "20"
                width: 70
                onEditingFinished: timer_controller.set_last_set_seconds(parseInt(input_seconds.text))
            }
        }
        GroupBox {
            id: settings_group
            title: "Settings"
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width - main_window.margin * 2

            Column {
                spacing: main_window.spacing_small
                width: parent.width
                Label {
                    text: "Speech Voice"
                }
                ComboBox {
                    id: speech_voice_selection
                    model: tts.get_available_voices()
                    width: parent.width
                    onActivated: {
                        settings.save_setting("speech/voice", currentIndex);
                        tts.say(currentText);
                    }
                }
                Label {
                    text: "Speech Volume"
                }
                Slider {
                    id: speech_volume_slider
                    from: 0
                    to: 100
                    stepSize: 1
                    value: 50
                    width: parent.width

                    ToolTip.visible: hovered || pressed
                    ToolTip.text: value.toFixed(0)

                    onPressedChanged: {
                        if (!pressed) {
                            settings.save_setting("speech/volume", value);
                            tts.say_test();
                        }
                    }
                }
                Label {
                    text: "Lead time (seconds)"
                    ToolTip.text: "Set how many seconds in advance you’ll be alerted before the event begins."
                }
                Slider {
                    id: lead_time_slider
                    from: 5
                    to: 30
                    stepSize: 5
                    value: 20
                    width: parent.width

                    ToolTip.visible: hovered || pressed
                    ToolTip.text: value.toFixed(0)

                    onPressedChanged: {
                        if (!pressed) {
                            settings.save_setting("timer/lead_time", value);
                        }
                    }
                }
                Label {
                    text: "Keybinds"
                }
                Button {
                    id: pause_keybind_button
                    text: {
                        if (input.pause_keybind_active) {
                            return "Setting key: " + input.get_key_name(input.pause_key);
                        } else {
                            return "Start/Stop Timer: " + input.get_key_name(input.pause_key);
                        }
                    }
                    highlighted: input.pause_keybind_active
                    enabled: !input.set_keybind_active
                    ToolTip.visible: hovered
                    ToolTip.text: "Click to to enable keybinding, then press desired key. Click again to cancel"
                    ToolTip.delay: 1000
                    onClicked: input.toggle_pause_keybind_active()
                }
                Button {
                    id: set_keybind_button
                    text: {
                        if (input.set_keybind_active) {
                            return "Setting key: " + input.get_key_name(input.set_key);
                        } else {
                            return "Set Timer: " + input.get_key_name(input.set_key);
                        }
                    }
                    highlighted: input.set_keybind_active
                    enabled: !input.pause_keybind_active
                    ToolTip.visible: hovered
                    ToolTip.text: "Click to to enable keybinding, then press desired key. Click again to cancel"
                    ToolTip.delay: 1000
                    onClicked: input.toggle_set_keybind_active()
                }
            }
        }
        GroupBox {
            title: "Overlay"
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width - main_window.margin * 2

            Column {
                width: main_window.width
                spacing: main_window.spacing_small
                Switch {
                    id: overlay_toggle_visible
                    text: "Show Overlay"
                    width: parent.width

                    onCheckedChanged: {
                        application.toggle_overlay_visible(checked);
                    }
                }
                Switch {
                    id: overlay_toggle_locked
                    text: "Lock Overlay"
                    enabled: overlay_toggle_visible.checked
                    checked: true
                    width: parent.width

                    onCheckedChanged: {
                        application.toggle_overlay_locked(checked);
                    }
                }
                Switch {
                    id: overlay_show_notifications
                    text: "Show Notifications"
                    enabled: overlay_toggle_visible.checked
                    checked: false
                    width: parent.width

                    onCheckedChanged: {
                        application.toggle_notifications(checked);
                    }
                }
            }
        }
    }

    onClosing: {
        Qt.quit();
    }
}
