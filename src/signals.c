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
#ifdef WIN32
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <windows.h>
#endif

#अन्तर्भाव "config.h"

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <stdbool.h>
#अन्तर्भाव <signal.h>

#अन्तर्भाव "mosquitto_broker_internal.h"

बाह्य पूर्णाङ्क g_run;

स्थिर bool flag_reload = false;
स्थिर bool flag_log_rotate = false;
#ifdef WITH_PERSISTENCE
स्थिर bool flag_db_backup = false;
#endif
स्थिर bool flag_वृक्ष_print = false;
स्थिर bool flag_xtreport = false;


स्थिर शून्य handle_signal(पूर्णाङ्क सङ्केत)
{
	UNUSED(सङ्केत);

	यदि(सङ्केत == SIGINT || सङ्केत == SIGTERM){
		g_run = 0;
#ifdef SIGHUP
	}अन्यथा यदि(सङ्केत == SIGHUP){
		flag_reload = true;
#endif
#ifdef SIGUSR1
	}अन्यथा यदि(सङ्केत == SIGUSR1){
#ifdef WITH_PERSISTENCE
		flag_db_backup = true;
#endif
#endif
#ifdef SIGUSR2
	}अन्यथा यदि(सङ्केत == SIGUSR2){
		flag_वृक्ष_print = true;
		flag_xtreport = true;
#endif
#ifdef SIGRTMIN
	}अन्यथा यदि(सङ्केत == SIGRTMIN){
		flag_log_rotate = true;
#endif
	}
}


शून्य signal__setup(शून्य)
{
	सङ्केत(SIGINT, handle_signal);
	सङ्केत(SIGTERM, handle_signal);
#ifdef SIGHUP
	सङ्केत(SIGHUP, handle_signal);
#endif
#ifndef WIN32
	सङ्केत(SIGUSR1, handle_signal);
	सङ्केत(SIGUSR2, handle_signal);
	सङ्केत(SIGPIPE, SIG_IGN);
#endif
#ifdef SIGRTMIN
	सङ्केत(SIGRTMIN, handle_signal);
#endif
#ifdef WIN32
	CreateThread(रिक्त, 0, SigThreadProc, रिक्त, 0, रिक्त);
#endif
}


पूर्णाङ्क signal__flag_check(शून्य)
{
#ifdef WITH_PERSISTENCE
	यदि(flag_db_backup){
		persist__backup(false);
		flag_db_backup = false;
	}
#endif
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
#ifdef WITH_BRIDGE
		bridge__reload();
#endif
		flag_reload = false;
	}
	यदि(flag_वृक्ष_print){
		sub_वृक्ष_print(db.normal_subs, 0);
		sub_वृक्ष_print(db.shared_subs, 0);
		flag_वृक्ष_print = false;
	}
#ifdef WITH_XTREPORT
	यदि(flag_xtreport){
		xtreport();
		flag_xtreport = false;
	}
#endif

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
#ifdef WIN32

#define MOSQ_MAX_EVTS 6


DWORD WINAPI SigThreadProc(शून्य *data)
{
	TCHAR evt_name[MAX_PATH];
	स्थिर HANDLE evt[MOSQ_MAX_EVTS];
	पूर्णाङ्क pid = GetCurrentProcessId();
	ध्रुव वर्ण *evt_names[MOSQ_MAX_EVTS] = {
		"shutdown",
		"reload",
		"backup",
		"log_rotate",
		"tree_print",
		"xtreport"
	};

	UNUSED(data);

	प्रति(पूर्णाङ्क i=0; i<MOSQ_MAX_EVTS; i++){
		sprintf_s(evt_name, MAX_PATH, "mosq%d_%s", pid, evt_names[i]);
		evt[i] = CreateEvent(रिक्त, FALSE, FALSE, evt_name);
	}

	यावत्(g_run){
		पूर्णाङ्क wr = WaitForMultipleObjects(MOSQ_MAX_EVTS, evt, FALSE, INFINITE);
		विकल्प(wr){
			पक्ष WAIT_OBJECT_0 + 0:
				handle_signal(SIGINT);
				भङ्ग;
			पक्ष WAIT_OBJECT_0 + 1:
				flag_reload = true;
				अनुवर्त;
			पक्ष WAIT_OBJECT_0 + 2:
#ifdef WITH_PERSISTENCE
				flag_db_backup = true;
#endif
				अनुवर्त;
				भङ्ग;
			पक्ष WAIT_OBJECT_0 + 3:
				flag_log_rotate = true;
				अनुवर्त;
			पक्ष WAIT_OBJECT_0 + 4:
				flag_वृक्ष_print = true;
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
#undef MOSQ_MAX_EVTS
#endif
