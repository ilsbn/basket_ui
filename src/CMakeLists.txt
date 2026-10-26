add_library(basket_ui_widgets STATIC
    main_window.cpp
    main_window.h
)
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
