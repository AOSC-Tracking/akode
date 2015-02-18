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
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${POLYP_INCLUDE_DIRS}
)


##### library ###################################

set( target libakode_polyp_sink )

tde_add_library(
  ${target} MODULE
  SOURCES polyp_sink.cpp
  LINK akode-shared ${POLYP_LIBRARIES}
  DESTINATION ${LIB_INSTALL_DIR}
)

