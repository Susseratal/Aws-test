include(CMakeFindDependencyMacro)

if (UNIX AND NOT APPLE AND NOT BYO_CRYPTO)
    find_dependency(s2n)
endif()

find_dependency(aws-c-common)
find_dependency(aws-c-cal)

if (BUILD_SHARED_LIBS)
    include(${CMAKE_CURRENT_LIST_DIR}/shared/aws-c-io-targets.cmake)
else()
    include(${CMAKE_CURRENT_LIST_DIR}/static/aws-c-io-targets.cmake)
endif()
