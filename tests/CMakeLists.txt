add_executable(basket_ui_tests
    test_main.cpp
    main_window_test.cpp
    basket_order_test.cpp
)
set_target_properties(basket_ui_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)
target_link_libraries(basket_ui_tests PRIVATE
    basket_ui_widgets
    # gmock includes GoogleTest on MSVC and links it transitively elsewhere.
    GTest::gmock
)
basket_deploy_qt_platform_plugins(basket_ui_tests)

include(GoogleTest)
gtest_discover_tests(basket_ui_tests
    DISCOVERY_MODE PRE_TEST
    PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=minimal"
        TIMEOUT 30
)

add_test(NAME basket_ui.help COMMAND basket_ui --help)
set_tests_properties(basket_ui.help PROPERTIES
    PASS_REGULAR_EXPRESSION "Allowed options"
    TIMEOUT 10
)

add_test(NAME basket_ui.invalid_option COMMAND basket_ui --unknown-option)
set_tests_properties(basket_ui.invalid_option PROPERTIES
    WILL_FAIL TRUE
    TIMEOUT 10
)
