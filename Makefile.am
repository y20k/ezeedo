## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src \
	data 

dist_doc_DATA = \
    README \
    COPYING \
    AUTHORS \
    LICENSE \
    ChangeLog \
    INSTALL \
    NEWS

# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(docdir)
