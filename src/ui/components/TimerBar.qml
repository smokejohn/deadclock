import QtQuick

Item {
    id: control

    // property int barWidth: 300
    // property int barHeight: 32
    property real radius: 4
    property color backgroundColor: Qt.rgba(0.0, 0.0, 0.0, 0.4)
    property color barColor: Qt.color("#298753")
    property real percentageFilled: 1.0
    property color textColor: Qt.color("#fff")
    property int fontSize: 20
    property string text: ""
    property string time: "00:00"
    property real margins: 8

    width: 300
    height: 32

    function format_time(time_left) {
        if (time_left < 60) {
            return time_left + "s";
        }

        let minutes = Math.floor(time_left / 60);
        let seconds = time_left % 60;

        let padded_seconds = seconds < 10 ? "0" + seconds : seconds;
        return minutes + ":" + padded_seconds;
    }

    Rectangle {
        id: bar_background
        width: parent.width
        height: parent.height
        radius: control.radius
        color: control.backgroundColor

        Rectangle {
            id: bar
            width: control.width * control.percentageFilled
            height: control.height
            radius: control.radius
            color: control.barColor
        }

        Text {
            id: bar_label
            anchors {
                verticalCenter: bar_background.verticalCenter
                left: bar_background.left
                leftMargin: control.margins
            }
            text: control.text
            font.pixelSize: control.fontSize
            color: control.textColor
        }

        Text {
            id: timer
            anchors {
                verticalCenter: bar_background.verticalCenter
                right: bar_background.right
                rightMargin: control.margins
            }
            text: format_time(control.time)
            font.pixelSize: control.fontSize
            color: control.textColor
        }
    }
}
