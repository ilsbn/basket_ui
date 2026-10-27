#include "basket_slots.h"

#include "basket_widget.h"

#include <QDragEnterEvent>
#include <QGridLayout>
#include <QMimeData>
#include <QPainter>
#include <QPainterPath>

#include <limits>

namespace basket {

BasketSlots::BasketSlots(QWidget* parent)
    : QWidget(parent)
    , grid_(new QGridLayout(this))
{
    setAcceptDrops(true);
    grid_->setObjectName(QStringLiteral("basketGrid"));
    grid_->setContentsMargins(72, 28, 72, 28);
    grid_->setHorizontalSpacing(26);
    grid_->setVerticalSpacing(40);
}

void BasketSlots::setBaskets(const std::array<BasketWidget*, 20>& baskets)
{
    while (auto* item = grid_->takeAt(0)) {
        delete item;
    }
    for (int position = 0; position < 20; ++position) {
        const int row = position / 5;
        const int column = row % 2 == 0 ? position % 5 : 4 - position % 5;
        grid_->addWidget(baskets[position], row, column);
    }
    grid_->activate();
    update();
}

void BasketSlots::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);
    if (grid_->count() != 20) {
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    const QColor purple(QStringLiteral("#7650ed"));
    painter.setPen(QPen(purple, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    QPainterPath path;
    const auto first = grid_->itemAtPosition(0, 0)->geometry();
    path.moveTo(first.left() - 48, first.center().y());
    for (int row = 0; row < 4; ++row) {
        const auto left = grid_->itemAtPosition(row, 0)->geometry();
        const auto right = grid_->itemAtPosition(row, 4)->geometry();
        const int direction = row % 2 == 0 ? 1 : -1;
        const qreal endX = direction == 1 ? right.right() + 36 : left.left() - 36;
        const qreal y = left.center().y();
        path.lineTo(endX, y);
        painter.drawLine(QPointF(endX - direction * 12, y - 6), QPointF(endX, y));
        painter.drawLine(QPointF(endX - direction * 12, y + 6), QPointF(endX, y));
        if (row < 3) {
            path.lineTo(endX, grid_->itemAtPosition(row + 1, 0)->geometry().center().y());
        }
    }
    painter.drawPath(path);
    QFont font = painter.font();
    font.setPixelSize(16);
    painter.setFont(font);
    painter.drawText(QRect(first.left() - 62, first.center().y() - 34, 60, 24),
                     Qt::AlignCenter, tr("first"));
    const auto last = grid_->itemAtPosition(3, 0)->geometry();
    painter.drawText(QRect(last.left() - 62, last.center().y() - 34, 60, 24),
                     Qt::AlignCenter, tr("last"));

    if (dropPosition_ >= 0 && dropPosition_ < grid_->count()) {
        painter.setPen(QPen(purple, 2, Qt::DashLine));
        painter.drawRoundedRect(grid_->itemAt(dropPosition_)->geometry().adjusted(-5, -5, 5, 5),
                                18, 18);
    }
}

bool BasketSlots::acceptsDrag(const QDropEvent* event) const
{
    const auto* widget = qobject_cast<BasketWidget*>(event->source());
    return widget && grid_->indexOf(widget) >= 0
        && event->mimeData()->hasFormat(BasketWidget::MimeType);
}

int BasketSlots::positionAt(const QPoint& point) const
{
    int nearest = -1;
    qint64 bestDistance = std::numeric_limits<qint64>::max();
    for (int position = 0; position < grid_->count(); ++position) {
        const auto offset = grid_->itemAt(position)->geometry().center() - point;
        const qint64 distance = qint64(offset.x()) * offset.x() + qint64(offset.y()) * offset.y();
        if (distance < bestDistance) {
            nearest = position;
            bestDistance = distance;
        }
    }
    return nearest;
}

void BasketSlots::dragEnterEvent(QDragEnterEvent* event)
{
    dragMoveEvent(event);
}

void BasketSlots::dragMoveEvent(QDragMoveEvent* event)
{
    if (!acceptsDrag(event)) {
        event->ignore();
        return;
    }
    dropPosition_ = positionAt(event->position().toPoint());
    event->setDropAction(Qt::MoveAction);
    event->accept();
    update();
}

void BasketSlots::dragLeaveEvent(QDragLeaveEvent* event)
{
    dropPosition_ = -1;
    update();
    event->accept();
}

void BasketSlots::dropEvent(QDropEvent* event)
{
    if (!acceptsDrag(event)) {
        event->ignore();
        return;
    }
    const int from = grid_->indexOf(qobject_cast<BasketWidget*>(event->source()));
    const int to = positionAt(event->position().toPoint());
    dropPosition_ = -1;
    event->setDropAction(Qt::MoveAction);
    event->accept();
    emit basketMoved(from, to);
    update();
}

} // namespace basket
