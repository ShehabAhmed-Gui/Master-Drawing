#include "drawingcanvas.h"

#include <QCursor>
#include <QHoverEvent>
#include <QLineF>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPathStroker>
#include <QRectF>

namespace {

class AddCommand : public QUndoCommand
{
public:
    AddCommand(QList<Shape> &shapes, const Shape &shape, QQuickItem *item)
        : m_shapes(shapes), m_shape(shape), m_item(item) {}

    void undo() override { m_shapes.removeLast(); m_item->update(); }
    void redo() override { m_shapes.append(m_shape); m_item->update(); }

private:
    QList<Shape> &m_shapes;
    Shape m_shape;
    QQuickItem *m_item;
};

// The shapes are already removed while dragging, so the first redo() is a no-op.
class EraseCommand : public QUndoCommand
{
public:
    EraseCommand(QList<Shape> &shapes, const QList<QPair<int, Shape>> &erased, QQuickItem *item)
        : m_shapes(shapes)
        , m_erased(erased)
        , m_item(item) {}

    void undo() override
    {
        for (auto it = m_erased.crbegin(); it != m_erased.crend(); ++it)
            m_shapes.insert(it->first, it->second);
        m_item->update();
    }

    void redo() override
    {
        // Nothing to redo
        if (m_first) {
            m_first = false;
            return;
        }
        for (const auto &e : std::as_const(m_erased))
            m_shapes.removeAt(e.first);
        m_item->update();
    }

private:
    QList<Shape> &m_shapes;
    QList<QPair<int, Shape>> m_erased;
    QQuickItem *m_item;
    bool m_first = true;
};

QPainterPath segmentPath(const QPointF &from, const QPointF &to)
{
    QPainterPath path(from);
    path.lineTo(to == from ? to + QPointF(0.01, 0) : to);
    return path;
}

} // namespace

DrawingCanvas::DrawingCanvas(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    setAcceptedMouseButtons(Qt::LeftButton);
    setAcceptHoverEvents(true);
    setAntialiasing(true);
    setRenderTarget(QQuickPaintedItem::FramebufferObject);

    auto refreshCursor = [this] {
        setCursor(m_tool == Eraser ? Qt::BlankCursor : Qt::CrossCursor);
        update();
    };

    connect(this, &DrawingCanvas::toolChanged, this, refreshCursor);
    connect(this, &DrawingCanvas::optionsChanged, this, [this] { update(); });
    connect(&m_undo, &QUndoStack::indexChanged, this, &DrawingCanvas::undoStateChanged);
    refreshCursor();
}

Shape DrawingCanvas::makeShape() const
{
    Shape s;
    s.pen = QPen(m_strokeColor, m_size, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    s.opacity = m_drawOpacity;
    if ((m_tool == Rectangle || m_tool == Circle) && m_fillEnabled)
        s.brush = QBrush(m_fillColor);
    return s;
}

void DrawingCanvas::updateCurrent(const QPointF &pos)
{
    QPainterPath path;
    switch (m_tool) {
    case Pen: {
        // Quad curves through midpoints keep freehand strokes smooth
        path = m_current.path;
        path.quadTo(m_last, (m_last + pos) / 2);
        m_last = pos;
        break;
    }
    case Line:
        path = segmentPath(m_start, pos);
        break;
    case Rectangle:
        path.addRect(QRectF(m_start, pos).normalized());
        break;
    case Circle: {
        const qreal r = QLineF(m_start, pos).length();
        path.addEllipse(m_start, r, r);
        break;
    }
    default:
        return;
    }
    m_current.path = path;
}

void DrawingCanvas::eraseSegment(const QPointF &from, const QPointF &to)
{
    QPainterPathStroker eraserStroker;
    eraserStroker.setWidth(m_size);
    eraserStroker.setCapStyle(Qt::RoundCap);
    const QPainterPath eraser = eraserStroker.createStroke(segmentPath(from, to));

    for (int i = m_shapes.size() - 1; i >= 0; --i) {
        const Shape &s = m_shapes.at(i);
        QPainterPathStroker stroker;
        stroker.setWidth(qMax<qreal>(s.pen.widthF(), 1));
        stroker.setCapStyle(Qt::RoundCap);
        stroker.setJoinStyle(Qt::RoundJoin);
        QPainterPath hit = stroker.createStroke(s.path);
        if (s.brush.style() != Qt::NoBrush)
            hit = hit.united(s.path);

        if (hit.intersects(eraser)) {
            m_erased.append({i, s});
            m_shapes.removeAt(i);
        }
    }
}

void DrawingCanvas::finishStroke()
{
    if (m_tool == Eraser) {
        if (!m_erased.isEmpty())
            m_undo.push(new EraseCommand(m_shapes, m_erased, this));
        m_erased.clear();
        return;
    }
    if (m_tool == Pen)
        m_current.path.lineTo(m_last);
    if (!m_current.path.isEmpty() && (m_tool == Pen || m_start != m_last))
        m_undo.push(new AddCommand(m_shapes, m_current, this));
    m_current = Shape();
}

void DrawingCanvas::mousePressEvent(QMouseEvent *event)
{
    const QPointF pos = event->position();
    m_active = true;
    m_start = m_last = m_hover = pos;
    m_hovering = true;

    if (m_tool == Eraser) {
        m_erased.clear();
        eraseSegment(pos, pos);
    } else {
        m_current = makeShape();
        if (m_tool == Pen)
            m_current.path = segmentPath(pos, pos);
    }
    update();
}

void DrawingCanvas::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_active)
        return;
    const QPointF pos = event->position();
    m_hover = pos;

    if (m_tool == Eraser) {
        eraseSegment(m_last, pos);
        m_last = pos;
    } else {
        if (m_tool != Pen)
            m_last = pos;
        updateCurrent(pos);
    }
    update();
}

void DrawingCanvas::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    if (!m_active)
        return;
    m_active = false;
    finishStroke();
    update();
}

void DrawingCanvas::hoverMoveEvent(QHoverEvent *event)
{
    m_hover = event->position();
    m_hovering = true;
    if (m_tool == Eraser)
        update();
}

void DrawingCanvas::hoverLeaveEvent(QHoverEvent *event)
{
    Q_UNUSED(event);

    m_hovering = false;
    update();
}

void DrawingCanvas::drawShape(QPainter *painter, const Shape &shape) const
{
    painter->setOpacity(shape.opacity);
    painter->setPen(shape.pen);
    painter->setBrush(shape.brush);
    painter->drawPath(shape.path);
}

void DrawingCanvas::paint(QPainter *painter)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->fillRect(boundingRect(), Qt::white);

    for (const Shape &s : std::as_const(m_shapes))
        drawShape(painter, s);
    if (m_active && m_tool != Eraser)
        drawShape(painter, m_current);

    if (m_tool == Eraser && m_hovering) {
        painter->setOpacity(1.0);
        painter->setPen(QPen(QColor("#8b8d98"), 1.5));
        painter->setBrush(QColor(255, 255, 255, 120));
        painter->drawEllipse(m_hover, m_size / 2, m_size / 2);
    }
}
