cmake_minimum_required(VERSION 3.8)

project(STX_ETX_Parser C)

### Overwrite default install prefix.
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_LIST_DIR}/../artifacts" CACHE PATH "..." FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

add_subdirectory(src)

# add_subdirectory(docs)

add_subdirectory(unittests)