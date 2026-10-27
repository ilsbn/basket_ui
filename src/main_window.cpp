#include "main_window.h"

#include "basket_overview.h"

namespace basket {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    using enum BasketStatus;
    constexpr BasketArray initialBaskets{{
        {1, 5, Sold},
        {2, 2, Empty},
        {3, 3, Empty},
        {4, 4, InProgress},
        {5, 1, Pending},
        {6, 6, Sold},
        {7, 7, Empty},
        {8, 8, Empty},
        {9, 9, InProgress},
        {10, 10, Empty},
        {11, 11, Empty},
        {12, 12, Pending},
        {13, 13, Empty},
        {14, 14, InProgress},
        {15, 15, Pending},
        {16, 16, Sold},
        {17, 17, Empty},
        {18, 18, Empty},
        {19, 19, InProgress},
        {20, 20, Pending},
    }};

    setWindowTitle(QStringLiteral("Basket UI"));
    baskets_ = initialBaskets;
    auto* overview = new BasketOverview(baskets_, this);
    baskets_ = overview->baskets();
    connect(overview, &BasketOverview::basketsChanged, this, [this](const BasketArray& updated) {
        baskets_ = updated;
        emit basketsChanged(baskets_);
    });
    overviewSettings_ = overview->settings();
    const auto updateSettings = [this, overview] {
        overviewSettings_ = overview->settings();
        emit overviewSettingsChanged(overviewSettings_);
    };
    connect(overview, &BasketOverview::inProgressChanged, this, updateSettings);
    connect(overview, &BasketOverview::pendingChanged, this, updateSettings);
    connect(overview, &BasketOverview::basketTypeChanged, this, updateSettings);
    setCentralWidget(overview);
    resize(1120, 850);
}

} // namespace basket
