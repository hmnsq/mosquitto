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

extern цяло g_run;

static bool flag_reload = false;
static bool flag_log_rotate = false;
#ifdef WITH_PERSISTENCE
static bool flag_db_backup = false;
#endif
static bool flag_tree_print = false;
static bool flag_xtreport = false;


static void handle_signal(цяло signal)
{
	UNUSED(signal);

	ако(signal == SIGINT || signal == SIGTERM){
		g_run = 0;
#ifdef SIGHUP
	}иначе ако(signal == SIGHUP){
		flag_reload = true;
#endif
#ifdef SIGUSR1
	}иначе ако(signal == SIGUSR1){
#ifdef WITH_PERSISTENCE
		flag_db_backup = true;
#endif
#endif
#ifdef SIGUSR2
	}иначе ако(signal == SIGUSR2){
		flag_tree_print = true;
		flag_xtreport = true;
#endif
#ifdef SIGRTMIN
	}иначе ако(signal == SIGRTMIN){
		flag_log_rotate = true;
#endif
	}
}


void signal__setup(void)
{
	signal(SIGINT, handle_signal);
	signal(SIGTERM, handle_signal);
#ifdef SIGHUP
	signal(SIGHUP, handle_signal);
#endif
#ifndef WIN32
	signal(SIGUSR1, handle_signal);
	signal(SIGUSR2, handle_signal);
	signal(SIGPIPE, SIG_IGN);
#endif
#ifdef SIGRTMIN
	signal(SIGRTMIN, handle_signal);
#endif
#ifdef WIN32
	CreateThread(NULL, 0, SigThreadProc, NULL, 0, NULL);
#endif
}


цяло signal__flag_check(void)
{
#ifdef WITH_PERSISTENCE
	ако(flag_db_backup){
		persist__backup(false);
		flag_db_backup = false;
	}
#endif
	ако(flag_log_rotate){
		log__close(db.config);
		log__init(db.config);
		flag_log_rotate = false;
	}
	ако(flag_reload){
		цяло rc;
		log__printf(NULL, MOSQ_LOG_INFO, "Reloading config.");
		config__read(db.config, true);
		listeners__reload_all_certificates();
		rc = plugin__handle_reload();
		ако(rc){
			върни rc;
		}
		mosquitto_security_cleanup(true);
		rc = mosquitto_security_init(true);
		ако(rc){
			върни rc;
		}
		rc = mosquitto_security_apply_default();
		ако(rc){
			върни rc;
		}
		log__close(db.config);
		log__init(db.config);
		keepalive__cleanup();
		rc = keepalive__init();
		ако(rc){
			върни rc;
		}
		broker_control__reload();
#ifdef WITH_BRIDGE
		bridge__reload();
#endif
		flag_reload = false;
	}
	ако(flag_tree_print){
		sub__tree_print(db.normal_subs, 0);
		sub__tree_print(db.shared_subs, 0);
		flag_tree_print = false;
	}
#ifdef WITH_XTREPORT
	ако(flag_xtreport){
		xtreport();
		flag_xtreport = false;
	}
#endif

	върни MOSQ_ERR_SUCCESS;
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
	цяло pid = GetCurrentProcessId();
	const char *evt_names[MOSQ_MAX_EVTS] = {
		"shutdown",
		"reload",
		"backup",
		"log_rotate",
		"tree_print",
		"xtreport"
	};

	UNUSED(data);

	за(цяло i=0; i<MOSQ_MAX_EVTS; i++){
		sprintf_s(evt_name, MAX_PATH, "mosq%d_%s", pid, evt_names[i]);
		evt[i] = CreateEvent(NULL, FALSE, FALSE, evt_name);
	}

	докато(g_run){
		цяло wr = WaitForMultipleObjects(MOSQ_MAX_EVTS, evt, FALSE, INFINITE);
		switch(wr){
			case WAIT_OBJECT_0 + 0:
				handle_signal(SIGINT);
				break;
			case WAIT_OBJECT_0 + 1:
				flag_reload = true;
				continue;
			case WAIT_OBJECT_0 + 2:
#ifdef WITH_PERSISTENCE
				flag_db_backup = true;
#endif
				continue;
				break;
			case WAIT_OBJECT_0 + 3:
				flag_log_rotate = true;
				continue;
			case WAIT_OBJECT_0 + 4:
				flag_tree_print = true;
				continue;
			case WAIT_OBJECT_0 + 5:
				flag_xtreport = true;
				continue;
		}
	}
	за(цяло i=0; i<MOSQ_MAX_EVTS; i++){
		ако(evt[i]){
			CloseHandle(evt[i]);
		}
	}
	върни 0;
}
#undef MOSQ_MAX_EVTS
#endif
