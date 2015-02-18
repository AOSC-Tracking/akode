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
  ${SPEEX_INCLUDE_DIRS}
)


##### library ###################################

set( target libakode_xiph_decoder )

set( ${target}_SRCS
  flac_decoder.cpp
  flac113_decoder.cpp
  speex_decoder.cpp
  vorbis_decoder.cpp
  xiph_decoder.cpp
)

tde_add_library(
  ${target} MODULE
  SOURCES ${${target}_SRCS}
  LINK
    akode-shared
    ${FLAC_LIBRARIES} ${OGGFLAC_LIBRARIES}
    ${VORBIS_LIBRARIES} ${VORBISFILE_LIBRARIES}
    ${SPEEX_LIBRARIES}
  DESTINATION ${LIB_INSTALL_DIR}
)

