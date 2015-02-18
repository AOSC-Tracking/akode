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

set( target akode_mppdec )

set( ${target}_SRCS
  bitstream.cpp
  huffsv46.cpp
  huffsv7.cpp
  idtag.cpp
  mpc_dec.cpp
  requant.cpp
  streaminfo.cpp
  synth_filter.cpp
)

tde_add_library(
  ${target} STATIC_PIC
  SOURCES ${${target}_SRCS}
)

