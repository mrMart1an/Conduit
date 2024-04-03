gtest_dep = dependency(
    'gtest',
    fallback : ['gtest', 'gtest_dep'],
    required : false
)

# Engine dependency with private headers for testing
engine_lib_test_dep = declare_dependency(
    include_directories : [public_inc, private_inc],
    link_with : engine_lib
)

# If unity is not found don't compile the tests
if gtest_dep.found()
    subdir('events')
else
    warning('google test not found, test compilation aborted')
endif
