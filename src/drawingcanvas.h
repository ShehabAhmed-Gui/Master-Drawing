#pragma once

#include <QBrush>
#include <QColor>
#include <QList>
#include <QPainterPath>
#include <QPen>
#include <QPair>
#include <QQuickItem>
#include <QQuickPaintedItem>
#include <QUndoStack>
#include <QtQml/qqmlregistration.h>

struct Shape
{
    QPainterPath path;
    QPen pen;
    QBrush brush;
    qreal opacity = 1.0;
};

class DrawingCanvas : public QQuickPaintedItem
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(Tool tool MEMBER m_tool NOTIFY toolChanged)
    Q_PROPERTY(QColor strokeColor MEMBER m_strokeColor NOTIFY optionsChanged)
    Q_PROPERTY(QColor fillColor MEMBER m_fillColor NOTIFY optionsChanged)
    Q_PROPERTY(bool fillEnabled MEMBER m_fillEnabled NOTIFY optionsChanged)
    Q_PROPERTY(qreal size MEMBER m_size NOTIFY optionsChanged)
    Q_PROPERTY(qreal drawOpacity MEMBER m_drawOpacity NOTIFY optionsChanged)
    Q_PROPERTY(bool canUndo READ canUndo NOTIFY undoStateChanged)
    Q_PROPERTY(bool canRedo READ canRedo NOTIFY undoStateChanged)

public:
    enum Tool { Pen, Eraser, Rectangle, Circle, Line };
    Q_ENUM(Tool)

    explicit DrawingCanvas(QQuickItem *parent = nullptr);

    bool canUndo() const { return m_undo.canUndo(); }
    bool canRedo() const { return m_undo.canRedo(); }

    Q_INVOKABLE void undo() { m_undo.undo(); }
    Q_INVOKABLE void redo() { m_undo.redo(); }

    void paint(QPainter *painter) override;

signals:
    void toolChanged();
    void optionsChanged();
    void undoStateChanged();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void hoverMoveEvent(QHoverEvent *event) override;
    void hoverLeaveEvent(QHoverEvent *event) override;

private:
    Shape makeShape() const;
    void updateCurrent(const QPointF &pos);
    void eraseSegment(const QPointF &from, const QPointF &to);
    void finishStroke();
    void drawShape(QPainter *painter, const Shape &shape) const;

    Tool m_tool = Pen;
    QColor m_strokeColor = QColor(0x1f2430);
    QColor m_fillColor = QColor(0x3e63dd);
    bool m_fillEnabled = false;

    qreal m_size = 4;
    qreal m_drawOpacity = 1.0;

    QList<Shape> m_shapes;
    Shape m_current;
    QUndoStack m_undo;

    bool m_active = false;
    bool m_hovering = false;

    QPointF m_start;
    QPointF m_last;
    QPointF m_hover;
    QList<QPair<int, Shape>> m_erased;
};
