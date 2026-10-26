#include "basket_widget.h"

#include "ui_basket_widget.h"

#include <QStyle>

// Keep the resource object linked when the widget is used from a static library.
static void initializeBasketResources()
{
    Q_INIT_RESOURCE(basket_resources);
}

namespace basket {

BasketWidget::BasketWidget(const BasketDetails& details, QWidget* parent)
    : QFrame(parent)
    , details_(details)
    , ui_(std::make_unique<Ui::BasketWidget>())
{
    initializeBasketResources();
    ui_->setupUi(this);
    ui_->indexLabel->setText(tr("Index: %1").arg(details.index));
    ui_->sequenceSpinBox->setValue(details.sequence);
    ui_->sequenceSpinBox->setAccessibleName(tr("Basket %1 sequence").arg(details.index));
    ui_->statusComboBox->setCurrentIndex(static_cast<int>(details.status));
    ui_->statusComboBox->setAccessibleName(tr("Basket %1 status").arg(details.index));
    const auto updateColour = [this] {
        setProperty("basketState", ui_->statusComboBox->currentText());
        style()->unpolish(this);
        style()->polish(this);
        update();
    };
    connect(ui_->sequenceSpinBox, &QSpinBox::valueChanged, this, [this](int sequence) {
        details_.sequence = sequence;
        emit basketChanged(details_);
    });
    connect(ui_->statusComboBox, &QComboBox::currentIndexChanged, this,
            [this, updateColour](int status) {
                details_.status = static_cast<BasketStatus>(status);
                updateColour();
                emit basketChanged(details_);
            });
    updateColour();
}

BasketWidget::~BasketWidget() = default;

} // namespace basket
