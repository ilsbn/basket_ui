#include "main_window.h"
#include "basket_overview.h"
#include "basket_widget.h"

#include <QApplication>
#include <QComboBox>
#include <QGridLayout>
#include <QImage>
#include <QLabel>
#include <QPixmap>
#include <QSpinBox>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <vector>

TEST(QtRuntime, DecodesPngImagesRequiredByWindowsCursors)
{
    // Qt's Windows hand/drag cursors load PNG resources even in a lean Qt build.
    const auto png = QByteArray::fromBase64(
        "iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVR4nGMoC3j7HwAFpgKzvQUGUgAAAABJRU5ErkJggg==");
    const auto image = QImage::fromData(png, "PNG");
    ASSERT_FALSE(image.isNull()) << "Qt requires the vcpkg qtbase[png] feature for its cursor assets";
    EXPECT_EQ(image.size(), QSize(1, 1));
    EXPECT_EQ(image.pixelColor(0, 0), QColor(QStringLiteral("#7650ed")));
}

TEST(BasketOverview, EmitsChangesForEachOverviewField)
{
    basket::BasketOverview overview(basket::BasketArray{});
    auto* inProgress = overview.findChild<QSpinBox*>("inProgressSpinBox");
    auto* pending = overview.findChild<QSpinBox*>("pendingSpinBox");
    auto* type = overview.findChild<QComboBox*>("basketTypeComboBox");
    ASSERT_NE(inProgress, nullptr);
    ASSERT_NE(pending, nullptr);
    ASSERT_NE(type, nullptr);
    std::vector<int> inProgressChanges;
    std::vector<int> pendingChanges;
    std::vector<QString> typeChanges;
    QObject::connect(&overview, &basket::BasketOverview::inProgressChanged, &overview,
                     [&](int value) { inProgressChanges.push_back(value); });
    QObject::connect(&overview, &basket::BasketOverview::pendingChanged, &overview,
                     [&](int value) { pendingChanges.push_back(value); });
    QObject::connect(&overview, &basket::BasketOverview::basketTypeChanged, &overview,
                     [&](const QString& value) { typeChanges.push_back(value); });

    inProgress->setValue(8);
    pending->setValue(12);
    type->setCurrentText(QStringLiteral("premium"));
    type->setCurrentText(QStringLiteral("platinum"));
    EXPECT_THAT(inProgressChanges, ::testing::ElementsAre(8));
    EXPECT_THAT(pendingChanges, ::testing::ElementsAre(12));
    EXPECT_THAT(typeChanges, ::testing::ElementsAre(QStringLiteral("premium"),
                                                  QStringLiteral("platinum")));
}

TEST(MainWindow, KeepsAndNotifiesOverviewSettingsSnapshots)
{
    basket::MainWindow window;
    auto* inProgress = window.findChild<QSpinBox*>("inProgressSpinBox");
    auto* pending = window.findChild<QSpinBox*>("pendingSpinBox");
    auto* type = window.findChild<QComboBox*>("basketTypeComboBox");
    ASSERT_NE(inProgress, nullptr);
    ASSERT_NE(pending, nullptr);
    ASSERT_NE(type, nullptr);
    EXPECT_EQ(window.overviewSettings().inProgress, 4);
    EXPECT_EQ(window.overviewSettings().pending, 4);
    EXPECT_EQ(window.overviewSettings().basketType, QStringLiteral("standard"));
    std::vector<basket::BasketOverviewSettings> changes;
    QObject::connect(&window, &basket::MainWindow::overviewSettingsChanged, &window,
                     [&](const basket::BasketOverviewSettings& settings) {
                         changes.push_back(settings);
                     }, Qt::QueuedConnection);

    inProgress->setValue(8);
    pending->setValue(12);
    type->setCurrentText(QStringLiteral("premium"));
    QApplication::processEvents();
    ASSERT_EQ(changes.size(), 3);
    EXPECT_EQ(changes[0].inProgress, 8);
    EXPECT_EQ(changes[0].pending, 4);
    EXPECT_EQ(changes[0].basketType, QStringLiteral("standard"));
    EXPECT_EQ(changes[1].inProgress, 8);
    EXPECT_EQ(changes[1].pending, 12);
    EXPECT_EQ(changes[1].basketType, QStringLiteral("standard"));
    EXPECT_EQ(changes[2].inProgress, 8);
    EXPECT_EQ(changes[2].pending, 12);
    EXPECT_EQ(changes[2].basketType, QStringLiteral("premium"));
    EXPECT_EQ(window.overviewSettings().inProgress, 8);
    EXPECT_EQ(window.overviewSettings().pending, 12);
    EXPECT_EQ(window.overviewSettings().basketType, QStringLiteral("premium"));

    inProgress->setValue(8);
    pending->setValue(12);
    type->setCurrentText(QStringLiteral("premium"));
    QApplication::processEvents();
    EXPECT_EQ(changes.size(), 3);
}

TEST(MainWindow, NotifiesWithUpdatedBasketSnapshots)
{
    basket::MainWindow window;
    std::vector<basket::BasketArray> changes;
    QObject::connect(&window, &basket::MainWindow::basketsChanged, &window,
                     [&](const basket::BasketArray& baskets) { changes.push_back(baskets); },
                     Qt::QueuedConnection);

    const auto tiles = window.findChildren<basket::BasketWidget*>();
    ASSERT_EQ(tiles.size(), 20);
    auto* status = tiles[6]->findChild<QComboBox*>("statusComboBox");
    ASSERT_NE(status, nullptr);

    status->setCurrentIndex(static_cast<int>(basket::BasketStatus::Pending));
    status->setCurrentIndex(static_cast<int>(basket::BasketStatus::Sold));
    QApplication::processEvents();
    ASSERT_EQ(changes.size(), 2);
    EXPECT_EQ(changes[0][6].index, 7);
    EXPECT_EQ(changes[0][6].sequence, 7);
    EXPECT_EQ(changes[0][6].status, basket::BasketStatus::Pending);
    EXPECT_EQ(changes[1][6].index, 7);
    EXPECT_EQ(changes[1][6].sequence, 7);
    EXPECT_EQ(changes[1][6].status, basket::BasketStatus::Sold);
    EXPECT_EQ(changes[1][0].index, 5);
    EXPECT_EQ(changes[1][0].sequence, 1);
    EXPECT_EQ(changes[1][0].status, basket::BasketStatus::Pending);
    EXPECT_EQ(window.baskets()[6].sequence, 7);
    EXPECT_EQ(window.baskets()[6].status, basket::BasketStatus::Sold);

    status->setCurrentIndex(static_cast<int>(basket::BasketStatus::Sold));
    QApplication::processEvents();
    EXPECT_EQ(changes.size(), 2);
}

TEST(BasketOverview, UpdatesTheEditedArrayPositionWithNonSequentialIndices)
{
    basket::BasketArray original{};
    original.back() = {42, 321, basket::BasketStatus::Sold};
    basket::BasketOverview overview(original);
    basket::BasketArray updated{};
    int notifications = 0;
    QObject::connect(&overview, &basket::BasketOverview::basketsChanged, &overview,
                     [&](const basket::BasketArray& baskets) {
                         updated = baskets;
                         ++notifications;
                     });

    const auto* grid = overview.findChild<QGridLayout*>("basketGrid");
    ASSERT_NE(grid, nullptr);
    const auto* item = grid->itemAtPosition(3, 0);
    ASSERT_NE(item, nullptr);
    auto* tile = item->widget();
    ASSERT_NE(tile, nullptr);
    auto* status = tile->findChild<QComboBox*>("statusComboBox");
    ASSERT_NE(status, nullptr);
    status->setCurrentIndex(static_cast<int>(basket::BasketStatus::Pending));

    ASSERT_EQ(notifications, 1);
    EXPECT_EQ(updated.back().index, 42);
    EXPECT_EQ(updated.back().sequence, 321);
    EXPECT_EQ(updated.back().status, basket::BasketStatus::Pending);
    EXPECT_EQ(updated.front().index, 0);
    EXPECT_EQ(updated.front().sequence, 0);
    EXPECT_EQ(original.back().status, basket::BasketStatus::Sold);
}

TEST(MainWindow, HostsTheBasketOverview)
{
    const basket::MainWindow window;

    ASSERT_NE(window.centralWidget(), nullptr);
    EXPECT_THAT(window.centralWidget()->objectName().toStdString(),
                ::testing::StrEq("basketOverview"));
    EXPECT_THAT(window.windowTitle().toStdString(), ::testing::StrEq("Basket UI"));
    EXPECT_FALSE(window.isVisible());
    EXPECT_EQ(window.findChildren<basket::BasketWidget*>().size(), 20);
}

TEST(BasketOverview, DisplaysSuppliedDetailsBySequence)
{
    basket::BasketArray baskets{};
    baskets.back() = {42, 321, basket::BasketStatus::Sold};
    const basket::BasketOverview overview(baskets);

    const auto* grid = overview.findChild<QGridLayout*>("basketGrid");
    ASSERT_NE(grid, nullptr);
    ASSERT_EQ(grid->count(), 20);
    const auto* lastItem = grid->itemAtPosition(3, 0);
    ASSERT_NE(lastItem, nullptr);
    const auto* last = qobject_cast<basket::BasketWidget*>(lastItem->widget());
    ASSERT_NE(last, nullptr);
    const auto* index = last->findChild<QLabel*>("indexLabel");
    const auto* status = last->findChild<QComboBox*>("statusComboBox");
    ASSERT_NE(index, nullptr);
    ASSERT_NE(status, nullptr);
    EXPECT_EQ(index->text(), QStringLiteral("42"));
    EXPECT_EQ(last->details().sequence, 321);
    EXPECT_EQ(status->currentText(), QStringLiteral("sold"));
}

TEST(BasketWidget, RendersProvidedDetails)
{
    const basket::BasketWidget widget({42, 123, basket::BasketStatus::Pending});
    const auto* index = widget.findChild<QLabel*>("indexLabel");
    EXPECT_EQ(widget.findChild<QSpinBox*>("sequenceSpinBox"), nullptr);
    const auto* status = widget.findChild<QComboBox*>("statusComboBox");
    ASSERT_NE(index, nullptr);
    ASSERT_NE(status, nullptr);

    EXPECT_EQ(index->text(), QStringLiteral("42"));
    EXPECT_EQ(widget.details().sequence, 123);
    EXPECT_EQ(status->currentText(), QStringLiteral("pending"));
    EXPECT_EQ(widget.property("basketState").toString(), QStringLiteral("pending"));
}

TEST(BasketWidget, RepaintsWhenStatusChanges)
{
    basket::BasketWidget widget({1, 1, basket::BasketStatus::Empty});
    auto* status = widget.findChild<QComboBox*>("statusComboBox");
    ASSERT_NE(status, nullptr);
    widget.resize(220, 160);
    widget.show();

    const struct {
        basket::BasketStatus status;
        const char* colour;
    } cases[] = {
        {basket::BasketStatus::Sold, "#ffd0d0"},
        {basket::BasketStatus::InProgress, "#b8edbe"},
        {basket::BasketStatus::Pending, "#fff0a6"},
        {basket::BasketStatus::Empty, "#f0f0f5"},
    };

    for (const auto& entry : cases) {
        SCOPED_TRACE(entry.colour);
        status->setCurrentIndex(static_cast<int>(entry.status));
        QApplication::processEvents();
        const auto rendered = widget.grab().toImage();
        EXPECT_EQ(rendered.pixelColor(20, rendered.height() - 20), QColor(entry.colour));
    }
}

TEST(MainWindow, CanBeShownAndClosed)
{
    basket::MainWindow window;

    window.show();
    QApplication::processEvents();
    ASSERT_TRUE(window.isVisible());

    EXPECT_TRUE(window.close());
    QApplication::processEvents();
    EXPECT_FALSE(window.isVisible());
}
