project(
    'network_emul',
    'cpp',
    default_options : [
        'cpp_std=c++2a'
    ]
)

subdir('utils')
subdir('MobileClient')
subdir('test')

ex_source_files = [
    'main.cpp'
]

executable(
    'ne_client', 
    sources : [ex_source_files],
    dependencies : [MobileClient_lib_dep, utils_lib_dep]
)
