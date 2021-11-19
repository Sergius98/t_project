project(
    'network_emul',
    'cpp',
    default_options : [
        'cpp_std=c++11'
    ]
)

subdir('MobileClient')

ex_source_files = [
    'main.cpp'
    ]

executable(
    'ne_client', 
    sources : [ex_source_files],
    dependencies : [MobileClient_lib_dep]
    )
