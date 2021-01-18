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

cmake_minimum_required( VERSION 2.8.12 )


##### general package setup

project( akode )
set( PACKAGE akode )
set( VERSION 2.0.2 )


##### include essential cmake modules

include( FindPkgConfig )
include( CheckCXXSourceCompiles )
include( CheckFunctionExists )
include( CheckSymbolExists )
include( CheckIncludeFile )
include( CheckLibraryExists )


##### include our cmake modules

set( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" )
include( TDEMacros )


##### setup install paths

include( TDESetupPaths )
tde_setup_paths( )


##### optional stuff

option( WITH_ALL_OPTIONS    "Enable all optional support" OFF )
option( WITH_GCC_VISIBILITY "Enable fvisibility and fvisibility-inlines-hidden" ${WITH_ALL_OPTIONS} )

OPTION( WITH_LIBLTDL        "Build with libltdl"         OFF )
OPTION( WITH_ALSA_SINK      "Build with alsa sink"       ${WITH_ALL_OPTIONS} )
OPTION( WITH_JACK_SINK      "Build with jack sink"       ${WITH_ALL_OPTIONS} )
OPTION( WITH_PULSE_SINK     "Build with pulseaudio sink" ${WITH_ALL_OPTIONS} )
OPTION( WITH_OSS_SINK       "Build with oss sink"        ${WITH_ALL_OPTIONS} )
OPTION( WITH_SUN_SINK       "Build with sun sink"        OFF )
OPTION( WITH_FFMPEG_DECODER "Build with ffmpeg decoder"  OFF )
option( WITH_MPC_DECODER    "Build with mpc decoder"     ${WITH_ALL_OPTIONS} )
option( WITH_MPEG_DECODER   "Build with mpeg decoder"    ${WITH_ALL_OPTIONS} )
OPTION( WITH_SRC_RESAMPLER  "Build with src resampler"   ${WITH_ALL_OPTIONS} )
option( WITH_XIPH_DECODER   "Build with xiph decoder"    ${WITH_ALL_OPTIONS} )


##### configure checks

include( ConfigureChecks.cmake )


###### global compiler settings

add_definitions( -DHAVE_CONFIG_H )

set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined" )


##### source directories

add_subdirectory( ${PROJECT_NAME} )


##### write configure files

configure_file( config.h.cmake config.h @ONLY )
