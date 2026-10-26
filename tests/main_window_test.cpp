#include "main_window.h"

#include <QApplication>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

TEST(MainWindow, StartsWithAnEmptyContentArea)
{
    const basket::MainWindow window;

    EXPECT_EQ(window.centralWidget(), nullptr);
    EXPECT_THAT(window.windowTitle().toStdString(), ::testing::StrEq("Basket UI"));
    EXPECT_FALSE(window.isVisible());
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
