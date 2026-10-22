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

#𐤀𐤌 defined(WIN32) || defined(__CYGWIN__)

#ضمن "config.h"

#ضمن "mosquitto_broker_internal.h"
#ضمن <windows.h>

خارجي صحيح g_run;
SERVICE_STATUS_HANDLE service_handle = 0;
ساكن SERVICE_STATUS service_status;
صحيح main(صحيح argc, حرف *argv[]);


ساكن حرف *fix_name(حرف *name)
{
	size_t len;

	𐤀𐤌(strrchr(name, '\\')){
		name = strrchr(name, '\\') + 1;
	}
	len = strlen(name);
	𐤀𐤌(!strcasecmp(&name[len-4], ".exe")){
		name[len-4] = '\0';
	}

	أرجع name;
}


ساكن فراغ print_error(فراغ)
{
	حرف *buf = عدم;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			عدم, GetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, عدم);

	fprintf(stderr, "Error: %s\n", buf);
	LocalFree(buf);
}


/* Service control callback */
فراغ __stdcall service_handler(DWORD fdwControl)
{
	بدل(fdwControl){
		حالة SERVICE_CONTROL_CONTINUE:
			/* Continue from Paused state. */
			اكسر;
		حالة SERVICE_CONTROL_PAUSE:
			/* Pause service. */
			اكسر;
		حالة SERVICE_CONTROL_SHUTDOWN:
		/* System is shutting down. */
		حالة SERVICE_CONTROL_STOP:
			/* Service should stop. */
			service_status.dwCurrentState = SERVICE_STOP_PENDING;
			SetServiceStatus(service_handle, &service_status);
			g_run = 0;
			اكسر;
	}
}


/* Function called when started as a service. */
فراغ __stdcall service_main(DWORD dwArgc, LPTSTR *lpszArgv)
{
	حرف **argv;
	صحيح argc = 1;
	حرف conf_path[MAX_PATH + 20];
	صحيح rc;
	حرف *name;
	حرف env_name[MAX_PATH];

	UNUSED(dwArgc);

	name = fix_name(lpszArgv[0]);
	snprintf(env_name, حجم(env_name), "%s_DIR", name);
	لكل(صحيح i=0; i<strlen(env_name); i++){
		𐤀𐤌(env_name[i]>='A' && env_name[i]<='Z'){
			/* Keep upper case letter */
		}وإلا 𐤀𐤌(env_name[i]>='0' && env_name[i]<='9'){
			/* Keep number */
		}وإلا 𐤀𐤌(env_name[i]>='a' && env_name[i]<='z'){
			/* Convert lower case to upper case */
			env_name[i] -= 32;
		}وإلا{
			env_name[i] = '_';
		}
	}

	service_handle = RegisterServiceCtrlHandler(name, service_handler);
	𐤀𐤌(service_handle){
		memset(conf_path, 0, حجم(conf_path));
		rc = GetEnvironmentVariable(env_name, conf_path, MAX_PATH);
		𐤀𐤌(!rc || rc == MAX_PATH){
			service_status.dwCurrentState = SERVICE_STOPPED;
			SetServiceStatus(service_handle, &service_status);
			أرجع;
		}
		strcat(conf_path, "\\mosquitto.conf");

		argv = mosquitto_malloc(حجم(حرف *)*3);
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


فراغ service_install(حرف *name)
{
	SC_HANDLE sc_manager, svc_handle;
	حرف service_string[MAX_PATH + 20];
	حرف exe_path[MAX_PATH + 1];
	حرف display_name[MAX_PATH+1];
	SERVICE_DESCRIPTION svc_desc;

	memset(exe_path, 0, حجم(exe_path));
	𐤀𐤌(GetModuleFileName(عدم, exe_path, MAX_PATH) == MAX_PATH){
		fprintf(stderr, "Error: Path too long.\n");
		أرجع;
	}
	snprintf(service_string, حجم(service_string), "\"%s\" run", exe_path);

	name = fix_name(name);

	sc_manager = OpenSCManager(عدم, عدم, SC_MANAGER_CREATE_SERVICE);
	𐤀𐤌(sc_manager){
		𐤀𐤌(!strcmp(name, "mosquitto")){
			snprintf(display_name, حجم(display_name), "Mosquitto Broker");
		}وإلا{
			snprintf(display_name, حجم(display_name), "Mosquitto Broker (%s.exe)", name);
		}

		svc_handle = CreateService(sc_manager, name, display_name,
				SERVICE_START | SERVICE_STOP | SERVICE_CHANGE_CONFIG,
				SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
				service_string, عدم, عدم, عدم, عدم, عدم);

		𐤀𐤌(svc_handle){
			memset(&svc_desc, 0, حجم(svc_desc));
			svc_desc.lpDescription = "Eclipse Mosquitto MQTT v5/v3.1.1 broker";
			ChangeServiceConfig2(svc_handle, SERVICE_CONFIG_DESCRIPTION, &svc_desc);
			CloseServiceHandle(svc_handle);
		}وإلا{
			print_error();
		}
		CloseServiceHandle(sc_manager);
	}وإلا{
		print_error();
	}
}


فراغ service_uninstall(حرف *name)
{
	SC_HANDLE sc_manager, svc_handle;
	SERVICE_STATUS status;

	name = fix_name(name);

	sc_manager = OpenSCManager(عدم, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CONNECT);
	𐤀𐤌(sc_manager){
		svc_handle = OpenService(sc_manager, name, SERVICE_QUERY_STATUS | DELETE);
		𐤀𐤌(svc_handle){
			𐤀𐤌(QueryServiceStatus(svc_handle, &status)){
				𐤀𐤌(status.dwCurrentState == SERVICE_STOPPED){
					DeleteService(svc_handle);
				}
			}
			CloseServiceHandle(svc_handle);
		}وإلا{
			print_error();
		}
		CloseServiceHandle(sc_manager);
	}وإلا{
		print_error();
	}
}


فراغ service_run(حرف *name)
{
	SERVICE_TABLE_ENTRY ste[] = {
		{ name, service_main },
		{ عدم, عدم }
	};
	name = fix_name(name);
	ste[0].lpServiceName = name;
	StartServiceCtrlDispatcher(ste);
}

#endif
