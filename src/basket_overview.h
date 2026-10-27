#pragma once

#include "basket_details.h"
#include "basket_overview_settings.h"

#include <QWidget>

#include <memory>

namespace Ui {
class BasketOverview;
}

namespace basket {

class BasketWidget;

class BasketOverview final : public QWidget
{
    Q_OBJECT

public:
    explicit BasketOverview(const BasketArray& baskets, QWidget* parent = nullptr);
    ~BasketOverview() override;
    BasketOverviewSettings settings() const;
    const BasketArray& baskets() const { return baskets_; }
    bool moveBasket(int from, int to);

signals:
    void basketsChanged(const basket::BasketArray& baskets);
    void inProgressChanged(int value);
    void pendingChanged(int value);
    void basketTypeChanged(const QString& basketType);

private:
    BasketArray baskets_;
    std::array<BasketWidget*, 20> widgets_{};
    std::unique_ptr<Ui::BasketOverview> ui_;
};

} // namespace basket
