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
#अगर_घोषित WIN32
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <windows.h>
#पूर्ण_अगर

#समावेश "config.h"

#समावेश <stdio.h>
#समावेश <stdbool.h>
#समावेश <signal.h>

#समावेश "mosquitto_broker_internal.h"

बाह्य पूर्णांक g_run;

अटल bool flag_reload = false;
अटल bool flag_log_rotate = false;
#अगर_घोषित WITH_PERSISTENCE
अटल bool flag_db_backup = false;
#पूर्ण_अगर
अटल bool flag_वृक्ष_print = false;
अटल bool flag_xtreport = false;


अटल व्योम handle_signal(पूर्णांक संकेत)
{
	UNUSED(संकेत);

	अगर(संकेत == संक_विघ्न || संकेत == संक_इति){
		g_run = 0;
#अगर_घोषित SIGHUP
	}अन्यथा अगर(संकेत == SIGHUP){
		flag_reload = true;
#पूर्ण_अगर
#अगर_घोषित SIGUSR1
	}अन्यथा अगर(संकेत == SIGUSR1){
#अगर_घोषित WITH_PERSISTENCE
		flag_db_backup = true;
#पूर्ण_अगर
#पूर्ण_अगर
#अगर_घोषित SIGUSR2
	}अन्यथा अगर(संकेत == SIGUSR2){
		flag_वृक्ष_print = true;
		flag_xtreport = true;
#पूर्ण_अगर
#अगर_घोषित SIGRTMIN
	}अन्यथा अगर(संकेत == SIGRTMIN){
		flag_log_rotate = true;
#पूर्ण_अगर
	}
}


व्योम signal__setup(व्योम)
{
	संकेत(संक_विघ्न, handle_signal);
	संकेत(संक_इति, handle_signal);
#अगर_घोषित SIGHUP
	संकेत(SIGHUP, handle_signal);
#पूर्ण_अगर
#अगर_अघोषित WIN32
	संकेत(SIGUSR1, handle_signal);
	संकेत(SIGUSR2, handle_signal);
	संकेत(SIGPIPE, संक_छोड़ो);
#पूर्ण_अगर
#अगर_घोषित SIGRTMIN
	संकेत(SIGRTMIN, handle_signal);
#पूर्ण_अगर
#अगर_घोषित WIN32
	CreateThread(शून्य, 0, SigThreadProc, शून्य, 0, शून्य);
#पूर्ण_अगर
}


पूर्णांक signal__flag_check(व्योम)
{
#अगर_घोषित WITH_PERSISTENCE
	अगर(flag_db_backup){
		persist__backup(false);
		flag_db_backup = false;
	}
#पूर्ण_अगर
	अगर(flag_log_rotate){
		log__close(db.config);
		log__init(db.config);
		flag_log_rotate = false;
	}
	अगर(flag_reload){
		पूर्णांक rc;
		log__printf(शून्य, MOSQ_LOG_INFO, "Reloading config.");
		config__read(db.config, true);
		listeners__reload_all_certificates();
		rc = plugin__handle_reload();
		अगर(rc){
			वापस rc;
		}
		mosquitto_security_cleanup(true);
		rc = mosquitto_security_init(true);
		अगर(rc){
			वापस rc;
		}
		rc = mosquitto_security_apply_default();
		अगर(rc){
			वापस rc;
		}
		log__close(db.config);
		log__init(db.config);
		keepalive__cleanup();
		rc = keepalive__init();
		अगर(rc){
			वापस rc;
		}
		broker_control__reload();
#अगर_घोषित WITH_BRIDGE
		bridge__reload();
#पूर्ण_अगर
		flag_reload = false;
	}
	अगर(flag_वृक्ष_print){
		sub_वृक्ष_print(db.normal_subs, 0);
		sub_वृक्ष_print(db.shared_subs, 0);
		flag_वृक्ष_print = false;
	}
#अगर_घोषित WITH_XTREPORT
	अगर(flag_xtreport){
		xtreport();
		flag_xtreport = false;
	}
#पूर्ण_अगर

	वापस MOSQ_ERR_SUCCESS;
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
#अगर_घोषित WIN32

#घोषणा MOSQ_MAX_EVTS 6


DWORD WINAPI SigThreadProc(व्योम *तथ्य)
{
	TCHAR evt_name[MAX_PATH];
	अटल HANDLE evt[MOSQ_MAX_EVTS];
	पूर्णांक pid = GetCurrentProcessId();
	स्थिर अक्षर *evt_names[MOSQ_MAX_EVTS] = {
		"shutdown",
		"reload",
		"backup",
		"log_rotate",
		"tree_print",
		"xtreport"
	};

	UNUSED(तथ्य);

	क्रम(पूर्णांक i=0; i<MOSQ_MAX_EVTS; i++){
		sprintf_s(evt_name, MAX_PATH, "mosq%d_%s", pid, evt_names[i]);
		evt[i] = CreateEvent(शून्य, FALSE, FALSE, evt_name);
	}

	जबतक(g_run){
		पूर्णांक wr = WaitForMultipleObjects(MOSQ_MAX_EVTS, evt, FALSE, INFINITE);
		चयन(wr){
			हाल WAIT_OBJECT_0 + 0:
				handle_signal(संक_विघ्न);
				अवरोध;
			हाल WAIT_OBJECT_0 + 1:
				flag_reload = true;
				जारी;
			हाल WAIT_OBJECT_0 + 2:
#अगर_घोषित WITH_PERSISTENCE
				flag_db_backup = true;
#पूर्ण_अगर
				जारी;
				अवरोध;
			हाल WAIT_OBJECT_0 + 3:
				flag_log_rotate = true;
				जारी;
			हाल WAIT_OBJECT_0 + 4:
				flag_वृक्ष_print = true;
				जारी;
			हाल WAIT_OBJECT_0 + 5:
				flag_xtreport = true;
				जारी;
		}
	}
	क्रम(पूर्णांक i=0; i<MOSQ_MAX_EVTS; i++){
		अगर(evt[i]){
			CloseHandle(evt[i]);
		}
	}
	वापस 0;
}
#अघोषित MOSQ_MAX_EVTS
#पूर्ण_अगर
