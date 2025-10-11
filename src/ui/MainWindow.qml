import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.VectorImage
import QtQuick.Layouts

ApplicationWindow {
    id: main_window
    objectName: "main_window"
    visible: true
    width: root.implicitWidth + 40
    height: root.implicitHeight + 40
    minimumWidth: root.implicitWidth + 40
    minimumHeight: root.implicitHeight + 40
    maximumWidth: root.implicitWidth + 40
    maximumHeight: root.implicitHeight + 40
    title: "Deadclock"

    Material.theme: Material.Dark
    Material.accent: "#9b7858"

    Component.onCompleted: {
        // Load Settings from disk
        speech_volume_slider.value = settings.load_setting("speech/volume")
        speech_voice_selection.currentIndex = settings.load_setting("speech/voice")
        lead_time_slider.value = settings.load_setting("timer/lead_time")
    }

    ColumnLayout {
        id: root
        anchors.fill: parent
        anchors.centerIn: parent
        anchors.margins: 20
        spacing: 10
        Text {
            text: timer_controller.display_time
            font.pixelSize: 40
            color: "white"
            Layout.alignment: Qt.AlignHCenter
        }

        Row {
            spacing: 5
            Layout.alignment: Qt.AlignHCenter
            Button {
                text: timer_controller.is_running ? "Stop" : "Start"
                onClicked: {
                    if (timer_controller.is_running) {
                        timer_controller.pause()
                        return
                    }
                    timer_controller.start()
                }
            }
            Button {
                text: "Set"
                onClicked: timer_controller.set_time(parseInt(input_minutes.text), parseInt(input_seconds.text))
            }
        }
        Row {
            spacing: 10
            Layout.alignment: Qt.AlignHCenter
            TextField {
                id: input_minutes;
                placeholderText: "Min";
                text: "00"
                width: 60
                onEditingFinished: timer_controller.set_last_set_minutes(parseInt(input_minutes.text))
            }
            TextField {
                id: input_seconds;
                placeholderText: "Sec";
                text: "20"
                width: 60
                onEditingFinished: timer_controller.set_last_set_seconds(parseInt(input_seconds.text))
            }
        }
        GroupBox {
            title: "Settings"

            Column {
                spacing: 10
                width: 200
                Label {
                    text: "Speech Voice"
                }
                ComboBox {
                    id: speech_voice_selection
                    model: tts.get_available_voices()
                    width: parent.width
                    onActivated: {
                        settings.save_setting("speech/voice", currentIndex)
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
                            settings.save_setting("speech/volume", value)
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
                            settings.save_setting("timer/lead_time", value)
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
                            return "Setting key: " + input.get_key_name(input.pause_key)
                        } else {
                            return "Start/Stop Timer: " + input.get_key_name(input.pause_key)
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
                            return "Setting key: " + input.get_key_name(input.set_key)
                        } else {
                            return "Set Timer: " + input.get_key_name(input.set_key)
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
            Column {
                spacing: 5
                Switch {
                    id: overlay_toggle_visible
                    text: "Show Overlay"

                    onCheckedChanged: {
                        application.toggle_overlay_visible(checked)
                    }
                }
                Switch {
                    id: overlay_toggle_locked
                    text: "Lock Overlay"
                    enabled: overlay_toggle_visible.checked

                    onCheckedChanged: {
                        application.toggle_overlay_locked(checked)
                    }
                }
            }
        }

        GroupBox {
            title: "Clock Sync"
            Column {
                spacing: 5
                Switch {
                    id: clock_sync_toggle
                    text: "Automatic clock sync"
                    onCheckedChanged: {
                        application.toggle_clock_sync(checked)
                    }
                }
                Switch {
                    id: clock_sync_show_capture_rect
                    text: "Show capture region"
                    onCheckedChanged: {
                        const capture_rect = clock_reader.get_capture_region()
                        capture_region_debug.x = capture_rect.x
                        capture_region_debug.y = capture_rect.y
                        capture_region_debug.width = capture_rect.width
                        capture_region_debug.height = capture_rect.height
                    }
                }
            }
        }
    }

    Window {
        id: capture_region_debug
        visible: clock_sync_show_capture_rect.checked
        color: Qt.rgba(0.3, 0.0, 0.0, 0.5)
        flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
        x: 0
        y: 0
        width: 100
        height: 100
    }

    onClosing: {
        Qt.quit()
    }
}
