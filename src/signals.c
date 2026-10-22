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

#অন্তর্ভাব "config.h"

#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <stdbool.h>
#অন্তর্ভাব <signal.h>

#অন্তর্ভাব "mosquitto_broker_internal.h"

বাহ্যিক পূর্ণসংখ্যা g_run;

স্থির bool flag_reload = false;
স্থির bool flag_log_rotate = false;
#ifdef WITH_PERSISTENCE
স্থির bool flag_db_backup = false;
#endif
স্থির bool flag_বৃক্ষ_print = false;
স্থির bool flag_xtreport = false;


স্থির শূন্য handle_signal(পূর্ণসংখ্যা সংকেত)
{
	UNUSED(সংকেত);

	যদি(সংকেত == SIGINT || সংকেত == SIGTERM){
		g_run = 0;
#ifdef SIGHUP
	}অন্যথা যদি(সংকেত == SIGHUP){
		flag_reload = true;
#endif
#ifdef SIGUSR1
	}অন্যথা যদি(সংকেত == SIGUSR1){
#ifdef WITH_PERSISTENCE
		flag_db_backup = true;
#endif
#endif
#ifdef SIGUSR2
	}অন্যথা যদি(সংকেত == SIGUSR2){
		flag_বৃক্ষ_print = true;
		flag_xtreport = true;
#endif
#ifdef SIGRTMIN
	}অন্যথা যদি(সংকেত == SIGRTMIN){
		flag_log_rotate = true;
#endif
	}
}


শূন্য signal__setup(শূন্য)
{
	সংকেত(SIGINT, handle_signal);
	সংকেত(SIGTERM, handle_signal);
#ifdef SIGHUP
	সংকেত(SIGHUP, handle_signal);
#endif
#ifndef WIN32
	সংকেত(SIGUSR1, handle_signal);
	সংকেত(SIGUSR2, handle_signal);
	সংকেত(SIGPIPE, SIG_IGN);
#endif
#ifdef SIGRTMIN
	সংকেত(SIGRTMIN, handle_signal);
#endif
#ifdef WIN32
	CreateThread(ফাঁকা, 0, SigThreadProc, ফাঁকা, 0, ফাঁকা);
#endif
}


পূর্ণসংখ্যা signal__flag_check(শূন্য)
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
		পূর্ণসংখ্যা rc;
		log__printf(ফাঁকা, MOSQ_LOG_INFO, "Reloading config.");
		config__read(db.config, true);
		listeners__reload_all_certificates();
		rc = plugin__handle_reload();
		যদি(rc){
			ফেরত rc;
		}
		mosquitto_security_cleanup(true);
		rc = mosquitto_security_init(true);
		যদি(rc){
			ফেরত rc;
		}
		rc = mosquitto_security_apply_default();
		যদি(rc){
			ফেরত rc;
		}
		log__close(db.config);
		log__init(db.config);
		keepalive__cleanup();
		rc = keepalive__init();
		যদি(rc){
			ফেরত rc;
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

	ফেরত MOSQ_ERR_SUCCESS;
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


DWORD WINAPI SigThreadProc(শূন্য *তথ্য)
{
	TCHAR evt_name[MAX_PATH];
	স্থির HANDLE evt[MOSQ_MAX_EVTS];
	পূর্ণসংখ্যা pid = GetCurrentProcessId();
	ধ্রুবক অক্ষর *evt_names[MOSQ_MAX_EVTS] = {
		"shutdown",
		"reload",
		"backup",
		"log_rotate",
		"tree_print",
		"xtreport"
	};

	UNUSED(তথ্য);

	জন্য(পূর্ণসংখ্যা i=0; i<MOSQ_MAX_EVTS; i++){
		sprintf_s(evt_name, MAX_PATH, "mosq%d_%s", pid, evt_names[i]);
		evt[i] = CreateEvent(ফাঁকা, FALSE, FALSE, evt_name);
	}

	যখন(g_run){
		পূর্ণসংখ্যা wr = WaitForMultipleObjects(MOSQ_MAX_EVTS, evt, FALSE, INFINITE);
		নির্বাচন(wr){
			ক্ষেত্র WAIT_OBJECT_0 + 0:
				handle_signal(SIGINT);
				বিরতি;
			ক্ষেত্র WAIT_OBJECT_0 + 1:
				flag_reload = true;
				চালু;
			ক্ষেত্র WAIT_OBJECT_0 + 2:
#ifdef WITH_PERSISTENCE
				flag_db_backup = true;
#endif
				চালু;
				বিরতি;
			ক্ষেত্র WAIT_OBJECT_0 + 3:
				flag_log_rotate = true;
				চালু;
			ক্ষেত্র WAIT_OBJECT_0 + 4:
				flag_বৃক্ষ_print = true;
				চালু;
			ক্ষেত্র WAIT_OBJECT_0 + 5:
				flag_xtreport = true;
				চালু;
		}
	}
	জন্য(পূর্ণসংখ্যা i=0; i<MOSQ_MAX_EVTS; i++){
		যদি(evt[i]){
			CloseHandle(evt[i]);
		}
	}
	ফেরত 0;
}
#undef MOSQ_MAX_EVTS
#endif
