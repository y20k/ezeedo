## Process this file with automake to produce Makefile.in


SUBDIRS = \
	icons

desktopdir = $(datadir)/applications
desktop_in_files = ezeedo.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)

UPDATE_DESKTOP = update-desktop-database $(datadir)/applications || :

install-data-hook: 
	$(UPDATE_DESKTOP)

uninstall-hook: 
	$(UPDATE_DESKTOP)

settingsdir = /usr/share/glib-2.0/schemas
settings_in_files = org.y20k.ezeedo.gschema.xml.in
settings_DATA = $(settings_in_files:.xml.in=.xml)
@GSETTINGS_RULES@

