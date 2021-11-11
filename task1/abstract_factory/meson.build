#### task1

ex_source_files = [
    'main.cpp',
    'source/ConcreteFactory1.cpp', 
    'source/ConcreteFactory2.cpp',
    'source/ConcreteProductA1.cpp',
    'source/ConcreteProductA2.cpp',
    'source/ConcreteProductB1.cpp',
    'source/ConcreteProductB2.cpp']

incdir = include_directories('include', 'source')

executable('task1', sources : [ex_source_files], include_directories : incdir)


#### task 2

abstract_factory_files = [
    'source/ConcreteFactory1.cpp', 
    'source/ConcreteFactory2.cpp',
    'source/ConcreteProductA1.cpp',
    'source/ConcreteProductA2.cpp',
    'source/ConcreteProductB1.cpp',
    'source/ConcreteProductB2.cpp',]

abstract_factory_include = include_directories('include', 'source')

abstract_factory_lib = shared_library(
    'abstract_factory',
    sources : [abstract_factory_files],
    include_directories : [abstract_factory_include]
)

abstract_factory_lib_dep = declare_dependency(
    link_with : [abstract_factory_lib],
    include_directories : [abstract_factory_include]
)

ex_source_files = ['main.cpp']

executable(
    'task2', 
    sources : [ex_source_files], 
    dependencies : [abstract_factory_lib_dep], 
    include_directories : []
)


#### task 3

gtest_dep = [
    #dependency('gtest', main : true),
    dependency('gtest', version : '1.10.0'),
    dependency('gmock', version : '1.10.0'),
    dependency('gtest_main', version : '1.10.0'),
]

ex_test_files = ['test/ConcreteProductA1Test.cpp']

task3 = executable(
    'task3test', 
    sources : [ex_test_files], 
    dependencies : [abstract_factory_lib_dep, gtest_dep], 
    include_directories : []
)

test('task3', task3)
