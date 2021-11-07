#include <xc.h>
#include <xc_timer.h>
#include <xc_win.h>
#include <signal.h>
#include <sys/time.h>

#define CLOCKID CLOCK_MONOTONIC


static bool cursor;


static void xc_timer_handler(int sig, siginfo_t *si, void *uc)
{
        cursor = !cursor;
        wrefresh(stdscr);
        mvwaddstr(stdscr, LINES-2, COLS-2, cursor ? "*" : " ");
        win_redraw_list();
}


int xc_timer_init(void)
{
        struct sigaction sa;
        sigset_t mask;
        struct sigevent sev;
        timer_t timerid;
        struct itimerval its;

        sa.sa_flags = 0;
        sa.sa_sigaction = xc_timer_handler;
        sigemptyset(&sa.sa_mask);
        if (sigaction(SIGALRM, &sa, NULL) == -1) {
                return -1;
        }

/*
        sigemptyset(&mask);
        sigaddset(&mask, SIGRTMIN);
        if (sigprocmask(SIG_SETMASK, &mask, NULL) == -1) {
                return -1;
        }
        sev.sigev_notify = SIGEV_SIGNAL;
        sev.sigev_signo = SIGRTMIN;
        sev.sigev_value.sival_ptr = &timerid;
        if (timer_create(CLOCKID, &sev, &timerid) == -1) {
                return -1;
        }
*/

        its.it_value.tv_sec = 0;
        its.it_value.tv_usec = XC_SYS_TICK * 1000;
        its.it_interval.tv_sec = its.it_value.tv_sec;
        its.it_interval.tv_usec = its.it_value.tv_usec;
        setitimer(ITIMER_REAL, &its, NULL);
/*
        if (timer_settime(timerid, 0, &its, NULL) == -1) {
                return -1;
        }

        if (sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1) {
                return -1;
        }
*/
        return 0;
}

