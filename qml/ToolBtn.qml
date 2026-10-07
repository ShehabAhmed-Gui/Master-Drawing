import QtQuick
import QtQuick.Controls

Button {
    id: root
    property bool active: false

    implicitWidth: 64
    implicitHeight: 36

    background: Rectangle {
        radius: 8
        color: root.active ? "#3e63dd" : root.hovered ? "#e6eaf3" : "#f2f4f9"
        Behavior on color { ColorAnimation { duration: 120 } }
    }
    contentItem: Text {
        text: root.text
        color: root.active ? "white" : "#2b3345"
        font.pixelSize: 13
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
