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
#اگر_تعریف_شده WIN32
#  ifndef WIN32_LEAN_AND_میانگین
#    define WIN32_LEAN_AND_میانگین
#  endif
#  include <windows.h>
#پایان_اگر

#ضمن "config.h"

#ضمن <stdio.h>
#ضمن <stdbool.h>
#ضمن <signal.h>

#ضمن "mosquitto_broker_internal.h"

خارجي صحيح g_پیاپی;

ساكن bool flag_reload = false;
ساكن bool flag_log_rotate = false;
#اگر_تعریف_شده WITH_PERSISTENCE
ساكن bool flag_db_backup = false;
#پایان_اگر
ساكن bool flag_درخت_print = false;
ساكن bool flag_xtreport = false;


ساكن فراغ handle_signal(صحيح إشارة)
{
	استفادهنشده(إشارة);

	إذا(إشارة == SIGINT || إشارة == SIGTERM){
		g_پیاپی = 0;
#اگر_تعریف_شده SIGHUP
	}وإلا إذا(إشارة == SIGHUP){
		flag_reload = true;
#پایان_اگر
#اگر_تعریف_شده SIGUSR1
	}وإلا إذا(إشارة == SIGUSR1){
#اگر_تعریف_شده WITH_PERSISTENCE
		flag_db_backup = true;
#پایان_اگر
#پایان_اگر
#اگر_تعریف_شده SIGUSR2
	}وإلا إذا(إشارة == SIGUSR2){
		flag_درخت_print = true;
		flag_xtreport = true;
#پایان_اگر
#اگر_تعریف_شده SIGRTMIN
	}وإلا إذا(إشارة == SIGRTMIN){
		flag_log_rotate = true;
#پایان_اگر
	}
}


فراغ signal_راهاندازی(فراغ)
{
	إشارة(SIGINT, handle_signal);
	إشارة(SIGTERM, handle_signal);
#اگر_تعریف_شده SIGHUP
	إشارة(SIGHUP, handle_signal);
#پایان_اگر
#اگر_تعریف_نشده WIN32
	إشارة(SIGUSR1, handle_signal);
	إشارة(SIGUSR2, handle_signal);
	إشارة(SIGPIPE, SIG_IGN);
#پایان_اگر
#اگر_تعریف_شده SIGRTMIN
	إشارة(SIGRTMIN, handle_signal);
#پایان_اگر
#اگر_تعریف_شده WIN32
	CreateThread(عدم, 0, SigThreadProc, عدم, 0, عدم);
#پایان_اگر
}


صحيح signal__flag_check(فراغ)
{
#اگر_تعریف_شده WITH_PERSISTENCE
	إذا(flag_db_backup){
		persist__backup(false);
		flag_db_backup = false;
	}
#پایان_اگر
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
#اگر_تعریف_شده WITH_BRIDGE
		bridge__reload();
#پایان_اگر
		flag_reload = false;
	}
	إذا(flag_درخت_print){
		sub_درخت_print(db.نرمال_subs, 0);
		sub_درخت_print(db.shared_subs, 0);
		flag_درخت_print = false;
	}
#اگر_تعریف_شده WITH_XTREPORT
	إذا(flag_xtreport){
		xtreport();
		flag_xtreport = false;
	}
#پایان_اگر

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
#اگر_تعریف_شده WIN32

#تعریف_کن MOSQ_MAX_EVTS 6


DWORD WINAPI SigThreadProc(فراغ *داده)
{
	TCHAR evt_name[MAX_مسیر];
	ساكن HANDLE evt[MOSQ_MAX_EVTS];
	صحيح pid = GetCurrentProcessId();
	ثابت حرف *evt_نامها[MOSQ_MAX_EVTS] = {
		"shutdown",
		"reload",
		"backup",
		"log_rotate",
		"tree_print",
		"xtreport"
	};

	استفادهنشده(داده);

	لكل(صحيح i=0; i<MOSQ_MAX_EVTS; i++){
		sprintf_s(evt_name, MAX_مسیر, "mosq%d_%s", pid, evt_نامها[i]);
		evt[i] = CreateEvent(عدم, FALSE, FALSE, evt_name);
	}

	طالما(g_پیاپی){
		صحيح wr = Wait_For_مضرب_Objects(MOSQ_MAX_EVTS, evt, FALSE, INFINITE);
		بدل(wr){
			حالة WAIT_OBJECT_0 + 0:
				handle_signal(SIGINT);
				اكسر;
			حالة WAIT_OBJECT_0 + 1:
				flag_reload = true;
				استمر;
			حالة WAIT_OBJECT_0 + 2:
#اگر_تعریف_شده WITH_PERSISTENCE
				flag_db_backup = true;
#پایان_اگر
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
#لغو_تعریف MOSQ_MAX_EVTS
#پایان_اگر
