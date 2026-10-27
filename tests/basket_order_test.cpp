#include "basket_overview.h"
#include "basket_slots.h"
#include "basket_widget.h"
#include "main_window.h"

#include <QApplication>
#include <QComboBox>
#include <QDrag>
#include <QDragEnterEvent>
#include <QGridLayout>
#include <QMimeData>
#include <QMouseEvent>
#include <QTimer>

#include <gtest/gtest.h>

#include <algorithm>

namespace {

basket::BasketArray sampleBaskets()
{
    basket::BasketArray baskets{};
    for (int position = 0; position < 20; ++position) {
        baskets[position] = {100 + position, position + 1, basket::BasketStatus::Empty};
    }
    return baskets;
}

basket::BasketWidget* tileAt(basket::BasketOverview& overview, int row, int column)
{
    auto* grid = overview.findChild<QGridLayout*>("basketGrid");
    if (!grid || !grid->itemAtPosition(row, column)) {
        return nullptr;
    }
    return qobject_cast<basket::BasketWidget*>(grid->itemAtPosition(row, column)->widget());
}

} // namespace

TEST(BasketOrder, SortsBySequenceAndUsesSnakeRows)
{
    auto baskets = sampleBaskets();
    std::reverse(baskets.begin(), baskets.end());
    basket::BasketOverview overview(baskets);
    ASSERT_NE(tileAt(overview, 0, 0), nullptr);
    ASSERT_NE(tileAt(overview, 1, 4), nullptr);
    ASSERT_NE(tileAt(overview, 1, 0), nullptr);
    ASSERT_NE(tileAt(overview, 3, 0), nullptr);
    EXPECT_EQ(tileAt(overview, 0, 0)->details().index, 100);
    EXPECT_EQ(tileAt(overview, 0, 4)->details().index, 104);
    EXPECT_EQ(tileAt(overview, 1, 4)->details().index, 105);
    EXPECT_EQ(tileAt(overview, 1, 0)->details().index, 109);
    EXPECT_EQ(tileAt(overview, 2, 0)->details().index, 110);
    EXPECT_EQ(tileAt(overview, 3, 4)->details().index, 115);
    EXPECT_EQ(tileAt(overview, 3, 0)->details().index, 119);
    EXPECT_EQ(overview.baskets().front().sequence, 1);
    EXPECT_EQ(overview.baskets().back().sequence, 20);
    EXPECT_EQ(baskets.front().index, 119);
}

TEST(BasketOrder, EqualSequencesKeepTheirInputOrder)
{
    auto baskets = sampleBaskets();
    baskets[0] = {42, 1, basket::BasketStatus::Sold};
    baskets[1] = {7, 1, basket::BasketStatus::Pending};
    basket::BasketOverview overview(baskets);
    EXPECT_EQ(overview.baskets()[0].index, 42);
    EXPECT_EQ(overview.baskets()[1].index, 7);
}

TEST(BasketOrder, MovingFirstToLastShiftsOthersAndRenumbersOnce)
{
    auto baskets = sampleBaskets();
    baskets.front().status = basket::BasketStatus::Sold;
    basket::BasketOverview overview(baskets);
    auto* first = tileAt(overview, 0, 0);
    ASSERT_NE(first, nullptr);
    int notifications = 0;
    basket::BasketArray saved{};
    QObject::connect(&overview, &basket::BasketOverview::basketsChanged, &overview,
                     [&](const basket::BasketArray& values) { saved = values; ++notifications; });

    ASSERT_TRUE(overview.moveBasket(0, 19));
    EXPECT_EQ(notifications, 1);
    EXPECT_EQ(saved.front().index, 101);
    EXPECT_EQ(saved[18].index, 119);
    EXPECT_EQ(saved.back().index, 100);
    EXPECT_EQ(saved.back().status, basket::BasketStatus::Sold);
    EXPECT_EQ(tileAt(overview, 3, 0), first);
    for (int position = 0; position < 20; ++position) {
        EXPECT_EQ(saved[position].sequence, position + 1);
    }
    EXPECT_EQ(first->details().sequence, 20);
    EXPECT_EQ(baskets.front().sequence, 1);
}

TEST(BasketOrder, MovingLastToFirstShiftsOthersAndIgnoresInvalidMoves)
{
    basket::BasketOverview overview(sampleBaskets());
    int notifications = 0;
    QObject::connect(&overview, &basket::BasketOverview::basketsChanged, &overview,
                     [&](const basket::BasketArray&) { ++notifications; });
    ASSERT_TRUE(overview.moveBasket(19, 0));
    EXPECT_EQ(overview.baskets().front().index, 119);
    EXPECT_EQ(overview.baskets()[1].index, 100);
    EXPECT_EQ(overview.baskets().back().index, 118);
    EXPECT_EQ(overview.baskets().back().sequence, 20);
    EXPECT_FALSE(overview.moveBasket(0, 0));
    EXPECT_FALSE(overview.moveBasket(-1, 0));
    EXPECT_FALSE(overview.moveBasket(0, 20));
    EXPECT_EQ(notifications, 1);
}

TEST(BasketOrder, StatusEditsAfterCrossingARowUpdateTheMovedBasket)
{
    basket::BasketOverview overview(sampleBaskets());
    ASSERT_TRUE(overview.moveBasket(4, 5));
    auto* moved = tileAt(overview, 1, 4);
    ASSERT_NE(moved, nullptr);
    EXPECT_EQ(moved->details().index, 104);
    auto* status = moved->findChild<QComboBox*>("statusComboBox");
    ASSERT_NE(status, nullptr);
    status->setCurrentIndex(static_cast<int>(basket::BasketStatus::Pending));
    EXPECT_EQ(overview.baskets()[5].index, 104);
    EXPECT_EQ(overview.baskets()[5].sequence, 6);
    EXPECT_EQ(overview.baskets()[5].status, basket::BasketStatus::Pending);
    EXPECT_EQ(overview.baskets()[4].index, 105);
    EXPECT_EQ(overview.baskets()[4].status, basket::BasketStatus::Empty);
}

TEST(BasketOrder, MainWindowReceivesTheReorderedDataForPersistence)
{
    basket::MainWindow window;
    auto* overview = qobject_cast<basket::BasketOverview*>(window.centralWidget());
    ASSERT_NE(overview, nullptr);
    int notifications = 0;
    basket::BasketArray saved{};
    QObject::connect(&window, &basket::MainWindow::basketsChanged, &window,
                     [&](const basket::BasketArray& values) { saved = values; ++notifications; });
    ASSERT_TRUE(overview->moveBasket(0, 19));
    EXPECT_EQ(notifications, 1);
    EXPECT_EQ(saved.back().index, 5);
    EXPECT_EQ(saved.back().sequence, 20);
    EXPECT_EQ(saved.back().status, basket::BasketStatus::Pending);
    EXPECT_EQ(window.baskets().back().index, 5);
}

TEST(BasketOrder, RejectsExternalDragsWithoutChangingData)
{
    basket::BasketOverview overview(sampleBaskets());
    auto* surface = overview.findChild<basket::BasketSlots*>("basketSlots");
    ASSERT_NE(surface, nullptr);
    QMimeData mime;
    mime.setData(basket::BasketWidget::MimeType, QByteArray());
    QDragEnterEvent event(QPoint(100, 100), Qt::MoveAction, &mime,
                          Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(surface, &event);
    EXPECT_FALSE(event.isAccepted());
    EXPECT_EQ(overview.baskets().front().index, 100);
}

TEST(BasketOrder, AcceptsAnInternalDropAndMovesTheSourceBasket)
{
    basket::BasketOverview overview(sampleBaskets());
    overview.resize(1120, 850);
    overview.show();
    QApplication::processEvents();
    auto* source = tileAt(overview, 0, 0);
    auto* target = tileAt(overview, 1, 4);
    auto* surface = overview.findChild<basket::BasketSlots*>("basketSlots");
    ASSERT_NE(source, nullptr);
    ASSERT_NE(target, nullptr);
    ASSERT_NE(surface, nullptr);
    const QPoint dropPoint = target->geometry().center();
    bool delivered = false;
    QTimer::singleShot(0, &overview, [&] {
        const auto* drag = source->findChild<QDrag*>();
        if (!drag || !drag->mimeData()) {
            ADD_FAILURE() << "The basket mouse gesture did not start a drag";
            QDrag::cancel();
            return;
        }
        const auto* mime = drag->mimeData();
        QDragEnterEvent enter(dropPoint, Qt::MoveAction, mime,
                              Qt::LeftButton, Qt::NoModifier);
        QApplication::sendEvent(surface, &enter);
        EXPECT_TRUE(enter.isAccepted());
        QDropEvent drop(QPointF(dropPoint), Qt::MoveAction, mime,
                        Qt::LeftButton, Qt::NoModifier);
        QApplication::sendEvent(surface, &drop);
        EXPECT_TRUE(drop.isAccepted());
        delivered = true;
        QDrag::cancel();
    });
    QTimer::singleShot(2000, &overview, [] { QDrag::cancel(); });
    const QPointF pressPoint(20, 40);
    const QPointF movePoint(55, 40);
    QMouseEvent press(QEvent::MouseButtonPress, pressPoint,
                      QPointF(source->mapToGlobal(pressPoint.toPoint())),
                      Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(source, &press);
    QMouseEvent move(QEvent::MouseMove, movePoint,
                     QPointF(source->mapToGlobal(movePoint.toPoint())),
                     Qt::NoButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(source, &move);
    EXPECT_TRUE(delivered);
    EXPECT_EQ(overview.baskets()[5].index, 100);
    EXPECT_EQ(overview.baskets()[5].sequence, 6);
    EXPECT_EQ(tileAt(overview, 1, 4), source);
}
