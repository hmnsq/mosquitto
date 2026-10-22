/*
Copyright (c) 2011-2021 Roger Light <roger@atchoo.org>

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
*/

#જો defined(WIN32) || defined(__CYGWIN__)

#અન્તર્ભાવ "config.h"

#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ <windows.h>

બાહ્ય પૂર્ણાંક g_run;
SERVICE_STATUS_HANDLE service_handle = 0;
સ્થિર SERVICE_STATUS service_status;
પૂર્ણાંક મુખ્ય(પૂર્ણાંક argc, અક્ષર *argv[]);


સ્થિર અક્ષર *fix_name(અક્ષર *name)
{
	size_t len;

	જો(strrchr(name, '\\')){
		name = strrchr(name, '\\') + 1;
	}
	len = strlen(name);
	જો(!strcasecmp(&name[len-4], ".exe")){
		name[len-4] = '\0';
	}

	પાછા name;
}


સ્થિર શૂન્ય print_error(શૂન્ય)
{
	અક્ષર *buf = ખાલી;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			ખાલી, GetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, ખાલી);

	fprintf(stderr, "Error: %s\n", buf);
	LocalFree(buf);
}


/* Service control callback */
શૂન્ય __stdcall service_handler(DWORD fdwControl)
{
	પસંદગી(fdwControl){
		સ્થિતિ SERVICE_CONTROL_CONTINUE:
			/* Continue from Paused state. */
			તોડો;
		સ્થિતિ SERVICE_CONTROL_PAUSE:
			/* Pause service. */
			તોડો;
		સ્થિતિ SERVICE_CONTROL_SHUTDOWN:
		/* System is shutting down. */
		સ્થિતિ SERVICE_CONTROL_STOP:
			/* Service should stop. */
			service_status.dwCurrentState = SERVICE_STOP_PENDING;
			SetServiceStatus(service_handle, &service_status);
			g_run = 0;
			તોડો;
	}
}


/* Function called when started as a service. */
શૂન્ય __stdcall service_main(DWORD dwArgc, LPTSTR *lpszArgv)
{
	અક્ષર **argv;
	પૂર્ણાંક argc = 1;
	અક્ષર conf_path[MAX_PATH + 20];
	પૂર્ણાંક rc;
	અક્ષર *name;
	અક્ષર env_name[MAX_PATH];

	UNUSED(dwArgc);

	name = fix_name(lpszArgv[0]);
	snprintf(env_name, કદ(env_name), "%s_DIR", name);
	માટે(પૂર્ણાંક i=0; i<strlen(env_name); i++){
		જો(env_name[i]>='A' && env_name[i]<='Z'){
			/* Keep upper case letter */
		}નહીંતર જો(env_name[i]>='0' && env_name[i]<='9'){
			/* Keep number */
		}નહીંતર જો(env_name[i]>='a' && env_name[i]<='z'){
			/* Convert lower case to upper case */
			env_name[i] -= 32;
		}નહીંતર{
			env_name[i] = '_';
		}
	}

	service_handle = RegisterServiceCtrlHandler(name, service_handler);
	જો(service_handle){
		memset(conf_path, 0, કદ(conf_path));
		rc = GetEnvironmentVariable(env_name, conf_path, MAX_PATH);
		જો(!rc || rc == MAX_PATH){
			service_status.dwCurrentState = SERVICE_STOPPED;
			SetServiceStatus(service_handle, &service_status);
			પાછા;
		}
		strcat(conf_path, "\\mosquitto.conf");

		argv = mosquitto_malloc(કદ(અક્ષર *)*3);
		argv[0] = name;
		argv[1] = "-c";
		argv[2] = conf_path;
		argc = 3;

		service_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		service_status.dwCurrentState = SERVICE_RUNNING;
		service_status.dwControlsAccepted = SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_STOP;
		service_status.dwWin32ExitCode = NO_ERROR;
		service_status.dwCheckPoint = 0;
		SetServiceStatus(service_handle, &service_status);

		મુખ્ય(argc, argv);
		mosquitto_FREE(argv);

		service_status.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(service_handle, &service_status);
	}
}


શૂન્ય service_install(અક્ષર *name)
{
	SC_HANDLE sc_manager, svc_handle;
	અક્ષર service_string[MAX_PATH + 20];
	અક્ષર exe_path[MAX_PATH + 1];
	અક્ષર display_name[MAX_PATH+1];
	SERVICE_DESCRIPTION svc_desc;

	memset(exe_path, 0, કદ(exe_path));
	જો(GetModuleFileName(ખાલી, exe_path, MAX_PATH) == MAX_PATH){
		fprintf(stderr, "Error: Path too long.\n");
		પાછા;
	}
	snprintf(service_string, કદ(service_string), "\"%s\" run", exe_path);

	name = fix_name(name);

	sc_manager = OpenSCManager(ખાલી, ખાલી, SC_MANAGER_CREATE_SERVICE);
	જો(sc_manager){
		જો(!strcmp(name, "mosquitto")){
			snprintf(display_name, કદ(display_name), "Mosquitto Broker");
		}નહીંતર{
			snprintf(display_name, કદ(display_name), "Mosquitto Broker (%s.exe)", name);
		}

		svc_handle = CreateService(sc_manager, name, display_name,
				SERVICE_START | SERVICE_STOP | SERVICE_CHANGE_CONFIG,
				SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
				service_string, ખાલી, ખાલી, ખાલી, ખાલી, ખાલી);

		જો(svc_handle){
			memset(&svc_desc, 0, કદ(svc_desc));
			svc_desc.lpDescription = "Eclipse Mosquitto MQTT v5/v3.1.1 broker";
			ChangeServiceConfig2(svc_handle, SERVICE_CONFIG_DESCRIPTION, &svc_desc);
			CloseServiceHandle(svc_handle);
		}નહીંતર{
			print_error();
		}
		CloseServiceHandle(sc_manager);
	}નહીંતર{
		print_error();
	}
}


શૂન્ય service_uninstall(અક્ષર *name)
{
	SC_HANDLE sc_manager, svc_handle;
	SERVICE_STATUS status;

	name = fix_name(name);

	sc_manager = OpenSCManager(ખાલી, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CONNECT);
	જો(sc_manager){
		svc_handle = OpenService(sc_manager, name, SERVICE_QUERY_STATUS | DELETE);
		જો(svc_handle){
			જો(QueryServiceStatus(svc_handle, &status)){
				જો(status.dwCurrentState == SERVICE_STOPPED){
					DeleteService(svc_handle);
				}
			}
			CloseServiceHandle(svc_handle);
		}નહીંતર{
			print_error();
		}
		CloseServiceHandle(sc_manager);
	}નહીંતર{
		print_error();
	}
}


શૂન્ય service_run(અક્ષર *name)
{
	SERVICE_TABLE_ENTRY ste[] = {
		{ name, service_main },
		{ ખાલી, ખાલી }
	};
	name = fix_name(name);
	ste[0].lpServiceName = name;
	StartServiceCtrlDispatcher(ste);
}

#endif
