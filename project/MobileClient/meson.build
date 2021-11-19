
MobileClient_files = [
    'source/NetConfAgent.cpp',
    ]

MobileClient_include = include_directories('include')


libpcre = dependency('libpcre', required : true)
sysrepo = dependency('sysrepo-cpp', required : true)
libyang = dependency ('libyang-cpp', required : true)

MobileClient_dependencies = [libpcre, sysrepo, libyang]


MobileClient_lib = shared_library(
    'MobileClient',
    sources : [MobileClient_files],
    include_directories : [MobileClient_include],
    dependencies : [MobileClient_dependencies]
)

MobileClient_lib_dep = [
    declare_dependency(
        link_with : [MobileClient_lib],
        include_directories : [MobileClient_include]
    )
]
