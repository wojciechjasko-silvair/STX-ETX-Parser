include(CTest)

## Define useful paths
set(CMOCK_GENERATION_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/cmock/lib/cmock.rb)
set(UNITY_GENERATION_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/unity/auto/generate_test_runner.rb)
set(CONFIG_FILE             ${CMAKE_CURRENT_LIST_DIR}/config.yml)
set(CMOCK_OUTPUT_DIR        ${CMAKE_CURRENT_BINARY_DIR}/mocks)
set(RUNNER_OUTPUT_DIR       ${CMAKE_CURRENT_BINARY_DIR}/runners)
set(UNITTEST_ARTEFACTS_DIR  test)


## Find ruby executable.
find_program(CMAKE_RUBY_EXECUTABLE ruby)
if(NOT CMAKE_RUBY_EXECUTABLE)
  message(FATAL_ERROR "Could not find 'ruby' command.")
endif(NOT CMAKE_RUBY_EXECUTABLE)


## Add custom target to force ctest to be more verbose.
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure)


## Create directories for mocks and test_runners
add_custom_target(cmock_output_dir
                  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMOCK_OUTPUT_DIR})
add_custom_target(runner_output_dir
                  COMMAND ${CMAKE_COMMAND} -E make_directory ${RUNNER_OUTPUT_DIR})
install(DIRECTORY   ${RUNNER_OUTPUT_DIR}
                    ${CMOCK_OUTPUT_DIR}
        DESTINATION ${UNITTEST_ARTEFACTS_DIR})


## Function generates mocks from given SRC(header file).
#  It sets variable (header name with "Mock" prefix) with path to generated c file in parent scope.
#  Warning: Mock generation fails in silence (please check CMOCK_OUTPUT_DIR for artifacts).
function(createCMock SRC)
  get_filename_component(SRC        ${SRC}  REALPATH)
  get_filename_component(FILE_NAME  ${SRC}  NAME_WE)
  set(OUTPUT Mock${FILE_NAME})

  add_custom_command(OUTPUT   ${CMOCK_OUTPUT_DIR}/${OUTPUT}.c ${CMOCK_OUTPUT_DIR}/${OUTPUT}.h
                     COMMAND  ${CMAKE_RUBY_EXECUTABLE} ${CMOCK_GENERATION_SCRIPT} -o${CONFIG_FILE} ${SRC}
                     DEPENDS  ${SRC} cmock_output_dir)

  set(${OUTPUT}  ${CMOCK_OUTPUT_DIR}/${OUTPUT}.c PARENT_SCOPE)
endfunction()


## Function creates test from given FILE_NAME(c file with test cases).
#  Target will have "test_" prefix.
function(createTest TARGET TEST_SRC)
  get_filename_component(FILE_NAME  ${TEST_SRC} NAME_WE)
  get_filename_component(SRC        ${TEST_SRC} REALPATH)

  set(RUNNER      ${RUNNER_OUTPUT_DIR}/${FILE_NAME}_Runner.c)
  set(OUTPUT_LOG  ${TARGET}.log)

  add_custom_command(OUTPUT   ${RUNNER}
                     COMMAND  ${CMAKE_RUBY_EXECUTABLE} ${UNITY_GENERATION_SCRIPT} ${CONFIG_FILE} ${SRC} ${RUNNER}
                     DEPENDS  ${SRC} runner_output_dir)
  add_executable(${TARGET} ${RUNNER}
                           ${TEST_SRC}
                           ${ARGN})

  target_link_libraries(${TARGET} unity)
  target_link_libraries(${TARGET} cmock)
  target_include_directories(${TARGET} PRIVATE ${CMOCK_OUTPUT_DIR})

  install(TARGETS     ${TARGET} DESTINATION ${UNITTEST_ARTEFACTS_DIR}/bin)
  add_dependencies(check ${TARGET})

  add_custom_command(TARGET ${TARGET}
                     POST_BUILD
                     COMMAND ${TARGET} | tee ${OUTPUT_LOG}
                     COMMENT "Running ${TARGET}" VERBATIM)

  add_test(${TARGET} ${TARGET})
  install(FILES       ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_LOG}
          DESTINATION ${UNITTEST_ARTEFACTS_DIR}/results)
endfunction()