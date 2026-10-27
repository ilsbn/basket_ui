#include "basket_widget.h"

#include "ui_basket_widget.h"

#include <QStyle>
#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>

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
    ui_->indexLabel->setText(QString::number(details.index));
    ui_->statusComboBox->setCurrentIndex(static_cast<int>(details.status));
    ui_->statusComboBox->setAccessibleName(tr("Basket %1 status").arg(details.index));
    const auto updateColour = [this] {
        setProperty("basketState", ui_->statusComboBox->currentText());
        style()->unpolish(this);
        style()->polish(this);
        update();
    };
    connect(ui_->statusComboBox, &QComboBox::currentIndexChanged, this,
            [this, updateColour](int status) {
                details_.status = static_cast<BasketStatus>(status);
                updateColour();
                emit basketChanged(details_);
            });
    updateColour();
}

BasketWidget::~BasketWidget() = default;

void BasketWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        dragStart_ = event->position().toPoint();
    }
    QFrame::mousePressEvent(event);
}

void BasketWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (!(event->buttons() & Qt::LeftButton)
        || (event->position().toPoint() - dragStart_).manhattanLength()
               < QApplication::startDragDistance()) {
        QFrame::mouseMoveEvent(event);
        return;
    }
    auto* drag = new QDrag(this);
    auto* mime = new QMimeData;
    mime->setData(MimeType, QByteArray());
    drag->setMimeData(mime);
    drag->setPixmap(grab());
    drag->setHotSpot(dragStart_);
    drag->exec(Qt::MoveAction);
}

} // namespace basket
