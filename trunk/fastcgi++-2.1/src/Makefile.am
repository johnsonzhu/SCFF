## @(#) Makefile.am - Automake file for the FastCGI++ src directory
##
## $Id$
##

DISTCLEANFILES = Makefile.in Makefile

INCLUDES = -I$(top_srcdir)/include
AM_CPPFLAGS = $(BOOST_CPPFLAGS)
LDADD = $(BOOST_THREAD_LIBS)

lib_LTLIBRARIES = libfastcgipp.la

libfastcgipp_la_LDFLAGS = -version-info 2:0:0

libfastcgipp_la_SOURCES = \
	http.cpp \
	protocol.cpp \
	request.cpp \
	manager.cpp \
	transceiver.cpp \
	fcgistream.cpp \
	utf8_codecvt_facet.cpp

if HAVE_MYSQL_H
libfastcgipp_la_SOURCES += mysql.cpp
LDADD += -lmysqlclient
endif

