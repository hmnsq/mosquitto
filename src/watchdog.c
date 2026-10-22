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

#समावेश "config.h"

#समावेश <stdlib.h>
#समावेश <time.h>
#समावेश "mosquitto.h"

#अगर_घोषित WITH_SYSTEMD
#  include <systemd/sd-daemon.h>
#पूर्ण_अगर

#अगर_घोषित WITH_SYSTEMD
अटल समय_प्रकार अग्र_ping = 0;
अटल समय_प्रकार ping_sec = 0;
#पूर्ण_अगर


व्योम watchdog__init(व्योम)
{
#अगर_घोषित WITH_SYSTEMD
	अक्षर *watchdog_usec = दो_पर्या("WATCHDOG_USEC");
	अग्र_ping = mosquitto_time();
	ping_sec = 0;

	अगर(watchdog_usec){
		अक्षर *endptr = शून्य;
		दीर्घ usec = म_से_दीर्घ(watchdog_usec, &endptr, 10);
		अगर(watchdog_usec[0] != '\0' && endptr[0] == '\0' && usec > 0){
			ping_sec = (usec / 1000000) / 2;
		}
		अग्र_ping = mosquitto_time();
	}
#पूर्ण_अगर
}


व्योम watchdog__check(व्योम)
{
#अगर_घोषित WITH_SYSTEMD
	अगर(ping_sec){
		समय_प्रकार now = mosquitto_time();
		अगर(now > अग्र_ping){
			sd_notify(0, "WATCHDOG=1");
			अग्र_ping = now + ping_sec;
		}
	}
#पूर्ण_अगर
}
