#ifndef __XC_H__
#define __XC_H__

#include <config.h>
#include <sys/queue.h>

#ifdef HAVE_NCURSES_H
#include <ncurses.h>
#elif defined(HAVE_NCURSESW_H)
#include <ncursesw.h>
#elif defined(HAVE_NCURSES_CURSES_H)
#include <ncurses/curses.h>
#endif

#endif
