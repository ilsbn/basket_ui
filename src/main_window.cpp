#include "main_window.h"

#include "basket_overview.h"

namespace basket {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("Basket UI"));
    setCentralWidget(new BasketOverview(this));
    resize(1120, 850);
}

} // namespace basket
