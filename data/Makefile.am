## Process this file with automake to produce Makefile.in


SUBDIRS = \
	icons

gsettings_SCHEMAS = org.y20k.ezeedo.gschema.xml

@GSETTINGS_RULES@

desktopdir = $(datadir)/applications
desktop_in_files = ezeedo.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)

UPDATE_DESKTOP = update-desktop-database $(datadir)/applications || :

install-data-hook: 
	$(UPDATE_DESKTOP)

uninstall-hook: 
	$(UPDATE_DESKTOP)

