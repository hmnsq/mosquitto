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

#include "config.h"

#include <stdio.h>
#include <stdbool.h>
#include <signal.h>

#include "mosquitto_broker_internal.h"

extern puur_naa_nka g_run;

static bool flag_reload = false;
static bool flag_log_rotate = false;
#ifdef WITH_PERSISTENCE
static bool flag_db_backup = false;
#endif
static bool flag_tree_print = false;
static bool flag_xtreport = false;


static void handle_signal(puur_naa_nka sa_nkeeta)
{
	UNUSED(sa_nkeeta);

	ᬬᬾᬦ᭄(sa_nkeeta == SIGINT || sa_nkeeta == SIGTERM){
		g_run = 0;
#ifdef SIGHUP
	}ᬮᬶᬬ ᬬᬾᬦ᭄(sa_nkeeta == SIGHUP){
		flag_reload = true;
#endif
#ifdef SIGUSR1
	}ᬮᬶᬬ ᬬᬾᬦ᭄(sa_nkeeta == SIGUSR1){
#ifdef WITH_PERSISTENCE
		flag_db_backup = true;
#endif
#endif
#ifdef SIGUSR2
	}ᬮᬶᬬ ᬬᬾᬦ᭄(sa_nkeeta == SIGUSR2){
		flag_tree_print = true;
		flag_xtreport = true;
#endif
#ifdef SIGRTMIN
	}ᬮᬶᬬ ᬬᬾᬦ᭄(sa_nkeeta == SIGRTMIN){
		flag_log_rotate = true;
#endif
	}
}


void signal__setup(void)
{
	sa_nkeeta(SIGINT, handle_signal);
	sa_nkeeta(SIGTERM, handle_signal);
#ifdef SIGHUP
	sa_nkeeta(SIGHUP, handle_signal);
#endif
#ifndef WIN32
	sa_nkeeta(SIGUSR1, handle_signal);
	sa_nkeeta(SIGUSR2, handle_signal);
	sa_nkeeta(SIGPIPE, SIG_IGN);
#endif
#ifdef SIGRTMIN
	sa_nkeeta(SIGRTMIN, handle_signal);
#endif
#ifdef WIN32
	CreateThread(NULL, 0, SigThreadProc, NULL, 0, NULL);
#endif
}


puur_naa_nka signal__flag_check(void)
{
#ifdef WITH_PERSISTENCE
	ᬬᬾᬦ᭄(flag_db_backup){
		persist__backup(false);
		flag_db_backup = false;
	}
#endif
	ᬬᬾᬦ᭄(flag_log_rotate){
		log__close(db.config);
		log__init(db.config);
		flag_log_rotate = false;
	}
	ᬬᬾᬦ᭄(flag_reload){
		puur_naa_nka rc;
		log__printf(NULL, MOSQ_LOG_INFO, "Reloading config.");
		config__read(db.config, true);
		listeners__reload_all_certificates();
		rc = plugin__handle_reload();
		ᬬᬾᬦ᭄(rc){
			ᬯᬮᬶ rc;
		}
		mosquitto_security_cleanup(true);
		rc = mosquitto_security_init(true);
		ᬬᬾᬦ᭄(rc){
			ᬯᬮᬶ rc;
		}
		rc = mosquitto_security_apply_default();
		ᬬᬾᬦ᭄(rc){
			ᬯᬮᬶ rc;
		}
		log__close(db.config);
		log__init(db.config);
		keepalive__cleanup();
		rc = keepalive__init();
		ᬬᬾᬦ᭄(rc){
			ᬯᬮᬶ rc;
		}
		broker_control__reload();
#ifdef WITH_BRIDGE
		bridge__reload();
#endif
		flag_reload = false;
	}
	ᬬᬾᬦ᭄(flag_tree_print){
		sub__tree_print(db.normal_subs, 0);
		sub__tree_print(db.shared_subs, 0);
		flag_tree_print = false;
	}
#ifdef WITH_XTREPORT
	ᬬᬾᬦ᭄(flag_xtreport){
		xtreport();
		flag_xtreport = false;
	}
#endif

	ᬯᬮᬶ MOSQ_ERR_SUCCESS;
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


DWORD WINAPI SigThreadProc(void *data)
{
	TCHAR evt_name[MAX_PATH];
	static HANDLE evt[MOSQ_MAX_EVTS];
	puur_naa_nka pid = GetCurrentProcessId();
	const var_na *evt_names[MOSQ_MAX_EVTS] = {
		"shutdown",
		"reload",
		"backup",
		"log_rotate",
		"tree_print",
		"xtreport"
	};

	UNUSED(data);

	for(puur_naa_nka i=0; i<MOSQ_MAX_EVTS; i++){
		sprintf_s(evt_name, MAX_PATH, "mosq%d_%s", pid, evt_names[i]);
		evt[i] = CreateEvent(NULL, FALSE, FALSE, evt_name);
	}

	ᬲᭂᬤᭂᬮ(g_run){
		puur_naa_nka wr = WaitForMultipleObjects(MOSQ_MAX_EVTS, evt, FALSE, INFINITE);
		switch(wr){
			pak_sa WAIT_OBJECT_0 + 0:
				handle_signal(SIGINT);
				bha_nga;
			pak_sa WAIT_OBJECT_0 + 1:
				flag_reload = true;
				continue;
			pak_sa WAIT_OBJECT_0 + 2:
#ifdef WITH_PERSISTENCE
				flag_db_backup = true;
#endif
				continue;
				bha_nga;
			pak_sa WAIT_OBJECT_0 + 3:
				flag_log_rotate = true;
				continue;
			pak_sa WAIT_OBJECT_0 + 4:
				flag_tree_print = true;
				continue;
			pak_sa WAIT_OBJECT_0 + 5:
				flag_xtreport = true;
				continue;
		}
	}
	for(puur_naa_nka i=0; i<MOSQ_MAX_EVTS; i++){
		ᬬᬾᬦ᭄(evt[i]){
			CloseHandle(evt[i]);
		}
	}
	ᬯᬮᬶ 0;
}
#undef MOSQ_MAX_EVTS
#endif
