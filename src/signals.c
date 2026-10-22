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

#അന്തര്ഭാവ "config.h"

#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <stdbool.h>
#അന്തര്ഭാവ <signal.h>

#അന്തര്ഭാവ "mosquitto_broker_internal.h"

ബാഹ്യ പൂർണ്ണസംഖ്യ g_run;

സ്ഥിതം bool flag_reload = false;
സ്ഥിതം bool flag_log_rotate = false;
#ifdef WITH_PERSISTENCE
സ്ഥിതം bool flag_db_backup = false;
#endif
സ്ഥിതം bool flag_വൃക്ഷം_print = false;
സ്ഥിതം bool flag_xtreport = false;


സ്ഥിതം ശൂന്യം handle_signal(പൂർണ്ണസംഖ്യ സൂചന)
{
	UNUSED(സൂചന);

	എങ്കിൽ(സൂചന == SIGINT || സൂചന == SIGTERM){
		g_run = 0;
#ifdef SIGHUP
	}അല്ലെങ്കിൽ എങ്കിൽ(സൂചന == SIGHUP){
		flag_reload = true;
#endif
#ifdef SIGUSR1
	}അല്ലെങ്കിൽ എങ്കിൽ(സൂചന == SIGUSR1){
#ifdef WITH_PERSISTENCE
		flag_db_backup = true;
#endif
#endif
#ifdef SIGUSR2
	}അല്ലെങ്കിൽ എങ്കിൽ(സൂചന == SIGUSR2){
		flag_വൃക്ഷം_print = true;
		flag_xtreport = true;
#endif
#ifdef SIGRTMIN
	}അല്ലെങ്കിൽ എങ്കിൽ(സൂചന == SIGRTMIN){
		flag_log_rotate = true;
#endif
	}
}


ശൂന്യം signal__setup(ശൂന്യം)
{
	സൂചന(SIGINT, handle_signal);
	സൂചന(SIGTERM, handle_signal);
#ifdef SIGHUP
	സൂചന(SIGHUP, handle_signal);
#endif
#ifndef WIN32
	സൂചന(SIGUSR1, handle_signal);
	സൂചന(SIGUSR2, handle_signal);
	സൂചന(SIGPIPE, SIG_IGN);
#endif
#ifdef SIGRTMIN
	സൂചന(SIGRTMIN, handle_signal);
#endif
#ifdef WIN32
	CreateThread(ശൂന്യ, 0, SigThreadProc, ശൂന്യ, 0, ശൂന്യ);
#endif
}


പൂർണ്ണസംഖ്യ signal__flag_check(ശൂന്യം)
{
#ifdef WITH_PERSISTENCE
	എങ്കിൽ(flag_db_backup){
		persist__backup(false);
		flag_db_backup = false;
	}
#endif
	എങ്കിൽ(flag_log_rotate){
		log__close(db.config);
		log__init(db.config);
		flag_log_rotate = false;
	}
	എങ്കിൽ(flag_reload){
		പൂർണ്ണസംഖ്യ rc;
		log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Reloading config.");
		config__read(db.config, true);
		listeners__reload_all_certificates();
		rc = plugin__handle_reload();
		എങ്കിൽ(rc){
			തിരികെ rc;
		}
		mosquitto_security_cleanup(true);
		rc = mosquitto_security_init(true);
		എങ്കിൽ(rc){
			തിരികെ rc;
		}
		rc = mosquitto_security_apply_default();
		എങ്കിൽ(rc){
			തിരികെ rc;
		}
		log__close(db.config);
		log__init(db.config);
		keepalive__cleanup();
		rc = keepalive__init();
		എങ്കിൽ(rc){
			തിരികെ rc;
		}
		broker_control__reload();
#ifdef WITH_BRIDGE
		bridge__reload();
#endif
		flag_reload = false;
	}
	എങ്കിൽ(flag_വൃക്ഷം_print){
		sub_വൃക്ഷം_print(db.normal_subs, 0);
		sub_വൃക്ഷം_print(db.shared_subs, 0);
		flag_വൃക്ഷം_print = false;
	}
#ifdef WITH_XTREPORT
	എങ്കിൽ(flag_xtreport){
		xtreport();
		flag_xtreport = false;
	}
#endif

	തിരികെ MOSQ_ERR_SUCCESS;
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


DWORD WINAPI SigThreadProc(ശൂന്യം *data)
{
	TCHAR evt_name[MAX_PATH];
	സ്ഥിതം HANDLE evt[MOSQ_MAX_EVTS];
	പൂർണ്ണസംഖ്യ pid = GetCurrentProcessId();
	സ്ഥിരം അക്ഷരം *evt_names[MOSQ_MAX_EVTS] = {
		"shutdown",
		"reload",
		"backup",
		"log_rotate",
		"tree_print",
		"xtreport"
	};

	UNUSED(data);

	വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<MOSQ_MAX_EVTS; i++){
		sprintf_s(evt_name, MAX_PATH, "mosq%d_%s", pid, evt_names[i]);
		evt[i] = CreateEvent(ശൂന്യ, FALSE, FALSE, evt_name);
	}

	ആകുമ്പോൾ(g_run){
		പൂർണ്ണസംഖ്യ wr = WaitForMultipleObjects(MOSQ_MAX_EVTS, evt, FALSE, INFINITE);
		തിരഞ്ഞെടുപ്പ്(wr){
			അവസ്ഥ WAIT_OBJECT_0 + 0:
				handle_signal(SIGINT);
				നിർത്തുക;
			അവസ്ഥ WAIT_OBJECT_0 + 1:
				flag_reload = true;
				തുടരുക;
			അവസ്ഥ WAIT_OBJECT_0 + 2:
#ifdef WITH_PERSISTENCE
				flag_db_backup = true;
#endif
				തുടരുക;
				നിർത്തുക;
			അവസ്ഥ WAIT_OBJECT_0 + 3:
				flag_log_rotate = true;
				തുടരുക;
			അവസ്ഥ WAIT_OBJECT_0 + 4:
				flag_വൃക്ഷം_print = true;
				തുടരുക;
			അവസ്ഥ WAIT_OBJECT_0 + 5:
				flag_xtreport = true;
				തുടരുക;
		}
	}
	വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<MOSQ_MAX_EVTS; i++){
		എങ്കിൽ(evt[i]){
			CloseHandle(evt[i]);
		}
	}
	തിരികെ 0;
}
#undef MOSQ_MAX_EVTS
#endif
