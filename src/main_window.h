#pragma once

#include "basket_details.h"
#include "basket_overview_settings.h"

#include <QMainWindow>

namespace basket {

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    const BasketArray& baskets() const { return baskets_; }
    const BasketOverviewSettings& overviewSettings() const { return overviewSettings_; }

signals:
    void basketsChanged(const basket::BasketArray& baskets);
    void overviewSettingsChanged(const basket::BasketOverviewSettings& settings);

private:
    BasketArray baskets_{};
    BasketOverviewSettings overviewSettings_{};
};

} // namespace basket
