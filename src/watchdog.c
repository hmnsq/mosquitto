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

#अन्तर्भाव "config.h"

#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <time.h>
#अन्तर्भाव "mosquitto.h"

#यदि_परिभाषितम् WITH_SYSTEMD
#  include <systemd/sd-daemon.h>
#यदि_अन्तः

#यदि_परिभाषितम् WITH_SYSTEMD
स्थिर time_t अग्र_ping = 0;
स्थिर time_t ping_sec = 0;
#यदि_अन्तः


शून्य watchdog__init(शून्य)
{
#यदि_परिभाषितम् WITH_SYSTEMD
	वर्ण *watchdog_usec = getenv("WATCHDOG_USEC");
	अग्र_ping = mosquitto_time();
	ping_sec = 0;

	यदि(watchdog_usec){
		वर्ण *endptr = रिक्त;
		दीर्घ usec = strtol(watchdog_usec, &endptr, 10);
		यदि(watchdog_usec[0] != '\0' && endptr[0] == '\0' && usec > 0){
			ping_sec = (usec / 1000000) / 2;
		}
		अग्र_ping = mosquitto_time();
	}
#यदि_अन्तः
}


शून्य watchdog__check(शून्य)
{
#यदि_परिभाषितम् WITH_SYSTEMD
	यदि(ping_sec){
		time_t now = mosquitto_time();
		यदि(now > अग्र_ping){
			sd_notify(0, "WATCHDOG=1");
			अग्र_ping = now + ping_sec;
		}
	}
#यदि_अन्तः
}
