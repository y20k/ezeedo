## Process this file with automake to produce Makefile.in
## Created by Anjuta

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(EZEEDO_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -std=c99\
	 -Wall\
	 -Werror\
	 -rdynamic\
	 -g

bin_PROGRAMS = ezeedo

ezeedo_SOURCES = \
	main.c \
	categorylist.c \
	categorylist.h \
	core.c \
	core.h \
	helpers.c \
	helpers.h \
	preferences.c \
	preferences.h \
	taskentry.c \
	taskentry.h \
	tasklist.c \
	tasklist.h \
	start.c \
	start.h	

ezeedo_LDFLAGS = \
	-Wl,--export-dynamic

ezeedo_LDADD = $(EZEEDO_LIBS)




