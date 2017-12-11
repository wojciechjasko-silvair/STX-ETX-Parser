add_definitions(-DCMOCK_MEM_DYNAMIC)
add_definitions(-DUNITY_FLOAT_PRECISION=0.001f)
add_definitions(-DUNITY_FLOAT_VERBOSE)

# Create library with Unity
add_library(unity STATIC unity/src/unity.c)
target_include_directories(unity PUBLIC unity/src)

# Create library with CMock
add_library(cmock STATIC cmock/src/cmock.c)
target_include_directories(cmock PUBLIC unity/src cmock/src)