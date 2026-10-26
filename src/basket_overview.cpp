#include "basket_overview.h"

#include "ui_basket_overview.h"

// Keep the resource object linked when the widget is used from a static library.
static void initializeBasketResources()
{
    Q_INIT_RESOURCE(basket_resources);
}

namespace basket {

BasketOverview::BasketOverview(QWidget* parent)
    : QWidget(parent)
    , ui_(std::make_unique<Ui::BasketOverview>())
{
    initializeBasketResources();
    ui_->setupUi(this);
}

BasketOverview::~BasketOverview() = default;

} // namespace basket
