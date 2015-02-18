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
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)

if( UNIX )
  set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden" )
  set( __KDE_HAVE_GCC_VISIBILITY 1 )
endif( UNIX )
configure_file( akode_export.h.cmake akode_export.h @ONLY )


##### library ###################################

set( target akode )

set( ${target}_SRCS
  bytebuffer.cpp
  audiobuffer.cpp
  pluginhandler.cpp
  decoderpluginhandler.cpp
  resamplerpluginhandler.cpp
  sinkpluginhandler.cpp
  encoderpluginhandler.cpp
  fast_resampler.cpp
  crossfader.cpp
  volumefilter.cpp
  localfile.cpp
  mmapfile.cpp
  wav_decoder.cpp
  auto_sink.cpp
  void_sink.cpp
  converter.cpp
  buffered_decoder.cpp
  player.cpp
  magic.cpp
)

tde_add_library(
  ${target} SHARED
  VERSION 2.0.0
  SOURCES ${${target}_SRCS}
  LINK ${CMAKE_THREAD_LIBS_INIT} ${AKODE_LIBDL} ${SEM_LIBRARIES}
  DESTINATION ${LIB_INSTALL_DIR}
)

set_property(
  TARGET ${target}-shared
  APPEND PROPERTY COMPILE_DEFINITIONS
  AKODE_SEARCHDIR="${LIB_INSTALL_DIR}"
)


##### headers ###################################

set( ${target}_INCLUDES
  ${CMAKE_CURRENT_BINARY_DIR}/akode_export.h
  akodelib.h
  decoder.h
  sink.h
  encoder.h
  audioconfiguration.h
  audioframe.h
  audiobuffer.h
  bytebuffer.h
  file.h
  localfile.h
  mmapfile.h
  pluginhandler.h
  crossfader.h
  volumefilter.h
  resampler.h
  fast_resampler.h
  buffered_decoder.h
  wav_decoder.h
  auto_sink.h
  void_sink.h
  player.h
  magic.h
  converter.h
  framedecoder.h
)

install(
  FILES ${${target}_INCLUDES}
  DESTINATION ${INCLUDE_INSTALL_DIR}/akode
)

