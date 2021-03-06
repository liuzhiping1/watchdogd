/* Advanced watchdog daemon
 *
 * Copyright (C) 2008       Michele d'Amico <michele.damico@fitre.it>
 * Copyright (C) 2008       Mike Frysinger <vapier@gentoo.org>
 * Copyright (C) 2012-2016  Joachim Nilsson <troglobit@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef WDT_H_
#define WDT_H_

#include "config.h"
#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/reboot.h>
#include <linux/types.h>
#include <linux/watchdog.h>
#include <signal.h>
#include <paths.h>
#include <syslog.h>
#include <sched.h>

#include "lite/lite.h"
#include "uev/uev.h"

#include "private.h"
#include "wdog.h"

#define WDT_DEVNODE          _PATH_DEV      "watchdog"
#define WDT_TIMEOUT_DEFAULT  20
#define WDT_KICK_DEFAULT     (WDT_TIMEOUT_DEFAULT / 2)

#define WDT_REASON_PID "PID                 "
#define WDT_REASON_WID "Watchdog ID         "
#define WDT_REASON_LBL "Label               "
#define WDT_REASON_CSE "Reset cause         "
#define WDT_REASON_STR "Reset cause reason  "
#define WDT_REASON_CNT "Counter             "
#define WDT_REASON_WDT "Boot status (WDIOF) "
#define WDT_REASON_TMO "Timeout (sec)       "
#define WDT_REASON_INT "Kick interval       "

#define ERROR(fmt, args...)  syslog(LOG_ERR,     fmt, ##args)
#define PERROR(fmt, args...) syslog(LOG_ERR,     fmt ": %s", ##args, strerror(errno))
#define DEBUG(fmt, args...)  syslog(LOG_DEBUG,   fmt, ##args)
#define LOG(fmt, args...)    syslog(LOG_NOTICE,  fmt, ##args)
#define INFO(fmt, args...)   syslog(LOG_INFO,    fmt, ##args)
#define WARN(fmt, args...)   syslog(LOG_WARNING, fmt, ##args)

/* Global variables */
extern int   magic;
extern int   enabled;
extern int   loglevel;
extern int   period;
extern char *__progname;
#ifndef TESTMODE_DISABLED
extern int   __wdt_testmode;
#endif

int wdt_enable         (int enable);
int wdt_debug          (int enable);
int wdt_reset_cause    (wdog_reason_t *cause);
int wdt_clear_cause    (void);

int wdt_kick           (char *msg);
int wdt_set_timeout    (int count);
int wdt_get_timeout    (void);
int wdt_get_bootstatus (void);
int wdt_close          (uev_ctx_t *ctx);
int wdt_reboot         (uev_ctx_t *ctx, pid_t pid, wdog_reason_t *reason, int timeout);
int wdt_forced_reboot  (uev_ctx_t *ctx, pid_t pid, char *label, int timeout);

static inline int wdt_testmode(void)
{
#ifndef TESTMODE_DISABLED
	return __wdt_testmode;
#else
	return 0;
#endif
}

#ifdef HAVE_FINIT_FINIT_H
int     wdt_handover(int *exist);
#else
#define wdt_handover(exist) 0
#endif

#endif /* WDT_H_ */

/**
 * Local Variables:
 *  c-file-style: "linux"
 *  indent-tabs-mode: t
 * End:
 */
