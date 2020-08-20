AC_INIT([xc], [1.0], [bug-automake@gnu.org])
AM_INIT_AUTOMAKE([-Wall, -Werror foreign])
AC_PROG_CC
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
	Makefile
	src/Makefile
])
AC_CONFIG_MACRO_DIR([m4])

AX_WITH_CURSES
case "$ax_cv_ncursesw" in "no")
	AC_MSG_ERROR([No ncurses with wide-char support found])
esac


AC_OUTPUT

