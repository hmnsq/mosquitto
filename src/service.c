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

#اگر defined(WIN32) || defined(__CYGWIN__)

#شامل "config.h"

#شامل "mosquitto_broker_internal.h"
#شامل <windows.h>

بیرونی صحیح g_run;
SERVICE_STATUS_HANDLE service_handle = 0;
ایستا SERVICE_STATUS service_status;
صحیح اصلی(صحیح argc, نویسه *argv[]);


ایستا نویسه *fix_name(نویسه *name)
{
	size_t len;

	اگر(strrchr(name, '\\')){
		name = strrchr(name, '\\') + 1;
	}
	len = strlen(name);
	اگر(!strcasecmp(&name[len-4], ".exe")){
		name[len-4] = '\0';
	}

	بازگشت name;
}


ایستا تهی print_error(تهی)
{
	نویسه *buf = پوچ;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			پوچ, GetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, پوچ);

	fprintf(stderr, "Error: %s\n", buf);
	LocalFree(buf);
}


/* Service control callback */
تهی __stdcall service_handler(DWORD fdwControl)
{
	انتخاب(fdwControl){
		حالت SERVICE_CONTROL_CONTINUE:
			/* Continue from Paused state. */
			بشکن;
		حالت SERVICE_CONTROL_PAUSE:
			/* Pause service. */
			بشکن;
		حالت SERVICE_CONTROL_SHUTDOWN:
		/* System is shutting down. */
		حالت SERVICE_CONTROL_STOP:
			/* Service should stop. */
			service_status.dwCurrentState = SERVICE_STOP_PENDING;
			SetServiceStatus(service_handle, &service_status);
			g_run = 0;
			بشکن;
	}
}


/* Function called when started as a service. */
تهی __stdcall service_main(DWORD dwArgc, LPTSTR *lpszArgv)
{
	نویسه **argv;
	صحیح argc = 1;
	نویسه conf_path[MAX_PATH + 20];
	صحیح rc;
	نویسه *name;
	نویسه env_name[MAX_PATH];

	UNUSED(dwArgc);

	name = fix_name(lpszArgv[0]);
	snprintf(env_name, اندازه(env_name), "%s_DIR", name);
	برای(صحیح i=0; i<strlen(env_name); i++){
		اگر(env_name[i]>='A' && env_name[i]<='Z'){
			/* Keep upper case letter */
		}وگرنه اگر(env_name[i]>='0' && env_name[i]<='9'){
			/* Keep number */
		}وگرنه اگر(env_name[i]>='a' && env_name[i]<='z'){
			/* Convert lower case to upper case */
			env_name[i] -= 32;
		}وگرنه{
			env_name[i] = '_';
		}
	}

	service_handle = RegisterServiceCtrlHandler(name, service_handler);
	اگر(service_handle){
		memset(conf_path, 0, اندازه(conf_path));
		rc = GetEnvironmentVariable(env_name, conf_path, MAX_PATH);
		اگر(!rc || rc == MAX_PATH){
			service_status.dwCurrentState = SERVICE_STOPPED;
			SetServiceStatus(service_handle, &service_status);
			بازگشت;
		}
		strcat(conf_path, "\\mosquitto.conf");

		argv = mosquitto_malloc(اندازه(نویسه *)*3);
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

		اصلی(argc, argv);
		mosquitto_FREE(argv);

		service_status.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(service_handle, &service_status);
	}
}


تهی service_install(نویسه *name)
{
	SC_HANDLE sc_manager, svc_handle;
	نویسه service_string[MAX_PATH + 20];
	نویسه exe_path[MAX_PATH + 1];
	نویسه display_name[MAX_PATH+1];
	SERVICE_DESCRIPTION svc_desc;

	memset(exe_path, 0, اندازه(exe_path));
	اگر(GetModuleFileName(پوچ, exe_path, MAX_PATH) == MAX_PATH){
		fprintf(stderr, "Error: Path too long.\n");
		بازگشت;
	}
	snprintf(service_string, اندازه(service_string), "\"%s\" run", exe_path);

	name = fix_name(name);

	sc_manager = OpenSCManager(پوچ, پوچ, SC_MANAGER_CREATE_SERVICE);
	اگر(sc_manager){
		اگر(!strcmp(name, "mosquitto")){
			snprintf(display_name, اندازه(display_name), "Mosquitto Broker");
		}وگرنه{
			snprintf(display_name, اندازه(display_name), "Mosquitto Broker (%s.exe)", name);
		}

		svc_handle = CreateService(sc_manager, name, display_name,
				SERVICE_START | SERVICE_STOP | SERVICE_CHANGE_CONFIG,
				SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
				service_string, پوچ, پوچ, پوچ, پوچ, پوچ);

		اگر(svc_handle){
			memset(&svc_desc, 0, اندازه(svc_desc));
			svc_desc.lpDescription = "Eclipse Mosquitto MQTT v5/v3.1.1 broker";
			ChangeServiceConfig2(svc_handle, SERVICE_CONFIG_DESCRIPTION, &svc_desc);
			CloseServiceHandle(svc_handle);
		}وگرنه{
			print_error();
		}
		CloseServiceHandle(sc_manager);
	}وگرنه{
		print_error();
	}
}


تهی service_uninstall(نویسه *name)
{
	SC_HANDLE sc_manager, svc_handle;
	SERVICE_STATUS status;

	name = fix_name(name);

	sc_manager = OpenSCManager(پوچ, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CONNECT);
	اگر(sc_manager){
		svc_handle = OpenService(sc_manager, name, SERVICE_QUERY_STATUS | DELETE);
		اگر(svc_handle){
			اگر(QueryServiceStatus(svc_handle, &status)){
				اگر(status.dwCurrentState == SERVICE_STOPPED){
					DeleteService(svc_handle);
				}
			}
			CloseServiceHandle(svc_handle);
		}وگرنه{
			print_error();
		}
		CloseServiceHandle(sc_manager);
	}وگرنه{
		print_error();
	}
}


تهی service_run(نویسه *name)
{
	SERVICE_TABLE_ENTRY ste[] = {
		{ name, service_main },
		{ پوچ, پوچ }
	};
	name = fix_name(name);
	ste[0].lpServiceName = name;
	StartServiceCtrlDispatcher(ste);
}

#endif
