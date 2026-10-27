add_library(basket_ui_widgets STATIC
    basket_details.h
    basket_slots.cpp
    basket_slots.h
    basket_overview.cpp
    basket_overview.h
    basket_overview_settings.h
    basket_overview.ui
    basket_widget.cpp
    basket_widget.h
    basket_widget.ui
    basket_resources.qrc
    main_window.cpp
    main_window.h
)
set_target_properties(basket_ui_widgets PROPERTIES AUTORCC ON)
target_include_directories(basket_ui_widgets PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(basket_ui_widgets PUBLIC Qt6::Widgets)

qt_add_executable(basket_ui main.cpp)
target_link_libraries(basket_ui PRIVATE
    basket_ui_widgets
    Boost::program_options
    spdlog::spdlog
)
basket_deploy_qt_platform_plugins(basket_ui)

if(MSVC)
    target_compile_options(basket_ui_widgets PRIVATE /W4 /permissive-)
    target_compile_options(basket_ui PRIVATE /W4 /permissive-)
else()
    target_compile_options(basket_ui_widgets PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(basket_ui PRIVATE -Wall -Wextra -Wpedantic)
endif()
