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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <time.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_SYSTEMD
#  include <systemd/sd-daemon.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_SYSTEMD
ਸਥਿਰ time_t ਅਗਲਾ_ping = 0;
ਸਥਿਰ time_t ping_sec = 0;
#ਜੇ_ਅੰਤ


ਖਾਲੀ watchdog__init(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_SYSTEMD
	ਅੱਖਰ *watchdog_usec = getenv("WATCHDOG_USEC");
	ਅਗਲਾ_ping = mosquitto_ਸਮਾਂ();
	ping_sec = 0;

	ਜੇ(watchdog_usec){
		ਅੱਖਰ *endptr = ਨਿਰਾ;
		ਲੰਮਾ usec = strtol(watchdog_usec, &endptr, 10);
		ਜੇ(watchdog_usec[0] != '\0' && endptr[0] == '\0' && usec > 0){
			ping_sec = (usec / 1000000) / 2;
		}
		ਅਗਲਾ_ping = mosquitto_ਸਮਾਂ();
	}
#ਜੇ_ਅੰਤ
}


ਖਾਲੀ watchdog__check(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_SYSTEMD
	ਜੇ(ping_sec){
		time_t now = mosquitto_ਸਮਾਂ();
		ਜੇ(now > ਅਗਲਾ_ping){
			sd_notify(0, "WATCHDOG=1");
			ਅਗਲਾ_ping = now + ping_sec;
		}
	}
#ਜੇ_ਅੰਤ
}
