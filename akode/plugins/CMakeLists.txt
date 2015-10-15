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


##### build #####################################

tde_conditional_add_subdirectory( WITH_ALSA_SINK alsa_sink )
tde_conditional_add_subdirectory( WITH_JACK_SINK jack_sink )
tde_conditional_add_subdirectory( WITH_OSS_SINK oss_sink )
tde_conditional_add_subdirectory( WITH_PULSE_SINK polyp_sink )
tde_conditional_add_subdirectory( WITH_SUN_SINK sun_sink )

tde_conditional_add_subdirectory( WITH_FFMPEG_DECODER ffmpeg_decoder )
tde_conditional_add_subdirectory( WITH_MPC_DECODER mpc_decoder )
tde_conditional_add_subdirectory( WITH_MPEG_DECODER mpeg_decoder )
tde_conditional_add_subdirectory( WITH_SRC_RESAMPLER src_resampler )
tde_conditional_add_subdirectory( WITH_XIPH_DECODER xiph_decoder )

