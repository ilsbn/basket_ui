#include "basket_overview.h"

#include "basket_widget.h"
#include "ui_basket_overview.h"

#include <algorithm>

namespace basket {

BasketOverview::BasketOverview(const BasketArray& baskets, QWidget* parent)
    : QWidget(parent)
    , baskets_(baskets)
    , ui_(std::make_unique<Ui::BasketOverview>())
{
    ui_->setupUi(this);
    std::stable_sort(baskets_.begin(), baskets_.end(), [](const auto& left, const auto& right) {
        return left.sequence < right.sequence;
    });
    connect(ui_->inProgressSpinBox, &QSpinBox::valueChanged,
            this, &BasketOverview::inProgressChanged);
    connect(ui_->pendingSpinBox, &QSpinBox::valueChanged,
            this, &BasketOverview::pendingChanged);
    connect(ui_->basketTypeComboBox, &QComboBox::currentTextChanged,
            this, &BasketOverview::basketTypeChanged);

    int position = 0;
    for (const auto& details : baskets_) {
        auto* widget = new BasketWidget(details, this);
        widgets_[position] = widget;
        connect(widget, &BasketWidget::basketChanged, this,
                [this, widget](const BasketDetails& updated) {
                    const auto position = std::find(widgets_.begin(), widgets_.end(), widget)
                                        - widgets_.begin();
                    baskets_[position] = updated;
                    emit basketsChanged(baskets_);
                });
        ++position;
    }
    ui_->basketSlots->setBaskets(widgets_);
    connect(ui_->basketSlots, &BasketSlots::basketMoved, this, &BasketOverview::moveBasket);
}

BasketOverview::~BasketOverview() = default;

bool BasketOverview::moveBasket(int from, int to)
{
    if (from < 0 || from >= 20 || to < 0 || to >= 20 || from == to) {
        return false;
    }
    const int step = from < to ? 1 : -1;
    for (int position = from; position != to; position += step) {
        std::swap(baskets_[position], baskets_[position + step]);
        std::swap(widgets_[position], widgets_[position + step]);
    }
    for (int position = 0; position < 20; ++position) {
        baskets_[position].sequence = position + 1;
        widgets_[position]->setSequence(position + 1);
    }
    ui_->basketSlots->setBaskets(widgets_);
    emit basketsChanged(baskets_);
    return true;
}

BasketOverviewSettings BasketOverview::settings() const
{
    return {ui_->inProgressSpinBox->value(), ui_->pendingSpinBox->value(),
            ui_->basketTypeComboBox->currentText()};
}

} // namespace basket
