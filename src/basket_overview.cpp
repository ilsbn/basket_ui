#include "basket_overview.h"

#include "basket_widget.h"
#include "ui_basket_overview.h"

namespace basket {

BasketOverview::BasketOverview(const BasketArray& baskets, QWidget* parent)
    : QWidget(parent)
    , baskets_(baskets)
    , ui_(std::make_unique<Ui::BasketOverview>())
{
    ui_->setupUi(this);
    connect(ui_->inProgressSpinBox, &QSpinBox::valueChanged,
            this, &BasketOverview::inProgressChanged);
    connect(ui_->pendingSpinBox, &QSpinBox::valueChanged,
            this, &BasketOverview::pendingChanged);
    connect(ui_->basketTypeComboBox, &QComboBox::currentTextChanged,
            this, &BasketOverview::basketTypeChanged);

    int position = 0;
    for (const auto& details : baskets_) {
        auto* widget = new BasketWidget(details, this);
        connect(widget, &BasketWidget::basketChanged, this,
                [this, position](const BasketDetails& updated) {
                    baskets_[position] = updated;
                    emit basketsChanged(baskets_);
                });
        ui_->basketGrid->addWidget(widget, position / 5, position % 5);
        ++position;
    }
}

BasketOverview::~BasketOverview() = default;

BasketOverviewSettings BasketOverview::settings() const
{
    return {ui_->inProgressSpinBox->value(), ui_->pendingSpinBox->value(),
            ui_->basketTypeComboBox->currentText()};
}

} // namespace basket
