#include "main_window.h"

#include <QApplication>

#include <boost/program_options.hpp>
#include <spdlog/spdlog.h>

#include <cstdlib>
#include <exception>
#include <iostream>

int main(int argc, char* argv[])
{
    namespace po = boost::program_options;

    try {
        po::options_description options("Allowed options");
        options.add_options()("help,h", "Show command-line help");

        po::variables_map arguments;
        po::store(po::parse_command_line(argc, argv, options), arguments);
        po::notify(arguments);

        if (arguments.count("help") != 0) {
            std::cout << options << '\n';
            return EXIT_SUCCESS;
        }

        QApplication application(argc, argv);
        QApplication::setApplicationName(QStringLiteral("Basket UI"));
        QApplication::setApplicationVersion(QStringLiteral("0.1.0"));

        spdlog::info("Starting Basket UI");

        basket::MainWindow window;
        window.show();

        return application.exec();
    } catch (const std::exception& error) {
        spdlog::error("{}", error.what());
        return EXIT_FAILURE;
    }
}
