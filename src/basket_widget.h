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
    explicit BasketWidget(const BasketDetails& details, QWidget* parent = nullptr);
    ~BasketWidget() override;

signals:
    void basketChanged(const basket::BasketDetails& details);

private:
    BasketDetails details_;
    std::unique_ptr<Ui::BasketWidget> ui_;
};

} // namespace basket
