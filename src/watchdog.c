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

#అన్తర్భావ "config.h"

#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <time.h>
#అన్తర్భావ "mosquitto.h"

#నిర్వచితమైతే WITH_SYSTEMD
#  include <systemd/sd-daemon.h>
#అయితే_ముగింపు

#నిర్వచితమైతే WITH_SYSTEMD
స్థిర time_t తదుపరి_ping = 0;
స్థిర time_t ping_sec = 0;
#అయితే_ముగింపు


శూన్య watchdog__init(శూన్య)
{
#నిర్వచితమైతే WITH_SYSTEMD
	అక్షరం *watchdog_usec = getenv("WATCHDOG_USEC");
	తదుపరి_ping = mosquitto_సమయం();
	ping_sec = 0;

	ఒకవేళ(watchdog_usec){
		అక్షరం *endptr = ఖాళీ;
		దీర్ఘ usec = strtol(watchdog_usec, &endptr, 10);
		ఒకవేళ(watchdog_usec[0] != '\0' && endptr[0] == '\0' && usec > 0){
			ping_sec = (usec / 1000000) / 2;
		}
		తదుపరి_ping = mosquitto_సమయం();
	}
#అయితే_ముగింపు
}


శూన్య watchdog__check(శూన్య)
{
#నిర్వచితమైతే WITH_SYSTEMD
	ఒకవేళ(ping_sec){
		time_t now = mosquitto_సమయం();
		ఒకవేళ(now > తదుపరి_ping){
			sd_notify(0, "WATCHDOG=1");
			తదుపరి_ping = now + ping_sec;
		}
	}
#అయితే_ముగింపు
}
