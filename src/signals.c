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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WIN32
#  ifndef WIN32_LEAN_AND_ਔਸਤ
#    define WIN32_LEAN_AND_ਔਸਤ
#  endif
#  include <windows.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdbool.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <signal.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"

ਬਾਹਰੀ ਪੂਰਨ g_ਲੜੀਵਾਰ;

ਸਥਿਰ bool flag_reload = false;
ਸਥਿਰ bool flag_log_rotate = false;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_PERSISTENCE
ਸਥਿਰ bool flag_db_backup = false;
#ਜੇ_ਅੰਤ
ਸਥਿਰ bool flag_ਰੁੱਖ_print = false;
ਸਥਿਰ bool flag_xtreport = false;


ਸਥਿਰ ਖਾਲੀ handle_signal(ਪੂਰਨ ਸੰਕੇਤ)
{
	ਅਣਵਰਤਿਆ(ਸੰਕੇਤ);

	ਜੇ(ਸੰਕੇਤ == SIGINT || ਸੰਕੇਤ == SIGTERM){
		g_ਲੜੀਵਾਰ = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIGHUP
	}ਨਹੀਂਤਾਂ ਜੇ(ਸੰਕੇਤ == SIGHUP){
		flag_reload = true;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIGUSR1
	}ਨਹੀਂਤਾਂ ਜੇ(ਸੰਕੇਤ == SIGUSR1){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_PERSISTENCE
		flag_db_backup = true;
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIGUSR2
	}ਨਹੀਂਤਾਂ ਜੇ(ਸੰਕੇਤ == SIGUSR2){
		flag_ਰੁੱਖ_print = true;
		flag_xtreport = true;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIGRTMIN
	}ਨਹੀਂਤਾਂ ਜੇ(ਸੰਕੇਤ == SIGRTMIN){
		flag_log_rotate = true;
#ਜੇ_ਅੰਤ
	}
}


ਖਾਲੀ signal_ਤਿਆਰੀ(ਖਾਲੀ)
{
	ਸੰਕੇਤ(SIGINT, handle_signal);
	ਸੰਕੇਤ(SIGTERM, handle_signal);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIGHUP
	ਸੰਕੇਤ(SIGHUP, handle_signal);
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
	ਸੰਕੇਤ(SIGUSR1, handle_signal);
	ਸੰਕੇਤ(SIGUSR2, handle_signal);
	ਸੰਕੇਤ(SIGPIPE, SIG_IGN);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIGRTMIN
	ਸੰਕੇਤ(SIGRTMIN, handle_signal);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WIN32
	CreateThread(ਨਿਰਾ, 0, SigThreadProc, ਨਿਰਾ, 0, ਨਿਰਾ);
#ਜੇ_ਅੰਤ
}


ਪੂਰਨ signal__flag_check(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_PERSISTENCE
	ਜੇ(flag_db_backup){
		persist__backup(false);
		flag_db_backup = false;
	}
#ਜੇ_ਅੰਤ
	ਜੇ(flag_log_rotate){
		log__close(db.config);
		log__init(db.config);
		flag_log_rotate = false;
	}
	ਜੇ(flag_reload){
		ਪੂਰਨ rc;
		log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Reloading config.");
		config__read(db.config, true);
		listeners__reload_all_certificates();
		rc = plugin__handle_reload();
		ਜੇ(rc){
			ਵਾਪਸ rc;
		}
		mosquitto_security_cleanup(true);
		rc = mosquitto_security_init(true);
		ਜੇ(rc){
			ਵਾਪਸ rc;
		}
		rc = mosquitto_security_apply_default();
		ਜੇ(rc){
			ਵਾਪਸ rc;
		}
		log__close(db.config);
		log__init(db.config);
		keepalive__cleanup();
		rc = keepalive__init();
		ਜੇ(rc){
			ਵਾਪਸ rc;
		}
		broker_control__reload();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
		bridge__reload();
#ਜੇ_ਅੰਤ
		flag_reload = false;
	}
	ਜੇ(flag_ਰੁੱਖ_print){
		sub_ਰੁੱਖ_print(db.ਸਾਧਾਰਨ_subs, 0);
		sub_ਰੁੱਖ_print(db.shared_subs, 0);
		flag_ਰੁੱਖ_print = false;
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_XTREPORT
	ਜੇ(flag_xtreport){
		xtreport();
		flag_xtreport = false;
	}
#ਜੇ_ਅੰਤ

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WIN32

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOSQ_MAX_EVTS 6


DWORD WINAPI SigThreadProc(ਖਾਲੀ *ਅੰਕੜੇ)
{
	TCHAR evt_name[MAX_ਰਸਤਾ];
	ਸਥਿਰ HANDLE evt[MOSQ_MAX_EVTS];
	ਪੂਰਨ pid = GetCurrentProcessId();
	ਅਚਲ ਅੱਖਰ *evt_ਨਾਂ[MOSQ_MAX_EVTS] = {
		"shutdown",
		"reload",
		"backup",
		"log_rotate",
		"tree_print",
		"xtreport"
	};

	ਅਣਵਰਤਿਆ(ਅੰਕੜੇ);

	ਲਈ(ਪੂਰਨ i=0; i<MOSQ_MAX_EVTS; i++){
		sprintf_s(evt_name, MAX_ਰਸਤਾ, "mosq%d_%s", pid, evt_ਨਾਂ[i]);
		evt[i] = CreateEvent(ਨਿਰਾ, FALSE, FALSE, evt_name);
	}

	ਜਦੋਂ(g_ਲੜੀਵਾਰ){
		ਪੂਰਨ wr = Wait_For_ਗੁਣਜ_Objects(MOSQ_MAX_EVTS, evt, FALSE, INFINITE);
		ਚੋਣ(wr){
			ਹਾਲਤ WAIT_OBJECT_0 + 0:
				handle_signal(SIGINT);
				ਰੋਕੋ;
			ਹਾਲਤ WAIT_OBJECT_0 + 1:
				flag_reload = true;
				ਜਾਰੀ;
			ਹਾਲਤ WAIT_OBJECT_0 + 2:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_PERSISTENCE
				flag_db_backup = true;
#ਜੇ_ਅੰਤ
				ਜਾਰੀ;
				ਰੋਕੋ;
			ਹਾਲਤ WAIT_OBJECT_0 + 3:
				flag_log_rotate = true;
				ਜਾਰੀ;
			ਹਾਲਤ WAIT_OBJECT_0 + 4:
				flag_ਰੁੱਖ_print = true;
				ਜਾਰੀ;
			ਹਾਲਤ WAIT_OBJECT_0 + 5:
				flag_xtreport = true;
				ਜਾਰੀ;
		}
	}
	ਲਈ(ਪੂਰਨ i=0; i<MOSQ_MAX_EVTS; i++){
		ਜੇ(evt[i]){
			CloseHandle(evt[i]);
		}
	}
	ਵਾਪਸ 0;
}
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ MOSQ_MAX_EVTS
#ਜੇ_ਅੰਤ
