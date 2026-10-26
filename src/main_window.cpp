#include "main_window.h"

namespace basket {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("Basket UI"));
    resize(1024, 768);
}

} // namespace basket
