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

#অন্তর্ভাৱ "config.h"

#অন্তর্ভাৱ <stdio.h>
#অন্তর্ভাৱ <stdbool.h>
#অন্তর্ভাৱ <signal.h>

#অন্তর্ভাৱ "mosquitto_broker_internal.h"

বাহ্য পূর্ণাঙ্ক g_run;

স্থির bool flag_reload = false;
স্থির bool flag_log_rotate = false;
#ifdef WITH_PERSISTENCE
স্থির bool flag_db_backup = false;
#endif
স্থির bool flag_বৃক্ষ_print = false;
স্থির bool flag_xtreport = false;


স্থির শূন্য handle_signal(পূর্ণাঙ্ক সঙ্কেত)
{
	UNUSED(সঙ্কেত);

	যদি(সঙ্কেত == SIGINT || সঙ্কেত == SIGTERM){
		g_run = 0;
#ifdef SIGHUP
	}অন্যথা যদি(সঙ্কেত == SIGHUP){
		flag_reload = true;
#endif
#ifdef SIGUSR1
	}অন্যথা যদি(সঙ্কেত == SIGUSR1){
#ifdef WITH_PERSISTENCE
		flag_db_backup = true;
#endif
#endif
#ifdef SIGUSR2
	}অন্যথা যদি(সঙ্কেত == SIGUSR2){
		flag_বৃক্ষ_print = true;
		flag_xtreport = true;
#endif
#ifdef SIGRTMIN
	}অন্যথা যদি(সঙ্কেত == SIGRTMIN){
		flag_log_rotate = true;
#endif
	}
}


শূন্য signal__setup(শূন্য)
{
	সঙ্কেত(SIGINT, handle_signal);
	সঙ্কেত(SIGTERM, handle_signal);
#ifdef SIGHUP
	সঙ্কেত(SIGHUP, handle_signal);
#endif
#ifndef WIN32
	সঙ্কেত(SIGUSR1, handle_signal);
	সঙ্কেত(SIGUSR2, handle_signal);
	সঙ্কেত(SIGPIPE, SIG_IGN);
#endif
#ifdef SIGRTMIN
	সঙ্কেত(SIGRTMIN, handle_signal);
#endif
#ifdef WIN32
	CreateThread(রিক্ত, 0, SigThreadProc, রিক্ত, 0, রিক্ত);
#endif
}


পূর্ণাঙ্ক signal__flag_check(শূন্য)
{
#ifdef WITH_PERSISTENCE
	যদি(flag_db_backup){
		persist__backup(false);
		flag_db_backup = false;
	}
#endif
	যদি(flag_log_rotate){
		log__close(db.config);
		log__init(db.config);
		flag_log_rotate = false;
	}
	যদি(flag_reload){
		পূর্ণাঙ্ক rc;
		log__printf(রিক্ত, MOSQ_LOG_INFO, "Reloading config.");
		config__read(db.config, true);
		listeners__reload_all_certificates();
		rc = plugin__handle_reload();
		যদি(rc){
			ঘূৰাই rc;
		}
		mosquitto_security_cleanup(true);
		rc = mosquitto_security_init(true);
		যদি(rc){
			ঘূৰাই rc;
		}
		rc = mosquitto_security_apply_default();
		যদি(rc){
			ঘূৰাই rc;
		}
		log__close(db.config);
		log__init(db.config);
		keepalive__cleanup();
		rc = keepalive__init();
		যদি(rc){
			ঘূৰাই rc;
		}
		broker_control__reload();
#ifdef WITH_BRIDGE
		bridge__reload();
#endif
		flag_reload = false;
	}
	যদি(flag_বৃক্ষ_print){
		sub_বৃক্ষ_print(db.normal_subs, 0);
		sub_বৃক্ষ_print(db.shared_subs, 0);
		flag_বৃক্ষ_print = false;
	}
#ifdef WITH_XTREPORT
	যদি(flag_xtreport){
		xtreport();
		flag_xtreport = false;
	}
#endif

	ঘূৰাই MOSQ_ERR_SUCCESS;
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


DWORD WINAPI SigThreadProc(শূন্য *data)
{
	TCHAR evt_name[MAX_PATH];
	স্থির HANDLE evt[MOSQ_MAX_EVTS];
	পূর্ণাঙ্ক pid = GetCurrentProcessId();
	ধ্রুৱ ৱর্ণ *evt_names[MOSQ_MAX_EVTS] = {
		"shutdown",
		"reload",
		"backup",
		"log_rotate",
		"tree_print",
		"xtreport"
	};

	UNUSED(data);

	বাবে(পূর্ণাঙ্ক i=0; i<MOSQ_MAX_EVTS; i++){
		sprintf_s(evt_name, MAX_PATH, "mosq%d_%s", pid, evt_names[i]);
		evt[i] = CreateEvent(রিক্ত, FALSE, FALSE, evt_name);
	}

	যেতিয়ালৈ(g_run){
		পূর্ণাঙ্ক wr = WaitForMultipleObjects(MOSQ_MAX_EVTS, evt, FALSE, INFINITE);
		ৱিকল্প(wr){
			পক্ষ WAIT_OBJECT_0 + 0:
				handle_signal(SIGINT);
				ভঙ্গ;
			পক্ষ WAIT_OBJECT_0 + 1:
				flag_reload = true;
				অনুৱর্ত;
			পক্ষ WAIT_OBJECT_0 + 2:
#ifdef WITH_PERSISTENCE
				flag_db_backup = true;
#endif
				অনুৱর্ত;
				ভঙ্গ;
			পক্ষ WAIT_OBJECT_0 + 3:
				flag_log_rotate = true;
				অনুৱর্ত;
			পক্ষ WAIT_OBJECT_0 + 4:
				flag_বৃক্ষ_print = true;
				অনুৱর্ত;
			পক্ষ WAIT_OBJECT_0 + 5:
				flag_xtreport = true;
				অনুৱর্ত;
		}
	}
	বাবে(পূর্ণাঙ্ক i=0; i<MOSQ_MAX_EVTS; i++){
		যদি(evt[i]){
			CloseHandle(evt[i]);
		}
	}
	ঘূৰাই 0;
}
#undef MOSQ_MAX_EVTS
#endif
