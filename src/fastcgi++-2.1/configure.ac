AC_INIT([fastcgi++], [2.1], [Eddie Carle <eddie@erctech.org>], [fastcgi++])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_HEADER(include/fastcgi++/config.h)
AC_CONFIG_FILES(fastcgi++.pc)
AM_INIT_AUTOMAKE(1.9 dist-bzip2 no-dist-gzip tar-ustar check-news gnu no-dependencies)
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LIBTOOL

BOOST_REQUIRE([1.35.0])
BOOST_BIND
BOOST_DATE_TIME
BOOST_FUNCTION
BOOST_SMART_PTR
BOOST_THREADS
BOOST_SYSTEM
BOOST_IOSTREAMS

pkgConfigLibs="-lfastcgipp $BOOST_THREAD_LIBS $BOOST_DATE_TIME_LIBS"

AC_ARG_WITH([asql], AS_HELP_STRING([--without-asql], [build without ASql]))

AS_IF([test "x$with_asql" != "xno"], [AC_CHECK_HEADER(mysql/mysql.h, hasMysql=yes)],[hasMysql=no])
if test "$hasMysql" = yes
then
	AM_CONDITIONAL(HAVE_MYSQL_H, true)
	pkgConfigLibs="$pkgConfigLibs -lmysqlclient"
else
	AM_CONDITIONAL(HAVE_MYSQL_H, false)
fi

AC_SUBST(pkgConfigLibs)

## Linux keeps its endian determination in endian.h
AC_CHECK_HEADER(endian.h,
				[AC_DEFINE(HAVE_ENDIAN_H, 1, [Using "endian.h"])],
				[])
## FreeBSD keeps its "endianess" in machine/endian.h
AC_CHECK_HEADER(machine/endian.h,
				[AC_DEFINE(HAVE_MACHINE_ENDIAN_H, 1,
					 [Using "machine/endian.h"])],
				[])
## Some other *NIXs keep its "endianess" in arpa/nameser_compat.h
AC_CHECK_HEADER(arpa/nameser_compat.h,
				[AC_DEFINE(HAVE_ARPA_ENDIAN_H, 1,
					 [Using "arpa/nameser_compat.h"])],
				[])

AC_OUTPUT([Makefile \
                   src/Makefile \
                   include/Makefile \
						 examples/Makefile])
