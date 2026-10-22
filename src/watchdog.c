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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <time.h>
#ಅನ್ತರ್ಭಾವ "mosquitto.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_SYSTEMD
#  include <systemd/sd-daemon.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_SYSTEMD
ಸ್ಥಿರ time_t ಮುಂದಿನ_ping = 0;
ಸ್ಥಿರ time_t ping_sec = 0;
#ಆದರೆ_ಅಂತ್ಯ


ಶೂನ್ಯ watchdog__init(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_SYSTEMD
	ಅಕ್ಷರ *watchdog_usec = getenv("WATCHDOG_USEC");
	ಮುಂದಿನ_ping = mosquitto_ಸಮಯ();
	ping_sec = 0;

	ಆದರೆ(watchdog_usec){
		ಅಕ್ಷರ *endptr = ಖಾಲಿ;
		ದೀರ್ಘ usec = strtol(watchdog_usec, &endptr, 10);
		ಆದರೆ(watchdog_usec[0] != '\0' && endptr[0] == '\0' && usec > 0){
			ping_sec = (usec / 1000000) / 2;
		}
		ಮುಂದಿನ_ping = mosquitto_ಸಮಯ();
	}
#ಆದರೆ_ಅಂತ್ಯ
}


ಶೂನ್ಯ watchdog__check(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_SYSTEMD
	ಆದರೆ(ping_sec){
		time_t now = mosquitto_ಸಮಯ();
		ಆದರೆ(now > ಮುಂದಿನ_ping){
			sd_notify(0, "WATCHDOG=1");
			ಮುಂದಿನ_ping = now + ping_sec;
		}
	}
#ಆದರೆ_ಅಂತ್ಯ
}
