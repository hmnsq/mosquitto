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

#جيڪڏهن_وصف_ٿيل WITH_SYSTEMD
#  include <systemd/sd-daemon.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل WITH_SYSTEMD
ساكن time_t ايندڙ_ping = 0;
ساكن time_t ping_sec = 0;
#جيڪڏهن_پورو


فراغ watchdog__init(فراغ)
{
#جيڪڏهن_وصف_ٿيل WITH_SYSTEMD
	حرف *watchdog_usec = getenv("WATCHDOG_USEC");
	ايندڙ_ping = mosquitto_وقت();
	ping_sec = 0;

	إذا(watchdog_usec){
		حرف *endptr = عدم;
		ڊگهو usec = strtol(watchdog_usec, &endptr, 10);
		إذا(watchdog_usec[0] != '\0' && endptr[0] == '\0' && usec > 0){
			ping_sec = (usec / 1000000) / 2;
		}
		ايندڙ_ping = mosquitto_وقت();
	}
#جيڪڏهن_پورو
}


فراغ watchdog__check(فراغ)
{
#جيڪڏهن_وصف_ٿيل WITH_SYSTEMD
	إذا(ping_sec){
		time_t now = mosquitto_وقت();
		إذا(now > ايندڙ_ping){
			sd_notify(0, "WATCHDOG=1");
			ايندڙ_ping = now + ping_sec;
		}
	}
#جيڪڏهن_پورو
}
