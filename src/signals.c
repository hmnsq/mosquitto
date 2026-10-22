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
#اگر_تعریف_شدہ WIN32
#  ifndef WIN32_LEAN_AND_اوسط
#    define WIN32_LEAN_AND_اوسط
#  endif
#  include <windows.h>
#اگر_ختم

#شامل "config.h"

#شامل <stdio.h>
#شامل <stdbool.h>
#شامل <signal.h>

#شامل "mosquitto_broker_internal.h"

خارجی صحیح g_تسلسل;

ساکن bool flag_reload = false;
ساکن bool flag_log_rotate = false;
#اگر_تعریف_شدہ WITH_PERSISTENCE
ساکن bool flag_db_backup = false;
#اگر_ختم
ساکن bool flag_درخت_print = false;
ساکن bool flag_xtreport = false;


ساکن خالی handle_signal(صحیح اشارہ)
{
	غیر_مستعمل(اشارہ);

	اگر(اشارہ == SIGINT || اشارہ == SIGTERM){
		g_تسلسل = 0;
#اگر_تعریف_شدہ SIGHUP
	}ورنہ اگر(اشارہ == SIGHUP){
		flag_reload = true;
#اگر_ختم
#اگر_تعریف_شدہ SIGUSR1
	}ورنہ اگر(اشارہ == SIGUSR1){
#اگر_تعریف_شدہ WITH_PERSISTENCE
		flag_db_backup = true;
#اگر_ختم
#اگر_ختم
#اگر_تعریف_شدہ SIGUSR2
	}ورنہ اگر(اشارہ == SIGUSR2){
		flag_درخت_print = true;
		flag_xtreport = true;
#اگر_ختم
#اگر_تعریف_شدہ SIGRTMIN
	}ورنہ اگر(اشارہ == SIGRTMIN){
		flag_log_rotate = true;
#اگر_ختم
	}
}


خالی signal_تیاری(خالی)
{
	اشارہ(SIGINT, handle_signal);
	اشارہ(SIGTERM, handle_signal);
#اگر_تعریف_شدہ SIGHUP
	اشارہ(SIGHUP, handle_signal);
#اگر_ختم
#اگر_غیر_تعریف_شدہ WIN32
	اشارہ(SIGUSR1, handle_signal);
	اشارہ(SIGUSR2, handle_signal);
	اشارہ(SIGPIPE, SIG_IGN);
#اگر_ختم
#اگر_تعریف_شدہ SIGRTMIN
	اشارہ(SIGRTMIN, handle_signal);
#اگر_ختم
#اگر_تعریف_شدہ WIN32
	CreateThread(تہی, 0, SigThreadProc, تہی, 0, تہی);
#اگر_ختم
}


صحیح signal__flag_check(خالی)
{
#اگر_تعریف_شدہ WITH_PERSISTENCE
	اگر(flag_db_backup){
		persist__backup(false);
		flag_db_backup = false;
	}
#اگر_ختم
	اگر(flag_log_rotate){
		log__close(db.config);
		log__init(db.config);
		flag_log_rotate = false;
	}
	اگر(flag_reload){
		صحیح rc;
		log__printf(تہی, MOSQ_LOG_INFO, "Reloading config.");
		config__read(db.config, true);
		listeners__reload_all_certificates();
		rc = plugin__handle_reload();
		اگر(rc){
			واپس rc;
		}
		mosquitto_security_cleanup(true);
		rc = mosquitto_security_init(true);
		اگر(rc){
			واپس rc;
		}
		rc = mosquitto_security_apply_default();
		اگر(rc){
			واپس rc;
		}
		log__close(db.config);
		log__init(db.config);
		keepalive__cleanup();
		rc = keepalive__init();
		اگر(rc){
			واپس rc;
		}
		broker_control__reload();
#اگر_تعریف_شدہ WITH_BRIDGE
		bridge__reload();
#اگر_ختم
		flag_reload = false;
	}
	اگر(flag_درخت_print){
		sub_درخت_print(db.معمول_subs, 0);
		sub_درخت_print(db.shared_subs, 0);
		flag_درخت_print = false;
	}
#اگر_تعریف_شدہ WITH_XTREPORT
	اگر(flag_xtreport){
		xtreport();
		flag_xtreport = false;
	}
#اگر_ختم

	واپس MOSQ_ERR_SUCCESS;
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
#اگر_تعریف_شدہ WIN32

#تعریف_کرو MOSQ_MAX_EVTS 6


DWORD WINAPI SigThreadProc(خالی *ڈیٹا)
{
	TCHAR evt_name[MAX_راستہ];
	ساکن HANDLE evt[MOSQ_MAX_EVTS];
	صحیح pid = GetCurrentProcessId();
	ثابت حرف *evt_نام[MOSQ_MAX_EVTS] = {
		"shutdown",
		"reload",
		"backup",
		"log_rotate",
		"tree_print",
		"xtreport"
	};

	غیر_مستعمل(ڈیٹا);

	برائے(صحیح i=0; i<MOSQ_MAX_EVTS; i++){
		sprintf_s(evt_name, MAX_راستہ, "mosq%d_%s", pid, evt_نام[i]);
		evt[i] = CreateEvent(تہی, FALSE, FALSE, evt_name);
	}

	جب(g_تسلسل){
		صحیح wr = Wait_For_مضرب_Objects(MOSQ_MAX_EVTS, evt, FALSE, INFINITE);
		انتخاب(wr){
			حالت WAIT_OBJECT_0 + 0:
				handle_signal(SIGINT);
				توڑو;
			حالت WAIT_OBJECT_0 + 1:
				flag_reload = true;
				جاری;
			حالت WAIT_OBJECT_0 + 2:
#اگر_تعریف_شدہ WITH_PERSISTENCE
				flag_db_backup = true;
#اگر_ختم
				جاری;
				توڑو;
			حالت WAIT_OBJECT_0 + 3:
				flag_log_rotate = true;
				جاری;
			حالت WAIT_OBJECT_0 + 4:
				flag_درخت_print = true;
				جاری;
			حالت WAIT_OBJECT_0 + 5:
				flag_xtreport = true;
				جاری;
		}
	}
	برائے(صحیح i=0; i<MOSQ_MAX_EVTS; i++){
		اگر(evt[i]){
			CloseHandle(evt[i]);
		}
	}
	واپس 0;
}
#تعریف_منسوخ MOSQ_MAX_EVTS
#اگر_ختم
