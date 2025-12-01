import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts

import Deadclock.Ui 1.0

Item {
    width: parent ? parent.width : 0
    height: parent ? parent.height : 0

    RowLayout {
        anchors.fill: parent
        Rectangle {
            id: side_bar
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.preferredWidth: 2
            color: Constants.background_dark_color

            ColumnLayout {
                anchors.fill: parent

                Rectangle {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignTop
                    implicitHeight: 32
                    color: Qt.rgba(1.0, 1.0, 1.0, 0.2)

                    Label {
                        text: "Text-To-Speech"
                    }
                }
            }
        }
        Rectangle {
            id: settings_area
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.preferredWidth: 3
            color: Constants.background_color

            Switch {
                id: debug_tracking
                text: "Debug GameState Tracking"
                checked: false
                width: parent.width

                onToggled: {
                    gamestate_tracker.set_debug_tracking(checked);
                }
            }
        }
    }
}
