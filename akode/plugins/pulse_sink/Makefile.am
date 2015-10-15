INCLUDES = -I$(top_srcdir)/akode/lib -I$(top_builddir)/akode/lib $(PULSE_CFLAGS) $(all_includes)

lib_LTLIBRARIES	= libakode_pulse_sink.la

libakode_pulse_sink_la_SOURCES = pulse_sink.cpp
libakode_pulse_sink_la_LDFLAGS = -module -avoid-version -no-undefined $(PULSE_LDFLAGS)
libakode_pulse_sink_la_LIBADD = ../../lib/libakode.la $(PULSE_LIBADD)
