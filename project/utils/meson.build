
utils_files = [
    'source/PrintInterface.cpp',
    'source/StringInterface.cpp'
    ]

utils_include = include_directories('hpp')

utils_lib = shared_library(
    'utils',
    sources : [utils_files],
    include_directories : [utils_include],
    dependencies : []
)

utils_lib_dep = [
    declare_dependency(
        link_with : [utils_lib],
        include_directories : [utils_include]
    )
]
