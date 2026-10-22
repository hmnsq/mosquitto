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

#ضمن "config.h"

#ضمن <stdio.h>
#ضمن <stdbool.h>
#ضمن <signal.h>

#ضمن "mosquitto_broker_internal.h"

خارجي صحيح g_run;

ساكن bool flag_reload = false;
ساكن bool flag_log_rotate = false;
#ifdef WITH_PERSISTENCE
ساكن bool flag_db_backup = false;
#endif
ساكن bool flag_tree_print = false;
ساكن bool flag_xtreport = false;


ساكن فراغ handle_signal(صحيح إشارة)
{
	UNUSED(إشارة);

	𐡄𐡍(إشارة == SIGINT || إشارة == SIGTERM){
		g_run = 0;
#ifdef SIGHUP
	}وإلا 𐡄𐡍(إشارة == SIGHUP){
		flag_reload = true;
#endif
#ifdef SIGUSR1
	}وإلا 𐡄𐡍(إشارة == SIGUSR1){
#ifdef WITH_PERSISTENCE
		flag_db_backup = true;
#endif
#endif
#ifdef SIGUSR2
	}وإلا 𐡄𐡍(إشارة == SIGUSR2){
		flag_tree_print = true;
		flag_xtreport = true;
#endif
#ifdef SIGRTMIN
	}وإلا 𐡄𐡍(إشارة == SIGRTMIN){
		flag_log_rotate = true;
#endif
	}
}


فراغ signal__setup(فراغ)
{
	إشارة(SIGINT, handle_signal);
	إشارة(SIGTERM, handle_signal);
#ifdef SIGHUP
	إشارة(SIGHUP, handle_signal);
#endif
#ifndef WIN32
	إشارة(SIGUSR1, handle_signal);
	إشارة(SIGUSR2, handle_signal);
	إشارة(SIGPIPE, SIG_IGN);
#endif
#ifdef SIGRTMIN
	إشارة(SIGRTMIN, handle_signal);
#endif
#ifdef WIN32
	CreateThread(عدم, 0, SigThreadProc, عدم, 0, عدم);
#endif
}


صحيح signal__flag_check(فراغ)
{
#ifdef WITH_PERSISTENCE
	𐡄𐡍(flag_db_backup){
		persist__backup(false);
		flag_db_backup = false;
	}
#endif
	𐡄𐡍(flag_log_rotate){
		log__close(db.config);
		log__init(db.config);
		flag_log_rotate = false;
	}
	𐡄𐡍(flag_reload){
		صحيح rc;
		log__printf(عدم, MOSQ_LOG_INFO, "Reloading config.");
		config__read(db.config, true);
		listeners__reload_all_certificates();
		rc = plugin__handle_reload();
		𐡄𐡍(rc){
			أرجع rc;
		}
		mosquitto_security_cleanup(true);
		rc = mosquitto_security_init(true);
		𐡄𐡍(rc){
			أرجع rc;
		}
		rc = mosquitto_security_apply_default();
		𐡄𐡍(rc){
			أرجع rc;
		}
		log__close(db.config);
		log__init(db.config);
		keepalive__cleanup();
		rc = keepalive__init();
		𐡄𐡍(rc){
			أرجع rc;
		}
		broker_control__reload();
#ifdef WITH_BRIDGE
		bridge__reload();
#endif
		flag_reload = false;
	}
	𐡄𐡍(flag_tree_print){
		sub__tree_print(db.normal_subs, 0);
		sub__tree_print(db.shared_subs, 0);
		flag_tree_print = false;
	}
#ifdef WITH_XTREPORT
	𐡄𐡍(flag_xtreport){
		xtreport();
		flag_xtreport = false;
	}
#endif

	أرجع MOSQ_ERR_SUCCESS;
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


DWORD WINAPI SigThreadProc(فراغ *data)
{
	TCHAR evt_name[MAX_PATH];
	ساكن HANDLE evt[MOSQ_MAX_EVTS];
	صحيح pid = GetCurrentProcessId();
	ثابت حرف *evt_names[MOSQ_MAX_EVTS] = {
		"shutdown",
		"reload",
		"backup",
		"log_rotate",
		"tree_print",
		"xtreport"
	};

	UNUSED(data);

	لكل(صحيح i=0; i<MOSQ_MAX_EVTS; i++){
		sprintf_s(evt_name, MAX_PATH, "mosq%d_%s", pid, evt_names[i]);
		evt[i] = CreateEvent(عدم, FALSE, FALSE, evt_name);
	}

	طالما(g_run){
		صحيح wr = WaitForMultipleObjects(MOSQ_MAX_EVTS, evt, FALSE, INFINITE);
		بدل(wr){
			حالة WAIT_OBJECT_0 + 0:
				handle_signal(SIGINT);
				اكسر;
			حالة WAIT_OBJECT_0 + 1:
				flag_reload = true;
				استمر;
			حالة WAIT_OBJECT_0 + 2:
#ifdef WITH_PERSISTENCE
				flag_db_backup = true;
#endif
				استمر;
				اكسر;
			حالة WAIT_OBJECT_0 + 3:
				flag_log_rotate = true;
				استمر;
			حالة WAIT_OBJECT_0 + 4:
				flag_tree_print = true;
				استمر;
			حالة WAIT_OBJECT_0 + 5:
				flag_xtreport = true;
				استمر;
		}
	}
	لكل(صحيح i=0; i<MOSQ_MAX_EVTS; i++){
		𐡄𐡍(evt[i]){
			CloseHandle(evt[i]);
		}
	}
	أرجع 0;
}
#undef MOSQ_MAX_EVTS
#endif
