import QtQuick

Flow {
    id: root
    property color value
    property bool allowNone: false
    property bool none: false
    readonly property var colors: ["#1f2430", "#8b8d98", "#ffffff", "#e5484d", "#f5a524", "#f2d12c",
                                    "#30a46c", "#12a594", "#3e63dd", "#8e4ec6", "#e93d82", "#8b5a2b"]
    signal picked(color c)
    signal nonePicked()

    spacing: 8

    Rectangle {
        visible: root.allowNone
        width: 24; height: 24; radius: 12
        color: "white"
        border.width: root.none ? 2 : 1
        border.color: root.none ? "#3e63dd" : "#c9ceda"

        Rectangle {
            anchors.centerIn: parent
            width: 20; height: 2; rotation: -45
            color: "#e5484d"
        }

        MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: root.nonePicked() }
    }

    Repeater {
        model: root.colors

        Rectangle {
            readonly property bool selected: !root.none && Qt.colorEqual(modelData, root.value)
            width: 24; height: 24; radius: 12
            color: modelData
            border.width: selected ? 2 : 1
            border.color: selected ? "#3e63dd" : "#c9ceda"
            scale: selected ? 1.12 : 1
            Behavior on scale { NumberAnimation { duration: 100 } }
            MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: root.picked(modelData) }
        }
    }
}
