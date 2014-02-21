## @(#) Makefile.am - Automake file for the FastCGI++ interface files
##
## $Id$
##

DISTCLEANFILES = Makefile Makefile.in

nobase_include_HEADERS =  \
	./fastcgi++/manager.hpp \
	./fastcgi++/http.hpp \
	./fastcgi++/exceptions.hpp \
	./fastcgi++/protocol.hpp \
	./fastcgi++/fcgistream.hpp \
	./fastcgi++/transceiver.hpp \
	./fastcgi++/message.hpp \
	./fastcgi++/request.hpp

if HAVE_MYSQL_H
nobase_include_HEADERS += ./asql/asql.hpp \
								  ./asql/data.hpp \
								  ./asql/query.hpp \
								  ./asql/exception.hpp
endif

if HAVE_MYSQL_H
nobase_include_HEADERS += ./asql/mysql.hpp
endif

nobase_nodist_include_HEADERS = ./fastcgi++/config.h

noinst_HEADERS = \
	./utf8_codecvt.hpp \
	./utf8_codecvt_facet.hpp \
	./boost/iostreams/code_converter.hpp
