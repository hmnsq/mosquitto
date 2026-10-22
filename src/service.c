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

#ಯದಿ defined(WIN32) || defined(__CYGWIN__)

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ <windows.h>

ಬಾಹ್ಯ ಪೂರ್ಣಾಙ್ಕ g_run;
SERVICE_STATUS_HANDLE service_handle = 0;
ಸ್ಥಿರ SERVICE_STATUS service_status;
ಪೂರ್ಣಾಙ್ಕ main(ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ *argv[]);


ಸ್ಥಿರ ವರ್ಣ *fix_name(ವರ್ಣ *name)
{
	size_t len;

	ಯದಿ(strrchr(name, '\\')){
		name = strrchr(name, '\\') + 1;
	}
	len = strlen(name);
	ಯದಿ(!strcasecmp(&name[len-4], ".exe")){
		name[len-4] = '\0';
	}

	ಪ್ರತ್ಯಾಗಮ name;
}


ಸ್ಥಿರ ಶೂನ್ಯ print_error(ಶೂನ್ಯ)
{
	ವರ್ಣ *buf = ರಿಕ್ತ;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			ರಿಕ್ತ, GetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, ರಿಕ್ತ);

	fprintf(stderr, "Error: %s\n", buf);
	LocalFree(buf);
}


/* Service control callback */
ಶೂನ್ಯ __stdcall service_handler(DWORD fdwControl)
{
	ವಿಕಲ್ಪ(fdwControl){
		ಪಕ್ಷ SERVICE_CONTROL_CONTINUE:
			/* Continue from Paused state. */
			ಭಙ್ಗ;
		ಪಕ್ಷ SERVICE_CONTROL_PAUSE:
			/* Pause service. */
			ಭಙ್ಗ;
		ಪಕ್ಷ SERVICE_CONTROL_SHUTDOWN:
		/* System is shutting down. */
		ಪಕ್ಷ SERVICE_CONTROL_STOP:
			/* Service should stop. */
			service_status.dwCurrentState = SERVICE_STOP_PENDING;
			SetServiceStatus(service_handle, &service_status);
			g_run = 0;
			ಭಙ್ಗ;
	}
}


/* Function called when started as a service. */
ಶೂನ್ಯ __stdcall service_main(DWORD dwArgc, LPTSTR *lpszArgv)
{
	ವರ್ಣ **argv;
	ಪೂರ್ಣಾಙ್ಕ argc = 1;
	ವರ್ಣ conf_path[MAX_PATH + 20];
	ಪೂರ್ಣಾಙ್ಕ rc;
	ವರ್ಣ *name;
	ವರ್ಣ env_name[MAX_PATH];

	UNUSED(dwArgc);

	name = fix_name(lpszArgv[0]);
	snprintf(env_name, ಪರಿಮಾಣ(env_name), "%s_DIR", name);
	ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<strlen(env_name); i++){
		ಯದಿ(env_name[i]>='A' && env_name[i]<='Z'){
			/* Keep upper case letter */
		}ಅನ್ಯಥಾ ಯದಿ(env_name[i]>='0' && env_name[i]<='9'){
			/* Keep number */
		}ಅನ್ಯಥಾ ಯದಿ(env_name[i]>='a' && env_name[i]<='z'){
			/* Convert lower case to upper case */
			env_name[i] -= 32;
		}ಅನ್ಯಥಾ{
			env_name[i] = '_';
		}
	}

	service_handle = RegisterServiceCtrlHandler(name, service_handler);
	ಯದಿ(service_handle){
		memset(conf_path, 0, ಪರಿಮಾಣ(conf_path));
		rc = GetEnvironmentVariable(env_name, conf_path, MAX_PATH);
		ಯದಿ(!rc || rc == MAX_PATH){
			service_status.dwCurrentState = SERVICE_STOPPED;
			SetServiceStatus(service_handle, &service_status);
			ಪ್ರತ್ಯಾಗಮ;
		}
		strcat(conf_path, "\\mosquitto.conf");

		argv = mosquitto_malloc(ಪರಿಮಾಣ(ವರ್ಣ *)*3);
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

		main(argc, argv);
		mosquitto_FREE(argv);

		service_status.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(service_handle, &service_status);
	}
}


ಶೂನ್ಯ service_install(ವರ್ಣ *name)
{
	SC_HANDLE sc_manager, svc_handle;
	ವರ್ಣ service_string[MAX_PATH + 20];
	ವರ್ಣ exe_path[MAX_PATH + 1];
	ವರ್ಣ display_name[MAX_PATH+1];
	SERVICE_DESCRIPTION svc_desc;

	memset(exe_path, 0, ಪರಿಮಾಣ(exe_path));
	ಯದಿ(GetModuleFileName(ರಿಕ್ತ, exe_path, MAX_PATH) == MAX_PATH){
		fprintf(stderr, "Error: Path too long.\n");
		ಪ್ರತ್ಯಾಗಮ;
	}
	snprintf(service_string, ಪರಿಮಾಣ(service_string), "\"%s\" run", exe_path);

	name = fix_name(name);

	sc_manager = OpenSCManager(ರಿಕ್ತ, ರಿಕ್ತ, SC_MANAGER_CREATE_SERVICE);
	ಯದಿ(sc_manager){
		ಯದಿ(!strcmp(name, "mosquitto")){
			snprintf(display_name, ಪರಿಮಾಣ(display_name), "Mosquitto Broker");
		}ಅನ್ಯಥಾ{
			snprintf(display_name, ಪರಿಮಾಣ(display_name), "Mosquitto Broker (%s.exe)", name);
		}

		svc_handle = CreateService(sc_manager, name, display_name,
				SERVICE_START | SERVICE_STOP | SERVICE_CHANGE_CONFIG,
				SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
				service_string, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);

		ಯದಿ(svc_handle){
			memset(&svc_desc, 0, ಪರಿಮಾಣ(svc_desc));
			svc_desc.lpDescription = "Eclipse Mosquitto MQTT v5/v3.1.1 broker";
			ChangeServiceConfig2(svc_handle, SERVICE_CONFIG_DESCRIPTION, &svc_desc);
			CloseServiceHandle(svc_handle);
		}ಅನ್ಯಥಾ{
			print_error();
		}
		CloseServiceHandle(sc_manager);
	}ಅನ್ಯಥಾ{
		print_error();
	}
}


ಶೂನ್ಯ service_uninstall(ವರ್ಣ *name)
{
	SC_HANDLE sc_manager, svc_handle;
	SERVICE_STATUS status;

	name = fix_name(name);

	sc_manager = OpenSCManager(ರಿಕ್ತ, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CONNECT);
	ಯದಿ(sc_manager){
		svc_handle = OpenService(sc_manager, name, SERVICE_QUERY_STATUS | DELETE);
		ಯದಿ(svc_handle){
			ಯದಿ(QueryServiceStatus(svc_handle, &status)){
				ಯದಿ(status.dwCurrentState == SERVICE_STOPPED){
					DeleteService(svc_handle);
				}
			}
			CloseServiceHandle(svc_handle);
		}ಅನ್ಯಥಾ{
			print_error();
		}
		CloseServiceHandle(sc_manager);
	}ಅನ್ಯಥಾ{
		print_error();
	}
}


ಶೂನ್ಯ service_run(ವರ್ಣ *name)
{
	SERVICE_TABLE_ENTRY ste[] = {
		{ name, service_main },
		{ ರಿಕ್ತ, ರಿಕ್ತ }
	};
	name = fix_name(name);
	ste[0].lpServiceName = name;
	StartServiceCtrlDispatcher(ste);
}

#endif
