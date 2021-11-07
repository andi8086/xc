#include <xc.h>
#include <xc_timer.h>
#include <xc_win.h>
#include <signal.h>
#include <sys/time.h>

#define CLOCKID CLOCK_MONOTONIC


bool cursor;
uint64_t timer_counter;


static void xc_timer_handler(int sig, siginfo_t *si, void *uc)
{
        extern struct winlist_entry *cmdline;

        cursor = !cursor;
        __atomic_add_fetch(&timer_counter, 1, __ATOMIC_ACQ_REL);
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

        its.it_value.tv_sec = 0;
        its.it_value.tv_usec = XC_SYS_TICK * 1000;
        its.it_interval.tv_sec = its.it_value.tv_sec;
        its.it_interval.tv_usec = its.it_value.tv_usec;
        setitimer(ITIMER_REAL, &its, NULL);

        return 0;
}

