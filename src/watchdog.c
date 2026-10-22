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

#شامل "config.h"

#شامل <stdlib.h>
#شامل <time.h>
#شامل "mosquitto.h"

#اگر_تعریف_شدہ WITH_SYSTEMD
#  include <systemd/sd-daemon.h>
#اگر_ختم

#اگر_تعریف_شدہ WITH_SYSTEMD
ساکن time_t اگلا_ping = 0;
ساکن time_t ping_sec = 0;
#اگر_ختم


خالی watchdog__init(خالی)
{
#اگر_تعریف_شدہ WITH_SYSTEMD
	حرف *watchdog_usec = getenv("WATCHDOG_USEC");
	اگلا_ping = mosquitto_وقت();
	ping_sec = 0;

	اگر(watchdog_usec){
		حرف *endptr = تہی;
		طویل usec = strtol(watchdog_usec, &endptr, 10);
		اگر(watchdog_usec[0] != '\0' && endptr[0] == '\0' && usec > 0){
			ping_sec = (usec / 1000000) / 2;
		}
		اگلا_ping = mosquitto_وقت();
	}
#اگر_ختم
}


خالی watchdog__check(خالی)
{
#اگر_تعریف_شدہ WITH_SYSTEMD
	اگر(ping_sec){
		time_t now = mosquitto_وقت();
		اگر(now > اگلا_ping){
			sd_notify(0, "WATCHDOG=1");
			اگلا_ping = now + ping_sec;
		}
	}
#اگر_ختم
}
