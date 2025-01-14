#################################################
#
#  (C) 2015-2019 Slávek Banko
#  slavek (DOT) banko (AT) axis.cz
#
#  Improvements and feedback are welcome
#
#  This file is released under GPL >= 2
#
#################################################


##### set project version ########################

include( TDEVersion )
cmake_minimum_required( VERSION ${TDE_CMAKE_MINIMUM_VERSION} )
tde_set_project_version( )


##### general package setup

project( akode )
set( PACKAGE akode )
set( PACKAGE_VERSION 2.0.2 )


##### include essential cmake modules

include( FindPkgConfig )
include( CheckCXXSourceCompiles )
include( CheckFunctionExists )
include( CheckSymbolExists )
include( CheckIncludeFile )
include( CheckLibraryExists )
include( CheckStructHasMember )


##### include our cmake modules

include( TDEMacros )


##### setup install paths

include( TDESetupPaths )
tde_setup_paths( )


##### optional stuff

option( WITH_ALL_OPTIONS    "Enable all optional support" OFF )
option( WITH_GCC_VISIBILITY "Enable fvisibility and fvisibility-inlines-hidden" ${WITH_ALL_OPTIONS} )

option( WITH_LIBLTDL        "Build with libltdl"                     OFF )
option( WITH_ALSA_SINK      "Build with alsa sink"                   ${WITH_ALL_OPTIONS} )
option( WITH_JACK_SINK      "Build with jack sink"                   ${WITH_ALL_OPTIONS} )
option( WITH_PULSE_SINK     "Build with pulseaudio sink"             ${WITH_ALL_OPTIONS} )
option( WITH_OSS_SINK       "Build with oss sink"                    ${WITH_ALL_OPTIONS} )
option( WITH_SUN_SINK       "Build with sun sink"                    OFF )
option( WITH_FFMPEG_DECODER "Build with ffmpeg decoder"              OFF )
option( WITH_MPC_DECODER    "Build with musepack decoder"            ${WITH_ALL_OPTIONS} )
option( WITH_MPEG_DECODER   "Build with mpeg decoder"                ${WITH_ALL_OPTIONS} )
option( WITH_SRC_RESAMPLER  "Build with src resampler"               ${WITH_ALL_OPTIONS} )
option( WITH_XIPH_DECODER   "Build with xiph decoder"                ${WITH_ALL_OPTIONS} )
option( WITH_XIPH_FLAC      "Build xiph decoder with flac support"   ON )
option( WITH_XIPH_SPEEX     "Build xiph decoder with speex support"  ON )
option( WITH_XIPH_VORBIS    "Build xiph decoder with vorbis support" ON )


##### configure checks

include( ConfigureChecks.cmake )


###### global compiler settings

add_definitions( -DHAVE_CONFIG_H )

set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined" )


##### source directories

add_subdirectory( ${PROJECT_NAME} )


##### write configure files

configure_file( config.h.cmake config.h @ONLY )
