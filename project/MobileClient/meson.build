
MobileClient_files = [
    'source/NetConfAgent.cpp',]

MobileClient_include = include_directories('include')

MobileClient_lib = shared_library(
    'MobileClient',
    sources : [MobileClient_files],
    include_directories : [MobileClient_include]
)

MobileClient_lib_dep = declare_dependency(
    link_with : [MobileClient_lib],
    include_directories : [MobileClient_include]
)
