/*
Copyright (c) 2016-2021 Roger Light <roger@atchoo.org>

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
   Dmitry Kaukov - windows named events implementation.
*/
#यदि_परिभाषितम् WIN32
#  ifndef WIN32_LEAN_AND_माध्यम्
#    define WIN32_LEAN_AND_माध्यम्
#  endif
#  include <windows.h>
#यदि_अन्तः

#अन्तर्भाव "config.h"

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <stdbool.h>
#अन्तर्भाव <signal.h>

#अन्तर्भाव "mosquitto_broker_internal.h"

बाह्य पूर्णाङ्क g_सततम्;

स्थिर bool flag_reload = false;
स्थिर bool flag_log_rotate = false;
#यदि_परिभाषितम् WITH_PERSISTENCE
स्थिर bool flag_db_backup = false;
#यदि_अन्तः
स्थिर bool flag_वृक्षः_print = false;
स्थिर bool flag_xtreport = false;


स्थिर शून्य handle_signal(पूर्णाङ्क सङ्केत)
{
	अप्रयुक्तम्(सङ्केत);

	यदि(सङ्केत == SIGINT || सङ्केत == SIGTERM){
		g_सततम् = 0;
#यदि_परिभाषितम् SIGHUP
	}अन्यथा यदि(सङ्केत == SIGHUP){
		flag_reload = true;
#यदि_अन्तः
#यदि_परिभाषितम् SIGUSR1
	}अन्यथा यदि(सङ्केत == SIGUSR1){
#यदि_परिभाषितम् WITH_PERSISTENCE
		flag_db_backup = true;
#यदि_अन्तः
#यदि_अन्तः
#यदि_परिभाषितम् SIGUSR2
	}अन्यथा यदि(सङ्केत == SIGUSR2){
		flag_वृक्षः_print = true;
		flag_xtreport = true;
#यदि_अन्तः
#यदि_परिभाषितम् SIGRTMIN
	}अन्यथा यदि(सङ्केत == SIGRTMIN){
		flag_log_rotate = true;
#यदि_अन्तः
	}
}


शून्य signal_सज्जता(शून्य)
{
	सङ्केत(SIGINT, handle_signal);
	सङ्केत(SIGTERM, handle_signal);
#यदि_परिभाषितम् SIGHUP
	सङ्केत(SIGHUP, handle_signal);
#यदि_अन्तः
#यदि_अपरिभाषितम् WIN32
	सङ्केत(SIGUSR1, handle_signal);
	सङ्केत(SIGUSR2, handle_signal);
	सङ्केत(SIGPIPE, SIG_IGN);
#यदि_अन्तः
#यदि_परिभाषितम् SIGRTMIN
	सङ्केत(SIGRTMIN, handle_signal);
#यदि_अन्तः
#यदि_परिभाषितम् WIN32
	CreateThread(रिक्त, 0, SigThreadProc, रिक्त, 0, रिक्त);
#यदि_अन्तः
}


पूर्णाङ्क signal__flag_check(शून्य)
{
#यदि_परिभाषितम् WITH_PERSISTENCE
	यदि(flag_db_backup){
		persist__backup(false);
		flag_db_backup = false;
	}
#यदि_अन्तः
	यदि(flag_log_rotate){
		log__close(db.config);
		log__init(db.config);
		flag_log_rotate = false;
	}
	यदि(flag_reload){
		पूर्णाङ्क rc;
		log__printf(रिक्त, MOSQ_LOG_INFO, "Reloading config.");
		config__read(db.config, true);
		listeners__reload_all_certificates();
		rc = plugin__handle_reload();
		यदि(rc){
			प्रत्यागम rc;
		}
		mosquitto_security_cleanup(true);
		rc = mosquitto_security_init(true);
		यदि(rc){
			प्रत्यागम rc;
		}
		rc = mosquitto_security_apply_default();
		यदि(rc){
			प्रत्यागम rc;
		}
		log__close(db.config);
		log__init(db.config);
		keepalive__cleanup();
		rc = keepalive__init();
		यदि(rc){
			प्रत्यागम rc;
		}
		broker_control__reload();
#यदि_परिभाषितम् WITH_BRIDGE
		bridge__reload();
#यदि_अन्तः
		flag_reload = false;
	}
	यदि(flag_वृक्षः_print){
		sub_वृक्षः_print(db.प्रसामान्यम्_subs, 0);
		sub_वृक्षः_print(db.shared_subs, 0);
		flag_वृक्षः_print = false;
	}
#यदि_परिभाषितम् WITH_XTREPORT
	यदि(flag_xtreport){
		xtreport();
		flag_xtreport = false;
	}
#यदि_अन्तः

	प्रत्यागम MOSQ_ERR_SUCCESS;
}

/*
 *
 * Signalling mosquitto process on Win32.
 *
 *  On Windows we can use named events to pass signals to the mosquitto process.
 *  List of events :
 *
 *    mosqPID_shutdown
 *    mosqPID_reload
 *    mosqPID_backup
 *    mosqPID_log_rotate
 *    mosqPID_tree_print
 *    mosqPID_xtreport
 *
 * (where PID is the PID of the mosquitto process).
 */
#यदि_परिभाषितम् WIN32

#परिभाषय MOSQ_MAX_EVTS 6


DWORD WINAPI SigThreadProc(शून्य *दत्त)
{
	TCHAR evt_name[MAX_मार्गः];
	स्थिर HANDLE evt[MOSQ_MAX_EVTS];
	पूर्णाङ्क pid = GetCurrentProcessId();
	ध्रुव वर्ण *evt_नामानि[MOSQ_MAX_EVTS] = {
		"shutdown",
		"reload",
		"backup",
		"log_rotate",
		"tree_print",
		"xtreport"
	};

	अप्रयुक्तम्(दत्त);

	प्रति(पूर्णाङ्क i=0; i<MOSQ_MAX_EVTS; i++){
		sprintf_s(evt_name, MAX_मार्गः, "mosq%d_%s", pid, evt_नामानि[i]);
		evt[i] = CreateEvent(रिक्त, FALSE, FALSE, evt_name);
	}

	यावत्(g_सततम्){
		पूर्णाङ्क wr = Wait_For_गुणजः_Objects(MOSQ_MAX_EVTS, evt, FALSE, INFINITE);
		विकल्प(wr){
			पक्ष WAIT_OBJECT_0 + 0:
				handle_signal(SIGINT);
				भङ्ग;
			पक्ष WAIT_OBJECT_0 + 1:
				flag_reload = true;
				अनुवर्त;
			पक्ष WAIT_OBJECT_0 + 2:
#यदि_परिभाषितम् WITH_PERSISTENCE
				flag_db_backup = true;
#यदि_अन्तः
				अनुवर्त;
				भङ्ग;
			पक्ष WAIT_OBJECT_0 + 3:
				flag_log_rotate = true;
				अनुवर्त;
			पक्ष WAIT_OBJECT_0 + 4:
				flag_वृक्षः_print = true;
				अनुवर्त;
			पक्ष WAIT_OBJECT_0 + 5:
				flag_xtreport = true;
				अनुवर्त;
		}
	}
	प्रति(पूर्णाङ्क i=0; i<MOSQ_MAX_EVTS; i++){
		यदि(evt[i]){
			CloseHandle(evt[i]);
		}
	}
	प्रत्यागम 0;
}
#अपरिभाषय MOSQ_MAX_EVTS
#यदि_अन्तः
