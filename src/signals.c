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
#جيڪڏهن_وصف_ٿيل WIN32
#  ifndef WIN32_LEAN_AND_سراسري
#    define WIN32_LEAN_AND_سراسري
#  endif
#  include <windows.h>
#جيڪڏهن_پورو

#ضمن "config.h"

#ضمن <stdio.h>
#ضمن <stdbool.h>
#ضمن <signal.h>

#ضمن "mosquitto_broker_internal.h"

خارجي صحيح g_لاڳيتو;

ساكن bool flag_reload = false;
ساكن bool flag_log_rotate = false;
#جيڪڏهن_وصف_ٿيل WITH_PERSISTENCE
ساكن bool flag_db_backup = false;
#جيڪڏهن_پورو
ساكن bool flag_وڻ_print = false;
ساكن bool flag_xtreport = false;


ساكن فراغ handle_signal(صحيح إشارة)
{
	اڻ_استعمال_ٿيل(إشارة);

	إذا(إشارة == SIGINT || إشارة == SIGTERM){
		g_لاڳيتو = 0;
#جيڪڏهن_وصف_ٿيل SIGHUP
	}وإلا إذا(إشارة == SIGHUP){
		flag_reload = true;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل SIGUSR1
	}وإلا إذا(إشارة == SIGUSR1){
#جيڪڏهن_وصف_ٿيل WITH_PERSISTENCE
		flag_db_backup = true;
#جيڪڏهن_پورو
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل SIGUSR2
	}وإلا إذا(إشارة == SIGUSR2){
		flag_وڻ_print = true;
		flag_xtreport = true;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل SIGRTMIN
	}وإلا إذا(إشارة == SIGRTMIN){
		flag_log_rotate = true;
#جيڪڏهن_پورو
	}
}


فراغ signal_تياري(فراغ)
{
	إشارة(SIGINT, handle_signal);
	إشارة(SIGTERM, handle_signal);
#جيڪڏهن_وصف_ٿيل SIGHUP
	إشارة(SIGHUP, handle_signal);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل WIN32
	إشارة(SIGUSR1, handle_signal);
	إشارة(SIGUSR2, handle_signal);
	إشارة(SIGPIPE, SIG_IGN);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل SIGRTMIN
	إشارة(SIGRTMIN, handle_signal);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل WIN32
	CreateThread(عدم, 0, SigThreadProc, عدم, 0, عدم);
#جيڪڏهن_پورو
}


صحيح signal__flag_check(فراغ)
{
#جيڪڏهن_وصف_ٿيل WITH_PERSISTENCE
	إذا(flag_db_backup){
		persist__backup(false);
		flag_db_backup = false;
	}
#جيڪڏهن_پورو
	إذا(flag_log_rotate){
		log__close(db.config);
		log__init(db.config);
		flag_log_rotate = false;
	}
	إذا(flag_reload){
		صحيح rc;
		log__printf(عدم, MOSQ_LOG_INFO, "Reloading config.");
		config__read(db.config, true);
		listeners__reload_all_certificates();
		rc = plugin__handle_reload();
		إذا(rc){
			أرجع rc;
		}
		mosquitto_security_cleanup(true);
		rc = mosquitto_security_init(true);
		إذا(rc){
			أرجع rc;
		}
		rc = mosquitto_security_apply_default();
		إذا(rc){
			أرجع rc;
		}
		log__close(db.config);
		log__init(db.config);
		keepalive__cleanup();
		rc = keepalive__init();
		إذا(rc){
			أرجع rc;
		}
		broker_control__reload();
#جيڪڏهن_وصف_ٿيل WITH_BRIDGE
		bridge__reload();
#جيڪڏهن_پورو
		flag_reload = false;
	}
	إذا(flag_وڻ_print){
		sub_وڻ_print(db.عام_subs, 0);
		sub_وڻ_print(db.shared_subs, 0);
		flag_وڻ_print = false;
	}
#جيڪڏهن_وصف_ٿيل WITH_XTREPORT
	إذا(flag_xtreport){
		xtreport();
		flag_xtreport = false;
	}
#جيڪڏهن_پورو

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
#جيڪڏهن_وصف_ٿيل WIN32

#وصف_ڪريو MOSQ_MAX_EVTS 6


DWORD WINAPI SigThreadProc(فراغ *معلومات)
{
	TCHAR evt_name[MAX_رستو];
	ساكن HANDLE evt[MOSQ_MAX_EVTS];
	صحيح pid = GetCurrentProcessId();
	ثابت حرف *evt_نالا[MOSQ_MAX_EVTS] = {
		"shutdown",
		"reload",
		"backup",
		"log_rotate",
		"tree_print",
		"xtreport"
	};

	اڻ_استعمال_ٿيل(معلومات);

	لكل(صحيح i=0; i<MOSQ_MAX_EVTS; i++){
		sprintf_s(evt_name, MAX_رستو, "mosq%d_%s", pid, evt_نالا[i]);
		evt[i] = CreateEvent(عدم, FALSE, FALSE, evt_name);
	}

	طالما(g_لاڳيتو){
		صحيح wr = Wait_For_گڻو_Objects(MOSQ_MAX_EVTS, evt, FALSE, INFINITE);
		بدل(wr){
			حالة WAIT_OBJECT_0 + 0:
				handle_signal(SIGINT);
				اكسر;
			حالة WAIT_OBJECT_0 + 1:
				flag_reload = true;
				استمر;
			حالة WAIT_OBJECT_0 + 2:
#جيڪڏهن_وصف_ٿيل WITH_PERSISTENCE
				flag_db_backup = true;
#جيڪڏهن_پورو
				استمر;
				اكسر;
			حالة WAIT_OBJECT_0 + 3:
				flag_log_rotate = true;
				استمر;
			حالة WAIT_OBJECT_0 + 4:
				flag_وڻ_print = true;
				استمر;
			حالة WAIT_OBJECT_0 + 5:
				flag_xtreport = true;
				استمر;
		}
	}
	لكل(صحيح i=0; i<MOSQ_MAX_EVTS; i++){
		إذا(evt[i]){
			CloseHandle(evt[i]);
		}
	}
	أرجع 0;
}
#وصف_رد MOSQ_MAX_EVTS
#جيڪڏهن_پورو
