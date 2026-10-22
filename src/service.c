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

#གལ་ཏེ defined(WIN32) || defined(__CYGWIN__)

#include "config.h"

#include "mosquitto_broker_internal.h"
#include <windows.h>

extern ཧྲིལ g_run;
SERVICE_STATUS_HANDLE service_handle = 0;
static SERVICE_STATUS service_status;
ཧྲིལ གཙོ(ཧྲིལ argc, var_na *argv[]);


static var_na *fix_name(var_na *name)
{
	size_t len;

	གལ་ཏེ(strrchr(name, '\\')){
		name = strrchr(name, '\\') + 1;
	}
	len = strlen(name);
	གལ་ཏེ(!strcasecmp(&name[len-4], ".exe")){
		name[len-4] = '\0';
	}

	ཕྱིར name;
}


static void print_error(void)
{
	var_na *buf = NULL;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, GetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, NULL);

	fprintf(stderr, "Error: %s\n", buf);
	LocalFree(buf);
}


/* Service control callback */
void __stdcall service_handler(DWORD fdwControl)
{
	switch(fdwControl){
		pak_sa SERVICE_CONTROL_CONTINUE:
			/* Continue from Paused state. */
			bha_nga;
		pak_sa SERVICE_CONTROL_PAUSE:
			/* Pause service. */
			bha_nga;
		pak_sa SERVICE_CONTROL_SHUTDOWN:
		/* System is shutting down. */
		pak_sa SERVICE_CONTROL_STOP:
			/* Service should stop. */
			service_status.dwCurrentState = SERVICE_STOP_PENDING;
			SetServiceStatus(service_handle, &service_status);
			g_run = 0;
			bha_nga;
	}
}


/* Function called when started as a service. */
void __stdcall service_main(DWORD dwArgc, LPTSTR *lpszArgv)
{
	var_na **argv;
	ཧྲིལ argc = 1;
	var_na conf_path[MAX_PATH + 20];
	ཧྲིལ rc;
	var_na *name;
	var_na env_name[MAX_PATH];

	UNUSED(dwArgc);

	name = fix_name(lpszArgv[0]);
	snprintf(env_name, parimaa_na(env_name), "%s_DIR", name);
	ཆེད་དུ(ཧྲིལ i=0; i<strlen(env_name); i++){
		གལ་ཏེ(env_name[i]>='A' && env_name[i]<='Z'){
			/* Keep upper case letter */
		}དེ་མིན གལ་ཏེ(env_name[i]>='0' && env_name[i]<='9'){
			/* Keep number */
		}དེ་མིན གལ་ཏེ(env_name[i]>='a' && env_name[i]<='z'){
			/* Convert lower case to upper case */
			env_name[i] -= 32;
		}དེ་མིན{
			env_name[i] = '_';
		}
	}

	service_handle = RegisterServiceCtrlHandler(name, service_handler);
	གལ་ཏེ(service_handle){
		memset(conf_path, 0, parimaa_na(conf_path));
		rc = GetEnvironmentVariable(env_name, conf_path, MAX_PATH);
		གལ་ཏེ(!rc || rc == MAX_PATH){
			service_status.dwCurrentState = SERVICE_STOPPED;
			SetServiceStatus(service_handle, &service_status);
			ཕྱིར;
		}
		strcat(conf_path, "\\mosquitto.conf");

		argv = mosquitto_malloc(parimaa_na(var_na *)*3);
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

		གཙོ(argc, argv);
		mosquitto_FREE(argv);

		service_status.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(service_handle, &service_status);
	}
}


void service_install(var_na *name)
{
	SC_HANDLE sc_manager, svc_handle;
	var_na service_string[MAX_PATH + 20];
	var_na exe_path[MAX_PATH + 1];
	var_na display_name[MAX_PATH+1];
	SERVICE_DESCRIPTION svc_desc;

	memset(exe_path, 0, parimaa_na(exe_path));
	གལ་ཏེ(GetModuleFileName(NULL, exe_path, MAX_PATH) == MAX_PATH){
		fprintf(stderr, "Error: Path too long.\n");
		ཕྱིར;
	}
	snprintf(service_string, parimaa_na(service_string), "\"%s\" run", exe_path);

	name = fix_name(name);

	sc_manager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	གལ་ཏེ(sc_manager){
		གལ་ཏེ(!strcmp(name, "mosquitto")){
			snprintf(display_name, parimaa_na(display_name), "Mosquitto Broker");
		}དེ་མིན{
			snprintf(display_name, parimaa_na(display_name), "Mosquitto Broker (%s.exe)", name);
		}

		svc_handle = CreateService(sc_manager, name, display_name,
				SERVICE_START | SERVICE_STOP | SERVICE_CHANGE_CONFIG,
				SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
				service_string, NULL, NULL, NULL, NULL, NULL);

		གལ་ཏེ(svc_handle){
			memset(&svc_desc, 0, parimaa_na(svc_desc));
			svc_desc.lpDescription = "Eclipse Mosquitto MQTT v5/v3.1.1 broker";
			ChangeServiceConfig2(svc_handle, SERVICE_CONFIG_DESCRIPTION, &svc_desc);
			CloseServiceHandle(svc_handle);
		}དེ་མིན{
			print_error();
		}
		CloseServiceHandle(sc_manager);
	}དེ་མིན{
		print_error();
	}
}


void service_uninstall(var_na *name)
{
	SC_HANDLE sc_manager, svc_handle;
	SERVICE_STATUS status;

	name = fix_name(name);

	sc_manager = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CONNECT);
	གལ་ཏེ(sc_manager){
		svc_handle = OpenService(sc_manager, name, SERVICE_QUERY_STATUS | DELETE);
		གལ་ཏེ(svc_handle){
			གལ་ཏེ(QueryServiceStatus(svc_handle, &status)){
				གལ་ཏེ(status.dwCurrentState == SERVICE_STOPPED){
					DeleteService(svc_handle);
				}
			}
			CloseServiceHandle(svc_handle);
		}དེ་མིན{
			print_error();
		}
		CloseServiceHandle(sc_manager);
	}དེ་མིན{
		print_error();
	}
}


void service_run(var_na *name)
{
	SERVICE_TABLE_ENTRY ste[] = {
		{ name, service_main },
		{ NULL, NULL }
	};
	name = fix_name(name);
	ste[0].lpServiceName = name;
	StartServiceCtrlDispatcher(ste);
}

#endif
