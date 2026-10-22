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

#அந்தர்பாவ "config.h"

#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <time.h>
#அந்தர்பாவ "mosquitto.h"

#ifdef WITH_SYSTEMD
#  include <systemd/sd-daemon.h>
#endif

#ifdef WITH_SYSTEMD
நிலையான time_t next_ping = 0;
நிலையான time_t ping_sec = 0;
#endif


வெறுமை watchdog__init(வெறுமை)
{
#ifdef WITH_SYSTEMD
	எழுத்து *watchdog_usec = getenv("WATCHDOG_USEC");
	next_ping = mosquitto_time();
	ping_sec = 0;

	எனில்(watchdog_usec){
		எழுத்து *endptr = வெற்று;
		long usec = strtol(watchdog_usec, &endptr, 10);
		எனில்(watchdog_usec[0] != '\0' && endptr[0] == '\0' && usec > 0){
			ping_sec = (usec / 1000000) / 2;
		}
		next_ping = mosquitto_time();
	}
#endif
}


வெறுமை watchdog__check(வெறுமை)
{
#ifdef WITH_SYSTEMD
	எனில்(ping_sec){
		time_t now = mosquitto_time();
		எனில்(now > next_ping){
			sd_notify(0, "WATCHDOG=1");
			next_ping = now + ping_sec;
		}
	}
#endif
}
