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

#અન્તર્ભાવ "config.h"

#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <stdbool.h>
#અન્તર્ભાવ <signal.h>

#અન્તર્ભાવ "mosquitto_broker_internal.h"

બાહ્ય પૂર્ણાંક g_run;

સ્થિર bool flag_reload = false;
સ્થિર bool flag_log_rotate = false;
#ifdef WITH_PERSISTENCE
સ્થિર bool flag_db_backup = false;
#endif
સ્થિર bool flag_વૃક્ષ_print = false;
સ્થિર bool flag_xtreport = false;


સ્થિર શૂન્ય handle_signal(પૂર્ણાંક સંકેત)
{
	UNUSED(સંકેત);

	જો(સંકેત == SIGINT || સંકેત == SIGTERM){
		g_run = 0;
#ifdef SIGHUP
	}નહીંતર જો(સંકેત == SIGHUP){
		flag_reload = true;
#endif
#ifdef SIGUSR1
	}નહીંતર જો(સંકેત == SIGUSR1){
#ifdef WITH_PERSISTENCE
		flag_db_backup = true;
#endif
#endif
#ifdef SIGUSR2
	}નહીંતર જો(સંકેત == SIGUSR2){
		flag_વૃક્ષ_print = true;
		flag_xtreport = true;
#endif
#ifdef SIGRTMIN
	}નહીંતર જો(સંકેત == SIGRTMIN){
		flag_log_rotate = true;
#endif
	}
}


શૂન્ય signal__setup(શૂન્ય)
{
	સંકેત(SIGINT, handle_signal);
	સંકેત(SIGTERM, handle_signal);
#ifdef SIGHUP
	સંકેત(SIGHUP, handle_signal);
#endif
#ifndef WIN32
	સંકેત(SIGUSR1, handle_signal);
	સંકેત(SIGUSR2, handle_signal);
	સંકેત(SIGPIPE, SIG_IGN);
#endif
#ifdef SIGRTMIN
	સંકેત(SIGRTMIN, handle_signal);
#endif
#ifdef WIN32
	CreateThread(ખાલી, 0, SigThreadProc, ખાલી, 0, ખાલી);
#endif
}


પૂર્ણાંક signal__flag_check(શૂન્ય)
{
#ifdef WITH_PERSISTENCE
	જો(flag_db_backup){
		persist__backup(false);
		flag_db_backup = false;
	}
#endif
	જો(flag_log_rotate){
		log__close(db.config);
		log__init(db.config);
		flag_log_rotate = false;
	}
	જો(flag_reload){
		પૂર્ણાંક rc;
		log__printf(ખાલી, MOSQ_LOG_INFO, "Reloading config.");
		config__read(db.config, true);
		listeners__reload_all_certificates();
		rc = plugin__handle_reload();
		જો(rc){
			પાછા rc;
		}
		mosquitto_security_cleanup(true);
		rc = mosquitto_security_init(true);
		જો(rc){
			પાછા rc;
		}
		rc = mosquitto_security_apply_default();
		જો(rc){
			પાછા rc;
		}
		log__close(db.config);
		log__init(db.config);
		keepalive__cleanup();
		rc = keepalive__init();
		જો(rc){
			પાછા rc;
		}
		broker_control__reload();
#ifdef WITH_BRIDGE
		bridge__reload();
#endif
		flag_reload = false;
	}
	જો(flag_વૃક્ષ_print){
		sub_વૃક્ષ_print(db.normal_subs, 0);
		sub_વૃક્ષ_print(db.shared_subs, 0);
		flag_વૃક્ષ_print = false;
	}
#ifdef WITH_XTREPORT
	જો(flag_xtreport){
		xtreport();
		flag_xtreport = false;
	}
#endif

	પાછા MOSQ_ERR_SUCCESS;
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


DWORD WINAPI SigThreadProc(શૂન્ય *data)
{
	TCHAR evt_name[MAX_PATH];
	સ્થિર HANDLE evt[MOSQ_MAX_EVTS];
	પૂર્ણાંક pid = GetCurrentProcessId();
	અચલ અક્ષર *evt_names[MOSQ_MAX_EVTS] = {
		"shutdown",
		"reload",
		"backup",
		"log_rotate",
		"tree_print",
		"xtreport"
	};

	UNUSED(data);

	માટે(પૂર્ણાંક i=0; i<MOSQ_MAX_EVTS; i++){
		sprintf_s(evt_name, MAX_PATH, "mosq%d_%s", pid, evt_names[i]);
		evt[i] = CreateEvent(ખાલી, FALSE, FALSE, evt_name);
	}

	જ્યારે(g_run){
		પૂર્ણાંક wr = WaitForMultipleObjects(MOSQ_MAX_EVTS, evt, FALSE, INFINITE);
		પસંદગી(wr){
			સ્થિતિ WAIT_OBJECT_0 + 0:
				handle_signal(SIGINT);
				તોડો;
			સ્થિતિ WAIT_OBJECT_0 + 1:
				flag_reload = true;
				ચાલુ;
			સ્થિતિ WAIT_OBJECT_0 + 2:
#ifdef WITH_PERSISTENCE
				flag_db_backup = true;
#endif
				ચાલુ;
				તોડો;
			સ્થિતિ WAIT_OBJECT_0 + 3:
				flag_log_rotate = true;
				ચાલુ;
			સ્થિતિ WAIT_OBJECT_0 + 4:
				flag_વૃક્ષ_print = true;
				ચાલુ;
			સ્થિતિ WAIT_OBJECT_0 + 5:
				flag_xtreport = true;
				ચાલુ;
		}
	}
	માટે(પૂર્ણાંક i=0; i<MOSQ_MAX_EVTS; i++){
		જો(evt[i]){
			CloseHandle(evt[i]);
		}
	}
	પાછા 0;
}
#undef MOSQ_MAX_EVTS
#endif
