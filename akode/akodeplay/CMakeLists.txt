#################################################
#
#  (C) 2015 Slávek Banko
#  slavek (DOT) banko (AT) axis.cz
#
#  Improvements and feedback are welcome
#
#  This file is released under GPL >= 2
#
#################################################


include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_BINARY_DIR}/akode/lib
  ${CMAKE_SOURCE_DIR}/akode/lib
)


##### akodeplay #################################

set( target akodeplay )

tde_add_executable(
  ${target}
  SOURCES ${target}.cpp
  LINK akode-shared
  DESTINATION ${BIN_INSTALL_DIR}
)

