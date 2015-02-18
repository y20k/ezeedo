## Process this file with automake to produce Makefile.in

SUBDIRS = \
        icons

desktopdir = $(datadir)/applications
desktop_in_file = ezeedo.desktop.in
desktop_DATA = $(desktop_in_file:.desktop.in=.desktop)

UPDATE_DESKTOP = update-desktop-database $(datadir)/applications || :

install-data-hook:
        $(UPDATE_DESKTOP)

gsettings_SCHEMAS = org.y20k.ezeedo.gschema.xml
@GSETTINGS_RULES@

uninstall-hook:
        $(UPDATE_DESKTOP)
