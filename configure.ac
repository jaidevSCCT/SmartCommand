#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([SmartCommand], [1.0], [jaidev.csit.etc@gmail.com])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([SmartCommand.c],[SmartFunc.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_HAVE_LIBRARY([pcsclite])
# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdint.h stdlib.h string.h strings.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_CHECK_FUNCS([bzero])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
