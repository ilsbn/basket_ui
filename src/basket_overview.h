#pragma once

#include <QWidget>

#include <memory>

namespace Ui {
class BasketOverview;
}

namespace basket {

class BasketOverview final : public QWidget
{
    Q_OBJECT

public:
    explicit BasketOverview(QWidget* parent = nullptr);
    ~BasketOverview() override;

private:
    std::unique_ptr<Ui::BasketOverview> ui_;
};

} // namespace basket
