#pragma once

#include <QWidget>

#include <array>

class QGridLayout;

namespace basket {

class BasketWidget;

class BasketSlots final : public QWidget
{
    Q_OBJECT

public:
    explicit BasketSlots(QWidget* parent = nullptr);
    void setBaskets(const std::array<BasketWidget*, 20>& baskets);

signals:
    void basketMoved(int from, int to);

protected:
    void paintEvent(QPaintEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    bool acceptsDrag(const QDropEvent* event) const;
    int positionAt(const QPoint& point) const;

    QGridLayout* grid_;
    int dropPosition_ = -1;
};

} // namespace basket
