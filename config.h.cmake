
/* defined if you have libltdl library and header */
#cmakedefine HAVE_LIBLTDL

/* Define if your platform has posix_madvise */
#cmakedefine HAVE_POSIX_MADVISE

/* Define if your platform has posix_fadvise */
#cmakedefine HAVE_POSIX_FADVISE

/* Define if your platform has fadvise */
#cmakedefine HAVE_FADVISE

/* Define if your platform has madvise */
#cmakedefine HAVE_MADVISE

/* Define if madvise has no usefull prototype */
#cmakedefine NEED_MADVISE_PROTOTYPE

/* Define if your platform has getopt_long from glibc */
#cmakedefine HAVE_GNU_GETOPT

/* Define to 1 if you have the <stdint.h> header file. */
#cmakedefine HAVE_STDINT_H

/* Define to 1 if you have the <inttypes.h> header file. */
#cmakedefine HAVE_INTTYPES_H

/* Define to 1 if you have the <sys/types.h> header file. */
#cmakedefine HAVE_SYS_TYPES_H

/* Define to 1 if you have the <soundcard.h> header file. */
#cmakedefine HAVE_SOUNDCARD_H

/* Define to 1 if you have the <sys/soundcard.h> header file. */
#cmakedefine HAVE_SYS_SOUNDCARD_H

/* Define if you have libavcodec and libavformat from FFMPEG (required for WMA
   and RealAudio decoding) */
#cmakedefine HAVE_FFMPEG

/* Define if struct AVFrame have appropriate members */
#cmakedefine FFMPEG_AVFRAME_HAVE_PKT_SIZE 1
#cmakedefine FFMPEG_AVFRAME_HAVE_CHANNELS 1

/* Define if you have libFLAC 1.1.3 or newer */
#cmakedefine HAVE_LIBFLAC113

/* Define if you have libFLAC 1.1.1 or 1.1.2 */
#cmakedefine HAVE_LIBFLAC

/* Define if you have libOggFLAC (required for loading OggFLAC files) */
#cmakedefine HAVE_LIBOGGFLAC

/* Define if you have speex installed */
#cmakedefine HAVE_SPEEX

/* Define if you have libspeex 1.1.x installed */
#cmakedefine HAVE_SPEEX11

/* Define if you have one of the broken libspeex 1.1.x < 1.1.5 */
#cmakedefine BROKEN_SPEEX11

/* Define if you have ogg/vorbis installed */
#cmakedefine HAVE_OGG_VORBIS

