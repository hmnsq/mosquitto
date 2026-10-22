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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <stdbool.h>
#ଅନ୍ତର୍ଭାଵ <signal.h>

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"

ବାହ୍ୟ ପୂର୍ଣ୍ଣସଂଖ୍ୟା g_run;

ସ୍ଥିର bool flag_reload = false;
ସ୍ଥିର bool flag_log_rotate = false;
#ifdef WITH_PERSISTENCE
ସ୍ଥିର bool flag_db_backup = false;
#endif
ସ୍ଥିର bool flag_ବୃକ୍ଷ_print = false;
ସ୍ଥିର bool flag_xtreport = false;


ସ୍ଥିର ଶୂନ୍ୟ handle_signal(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ସଙ୍କେତ)
{
	UNUSED(ସଙ୍କେତ);

	ଯଦି(ସଙ୍କେତ == SIGINT || ସଙ୍କେତ == SIGTERM){
		g_run = 0;
#ifdef SIGHUP
	}ଅନ୍ୟଥା ଯଦି(ସଙ୍କେତ == SIGHUP){
		flag_reload = true;
#endif
#ifdef SIGUSR1
	}ଅନ୍ୟଥା ଯଦି(ସଙ୍କେତ == SIGUSR1){
#ifdef WITH_PERSISTENCE
		flag_db_backup = true;
#endif
#endif
#ifdef SIGUSR2
	}ଅନ୍ୟଥା ଯଦି(ସଙ୍କେତ == SIGUSR2){
		flag_ବୃକ୍ଷ_print = true;
		flag_xtreport = true;
#endif
#ifdef SIGRTMIN
	}ଅନ୍ୟଥା ଯଦି(ସଙ୍କେତ == SIGRTMIN){
		flag_log_rotate = true;
#endif
	}
}


ଶୂନ୍ୟ signal__setup(ଶୂନ୍ୟ)
{
	ସଙ୍କେତ(SIGINT, handle_signal);
	ସଙ୍କେତ(SIGTERM, handle_signal);
#ifdef SIGHUP
	ସଙ୍କେତ(SIGHUP, handle_signal);
#endif
#ifndef WIN32
	ସଙ୍କେତ(SIGUSR1, handle_signal);
	ସଙ୍କେତ(SIGUSR2, handle_signal);
	ସଙ୍କେତ(SIGPIPE, SIG_IGN);
#endif
#ifdef SIGRTMIN
	ସଙ୍କେତ(SIGRTMIN, handle_signal);
#endif
#ifdef WIN32
	CreateThread(ଖାଲି, 0, SigThreadProc, ଖାଲି, 0, ଖାଲି);
#endif
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା signal__flag_check(ଶୂନ୍ୟ)
{
#ifdef WITH_PERSISTENCE
	ଯଦି(flag_db_backup){
		persist__backup(false);
		flag_db_backup = false;
	}
#endif
	ଯଦି(flag_log_rotate){
		log__close(db.config);
		log__init(db.config);
		flag_log_rotate = false;
	}
	ଯଦି(flag_reload){
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
		log__printf(ଖାଲି, MOSQ_LOG_INFO, "Reloading config.");
		config__read(db.config, true);
		listeners__reload_all_certificates();
		rc = plugin__handle_reload();
		ଯଦି(rc){
			ଫେରନ୍ତୁ rc;
		}
		mosquitto_security_cleanup(true);
		rc = mosquitto_security_init(true);
		ଯଦି(rc){
			ଫେରନ୍ତୁ rc;
		}
		rc = mosquitto_security_apply_default();
		ଯଦି(rc){
			ଫେରନ୍ତୁ rc;
		}
		log__close(db.config);
		log__init(db.config);
		keepalive__cleanup();
		rc = keepalive__init();
		ଯଦି(rc){
			ଫେରନ୍ତୁ rc;
		}
		broker_control__reload();
#ifdef WITH_BRIDGE
		bridge__reload();
#endif
		flag_reload = false;
	}
	ଯଦି(flag_ବୃକ୍ଷ_print){
		sub_ବୃକ୍ଷ_print(db.normal_subs, 0);
		sub_ବୃକ୍ଷ_print(db.shared_subs, 0);
		flag_ବୃକ୍ଷ_print = false;
	}
#ifdef WITH_XTREPORT
	ଯଦି(flag_xtreport){
		xtreport();
		flag_xtreport = false;
	}
#endif

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
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


DWORD WINAPI SigThreadProc(ଶୂନ୍ୟ *data)
{
	TCHAR evt_name[MAX_PATH];
	ସ୍ଥିର HANDLE evt[MOSQ_MAX_EVTS];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid = GetCurrentProcessId();
	ଅଚଳ ଅକ୍ଷର *evt_names[MOSQ_MAX_EVTS] = {
		"shutdown",
		"reload",
		"backup",
		"log_rotate",
		"tree_print",
		"xtreport"
	};

	UNUSED(data);

	ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<MOSQ_MAX_EVTS; i++){
		sprintf_s(evt_name, MAX_PATH, "mosq%d_%s", pid, evt_names[i]);
		evt[i] = CreateEvent(ଖାଲି, FALSE, FALSE, evt_name);
	}

	ଯେତେବେଳେ(g_run){
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା wr = WaitForMultipleObjects(MOSQ_MAX_EVTS, evt, FALSE, INFINITE);
		ବାଛ(wr){
			ସ୍ଥିତି WAIT_OBJECT_0 + 0:
				handle_signal(SIGINT);
				ଭାଙ୍ଗ;
			ସ୍ଥିତି WAIT_OBJECT_0 + 1:
				flag_reload = true;
				ଜାରି;
			ସ୍ଥିତି WAIT_OBJECT_0 + 2:
#ifdef WITH_PERSISTENCE
				flag_db_backup = true;
#endif
				ଜାରି;
				ଭାଙ୍ଗ;
			ସ୍ଥିତି WAIT_OBJECT_0 + 3:
				flag_log_rotate = true;
				ଜାରି;
			ସ୍ଥିତି WAIT_OBJECT_0 + 4:
				flag_ବୃକ୍ଷ_print = true;
				ଜାରି;
			ସ୍ଥିତି WAIT_OBJECT_0 + 5:
				flag_xtreport = true;
				ଜାରି;
		}
	}
	ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<MOSQ_MAX_EVTS; i++){
		ଯଦି(evt[i]){
			CloseHandle(evt[i]);
		}
	}
	ଫେରନ୍ତୁ 0;
}
#undef MOSQ_MAX_EVTS
#endif
