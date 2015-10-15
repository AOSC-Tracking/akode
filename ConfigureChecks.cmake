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


##### check for system libraries ################

if( WITH_LIBLTDL )
    # check libltdl
    check_include_file( "ltdl.h" HAVE_LTDL_H )
    if( HAVE_LTDL_H )
        set( AKODE_LIBDL ltdl )
        check_library_exists( ${AKODE_LIBDL} lt_dlopen "" HAVE_LIBLTDL )
    endif( HAVE_LTDL_H)
    if( NOT HAVE_LIBLTDL )
        tde_message_fatal( "libltdl are required, but not found on your system" )
    endif( NOT HAVE_LIBLTDL )

else( WITH_LIBLTDL )
    # check libdl
    set( AKODE_LIBDL dl )
    check_library_exists( ${AKODE_LIBDL} dlopen /lib HAVE_LIBDL )
    if( NOT HAVE_LIBDL )
        unset( AKODE_LIBDL )
        check_function_exists( dlopen HAVE_DLOPEN )
        if( NOT HAVE_DLOPEN )
            tde_message_fatal( "libdl are required, but not found on your system" )
        endif( NOT HAVE_DLOPEN )
    endif( NOT HAVE_LIBDL )
endif( WITH_LIBLTDL )


find_package( Threads )


check_include_file( "semaphore.h" HAVE_SEM )
check_library_exists( rt sem_init "" HAVE_LIBRT )
if( HAVE_LIBRT )
    set( SEM_LIBRARIES rt )
endif( HAVE_LIBRT )


check_library_exists( c posix_madvise "" HAVE_POSIX_MADVISE )
check_library_exists( c posix_fadvise "" HAVE_POSIX_FADVISE )
check_library_exists( c madvise "" HAVE_MADVISE )
check_library_exists( c fadvise "" HAVE_FADVISE )
check_cxx_source_compiles( "
    #include <sys/types.h>
    #include <sys/mman.h>
    int main() { ::madvise((char*)0,0, MADV_SEQUENTIAL); return 0; }"
    HAVE_MADVISE_PROTOTYPE )
if( NOT HAVE_MADVISE_PROTOTYPE )
    set( NEED_MADVISE_PROTOTYPE 1 )
endif( NOT HAVE_MADVISE_PROTOTYPE )


check_include_file( "getopt.h" HAVE_GETOPT_H )
check_library_exists( c getopt_long "" HAVE_GNU_GETOPT )
check_include_file( "stdint.h" HAVE_STDINT_H )
if( NOT HAVE_STDINT_H )
    check_include_file( "inttypes.h" HAVE_INTTYPES_H )
endif( NOT HAVE_STDINT_H )
check_include_file( "sys/types.h" HAVE_SYS_TYPES_H )


##### check alsa support ########################

if( WITH_ALSA_SINK )

    pkg_search_module( ALSA alsa>=0.90 )

    if( NOT ALSA_FOUND )
        tde_message_fatal( "alsa >= 0.90 are required, but not found on your system" )
    endif( NOT ALSA_FOUND )

endif( WITH_ALSA_SINK )


##### check jack support ########################

if( WITH_JACK_SINK )

    pkg_search_module( JACK jack>=0.90 )

    if( NOT JACK_FOUND )
        tde_message_fatal( "jack >= 0.90 are required, but not found on your system" )
    endif( NOT JACK_FOUND )

endif( WITH_JACK_SINK )


##### check oss support #########################

if( WITH_OSS_SINK )

    check_include_file( "soundcard.h" HAVE_SOUNDCARD_H )
    if( NOT HAVE_SOUNDCARD_H )
        check_include_file( "sys/soundcard.h" HAVE_SYS_SOUNDCARD_H )
        if( NOT HAVE_SYS_SOUNDCARD_H )
            tde_message_fatal( "soundcard.h are required, but not found on your system" )
        endif( NOT HAVE_SYS_SOUNDCARD_H )
    endif( NOT HAVE_SOUNDCARD_H )

    check_library_exists( ossaudio _oss_ioctl "" HAVE_OSSAUDIO )
    if( HAVE_OSSAUDIO )
        set( OSSAUDIO_LIBRARIES "-lossaudio" )
    endif( HAVE_OSSAUDIO )

endif( WITH_OSS_SINK )


##### check polyp support #######################

if( WITH_PULSE_SINK )

    pkg_search_module( PULSE libpulse-simple>=0.9.2 )

    if( NOT PULSE_FOUND )
        tde_message_fatal( "libpulse-simple >= 0.9.2 are required, but not found on your system" )
    endif( NOT PULSE_FOUND )

endif( WITH_PULSE_SINK )


##### check sun support #########################

if( WITH_SUN_SINK )

    check_include_file( "sys/audioio.h" HAVE_AUDIOIO_H )
    if( NOT HAVE_AUDIOIO_H )
        tde_message_fatal( "sun audioio are required, but not found on your system" )
    endif( NOT HAVE_AUDIOIO_H )

endif( WITH_SUN_SINK )


##### check ffmpeg support ######################

if( WITH_FFMPEG_DECODER )

    pkg_search_module( AVFORMAT libavformat>=50 )
    if( NOT AVFORMAT_FOUND )
        tde_message_fatal( "libavformat >= 50 are required, but not found on your system" )
    endif( NOT AVFORMAT_FOUND )

    pkg_search_module( AVCODEC libavcodec>=50 )
    if( NOT AVCODEC_FOUND )
        tde_message_fatal( "libavcodec >= 50 are required, but not found on your system" )
    endif( NOT AVCODEC_FOUND )

    set( HAVE_FFMPEG 1 )

endif( WITH_FFMPEG_DECODER )


##### check mad support #########################

if( WITH_MPEG_DECODER )

    pkg_search_module( MAD mad )

    if( NOT MAD_FOUND )
        find_library( MAD_LIBRARIES NAMES mad )
        find_path( MAD_INCLUDE_DIRS mad.h )
        if( NOT MAD_LIBRARIES )
            tde_message_fatal( "mad are required, but not found on your system" )
        endif( NOT MAD_LIBRARIES )
    endif( NOT MAD_FOUND )

endif( WITH_MPEG_DECODER )


##### check FLAC support ########################

if( WITH_XIPH_DECODER )

    # check for FLAC module
    pkg_search_module( FLAC flac>=1.1.3 )
    if( FLAC_FOUND )
        set( HAVE_LIBFLAC113 1 )
    else( FLAC_FOUND )
        # check for FLAC 1.1.3
        check_include_file( "FLAC/metadata.h" HAVE_FLAC113_H )
        if( HAVE_FLAC113_H )
            tde_save_and_set( CMAKE_REQUIRED_LIBRARIES ogg )
            check_library_exists( FLAC FLAC__stream_decoder_seek_absolute "" HAVE_LIBFLAC113 )
            tde_restore( CMAKE_REQUIRED_LIBRARIES )
            if( HAVE_LIBFLAC113 )
                set( FLAC_LIBRARIES "-lFLAC -logg" )
            endif( HAVE_LIBFLAC113 )
        endif( HAVE_FLAC113_H )

        # check for FLAC 1.1.1 or 1.1.2
        if( NOT HAVE_LIBFLAC113 )
            check_include_file( "FLAC/seekable_stream_decoder.h" HAVE_FLAC_H )
            if( HAVE_FLAC_H )
                check_library_exists( FLAC FLAC__seekable_stream_decoder_process_single "" HAVE_LIBFLAC )
                if( HAVE_LIBFLAC )
                    set( FLAC_LIBRARIES "-lFLAC" )
                endif( HAVE_LIBFLAC )
            endif( HAVE_FLAC_H )

            check_include_file( "OggFLAC/seekable_stream_decoder.h" HAVE_OGGFLAC_H )
            if( HAVE_OGGFLAC_H )
                tde_save_and_set( CMAKE_REQUIRED_LIBRARIES m OggFLAC FLAC )
                check_library_exists( OggFLAC OggFLAC__seekable_stream_decoder_process_single "" HAVE_LIBOGGFLAC )
                tde_restore( CMAKE_REQUIRED_LIBRARIES )
                if( HAVE_LIBOGGFLAC )
                    set( OGGFLAC_LIBRARIES "-lOggFLAC" )
                endif( HAVE_LIBOGGFLAC )
            endif( HAVE_OGGFLAC_H )
        endif( NOT HAVE_LIBFLAC113 )
    endif( FLAC_FOUND )

    if( NOT FLAC_LIBRARIES )
        tde_message_fatal( "FLAC >= 1.1.1 are required, but not found on your system" )
    endif( NOT FLAC_LIBRARIES )

endif( WITH_XIPH_DECODER )


##### check speex support #######################

if( WITH_XIPH_DECODER )

    # check for speex module
    pkg_search_module( SPEEX speex>=1.2 )
    if( NOT SPEEX_FOUND )
        # check for speex >= 1.1
        pkg_search_module( SPEEX speex>=1.1 )
        if( SPEEX_FOUND )
            set( HAVE_SPEEX11 1 )
            check_library_exitss( speex speex_decode_int "" HAVE_SPEEX_DECODE_INT )
            if( NOT HAVE_SPEEX_DECODE_INT )
                set( BROKEN_SPEEX11 1 )
            endif( )
        else( )
            pkg_search_module( SPEEX speex )
        endif( )

    endif( )

    if( SPEEX_FOUND )
        set( HAVE_SPEEX 1 )
        if( NOT EXISTS ${SPEEX_INCLUDEDIR}/speex.h )
            find_path( SPEEX_EXTRA_INCLUDEDIR "speex.h" ${SPEEX_INCLUDEDIR}/speex )
            if( NOT SPEEX_EXTRA_INCLUDEDIR )
                tde_message_fatal( "speex are required, but header not found on your system" )
            endif( NOT SPEEX_EXTRA_INCLUDEDIR )
            list( APPEND SPEEX_INCLUDE_DIRS "${SPEEX_EXTRA_INCLUDEDIR}" )
        endif( NOT EXISTS ${SPEEX_INCLUDEDIR}/speex.h )
    else( SPEEX_FOUND )
        tde_message_fatal( "speex are required, but not found on your system" )
    endif( SPEEX_FOUND )

endif( WITH_XIPH_DECODER )


##### check ogg/vorbis support ##################

if( WITH_XIPH_DECODER )

    pkg_search_module( OGG ogg )
    if( NOT OGG_FOUND )
        tde_message_fatal( "ogg are required, but not found on your system" )
    endif( NOT OGG_FOUND )

    pkg_search_module( VORBIS vorbis )
    if( NOT VORBIS_FOUND )
        tde_message_fatal( "ogg/vorbis are required, but not found on your system" )
    endif( NOT VORBIS_FOUND )

    pkg_search_module( VORBISFILE vorbisfile )
    if( NOT VORBISFILE_FOUND )
        tde_message_fatal( "ogg/vorbisfile are required, but not found on your system" )
    endif( NOT VORBISFILE_FOUND )

    set( HAVE_OGG_VORBIS 1 )

endif( WITH_XIPH_DECODER )


##### check samplerate support ##################

if( WITH_SRC_RESAMPLER )

    pkg_search_module( SAMPLERATE samplerate )

    if( NOT SAMPLERATE_FOUND )
        find_library( SAMPLERATE_LIBRARIES NAMES samplerate )
        find_path( SAMPLERATE_INCLUDE_DIRS samplerate.h )
        if( NOT SAMPLERATE_LIBRARIES )
            tde_message_fatal( "samplerate are required, but not found on your system" )
        endif( NOT SAMPLERATE_LIBRARIES )
    endif( NOT SAMPLERATE_FOUND )

endif( WITH_SRC_RESAMPLER )

