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

#ଯଦି defined(WIN32) || defined(__CYGWIN__)

#ଅନ୍ତର୍ଭାଵ "config.h"

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ <windows.h>

ବାହ୍ୟ ପୂର୍ଣ୍ଣସଂଖ୍ୟା g_run;
SERVICE_STATUS_HANDLE service_handle = 0;
ସ୍ଥିର SERVICE_STATUS service_status;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୁଖ୍ୟ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅକ୍ଷର *argv[]);


ସ୍ଥିର ଅକ୍ଷର *fix_name(ଅକ୍ଷର *name)
{
	size_t len;

	ଯଦି(strrchr(name, '\\')){
		name = strrchr(name, '\\') + 1;
	}
	len = strlen(name);
	ଯଦି(!strcasecmp(&name[len-4], ".exe")){
		name[len-4] = '\0';
	}

	ଫେରନ୍ତୁ name;
}


ସ୍ଥିର ଶୂନ୍ୟ print_error(ଶୂନ୍ୟ)
{
	ଅକ୍ଷର *buf = ଖାଲି;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			ଖାଲି, GetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, ଖାଲି);

	fprintf(stderr, "Error: %s\n", buf);
	LocalFree(buf);
}


/* Service control callback */
ଶୂନ୍ୟ __stdcall service_handler(DWORD fdwControl)
{
	ବାଛ(fdwControl){
		ସ୍ଥିତି SERVICE_CONTROL_CONTINUE:
			/* Continue from Paused state. */
			ଭାଙ୍ଗ;
		ସ୍ଥିତି SERVICE_CONTROL_PAUSE:
			/* Pause service. */
			ଭାଙ୍ଗ;
		ସ୍ଥିତି SERVICE_CONTROL_SHUTDOWN:
		/* System is shutting down. */
		ସ୍ଥିତି SERVICE_CONTROL_STOP:
			/* Service should stop. */
			service_status.dwCurrentState = SERVICE_STOP_PENDING;
			SetServiceStatus(service_handle, &service_status);
			g_run = 0;
			ଭାଙ୍ଗ;
	}
}


/* Function called when started as a service. */
ଶୂନ୍ୟ __stdcall service_main(DWORD dwArgc, LPTSTR *lpszArgv)
{
	ଅକ୍ଷର **argv;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc = 1;
	ଅକ୍ଷର conf_path[MAX_PATH + 20];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ଅକ୍ଷର *name;
	ଅକ୍ଷର env_name[MAX_PATH];

	UNUSED(dwArgc);

	name = fix_name(lpszArgv[0]);
	snprintf(env_name, ଆକାର(env_name), "%s_DIR", name);
	ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<strlen(env_name); i++){
		ଯଦି(env_name[i]>='A' && env_name[i]<='Z'){
			/* Keep upper case letter */
		}ଅନ୍ୟଥା ଯଦି(env_name[i]>='0' && env_name[i]<='9'){
			/* Keep number */
		}ଅନ୍ୟଥା ଯଦି(env_name[i]>='a' && env_name[i]<='z'){
			/* Convert lower case to upper case */
			env_name[i] -= 32;
		}ଅନ୍ୟଥା{
			env_name[i] = '_';
		}
	}

	service_handle = RegisterServiceCtrlHandler(name, service_handler);
	ଯଦି(service_handle){
		memset(conf_path, 0, ଆକାର(conf_path));
		rc = GetEnvironmentVariable(env_name, conf_path, MAX_PATH);
		ଯଦି(!rc || rc == MAX_PATH){
			service_status.dwCurrentState = SERVICE_STOPPED;
			SetServiceStatus(service_handle, &service_status);
			ଫେରନ୍ତୁ;
		}
		strcat(conf_path, "\\mosquitto.conf");

		argv = mosquitto_malloc(ଆକାର(ଅକ୍ଷର *)*3);
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

		ମୁଖ୍ୟ(argc, argv);
		mosquitto_FREE(argv);

		service_status.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(service_handle, &service_status);
	}
}


ଶୂନ୍ୟ service_install(ଅକ୍ଷର *name)
{
	SC_HANDLE sc_manager, svc_handle;
	ଅକ୍ଷର service_string[MAX_PATH + 20];
	ଅକ୍ଷର exe_path[MAX_PATH + 1];
	ଅକ୍ଷର display_name[MAX_PATH+1];
	SERVICE_DESCRIPTION svc_desc;

	memset(exe_path, 0, ଆକାର(exe_path));
	ଯଦି(GetModuleFileName(ଖାଲି, exe_path, MAX_PATH) == MAX_PATH){
		fprintf(stderr, "Error: Path too long.\n");
		ଫେରନ୍ତୁ;
	}
	snprintf(service_string, ଆକାର(service_string), "\"%s\" run", exe_path);

	name = fix_name(name);

	sc_manager = OpenSCManager(ଖାଲି, ଖାଲି, SC_MANAGER_CREATE_SERVICE);
	ଯଦି(sc_manager){
		ଯଦି(!strcmp(name, "mosquitto")){
			snprintf(display_name, ଆକାର(display_name), "Mosquitto Broker");
		}ଅନ୍ୟଥା{
			snprintf(display_name, ଆକାର(display_name), "Mosquitto Broker (%s.exe)", name);
		}

		svc_handle = CreateService(sc_manager, name, display_name,
				SERVICE_START | SERVICE_STOP | SERVICE_CHANGE_CONFIG,
				SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
				service_string, ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି);

		ଯଦି(svc_handle){
			memset(&svc_desc, 0, ଆକାର(svc_desc));
			svc_desc.lpDescription = "Eclipse Mosquitto MQTT v5/v3.1.1 broker";
			ChangeServiceConfig2(svc_handle, SERVICE_CONFIG_DESCRIPTION, &svc_desc);
			CloseServiceHandle(svc_handle);
		}ଅନ୍ୟଥା{
			print_error();
		}
		CloseServiceHandle(sc_manager);
	}ଅନ୍ୟଥା{
		print_error();
	}
}


ଶୂନ୍ୟ service_uninstall(ଅକ୍ଷର *name)
{
	SC_HANDLE sc_manager, svc_handle;
	SERVICE_STATUS status;

	name = fix_name(name);

	sc_manager = OpenSCManager(ଖାଲି, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CONNECT);
	ଯଦି(sc_manager){
		svc_handle = OpenService(sc_manager, name, SERVICE_QUERY_STATUS | DELETE);
		ଯଦି(svc_handle){
			ଯଦି(QueryServiceStatus(svc_handle, &status)){
				ଯଦି(status.dwCurrentState == SERVICE_STOPPED){
					DeleteService(svc_handle);
				}
			}
			CloseServiceHandle(svc_handle);
		}ଅନ୍ୟଥା{
			print_error();
		}
		CloseServiceHandle(sc_manager);
	}ଅନ୍ୟଥା{
		print_error();
	}
}


ଶୂନ୍ୟ service_run(ଅକ୍ଷର *name)
{
	SERVICE_TABLE_ENTRY ste[] = {
		{ name, service_main },
		{ ଖାଲି, ଖାଲି }
	};
	name = fix_name(name);
	ste[0].lpServiceName = name;
	StartServiceCtrlDispatcher(ste);
}

#endif
