# Callback buffer test
callback_test = executable(
    'callback_test',
    'callbacks.cpp',
    dependencies : [engine_lib_test_dep, gtest_dep],

    cpp_pch : '../pch/testPch.h'
)
test('callback_test', callback_test)

# Event system test
event_system_test = executable(
    'event_system_test',
    'events.cpp',
    dependencies : [engine_lib_test_dep, gtest_dep],

    cpp_pch : '../pch/testPch.h'
)
test('event_system_test', event_system_test)
