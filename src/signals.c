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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <stdbool.h>
#ಅನ್ತರ್ಭಾವ <signal.h>

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"

ಬಾಹ್ಯ ಪೂರ್ಣಾಙ್ಕ g_run;

ಸ್ಥಿರ bool flag_reload = false;
ಸ್ಥಿರ bool flag_log_rotate = false;
#ifdef WITH_PERSISTENCE
ಸ್ಥಿರ bool flag_db_backup = false;
#endif
ಸ್ಥಿರ bool flag_ವೃಕ್ಷ_print = false;
ಸ್ಥಿರ bool flag_xtreport = false;


ಸ್ಥಿರ ಶೂನ್ಯ handle_signal(ಪೂರ್ಣಾಙ್ಕ ಸಙ್ಕೇತ)
{
	UNUSED(ಸಙ್ಕೇತ);

	ಯದಿ(ಸಙ್ಕೇತ == SIGINT || ಸಙ್ಕೇತ == SIGTERM){
		g_run = 0;
#ifdef SIGHUP
	}ಅನ್ಯಥಾ ಯದಿ(ಸಙ್ಕೇತ == SIGHUP){
		flag_reload = true;
#endif
#ifdef SIGUSR1
	}ಅನ್ಯಥಾ ಯದಿ(ಸಙ್ಕೇತ == SIGUSR1){
#ifdef WITH_PERSISTENCE
		flag_db_backup = true;
#endif
#endif
#ifdef SIGUSR2
	}ಅನ್ಯಥಾ ಯದಿ(ಸಙ್ಕೇತ == SIGUSR2){
		flag_ವೃಕ್ಷ_print = true;
		flag_xtreport = true;
#endif
#ifdef SIGRTMIN
	}ಅನ್ಯಥಾ ಯದಿ(ಸಙ್ಕೇತ == SIGRTMIN){
		flag_log_rotate = true;
#endif
	}
}


ಶೂನ್ಯ signal__setup(ಶೂನ್ಯ)
{
	ಸಙ್ಕೇತ(SIGINT, handle_signal);
	ಸಙ್ಕೇತ(SIGTERM, handle_signal);
#ifdef SIGHUP
	ಸಙ್ಕೇತ(SIGHUP, handle_signal);
#endif
#ifndef WIN32
	ಸಙ್ಕೇತ(SIGUSR1, handle_signal);
	ಸಙ್ಕೇತ(SIGUSR2, handle_signal);
	ಸಙ್ಕೇತ(SIGPIPE, SIG_IGN);
#endif
#ifdef SIGRTMIN
	ಸಙ್ಕೇತ(SIGRTMIN, handle_signal);
#endif
#ifdef WIN32
	CreateThread(ರಿಕ್ತ, 0, SigThreadProc, ರಿಕ್ತ, 0, ರಿಕ್ತ);
#endif
}


ಪೂರ್ಣಾಙ್ಕ signal__flag_check(ಶೂನ್ಯ)
{
#ifdef WITH_PERSISTENCE
	ಯದಿ(flag_db_backup){
		persist__backup(false);
		flag_db_backup = false;
	}
#endif
	ಯದಿ(flag_log_rotate){
		log__close(db.config);
		log__init(db.config);
		flag_log_rotate = false;
	}
	ಯದಿ(flag_reload){
		ಪೂರ್ಣಾಙ್ಕ rc;
		log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Reloading config.");
		config__read(db.config, true);
		listeners__reload_all_certificates();
		rc = plugin__handle_reload();
		ಯದಿ(rc){
			ಪ್ರತ್ಯಾಗಮ rc;
		}
		mosquitto_security_cleanup(true);
		rc = mosquitto_security_init(true);
		ಯದಿ(rc){
			ಪ್ರತ್ಯಾಗಮ rc;
		}
		rc = mosquitto_security_apply_default();
		ಯದಿ(rc){
			ಪ್ರತ್ಯಾಗಮ rc;
		}
		log__close(db.config);
		log__init(db.config);
		keepalive__cleanup();
		rc = keepalive__init();
		ಯದಿ(rc){
			ಪ್ರತ್ಯಾಗಮ rc;
		}
		broker_control__reload();
#ifdef WITH_BRIDGE
		bridge__reload();
#endif
		flag_reload = false;
	}
	ಯದಿ(flag_ವೃಕ್ಷ_print){
		sub_ವೃಕ್ಷ_print(db.normal_subs, 0);
		sub_ವೃಕ್ಷ_print(db.shared_subs, 0);
		flag_ವೃಕ್ಷ_print = false;
	}
#ifdef WITH_XTREPORT
	ಯದಿ(flag_xtreport){
		xtreport();
		flag_xtreport = false;
	}
#endif

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
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


DWORD WINAPI SigThreadProc(ಶೂನ್ಯ *data)
{
	TCHAR evt_name[MAX_PATH];
	ಸ್ಥಿರ HANDLE evt[MOSQ_MAX_EVTS];
	ಪೂರ್ಣಾಙ್ಕ pid = GetCurrentProcessId();
	ಧ್ರುವ ವರ್ಣ *evt_names[MOSQ_MAX_EVTS] = {
		"shutdown",
		"reload",
		"backup",
		"log_rotate",
		"tree_print",
		"xtreport"
	};

	UNUSED(data);

	ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<MOSQ_MAX_EVTS; i++){
		sprintf_s(evt_name, MAX_PATH, "mosq%d_%s", pid, evt_names[i]);
		evt[i] = CreateEvent(ರಿಕ್ತ, FALSE, FALSE, evt_name);
	}

	ಯಾವತ್(g_run){
		ಪೂರ್ಣಾಙ್ಕ wr = WaitForMultipleObjects(MOSQ_MAX_EVTS, evt, FALSE, INFINITE);
		ವಿಕಲ್ಪ(wr){
			ಪಕ್ಷ WAIT_OBJECT_0 + 0:
				handle_signal(SIGINT);
				ಭಙ್ಗ;
			ಪಕ್ಷ WAIT_OBJECT_0 + 1:
				flag_reload = true;
				ಅನುವರ್ತ;
			ಪಕ್ಷ WAIT_OBJECT_0 + 2:
#ifdef WITH_PERSISTENCE
				flag_db_backup = true;
#endif
				ಅನುವರ್ತ;
				ಭಙ್ಗ;
			ಪಕ್ಷ WAIT_OBJECT_0 + 3:
				flag_log_rotate = true;
				ಅನುವರ್ತ;
			ಪಕ್ಷ WAIT_OBJECT_0 + 4:
				flag_ವೃಕ್ಷ_print = true;
				ಅನುವರ್ತ;
			ಪಕ್ಷ WAIT_OBJECT_0 + 5:
				flag_xtreport = true;
				ಅನುವರ್ತ;
		}
	}
	ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<MOSQ_MAX_EVTS; i++){
		ಯದಿ(evt[i]){
			CloseHandle(evt[i]);
		}
	}
	ಪ್ರತ್ಯಾಗಮ 0;
}
#undef MOSQ_MAX_EVTS
#endif
