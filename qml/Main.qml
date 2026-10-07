import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Draw

ApplicationWindow {
    id: win
    width: 1100
    height: 720
    minimumWidth: 640
    minimumHeight: 520
    visible: true
    title: "Master Drawing"
    color: "#e9ecf3"

    component SectionLabel: Text {
        color: "#6b7385"
        font.pixelSize: 11
        font.bold: true
        font.capitalization: Font.AllUppercase
    }

    Shortcut { sequences: [StandardKey.Undo]; onActivated: canvas.undo() }
    Shortcut { sequences: [StandardKey.Redo]; onActivated: canvas.redo() }

    Rectangle {
        id: panel
        anchors { left: parent.left; top: parent.top; bottom: parent.bottom }
        width: 232
        color: "white"

        ColumnLayout {
            anchors { fill: parent; margins: 16 }
            spacing: 14

            SectionLabel { text: "Tools" }
            Flow {
                Layout.fillWidth: true
                spacing: 8
                Repeater {
                    model: [
                        { name: "Pen", tool: DrawingCanvas.Pen },
                        { name: "Eraser", tool: DrawingCanvas.Eraser },
                        { name: "Line", tool: DrawingCanvas.Line },
                        { name: "Rect", tool: DrawingCanvas.Rectangle },
                        { name: "Circle", tool: DrawingCanvas.Circle }
                    ]
                    ToolBtn {
                        text: modelData.name
                        active: canvas.tool === modelData.tool
                        onClicked: canvas.tool = modelData.tool
                    }
                }
            }

            SectionLabel { text: "Stroke color"; visible: canvas.tool !== DrawingCanvas.Eraser }
            Swatches {
                Layout.fillWidth: true
                visible: canvas.tool !== DrawingCanvas.Eraser
                value: canvas.strokeColor
                onPicked: c => canvas.strokeColor = c
            }

            SectionLabel {
                text: "Fill color"
                visible: canvas.tool === DrawingCanvas.Rectangle || canvas.tool === DrawingCanvas.Circle
            }
            Swatches {
                Layout.fillWidth: true
                visible: canvas.tool === DrawingCanvas.Rectangle || canvas.tool === DrawingCanvas.Circle
                allowNone: true
                value: canvas.fillColor
                none: !canvas.fillEnabled
                onPicked: c => { canvas.fillColor = c; canvas.fillEnabled = true }
                onNonePicked: canvas.fillEnabled = false
            }

            SectionLabel { text: (canvas.tool === DrawingCanvas.Eraser ? "Eraser size" : "Size") + "  " + Math.round(canvas.size) }
            Slider {
                Layout.fillWidth: true
                from: 1; to: 60
                value: canvas.size
                onMoved: canvas.size = value
            }

            SectionLabel {
                text: "Opacity  " + Math.round(canvas.drawOpacity * 100) + "%"
                visible: canvas.tool !== DrawingCanvas.Eraser
            }
            Slider {
                Layout.fillWidth: true
                visible: canvas.tool !== DrawingCanvas.Eraser
                from: 0.05; to: 1
                value: canvas.drawOpacity
                onMoved: canvas.drawOpacity = value
            }

            Item { Layout.fillHeight: true }

            RowLayout {
                Layout.fillWidth: true
                spacing: 8
                ToolBtn {
                    Layout.fillWidth: true
                    text: "Undo"
                    enabled: canvas.canUndo
                    opacity: enabled ? 1 : 0.4
                    onClicked: canvas.undo()
                }
                ToolBtn {
                    Layout.fillWidth: true
                    text: "Redo"
                    enabled: canvas.canRedo
                    opacity: enabled ? 1 : 0.4
                    onClicked: canvas.redo()
                }
            }
        }
    }

    Rectangle {
        anchors { left: panel.right; right: parent.right; top: parent.top; bottom: parent.bottom; margins: 16 }
        color: "white"
        border.color: "#d5d9e4"

        DrawingCanvas {
            id: canvas
            anchors { fill: parent; margins: 1 }
        }
    }
}
