/*
Copyright (c) 2009-2020 Roger Light <roger@atchoo.org>

All rights reserved. This program and the accompanying materials
are made available under the terms of the Eclipse Public License 2.0
and Eclipse Distribution License v1.0 which accompany this distribution.

The Eclipse Public License is available at
   https://www.eclipse.org/legal/epl-2.0/
and the Eclipse Distribution License is available at
  http://www.eclipse.org/org/documents/edl-v10.php.

SPDX-License-Identifier: EPL-2.0 OR BSD-3-Clause

Contributors:
   Roger Light - initial implementation and documentation.
   Tatsuzo Osawa - Add epoll.
*/

#ضمن "config.h"

#ضمن <stdlib.h>
#ضمن <time.h>
#ضمن "mosquitto.h"

#اگر_تعریف_شده WITH_SYSTEMD
#  include <systemd/sd-daemon.h>
#پایان_اگر

#اگر_تعریف_شده WITH_SYSTEMD
ساكن time_t بعدی_ping = 0;
ساكن time_t ping_sec = 0;
#پایان_اگر


فراغ watchdog__init(فراغ)
{
#اگر_تعریف_شده WITH_SYSTEMD
	حرف *watchdog_usec = getenv("WATCHDOG_USEC");
	بعدی_ping = mosquitto_زمان();
	ping_sec = 0;

	إذا(watchdog_usec){
		حرف *endptr = عدم;
		دراز usec = strtol(watchdog_usec, &endptr, 10);
		إذا(watchdog_usec[0] != '\0' && endptr[0] == '\0' && usec > 0){
			ping_sec = (usec / 1000000) / 2;
		}
		بعدی_ping = mosquitto_زمان();
	}
#پایان_اگر
}


فراغ watchdog__check(فراغ)
{
#اگر_تعریف_شده WITH_SYSTEMD
	إذا(ping_sec){
		time_t now = mosquitto_زمان();
		إذا(now > بعدی_ping){
			sd_notify(0, "WATCHDOG=1");
			بعدی_ping = now + ping_sec;
		}
	}
#پایان_اگر
}
