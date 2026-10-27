#pragma once

#include "basket_details.h"

#include <QFrame>

#include <memory>

namespace Ui {
class BasketWidget;
}

namespace basket {

class BasketWidget final : public QFrame
{
    Q_OBJECT

public:
    static constexpr auto MimeType = "application/x-basket-widget";
    explicit BasketWidget(const BasketDetails& details, QWidget* parent = nullptr);
    ~BasketWidget() override;
    const BasketDetails& details() const { return details_; }
    void setSequence(int sequence) { details_.sequence = sequence; }

signals:
    void basketChanged(const basket::BasketDetails& details);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    QPoint dragStart_;
    BasketDetails details_;
    std::unique_ptr<Ui::BasketWidget> ui_;
};

} // namespace basket
