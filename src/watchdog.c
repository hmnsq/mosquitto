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

#که_تعریف_شوی WITH_SYSTEMD
#  include <systemd/sd-daemon.h>
#د_که_پای

#که_تعریف_شوی WITH_SYSTEMD
ساكن time_t بل_ping = 0;
ساكن time_t ping_sec = 0;
#د_که_پای


فراغ watchdog__init(فراغ)
{
#که_تعریف_شوی WITH_SYSTEMD
	حرف *watchdog_usec = getenv("WATCHDOG_USEC");
	بل_ping = mosquitto_وخت();
	ping_sec = 0;

	إذا(watchdog_usec){
		حرف *endptr = عدم;
		اوږد usec = strtol(watchdog_usec, &endptr, 10);
		إذا(watchdog_usec[0] != '\0' && endptr[0] == '\0' && usec > 0){
			ping_sec = (usec / 1000000) / 2;
		}
		بل_ping = mosquitto_وخت();
	}
#د_که_پای
}


فراغ watchdog__check(فراغ)
{
#که_تعریف_شوی WITH_SYSTEMD
	إذا(ping_sec){
		time_t now = mosquitto_وخت();
		إذا(now > بل_ping){
			sd_notify(0, "WATCHDOG=1");
			بل_ping = now + ping_sec;
		}
	}
#د_که_پای
}
