#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([yummycacher], [0.1], [rosslagerwall@gmail.com])
AC_CONFIG_SRCDIR([src])
AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AM_PROG_CC_C_O

# Checks for libraries.
#AC_SEARCH_LIBS([gcry_md_open], [gcrypt])

# Checks for header files.
#AC_CHECK_HEADERS([arpa/inet.h fcntl.h netdb.h stdint.h stdlib.h string.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
#AC_C_INLINE
#AC_TYPE_OFF_T
#AC_TYPE_SIZE_T
#AC_TYPE_SSIZE_T
#AC_TYPE_UINT32_T
#AC_TYPE_UINT64_T

# Checks for library functions.
#AC_FUNC_MALLOC
#AC_FUNC_REALLOC
#AC_CHECK_FUNCS([memset select socket])

PKG_CHECK_MODULES([DEPS], [libcurl])
PKG_CHECK_MODULES([DEPS2], [glib-2.0])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
