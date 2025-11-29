import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

import Deadclock.Ui 1.0

ApplicationWindow {
    id: main_window
    objectName: "main_window"
    title: "Deadclock"
    visible: true
    flags: Qt.FramelessWindowHint | Qt.Window
    height: main_column.implicitHeight
    width: expanded ? window_width_expanded : window_width

    property int window_width: 300
    property int window_width_expanded: 900
    property bool expanded: false

    Material.theme: Material.Dark
    Material.accent: Constants.accent_color

    Column {
        id: main_column
        anchors.fill: parent
        // spacing: Constants.margin_medium

        TitleBar {
            id: title_bar
        }
        StackView {
            id: stack
            height: 980
            width: 300
            initialItem: main_ui

            Component {
                id: main_ui
                Main {
                    height: 980
                    width: 300
                }
            }

            Component {
                id: settings_ui
                Settings {
                    width: 900
                }
            }
        }
    }
    RoundButton {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: title_bar.height
        flat: true
        icon.source: main_window.expanded ? "qrc:/images/xmark_solid_full.svg" : "qrc:/images/gear_solid_full.svg"
        height: 42
        width: 42

        onClicked: {
            if (!main_window.expanded) {
                main_window.expanded = true;
                stack.push(settings_ui);
            } else {
                main_window.expanded = false;
                stack.pop();
            }
        }
    }

    onClosing: {
        Qt.quit();
    }
}
