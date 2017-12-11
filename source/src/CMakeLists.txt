file(GLOB LIB_SRC "./*.c")
file(GLOB LIB_PUBLIC_HEADER "./*.h")

add_library(STX_ETX STATIC ${LIB_SRC})
target_include_directories(STX_ETX PUBLIC .)

set_target_properties(STX_ETX PROPERTIES PUBLIC_HEADER "${LIB_PUBLIC_HEADER}")

install(TARGETS STX_ETX ARCHIVE       DESTINATION lib
                        PUBLIC_HEADER DESTINATION include)