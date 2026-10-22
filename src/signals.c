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

#அந்தர்பாவ "config.h"

#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <stdbool.h>
#அந்தர்பாவ <signal.h>

#அந்தர்பாவ "mosquitto_broker_internal.h"

வெளி முழு g_run;

நிலையான bool flag_reload = false;
நிலையான bool flag_log_rotate = false;
#ifdef WITH_PERSISTENCE
நிலையான bool flag_db_backup = false;
#endif
நிலையான bool flag_மரம்_print = false;
நிலையான bool flag_xtreport = false;


நிலையான வெறுமை handle_signal(முழு சமிக்ஞை)
{
	UNUSED(சமிக்ஞை);

	எனில்(சமிக்ஞை == SIGINT || சமிக்ஞை == SIGTERM){
		g_run = 0;
#ifdef SIGHUP
	}இல்லையெனில் எனில்(சமிக்ஞை == SIGHUP){
		flag_reload = true;
#endif
#ifdef SIGUSR1
	}இல்லையெனில் எனில்(சமிக்ஞை == SIGUSR1){
#ifdef WITH_PERSISTENCE
		flag_db_backup = true;
#endif
#endif
#ifdef SIGUSR2
	}இல்லையெனில் எனில்(சமிக்ஞை == SIGUSR2){
		flag_மரம்_print = true;
		flag_xtreport = true;
#endif
#ifdef SIGRTMIN
	}இல்லையெனில் எனில்(சமிக்ஞை == SIGRTMIN){
		flag_log_rotate = true;
#endif
	}
}


வெறுமை signal__setup(வெறுமை)
{
	சமிக்ஞை(SIGINT, handle_signal);
	சமிக்ஞை(SIGTERM, handle_signal);
#ifdef SIGHUP
	சமிக்ஞை(SIGHUP, handle_signal);
#endif
#ifndef WIN32
	சமிக்ஞை(SIGUSR1, handle_signal);
	சமிக்ஞை(SIGUSR2, handle_signal);
	சமிக்ஞை(SIGPIPE, SIG_IGN);
#endif
#ifdef SIGRTMIN
	சமிக்ஞை(SIGRTMIN, handle_signal);
#endif
#ifdef WIN32
	CreateThread(வெற்று, 0, SigThreadProc, வெற்று, 0, வெற்று);
#endif
}


முழு signal__flag_check(வெறுமை)
{
#ifdef WITH_PERSISTENCE
	எனில்(flag_db_backup){
		persist__backup(false);
		flag_db_backup = false;
	}
#endif
	எனில்(flag_log_rotate){
		log__close(db.config);
		log__init(db.config);
		flag_log_rotate = false;
	}
	எனில்(flag_reload){
		முழு rc;
		log__printf(வெற்று, MOSQ_LOG_INFO, "Reloading config.");
		config__read(db.config, true);
		listeners__reload_all_certificates();
		rc = plugin__handle_reload();
		எனில்(rc){
			திரும்பு rc;
		}
		mosquitto_security_cleanup(true);
		rc = mosquitto_security_init(true);
		எனில்(rc){
			திரும்பு rc;
		}
		rc = mosquitto_security_apply_default();
		எனில்(rc){
			திரும்பு rc;
		}
		log__close(db.config);
		log__init(db.config);
		keepalive__cleanup();
		rc = keepalive__init();
		எனில்(rc){
			திரும்பு rc;
		}
		broker_control__reload();
#ifdef WITH_BRIDGE
		bridge__reload();
#endif
		flag_reload = false;
	}
	எனில்(flag_மரம்_print){
		sub_மரம்_print(db.normal_subs, 0);
		sub_மரம்_print(db.shared_subs, 0);
		flag_மரம்_print = false;
	}
#ifdef WITH_XTREPORT
	எனில்(flag_xtreport){
		xtreport();
		flag_xtreport = false;
	}
#endif

	திரும்பு MOSQ_ERR_SUCCESS;
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


DWORD WINAPI SigThreadProc(வெறுமை *data)
{
	TCHAR evt_name[MAX_PATH];
	நிலையான HANDLE evt[MOSQ_MAX_EVTS];
	முழு pid = GetCurrentProcessId();
	மாறிலி எழுத்து *evt_names[MOSQ_MAX_EVTS] = {
		"shutdown",
		"reload",
		"backup",
		"log_rotate",
		"tree_print",
		"xtreport"
	};

	UNUSED(data);

	க்கு(முழு i=0; i<MOSQ_MAX_EVTS; i++){
		sprintf_s(evt_name, MAX_PATH, "mosq%d_%s", pid, evt_names[i]);
		evt[i] = CreateEvent(வெற்று, FALSE, FALSE, evt_name);
	}

	போது(g_run){
		முழு wr = WaitForMultipleObjects(MOSQ_MAX_EVTS, evt, FALSE, INFINITE);
		தேர்வு(wr){
			நிலை WAIT_OBJECT_0 + 0:
				handle_signal(SIGINT);
				நிறுத்து;
			நிலை WAIT_OBJECT_0 + 1:
				flag_reload = true;
				தொடர்;
			நிலை WAIT_OBJECT_0 + 2:
#ifdef WITH_PERSISTENCE
				flag_db_backup = true;
#endif
				தொடர்;
				நிறுத்து;
			நிலை WAIT_OBJECT_0 + 3:
				flag_log_rotate = true;
				தொடர்;
			நிலை WAIT_OBJECT_0 + 4:
				flag_மரம்_print = true;
				தொடர்;
			நிலை WAIT_OBJECT_0 + 5:
				flag_xtreport = true;
				தொடர்;
		}
	}
	க்கு(முழு i=0; i<MOSQ_MAX_EVTS; i++){
		எனில்(evt[i]){
			CloseHandle(evt[i]);
		}
	}
	திரும்பு 0;
}
#undef MOSQ_MAX_EVTS
#endif
