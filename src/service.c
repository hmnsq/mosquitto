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

#எனில் defined(WIN32) || defined(__CYGWIN__)

#அந்தர்பாவ "config.h"

#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ <windows.h>

வெளி முழு g_run;
SERVICE_STATUS_HANDLE service_handle = 0;
நிலையான SERVICE_STATUS service_status;
முழு முதன்மை(முழு argc, எழுத்து *argv[]);


நிலையான எழுத்து *fix_name(எழுத்து *name)
{
	size_t len;

	எனில்(strrchr(name, '\\')){
		name = strrchr(name, '\\') + 1;
	}
	len = strlen(name);
	எனில்(!strcasecmp(&name[len-4], ".exe")){
		name[len-4] = '\0';
	}

	திரும்பு name;
}


நிலையான வெறுமை print_error(வெறுமை)
{
	எழுத்து *buf = வெற்று;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			வெற்று, GetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, வெற்று);

	fprintf(stderr, "Error: %s\n", buf);
	LocalFree(buf);
}


/* Service control callback */
வெறுமை __stdcall service_handler(DWORD fdwControl)
{
	தேர்வு(fdwControl){
		நிலை SERVICE_CONTROL_CONTINUE:
			/* Continue from Paused state. */
			நிறுத்து;
		நிலை SERVICE_CONTROL_PAUSE:
			/* Pause service. */
			நிறுத்து;
		நிலை SERVICE_CONTROL_SHUTDOWN:
		/* System is shutting down. */
		நிலை SERVICE_CONTROL_STOP:
			/* Service should stop. */
			service_status.dwCurrentState = SERVICE_STOP_PENDING;
			SetServiceStatus(service_handle, &service_status);
			g_run = 0;
			நிறுத்து;
	}
}


/* Function called when started as a service. */
வெறுமை __stdcall service_main(DWORD dwArgc, LPTSTR *lpszArgv)
{
	எழுத்து **argv;
	முழு argc = 1;
	எழுத்து conf_path[MAX_PATH + 20];
	முழு rc;
	எழுத்து *name;
	எழுத்து env_name[MAX_PATH];

	UNUSED(dwArgc);

	name = fix_name(lpszArgv[0]);
	snprintf(env_name, அளவு(env_name), "%s_DIR", name);
	க்கு(முழு i=0; i<strlen(env_name); i++){
		எனில்(env_name[i]>='A' && env_name[i]<='Z'){
			/* Keep upper case letter */
		}இல்லையெனில் எனில்(env_name[i]>='0' && env_name[i]<='9'){
			/* Keep number */
		}இல்லையெனில் எனில்(env_name[i]>='a' && env_name[i]<='z'){
			/* Convert lower case to upper case */
			env_name[i] -= 32;
		}இல்லையெனில்{
			env_name[i] = '_';
		}
	}

	service_handle = RegisterServiceCtrlHandler(name, service_handler);
	எனில்(service_handle){
		memset(conf_path, 0, அளவு(conf_path));
		rc = GetEnvironmentVariable(env_name, conf_path, MAX_PATH);
		எனில்(!rc || rc == MAX_PATH){
			service_status.dwCurrentState = SERVICE_STOPPED;
			SetServiceStatus(service_handle, &service_status);
			திரும்பு;
		}
		strcat(conf_path, "\\mosquitto.conf");

		argv = mosquitto_malloc(அளவு(எழுத்து *)*3);
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

		முதன்மை(argc, argv);
		mosquitto_FREE(argv);

		service_status.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(service_handle, &service_status);
	}
}


வெறுமை service_install(எழுத்து *name)
{
	SC_HANDLE sc_manager, svc_handle;
	எழுத்து service_string[MAX_PATH + 20];
	எழுத்து exe_path[MAX_PATH + 1];
	எழுத்து display_name[MAX_PATH+1];
	SERVICE_DESCRIPTION svc_desc;

	memset(exe_path, 0, அளவு(exe_path));
	எனில்(GetModuleFileName(வெற்று, exe_path, MAX_PATH) == MAX_PATH){
		fprintf(stderr, "Error: Path too long.\n");
		திரும்பு;
	}
	snprintf(service_string, அளவு(service_string), "\"%s\" run", exe_path);

	name = fix_name(name);

	sc_manager = OpenSCManager(வெற்று, வெற்று, SC_MANAGER_CREATE_SERVICE);
	எனில்(sc_manager){
		எனில்(!strcmp(name, "mosquitto")){
			snprintf(display_name, அளவு(display_name), "Mosquitto Broker");
		}இல்லையெனில்{
			snprintf(display_name, அளவு(display_name), "Mosquitto Broker (%s.exe)", name);
		}

		svc_handle = CreateService(sc_manager, name, display_name,
				SERVICE_START | SERVICE_STOP | SERVICE_CHANGE_CONFIG,
				SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
				service_string, வெற்று, வெற்று, வெற்று, வெற்று, வெற்று);

		எனில்(svc_handle){
			memset(&svc_desc, 0, அளவு(svc_desc));
			svc_desc.lpDescription = "Eclipse Mosquitto MQTT v5/v3.1.1 broker";
			ChangeServiceConfig2(svc_handle, SERVICE_CONFIG_DESCRIPTION, &svc_desc);
			CloseServiceHandle(svc_handle);
		}இல்லையெனில்{
			print_error();
		}
		CloseServiceHandle(sc_manager);
	}இல்லையெனில்{
		print_error();
	}
}


வெறுமை service_uninstall(எழுத்து *name)
{
	SC_HANDLE sc_manager, svc_handle;
	SERVICE_STATUS status;

	name = fix_name(name);

	sc_manager = OpenSCManager(வெற்று, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CONNECT);
	எனில்(sc_manager){
		svc_handle = OpenService(sc_manager, name, SERVICE_QUERY_STATUS | DELETE);
		எனில்(svc_handle){
			எனில்(QueryServiceStatus(svc_handle, &status)){
				எனில்(status.dwCurrentState == SERVICE_STOPPED){
					DeleteService(svc_handle);
				}
			}
			CloseServiceHandle(svc_handle);
		}இல்லையெனில்{
			print_error();
		}
		CloseServiceHandle(sc_manager);
	}இல்லையெனில்{
		print_error();
	}
}


வெறுமை service_run(எழுத்து *name)
{
	SERVICE_TABLE_ENTRY ste[] = {
		{ name, service_main },
		{ வெற்று, வெற்று }
	};
	name = fix_name(name);
	ste[0].lpServiceName = name;
	StartServiceCtrlDispatcher(ste);
}

#endif
