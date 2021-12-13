gtest_dep = [
    dependency('gtest', main : true),
    dependency('gmock'),
]

test_incl_dirs = include_directories('mock')
mobCli_test = executable(
    'mobCli_test', 
    sources : ['source/MobileClientTest.cpp'], 
    dependencies : [MobileClient_lib_dep, utils_lib_dep, gtest_dep], 
    include_directories : [test_incl_dirs]
)
test('mobCli_test', mobCli_test)