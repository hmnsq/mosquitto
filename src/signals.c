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
#که_تعریف_شوی WIN32
#  ifndef WIN32_LEAN_AND_منځنۍ
#    define WIN32_LEAN_AND_منځنۍ
#  endif
#  include <windows.h>
#د_که_پای

#ضمن "config.h"

#ضمن <stdio.h>
#ضمن <stdbool.h>
#ضمن <signal.h>

#ضمن "mosquitto_broker_internal.h"

خارجي صحيح g_پرلپسې;

ساكن bool flag_reload = false;
ساكن bool flag_log_rotate = false;
#که_تعریف_شوی WITH_PERSISTENCE
ساكن bool flag_db_backup = false;
#د_که_پای
ساكن bool flag_ونه_print = false;
ساكن bool flag_xtreport = false;


ساكن فراغ handle_signal(صحيح إشارة)
{
	ناکارول_شوی(إشارة);

	إذا(إشارة == SIGINT || إشارة == SIGTERM){
		g_پرلپسې = 0;
#که_تعریف_شوی SIGHUP
	}وإلا إذا(إشارة == SIGHUP){
		flag_reload = true;
#د_که_پای
#که_تعریف_شوی SIGUSR1
	}وإلا إذا(إشارة == SIGUSR1){
#که_تعریف_شوی WITH_PERSISTENCE
		flag_db_backup = true;
#د_که_پای
#د_که_پای
#که_تعریف_شوی SIGUSR2
	}وإلا إذا(إشارة == SIGUSR2){
		flag_ونه_print = true;
		flag_xtreport = true;
#د_که_پای
#که_تعریف_شوی SIGRTMIN
	}وإلا إذا(إشارة == SIGRTMIN){
		flag_log_rotate = true;
#د_که_پای
	}
}


فراغ signal_تیاری(فراغ)
{
	إشارة(SIGINT, handle_signal);
	إشارة(SIGTERM, handle_signal);
#که_تعریف_شوی SIGHUP
	إشارة(SIGHUP, handle_signal);
#د_که_پای
#که_نه_تعریف_شوی WIN32
	إشارة(SIGUSR1, handle_signal);
	إشارة(SIGUSR2, handle_signal);
	إشارة(SIGPIPE, SIG_IGN);
#د_که_پای
#که_تعریف_شوی SIGRTMIN
	إشارة(SIGRTMIN, handle_signal);
#د_که_پای
#که_تعریف_شوی WIN32
	CreateThread(عدم, 0, SigThreadProc, عدم, 0, عدم);
#د_که_پای
}


صحيح signal__flag_check(فراغ)
{
#که_تعریف_شوی WITH_PERSISTENCE
	إذا(flag_db_backup){
		persist__backup(false);
		flag_db_backup = false;
	}
#د_که_پای
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
#که_تعریف_شوی WITH_BRIDGE
		bridge__reload();
#د_که_پای
		flag_reload = false;
	}
	إذا(flag_ونه_print){
		sub_ونه_print(db.نورمال_subs, 0);
		sub_ونه_print(db.shared_subs, 0);
		flag_ونه_print = false;
	}
#که_تعریف_شوی WITH_XTREPORT
	إذا(flag_xtreport){
		xtreport();
		flag_xtreport = false;
	}
#د_که_پای

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
#که_تعریف_شوی WIN32

#تعریف_کړه MOSQ_MAX_EVTS 6


DWORD WINAPI SigThreadProc(فراغ *معلومات)
{
	TCHAR evt_name[MAX_لار];
	ساكن HANDLE evt[MOSQ_MAX_EVTS];
	صحيح pid = GetCurrentProcessId();
	ثابت حرف *evt_نومونه[MOSQ_MAX_EVTS] = {
		"shutdown",
		"reload",
		"backup",
		"log_rotate",
		"tree_print",
		"xtreport"
	};

	ناکارول_شوی(معلومات);

	لكل(صحيح i=0; i<MOSQ_MAX_EVTS; i++){
		sprintf_s(evt_name, MAX_لار, "mosq%d_%s", pid, evt_نومونه[i]);
		evt[i] = CreateEvent(عدم, FALSE, FALSE, evt_name);
	}

	طالما(g_پرلپسې){
		صحيح wr = Wait_For_مضرب_Objects(MOSQ_MAX_EVTS, evt, FALSE, INFINITE);
		بدل(wr){
			حالة WAIT_OBJECT_0 + 0:
				handle_signal(SIGINT);
				اكسر;
			حالة WAIT_OBJECT_0 + 1:
				flag_reload = true;
				استمر;
			حالة WAIT_OBJECT_0 + 2:
#که_تعریف_شوی WITH_PERSISTENCE
				flag_db_backup = true;
#د_که_پای
				استمر;
				اكسر;
			حالة WAIT_OBJECT_0 + 3:
				flag_log_rotate = true;
				استمر;
			حالة WAIT_OBJECT_0 + 4:
				flag_ونه_print = true;
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
#تعریف_لغوه MOSQ_MAX_EVTS
#د_که_پای
