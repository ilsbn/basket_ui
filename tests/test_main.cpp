#include <QApplication>
#include <QByteArray>

#include <gmock/gmock.h>

int main(int argc, char* argv[])
{
    ::testing::InitGoogleMock(&argc, argv);

    if (!qEnvironmentVariableIsSet("QT_QPA_PLATFORM")) {
        qputenv("QT_QPA_PLATFORM", QByteArrayLiteral("minimal"));
    }

    QApplication application(argc, argv);
    return RUN_ALL_TESTS();
}
