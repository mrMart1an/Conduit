# entity register test
entity_test = executable(
    'entity_test',
    'entity.cpp',
    dependencies : [engine_lib_test_dep, gtest_dep],

    cpp_pch : '../pch/testPch.h'
)
test('entity_test', entity_test)

# component test
component_test = executable(
    'component_test',
    'component.cpp',
    dependencies : [engine_lib_test_dep, gtest_dep],

    cpp_pch : '../pch/testPch.h'
)
test('component_test', component_test)

# world test
world_test = executable(
    'world_test',
    'world.cpp',
    dependencies : [engine_lib_test_dep, gtest_dep],

    cpp_pch : '../pch/testPch.h'
)
test('world_test', world_test)
