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

#ifdef WITH_SYSTEMD
#  include <systemd/sd-daemon.h>
#endif

#ifdef WITH_SYSTEMD
स्थिर समय_प्रकार next_ping = 0;
स्थिर समय_प्रकार ping_sec = 0;
#endif


शून्य watchdog__init(शून्य)
{
#ifdef WITH_SYSTEMD
	वर्ण *watchdog_usec = दो_पर्या("WATCHDOG_USEC");
	next_ping = mosquitto_time();
	ping_sec = 0;

	यदि(watchdog_usec){
		वर्ण *endptr = रिक्त;
		दीर्घ usec = म_से_दीर्घ(watchdog_usec, &endptr, 10);
		यदि(watchdog_usec[0] != '\0' && endptr[0] == '\0' && usec > 0){
			ping_sec = (usec / 1000000) / 2;
		}
		next_ping = mosquitto_time();
	}
#endif
}


शून्य watchdog__check(शून्य)
{
#ifdef WITH_SYSTEMD
	यदि(ping_sec){
		समय_प्रकार now = mosquitto_time();
		यदि(now > next_ping){
			sd_notify(0, "WATCHDOG=1");
			next_ping = now + ping_sec;
		}
	}
#endif
}
