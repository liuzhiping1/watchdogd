/* CPU load average monitor
 *
 * Copyright (C) 2015  Christian Lockley <clockley1@gmail.com>
 * Copyright (C) 2015  Joachim Nilsson <troglobit@gmail.com>
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

#include "plugin.h"

static uev_t watcher;

/* Default: disabled -- recommended 0.8, 0.9 */
static double warning  = 0.0;
static double critical = 0.0;


static double num_cores(void)
{
	int num = sysconf(_SC_NPROCESSORS_ONLN);

	if (-1 == num)
		return (double)1.0; /* At least one core. */

	return (double)num;
}

static void cb(uev_t *w, void *arg, int events)
{
	double num = num_cores();
	double avg, load[3];

	memset(load, 0, sizeof(load));
	if (getloadavg(load, 3) == -1) {
		ERROR("Failed reading system loadavg");
		return;
	}

#ifdef SYSLOG_MARK
//	LOG("Load avg: %.2f, %.2f, %.2f (1, 5, 15 min) | Num CPU cores: %d",
//	    load[0], load[1], load[2], (int)num);
	LOG("Loadavg: %.2f, %.2f, %.2f (1, 5, 15 min)", load[0], load[1], load[2]);
#endif

	/* Compensate for number of CPU cores */
	load[0] /= num;
	load[1] /= num;
	load[2] /= num;
	avg = (load[0] + load[1]) / 2.0;

	DEBUG("Adjusted: %.2f, %.2f, %.2f (1, 5, 15 min), avg: %.2f (1 + 5), warning: %.2f, reboot: %.2f",
	      load[0], load[1], load[2], avg, warning, critical);

	if (avg > warning) {
		if (critical > 0.0 && avg > critical) {
			ERROR("System load too high, %.2f > %0.2f, rebooting system ...", avg, critical);
			if (script_exec("loadavg", 1, avg, warning, critical))
				wdt_forced_reboot(w->ctx, getpid(), wdt_plugin_label("loadavg"), WDOG_CPU_OVERLOAD);
			return;
		}

		WARN("System load average very high, %.2f > %0.2f!", avg, warning);
		script_exec("loadavg", 0, avg, warning, critical);
	}
}

/*
 * Every T seconds we check loadavg
 */
int loadavg_init(uev_ctx_t *ctx, int T)
{
	if (warning == 0.0 && critical == 0.0) {
		INFO("Load average monitor disabled.");
		return 1;
	}

	INFO("Load average monitor, period %d sec, warning: %.2f%%, reboot: %.2f%%",
	     T, warning * 100, critical * 100);

	return uev_timer_init(ctx, &watcher, cb, NULL, 1000, T * 1000);
}

/*
 * Parse '-a warning[,critical]' argument
 */
int loadavg_set(char *arg)
{
	return wdt_plugin_arg("Loadavg", arg, &warning, &critical);
}

/**
 * Local Variables:
 *  c-file-style: "linux"
 *  indent-tabs-mode: t
 * End:
 */
