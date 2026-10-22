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
#جے_تعریف_شدہ WIN32
#  ifndef WIN32_LEAN_AND_اوسط
#    define WIN32_LEAN_AND_اوسط
#  endif
#  include <windows.h>
#جے_انت

#ضمن "config.h"

#ضمن <stdio.h>
#ضمن <stdbool.h>
#ضمن <signal.h>

#ضمن "mosquitto_broker_internal.h"

خارجي صحيح g_لگاتار;

ساكن bool flag_reload = false;
ساكن bool flag_log_rotate = false;
#جے_تعریف_شدہ WITH_PERSISTENCE
ساكن bool flag_db_backup = false;
#جے_انت
ساكن bool flag_درخت_print = false;
ساكن bool flag_xtreport = false;


ساكن فراغ handle_signal(صحيح إشارة)
{
	بن_ورتیا(إشارة);

	إذا(إشارة == SIGINT || إشارة == SIGTERM){
		g_لگاتار = 0;
#جے_تعریف_شدہ SIGHUP
	}وإلا إذا(إشارة == SIGHUP){
		flag_reload = true;
#جے_انت
#جے_تعریف_شدہ SIGUSR1
	}وإلا إذا(إشارة == SIGUSR1){
#جے_تعریف_شدہ WITH_PERSISTENCE
		flag_db_backup = true;
#جے_انت
#جے_انت
#جے_تعریف_شدہ SIGUSR2
	}وإلا إذا(إشارة == SIGUSR2){
		flag_درخت_print = true;
		flag_xtreport = true;
#جے_انت
#جے_تعریف_شدہ SIGRTMIN
	}وإلا إذا(إشارة == SIGRTMIN){
		flag_log_rotate = true;
#جے_انت
	}
}


فراغ signal_تیاری(فراغ)
{
	إشارة(SIGINT, handle_signal);
	إشارة(SIGTERM, handle_signal);
#جے_تعریف_شدہ SIGHUP
	إشارة(SIGHUP, handle_signal);
#جے_انت
#جے_بنا_تعریف WIN32
	إشارة(SIGUSR1, handle_signal);
	إشارة(SIGUSR2, handle_signal);
	إشارة(SIGPIPE, SIG_IGN);
#جے_انت
#جے_تعریف_شدہ SIGRTMIN
	إشارة(SIGRTMIN, handle_signal);
#جے_انت
#جے_تعریف_شدہ WIN32
	CreateThread(عدم, 0, SigThreadProc, عدم, 0, عدم);
#جے_انت
}


صحيح signal__flag_check(فراغ)
{
#جے_تعریف_شدہ WITH_PERSISTENCE
	إذا(flag_db_backup){
		persist__backup(false);
		flag_db_backup = false;
	}
#جے_انت
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
#جے_تعریف_شدہ WITH_BRIDGE
		bridge__reload();
#جے_انت
		flag_reload = false;
	}
	إذا(flag_درخت_print){
		sub_درخت_print(db.عام_subs, 0);
		sub_درخت_print(db.shared_subs, 0);
		flag_درخت_print = false;
	}
#جے_تعریف_شدہ WITH_XTREPORT
	إذا(flag_xtreport){
		xtreport();
		flag_xtreport = false;
	}
#جے_انت

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
#جے_تعریف_شدہ WIN32

#تعریف_کرو MOSQ_MAX_EVTS 6


DWORD WINAPI SigThreadProc(فراغ *انکڑے)
{
	TCHAR evt_name[MAX_رستہ];
	ساكن HANDLE evt[MOSQ_MAX_EVTS];
	صحيح pid = GetCurrentProcessId();
	ثابت حرف *evt_ناں[MOSQ_MAX_EVTS] = {
		"shutdown",
		"reload",
		"backup",
		"log_rotate",
		"tree_print",
		"xtreport"
	};

	بن_ورتیا(انکڑے);

	لكل(صحيح i=0; i<MOSQ_MAX_EVTS; i++){
		sprintf_s(evt_name, MAX_رستہ, "mosq%d_%s", pid, evt_ناں[i]);
		evt[i] = CreateEvent(عدم, FALSE, FALSE, evt_name);
	}

	طالما(g_لگاتار){
		صحيح wr = Wait_For_مضرب_Objects(MOSQ_MAX_EVTS, evt, FALSE, INFINITE);
		بدل(wr){
			حالة WAIT_OBJECT_0 + 0:
				handle_signal(SIGINT);
				اكسر;
			حالة WAIT_OBJECT_0 + 1:
				flag_reload = true;
				استمر;
			حالة WAIT_OBJECT_0 + 2:
#جے_تعریف_شدہ WITH_PERSISTENCE
				flag_db_backup = true;
#جے_انت
				استمر;
				اكسر;
			حالة WAIT_OBJECT_0 + 3:
				flag_log_rotate = true;
				استمر;
			حالة WAIT_OBJECT_0 + 4:
				flag_درخت_print = true;
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
#تعریف_رد MOSQ_MAX_EVTS
#جے_انت
