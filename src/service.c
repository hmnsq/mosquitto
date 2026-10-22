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

#എങ്കിൽ defined(WIN32) || defined(__CYGWIN__)

#അന്തര്ഭാവ "config.h"

#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ <windows.h>

ബാഹ്യ പൂർണ്ണസംഖ്യ g_run;
SERVICE_STATUS_HANDLE service_handle = 0;
സ്ഥിതം SERVICE_STATUS service_status;
പൂർണ്ണസംഖ്യ പ്രധാന(പൂർണ്ണസംഖ്യ argc, അക്ഷരം *argv[]);


സ്ഥിതം അക്ഷരം *fix_name(അക്ഷരം *name)
{
	size_t len;

	എങ്കിൽ(strrchr(name, '\\')){
		name = strrchr(name, '\\') + 1;
	}
	len = strlen(name);
	എങ്കിൽ(!strcasecmp(&name[len-4], ".exe")){
		name[len-4] = '\0';
	}

	തിരികെ name;
}


സ്ഥിതം ശൂന്യം print_error(ശൂന്യം)
{
	അക്ഷരം *buf = ശൂന്യ;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			ശൂന്യ, GetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, ശൂന്യ);

	fprintf(stderr, "Error: %s\n", buf);
	LocalFree(buf);
}


/* Service control callback */
ശൂന്യം __stdcall service_handler(DWORD fdwControl)
{
	തിരഞ്ഞെടുപ്പ്(fdwControl){
		അവസ്ഥ SERVICE_CONTROL_CONTINUE:
			/* Continue from Paused state. */
			നിർത്തുക;
		അവസ്ഥ SERVICE_CONTROL_PAUSE:
			/* Pause service. */
			നിർത്തുക;
		അവസ്ഥ SERVICE_CONTROL_SHUTDOWN:
		/* System is shutting down. */
		അവസ്ഥ SERVICE_CONTROL_STOP:
			/* Service should stop. */
			service_status.dwCurrentState = SERVICE_STOP_PENDING;
			SetServiceStatus(service_handle, &service_status);
			g_run = 0;
			നിർത്തുക;
	}
}


/* Function called when started as a service. */
ശൂന്യം __stdcall service_main(DWORD dwArgc, LPTSTR *lpszArgv)
{
	അക്ഷരം **argv;
	പൂർണ്ണസംഖ്യ argc = 1;
	അക്ഷരം conf_path[MAX_PATH + 20];
	പൂർണ്ണസംഖ്യ rc;
	അക്ഷരം *name;
	അക്ഷരം env_name[MAX_PATH];

	UNUSED(dwArgc);

	name = fix_name(lpszArgv[0]);
	snprintf(env_name, വലുപ്പം(env_name), "%s_DIR", name);
	വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<strlen(env_name); i++){
		എങ്കിൽ(env_name[i]>='A' && env_name[i]<='Z'){
			/* Keep upper case letter */
		}അല്ലെങ്കിൽ എങ്കിൽ(env_name[i]>='0' && env_name[i]<='9'){
			/* Keep number */
		}അല്ലെങ്കിൽ എങ്കിൽ(env_name[i]>='a' && env_name[i]<='z'){
			/* Convert lower case to upper case */
			env_name[i] -= 32;
		}അല്ലെങ്കിൽ{
			env_name[i] = '_';
		}
	}

	service_handle = RegisterServiceCtrlHandler(name, service_handler);
	എങ്കിൽ(service_handle){
		memset(conf_path, 0, വലുപ്പം(conf_path));
		rc = GetEnvironmentVariable(env_name, conf_path, MAX_PATH);
		എങ്കിൽ(!rc || rc == MAX_PATH){
			service_status.dwCurrentState = SERVICE_STOPPED;
			SetServiceStatus(service_handle, &service_status);
			തിരികെ;
		}
		strcat(conf_path, "\\mosquitto.conf");

		argv = mosquitto_malloc(വലുപ്പം(അക്ഷരം *)*3);
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

		പ്രധാന(argc, argv);
		mosquitto_FREE(argv);

		service_status.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(service_handle, &service_status);
	}
}


ശൂന്യം service_install(അക്ഷരം *name)
{
	SC_HANDLE sc_manager, svc_handle;
	അക്ഷരം service_string[MAX_PATH + 20];
	അക്ഷരം exe_path[MAX_PATH + 1];
	അക്ഷരം display_name[MAX_PATH+1];
	SERVICE_DESCRIPTION svc_desc;

	memset(exe_path, 0, വലുപ്പം(exe_path));
	എങ്കിൽ(GetModuleFileName(ശൂന്യ, exe_path, MAX_PATH) == MAX_PATH){
		fprintf(stderr, "Error: Path too long.\n");
		തിരികെ;
	}
	snprintf(service_string, വലുപ്പം(service_string), "\"%s\" run", exe_path);

	name = fix_name(name);

	sc_manager = OpenSCManager(ശൂന്യ, ശൂന്യ, SC_MANAGER_CREATE_SERVICE);
	എങ്കിൽ(sc_manager){
		എങ്കിൽ(!strcmp(name, "mosquitto")){
			snprintf(display_name, വലുപ്പം(display_name), "Mosquitto Broker");
		}അല്ലെങ്കിൽ{
			snprintf(display_name, വലുപ്പം(display_name), "Mosquitto Broker (%s.exe)", name);
		}

		svc_handle = CreateService(sc_manager, name, display_name,
				SERVICE_START | SERVICE_STOP | SERVICE_CHANGE_CONFIG,
				SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
				service_string, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ);

		എങ്കിൽ(svc_handle){
			memset(&svc_desc, 0, വലുപ്പം(svc_desc));
			svc_desc.lpDescription = "Eclipse Mosquitto MQTT v5/v3.1.1 broker";
			ChangeServiceConfig2(svc_handle, SERVICE_CONFIG_DESCRIPTION, &svc_desc);
			CloseServiceHandle(svc_handle);
		}അല്ലെങ്കിൽ{
			print_error();
		}
		CloseServiceHandle(sc_manager);
	}അല്ലെങ്കിൽ{
		print_error();
	}
}


ശൂന്യം service_uninstall(അക്ഷരം *name)
{
	SC_HANDLE sc_manager, svc_handle;
	SERVICE_STATUS status;

	name = fix_name(name);

	sc_manager = OpenSCManager(ശൂന്യ, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CONNECT);
	എങ്കിൽ(sc_manager){
		svc_handle = OpenService(sc_manager, name, SERVICE_QUERY_STATUS | DELETE);
		എങ്കിൽ(svc_handle){
			എങ്കിൽ(QueryServiceStatus(svc_handle, &status)){
				എങ്കിൽ(status.dwCurrentState == SERVICE_STOPPED){
					DeleteService(svc_handle);
				}
			}
			CloseServiceHandle(svc_handle);
		}അല്ലെങ്കിൽ{
			print_error();
		}
		CloseServiceHandle(sc_manager);
	}അല്ലെങ്കിൽ{
		print_error();
	}
}


ശൂന്യം service_run(അക്ഷരം *name)
{
	SERVICE_TABLE_ENTRY ste[] = {
		{ name, service_main },
		{ ശൂന്യ, ശൂന്യ }
	};
	name = fix_name(name);
	ste[0].lpServiceName = name;
	StartServiceCtrlDispatcher(ste);
}

#endif
