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

#যদি defined(WIN32) || defined(__CYGWIN__)

#অন্তর্ভাব "config.h"

#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব <windows.h>

বাহ্যিক পূর্ণসংখ্যা g_run;
SERVICE_STATUS_HANDLE service_handle = 0;
স্থির SERVICE_STATUS service_status;
পূর্ণসংখ্যা প্রধান(পূর্ণসংখ্যা argc, অক্ষর *argv[]);


স্থির অক্ষর *fix_name(অক্ষর *name)
{
	size_t len;

	যদি(strrchr(name, '\\')){
		name = strrchr(name, '\\') + 1;
	}
	len = strlen(name);
	যদি(!strcasecmp(&name[len-4], ".exe")){
		name[len-4] = '\0';
	}

	ফেরত name;
}


স্থির শূন্য print_error(শূন্য)
{
	অক্ষর *buf = ফাঁকা;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_বাফার | FORMAT_MESSAGE_FROM_SYSTEM,
			ফাঁকা, GetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, ফাঁকা);

	fprintf(stderr, "Error: %s\n", buf);
	LocalFree(buf);
}


/* Service control callback */
শূন্য __stdcall service_handler(DWORD fdwControl)
{
	নির্বাচন(fdwControl){
		ক্ষেত্র SERVICE_CONTROL_CONTINUE:
			/* Continue from Paused state. */
			বিরতি;
		ক্ষেত্র SERVICE_CONTROL_PAUSE:
			/* Pause service. */
			বিরতি;
		ক্ষেত্র SERVICE_CONTROL_SHUTDOWN:
		/* System is shutting down. */
		ক্ষেত্র SERVICE_CONTROL_STOP:
			/* Service should stop. */
			service_status.dwCurrentState = SERVICE_STOP_PENDING;
			SetServiceStatus(service_handle, &service_status);
			g_run = 0;
			বিরতি;
	}
}


/* Function called when started as a service. */
শূন্য __stdcall service_main(DWORD dwArgc, LPTSTR *lpszArgv)
{
	অক্ষর **argv;
	পূর্ণসংখ্যা argc = 1;
	অক্ষর conf_path[MAX_PATH + 20];
	পূর্ণসংখ্যা rc;
	অক্ষর *name;
	অক্ষর env_name[MAX_PATH];

	UNUSED(dwArgc);

	name = fix_name(lpszArgv[0]);
	snprintf(env_name, আকার(env_name), "%s_DIR", name);
	জন্য(পূর্ণসংখ্যা i=0; i<strlen(env_name); i++){
		যদি(env_name[i]>='A' && env_name[i]<='Z'){
			/* Keep upper case letter */
		}অন্যথা যদি(env_name[i]>='0' && env_name[i]<='9'){
			/* Keep number */
		}অন্যথা যদি(env_name[i]>='a' && env_name[i]<='z'){
			/* Convert lower case to upper case */
			env_name[i] -= 32;
		}অন্যথা{
			env_name[i] = '_';
		}
	}

	service_handle = RegisterServiceCtrlHandler(name, service_handler);
	যদি(service_handle){
		memset(conf_path, 0, আকার(conf_path));
		rc = GetEnvironmentVariable(env_name, conf_path, MAX_PATH);
		যদি(!rc || rc == MAX_PATH){
			service_status.dwCurrentState = SERVICE_STOPPED;
			SetServiceStatus(service_handle, &service_status);
			ফেরত;
		}
		strcat(conf_path, "\\mosquitto.conf");

		argv = mosquitto_malloc(আকার(অক্ষর *)*3);
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

		প্রধান(argc, argv);
		mosquitto_FREE(argv);

		service_status.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(service_handle, &service_status);
	}
}


শূন্য service_install(অক্ষর *name)
{
	SC_HANDLE sc_manager, svc_handle;
	অক্ষর service_string[MAX_PATH + 20];
	অক্ষর exe_path[MAX_PATH + 1];
	অক্ষর display_name[MAX_PATH+1];
	SERVICE_DESCRIPTION svc_desc;

	memset(exe_path, 0, আকার(exe_path));
	যদি(GetModuleFileName(ফাঁকা, exe_path, MAX_PATH) == MAX_PATH){
		fprintf(stderr, "Error: Path too long.\n");
		ফেরত;
	}
	snprintf(service_string, আকার(service_string), "\"%s\" run", exe_path);

	name = fix_name(name);

	sc_manager = OpenSCManager(ফাঁকা, ফাঁকা, SC_MANAGER_CREATE_SERVICE);
	যদি(sc_manager){
		যদি(!strcmp(name, "mosquitto")){
			snprintf(display_name, আকার(display_name), "Mosquitto Broker");
		}অন্যথা{
			snprintf(display_name, আকার(display_name), "Mosquitto Broker (%s.exe)", name);
		}

		svc_handle = CreateService(sc_manager, name, display_name,
				SERVICE_START | SERVICE_STOP | SERVICE_CHANGE_CONFIG,
				SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
				service_string, ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা);

		যদি(svc_handle){
			memset(&svc_desc, 0, আকার(svc_desc));
			svc_desc.lpDescription = "Eclipse Mosquitto MQTT v5/v3.1.1 broker";
			ChangeServiceConfig2(svc_handle, SERVICE_CONFIG_DESCRIPTION, &svc_desc);
			CloseServiceHandle(svc_handle);
		}অন্যথা{
			print_error();
		}
		CloseServiceHandle(sc_manager);
	}অন্যথা{
		print_error();
	}
}


শূন্য service_uninstall(অক্ষর *name)
{
	SC_HANDLE sc_manager, svc_handle;
	SERVICE_STATUS status;

	name = fix_name(name);

	sc_manager = OpenSCManager(ফাঁকা, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CONNECT);
	যদি(sc_manager){
		svc_handle = OpenService(sc_manager, name, SERVICE_QUERY_STATUS | DELETE);
		যদি(svc_handle){
			যদি(QueryServiceStatus(svc_handle, &status)){
				যদি(status.dwCurrentState == SERVICE_STOPPED){
					DeleteService(svc_handle);
				}
			}
			CloseServiceHandle(svc_handle);
		}অন্যথা{
			print_error();
		}
		CloseServiceHandle(sc_manager);
	}অন্যথা{
		print_error();
	}
}


শূন্য service_run(অক্ষর *name)
{
	SERVICE_TABLE_ENTRY ste[] = {
		{ name, service_main },
		{ ফাঁকা, ফাঁকা }
	};
	name = fix_name(name);
	ste[0].lpServiceName = name;
	StartServiceCtrlDispatcher(ste);
}

#endif
