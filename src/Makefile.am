AM_CFLAGS           = -W -Wall -Wextra -Wno-unused-parameter -std=gnu99
bin_PROGRAMS        = watchdogctl
sbin_PROGRAMS       = watchdogd
dist_man8_MANS      = watchdogd.8
watchdogd_SOURCES   = watchdogd.c	wdt.h		\
		      plugin.c          plugin.h	\
		      script.c		script.h	\
		      pmon.c		pmon.h
if FINIT
watchdogd_SOURCES  += finit.c
endif
if FILENR_PLUGIN
watchdogd_SOURCES  += filenr.c		filenr.h
endif
if LOADAVG_PLUGIN
watchdogd_SOURCES  += loadavg.c		loadavg.h
endif
if MEMINFO_PLUGIN
watchdogd_SOURCES  += meminfo.c		meminfo.h
endif
if RCFILE
watchdogd_SOURCES  += rcfile.c
endif

watchdogd_CFLAGS    = $(uev_CFLAGS) $(lite_CFLAGS) $(AM_CFLAGS)
watchdogd_LDADD     = $(uev_LIBS)   $(lite_LIBS) libwdog.la

watchdogctl_SOURCES = watchdogctl.c
watchdogctl_LDADD   = libwdog.la

lib_LTLIBRARIES     = libwdog.la
pkgconfigdir        = $(libdir)/pkgconfig
pkgconfig_DATA      = libwdog.pc
pkgincludedir       = $(includedir)/wdog
pkginclude_HEADERS  =           wdog.h  compat.h
libwdog_la_SOURCES  = wdog.c	wdog.h  compat.h
libwdog_la_CFLAGS   = $(lite_CFLAGS) $(AM_CFLAGS)
libwdog_la_LDFLAGS  = -version-info 1:0:0

