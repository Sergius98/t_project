project(
    'network_emul',
    'cpp',
    default_options : [
        'cpp_std=c++2a'
    ]
)

#add_global_arguments('-fprofile-arcs', language : 'cpp')
#add_global_arguments('-ftest-coverage', language : 'cpp')
#add_global_arguments('-fPIC', language : 'cpp')

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
