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
)


##### library ###################################

set( target libakode_mpeg_decoder )

tde_add_library(
  ${target} MODULE
  SOURCES mpeg_decoder.cpp
  LINK akode-shared ${MAD_LIBRARIES}
  DESTINATION ${LIB_INSTALL_DIR}
)

