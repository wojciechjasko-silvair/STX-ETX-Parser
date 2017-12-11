list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/framework")
include(UnitTest)

add_subdirectory(framework)

set(TEST_PATH ./test)

createTest(test_STX_ETX ${TEST_PATH}/TC_STX_ETX.c)
target_link_libraries(test_STX_ETX STX_ETX)