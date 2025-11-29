import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

import Deadclock.Ui 1.0

Item {
    width: main_column.implicitWidth
    height: main_column.implicitHeight

    Component.onCompleted: {
        // Load Settings from disk
        speech_volume_slider.value = settings.load_setting("speech/volume");
        speech_voice_selection.currentIndex = settings.load_setting("speech/voice");
        lead_time_slider.value = settings.load_setting("timer/lead_time");
    }
    Column {
        id: main_column
        anchors.fill: parent
        spacing: Constants.margin_medium

        Item {
            id: spacer
            height: Constants.margin_small
            width: parent.width
        }
        Text {
            id: timer
            text: timer_controller.display_time
            anchors.horizontalCenter: parent.horizontalCenter
            font.pointSize: 36
            color: Qt.color("white")
        }
        Row {
            spacing: Constants.margin_small
            anchors.horizontalCenter: parent.horizontalCenter
            DCButton {
                text: timer_controller.is_running ? "Stop" : "Start"
                highlighted: !timer_controller.is_running
                onClicked: {
                    if (timer_controller.is_running) {
                        timer_controller.pause();
                        return;
                    }
                    timer_controller.start();
                }
            }
            DCButton {
                text: "Set"
                onClicked: timer_controller.set_time(parseInt(input_minutes.text), parseInt(input_seconds.text))
            }
        }
        Row {
            spacing: Constants.margin_small
            anchors.horizontalCenter: parent.horizontalCenter
            TextField {
                id: input_minutes
                placeholderText: "Min"
                text: "00"
                width: 70
                maximumLength: 3
                onEditingFinished: timer_controller.set_last_set_minutes(parseInt(input_minutes.text))
            }
            TextField {
                id: input_seconds
                placeholderText: "Sec"
                text: "20"
                width: 70
                maximumLength: 2

                onEditingFinished: timer_controller.set_last_set_seconds(parseInt(input_seconds.text))
            }
        }
        GroupBox {
            id: settings_group
            title: "Settings"
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width - Constants.margin_medium * 2

            Column {
                spacing: Constants.margin_small
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
                    // ToolTip.text: "Set how many seconds in advance you’ll be alerted before the event begins."
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
                DCButton {
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
                DCButton {
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
                Label {
                    text: "Alerts / Drills"
                }
                DCButton {
                    id: alert_popup_button
                    text: "Configure Alerts / Drills"
                    onClicked: alert_popup.open()
                }
            }
        }
        GroupBox {
            title: "Overlay"
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width - Constants.margin_medium * 2

            Column {
                width: main_window.width
                spacing: Constants.margin_small
                Switch {
                    id: overlay_toggle_visible
                    text: "Show Overlay"
                    checked: false
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
                    checked: settings.load_setting("notification/show") === "true" ? true : false
                    width: parent.width

                    onToggled: {
                        console.log("notification switch toggled");
                        settings.save_setting("notification/show", checked);
                    }
                }
            }
        }
        Popup {
            id: alert_popup
            modal: true
            focus: true
            width: main_window.width - main_window.margin * 2
            // height:
            anchors.centerIn: parent
            // closePolicy: Popup.NoAutoClose

            function set_switch_states(bitmask_string) {
                console.log("setting switch states with: ", bitmask_string);
                if (bitmask_string.length !== 7) {
                    console.warn("invalid input length for enabled event bitmask string");
                    return;
                }
                small_camps_switch.checked = bitmask_string.charAt(0) === "1";
                medium_camps_switch.checked = bitmask_string.charAt(1) === "1";
                large_camps_switch.checked = bitmask_string.charAt(2) === "1";
                midboss_switch.checked = bitmask_string.charAt(3) === "1";
                runes_switch.checked = bitmask_string.charAt(4) === "1";
                urn_switch.checked = bitmask_string.charAt(5) === "1";
                minimap_drill_switch.checked = bitmask_string.charAt(6) === "1";
            }
            Component.onCompleted: {
                set_switch_states(settings.load_setting("timer/enabled_events"));
                minimap_drill_interval.value = settings.load_setting("timer/minimap_drill_interval");
            }

            Material.theme: Material.Dark

            Overlay.modal: Rectangle {
                color: Qt.color("#80000000")
            }

            background: Rectangle {
                color: Material.background
                radius: 8
            }

            contentItem: Column {
                spacing: Constants.margin_small
                Switch {
                    id: small_camps_switch
                    text: "Small Camps"
                }
                Switch {
                    id: medium_camps_switch
                    text: "Medium Camps"
                }
                Switch {
                    id: large_camps_switch
                    text: "Large Camps"
                }
                Switch {
                    id: midboss_switch
                    text: "Mid Boss"
                }
                Switch {
                    id: runes_switch
                    text: "Runes"
                }
                Switch {
                    id: urn_switch
                    text: "Urn"
                }
                Switch {
                    id: minimap_drill_switch
                    text: "Minimap Drill"
                }
                Item {
                    width: parent.width
                    height: minimap_drill_label.implicitHeight
                    Label {
                        id: minimap_drill_label
                        text: "Highlight interval (seconds)"
                        enabled: minimap_drill_switch.checked
                        anchors.left: parent.left
                        anchors.leftMargin: main_window.spacing_small
                        anchors.verticalCenter: parent.bottom
                        // ToolTip.text: "How many seconds to wait inbetween highlighting the minimap"
                    }
                }
                Slider {
                    id: minimap_drill_interval
                    from: 15
                    to: 180
                    stepSize: 5
                    value: 60
                    width: parent.width
                    enabled: minimap_drill_switch.checked

                    ToolTip.visible: hovered || pressed
                    ToolTip.text: value.toFixed(0)

                    onPressedChanged: {
                        if (!pressed) {
                            settings.save_setting("timer/minimap_drill_interval", value);
                        }
                    }
                }

                DCButton {
                    text: "Apply"
                    highlighted: true
                    onClicked: {
                        // set current state in settings and adjust bitset in timercontroller
                        let bitmask_string = (small_camps_switch.checked ? "1" : "0") + (medium_camps_switch.checked ? "1" : "0") + (large_camps_switch.checked ? "1" : "0") + (midboss_switch.checked ? "1" : "0") + (runes_switch.checked ? "1" : "0") + (urn_switch.checked ? "1" : "0") + (minimap_drill_switch.checked ? "1" : "0");
                        console.log("Saving event mask: ", bitmask_string);
                        settings.save_setting("timer/enabled_events", bitmask_string);
                        alert_popup.close();
                    }
                }
            }
        }
    }
}
