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

#شامل "config.h"

#شامل <stdio.h>
#شامل <stdbool.h>
#شامل <signal.h>

#شامل "mosquitto_broker_internal.h"

بیرونی صحیح g_run;

ایستا bool flag_reload = false;
ایستا bool flag_log_rotate = false;
#ifdef WITH_PERSISTENCE
ایستا bool flag_db_backup = false;
#endif
ایستا bool flag_درخت_print = false;
ایستا bool flag_xtreport = false;


ایستا تهی handle_signal(صحیح سیگنال)
{
	UNUSED(سیگنال);

	اگر(سیگنال == SIGINT || سیگنال == SIGTERM){
		g_run = 0;
#ifdef SIGHUP
	}وگرنه اگر(سیگنال == SIGHUP){
		flag_reload = true;
#endif
#ifdef SIGUSR1
	}وگرنه اگر(سیگنال == SIGUSR1){
#ifdef WITH_PERSISTENCE
		flag_db_backup = true;
#endif
#endif
#ifdef SIGUSR2
	}وگرنه اگر(سیگنال == SIGUSR2){
		flag_درخت_print = true;
		flag_xtreport = true;
#endif
#ifdef SIGRTMIN
	}وگرنه اگر(سیگنال == SIGRTMIN){
		flag_log_rotate = true;
#endif
	}
}


تهی signal__setup(تهی)
{
	سیگنال(SIGINT, handle_signal);
	سیگنال(SIGTERM, handle_signal);
#ifdef SIGHUP
	سیگنال(SIGHUP, handle_signal);
#endif
#ifndef WIN32
	سیگنال(SIGUSR1, handle_signal);
	سیگنال(SIGUSR2, handle_signal);
	سیگنال(SIGPIPE, SIG_IGN);
#endif
#ifdef SIGRTMIN
	سیگنال(SIGRTMIN, handle_signal);
#endif
#ifdef WIN32
	CreateThread(پوچ, 0, SigThreadProc, پوچ, 0, پوچ);
#endif
}


صحیح signal__flag_check(تهی)
{
#ifdef WITH_PERSISTENCE
	اگر(flag_db_backup){
		persist__backup(false);
		flag_db_backup = false;
	}
#endif
	اگر(flag_log_rotate){
		log__close(db.config);
		log__init(db.config);
		flag_log_rotate = false;
	}
	اگر(flag_reload){
		صحیح rc;
		log__printf(پوچ, MOSQ_LOG_INFO, "Reloading config.");
		config__read(db.config, true);
		listeners__reload_all_certificates();
		rc = plugin__handle_reload();
		اگر(rc){
			بازگشت rc;
		}
		mosquitto_security_cleanup(true);
		rc = mosquitto_security_init(true);
		اگر(rc){
			بازگشت rc;
		}
		rc = mosquitto_security_apply_default();
		اگر(rc){
			بازگشت rc;
		}
		log__close(db.config);
		log__init(db.config);
		keepalive__cleanup();
		rc = keepalive__init();
		اگر(rc){
			بازگشت rc;
		}
		broker_control__reload();
#ifdef WITH_BRIDGE
		bridge__reload();
#endif
		flag_reload = false;
	}
	اگر(flag_درخت_print){
		sub_درخت_print(db.normal_subs, 0);
		sub_درخت_print(db.shared_subs, 0);
		flag_درخت_print = false;
	}
#ifdef WITH_XTREPORT
	اگر(flag_xtreport){
		xtreport();
		flag_xtreport = false;
	}
#endif

	بازگشت MOSQ_ERR_SUCCESS;
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


DWORD WINAPI SigThreadProc(تهی *data)
{
	TCHAR evt_name[MAX_PATH];
	ایستا HANDLE evt[MOSQ_MAX_EVTS];
	صحیح pid = GetCurrentProcessId();
	ثابت نویسه *evt_names[MOSQ_MAX_EVTS] = {
		"shutdown",
		"reload",
		"backup",
		"log_rotate",
		"tree_print",
		"xtreport"
	};

	UNUSED(data);

	برای(صحیح i=0; i<MOSQ_MAX_EVTS; i++){
		sprintf_s(evt_name, MAX_PATH, "mosq%d_%s", pid, evt_names[i]);
		evt[i] = CreateEvent(پوچ, FALSE, FALSE, evt_name);
	}

	وقتی(g_run){
		صحیح wr = WaitForMultipleObjects(MOSQ_MAX_EVTS, evt, FALSE, INFINITE);
		انتخاب(wr){
			حالت WAIT_OBJECT_0 + 0:
				handle_signal(SIGINT);
				بشکن;
			حالت WAIT_OBJECT_0 + 1:
				flag_reload = true;
				ادامه;
			حالت WAIT_OBJECT_0 + 2:
#ifdef WITH_PERSISTENCE
				flag_db_backup = true;
#endif
				ادامه;
				بشکن;
			حالت WAIT_OBJECT_0 + 3:
				flag_log_rotate = true;
				ادامه;
			حالت WAIT_OBJECT_0 + 4:
				flag_درخت_print = true;
				ادامه;
			حالت WAIT_OBJECT_0 + 5:
				flag_xtreport = true;
				ادامه;
		}
	}
	برای(صحیح i=0; i<MOSQ_MAX_EVTS; i++){
		اگر(evt[i]){
			CloseHandle(evt[i]);
		}
	}
	بازگشت 0;
}
#undef MOSQ_MAX_EVTS
#endif
