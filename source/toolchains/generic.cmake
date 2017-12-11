set(CMAKE_SYSTEM_PROCESSOR  Generic)

set(CMAKE_C_COMPILER    ${COMPILER_PREFIX}gcc)
set(CMAKE_CXX_COMPILER  ${COMPILER_PREFIX}g++)
set(AS                  ${COMPILER_PREFIX}as)
set(AR                  ${COMPILER_PREFIX}ar)
set(OBJCOPY             ${COMPILER_PREFIX}objcopy)
set(OBJDUMP             ${COMPILER_PREFIX}objdump)
set(SIZE                ${COMPILER_PREFIX}size)

set(CMAKE_C_FLAGS   "-std=gnu99 -Wall -fdata-sections -ffunction-sections"  CACHE INTERNAL "c compiler flags")
set(CMAKE_CXX_FLAGS "-Wall -fdata-sections -ffunction-sections"             CACHE INTERNAL "cxx compiler flags")
set(CMAKE_ASM_FLAGS ""                                                      CACHE INTERNAL "asm compiler flags")
set(CMAKE_EXE_LINKER_FLAGS "-Wl,--gc-sections"                              CACHE INTERNAL "exe link flags")

SET(CMAKE_C_FLAGS_DEBUG "-O0 -g -ggdb3 -DDEBUG"     CACHE INTERNAL "c debug compiler flags")
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -ggdb3 -DDEBUG"   CACHE INTERNAL "cxx debug compiler flags")
SET(CMAKE_ASM_FLAGS_DEBUG "-g -ggdb3 -DDEBUG"       CACHE INTERNAL "asm debug compiler flags")

SET(CMAKE_C_FLAGS_RELEASE "-O2 -g -ggdb3"   CACHE INTERNAL "c release compiler flags")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -g -ggdb3" CACHE INTERNAL "cxx release compiler flags")
SET(CMAKE_ASM_FLAGS_RELEASE ""              CACHE INTERNAL "asm release compiler flags")
