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

#অন্তর্ভাৱ "config.h"

#অন্তর্ভাৱ "mosquitto_broker_internal.h"
#অন্তর্ভাৱ <windows.h>

বাহ্য পূর্ণাঙ্ক g_run;
SERVICE_STATUS_HANDLE service_handle = 0;
স্থির SERVICE_STATUS service_status;
পূর্ণাঙ্ক main(পূর্ণাঙ্ক argc, ৱর্ণ *argv[]);


স্থির ৱর্ণ *fix_name(ৱর্ণ *name)
{
	size_t len;

	যদি(strrchr(name, '\\')){
		name = strrchr(name, '\\') + 1;
	}
	len = strlen(name);
	যদি(!strcasecmp(&name[len-4], ".exe")){
		name[len-4] = '\0';
	}

	ঘূৰাই name;
}


স্থির শূন্য print_error(শূন্য)
{
	ৱর্ণ *buf = রিক্ত;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			রিক্ত, GetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, রিক্ত);

	fprintf(stderr, "Error: %s\n", buf);
	LocalFree(buf);
}


/* Service control callback */
শূন্য __stdcall service_handler(DWORD fdwControl)
{
	ৱিকল্প(fdwControl){
		পক্ষ SERVICE_CONTROL_CONTINUE:
			/* Continue from Paused state. */
			ভঙ্গ;
		পক্ষ SERVICE_CONTROL_PAUSE:
			/* Pause service. */
			ভঙ্গ;
		পক্ষ SERVICE_CONTROL_SHUTDOWN:
		/* System is shutting down. */
		পক্ষ SERVICE_CONTROL_STOP:
			/* Service should stop. */
			service_status.dwCurrentState = SERVICE_STOP_PENDING;
			SetServiceStatus(service_handle, &service_status);
			g_run = 0;
			ভঙ্গ;
	}
}


/* Function called when started as a service. */
শূন্য __stdcall service_main(DWORD dwArgc, LPTSTR *lpszArgv)
{
	ৱর্ণ **argv;
	পূর্ণাঙ্ক argc = 1;
	ৱর্ণ conf_path[MAX_PATH + 20];
	পূর্ণাঙ্ক rc;
	ৱর্ণ *name;
	ৱর্ণ env_name[MAX_PATH];

	UNUSED(dwArgc);

	name = fix_name(lpszArgv[0]);
	snprintf(env_name, পরিমাণ(env_name), "%s_DIR", name);
	বাবে(পূর্ণাঙ্ক i=0; i<strlen(env_name); i++){
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
		memset(conf_path, 0, পরিমাণ(conf_path));
		rc = GetEnvironmentVariable(env_name, conf_path, MAX_PATH);
		যদি(!rc || rc == MAX_PATH){
			service_status.dwCurrentState = SERVICE_STOPPED;
			SetServiceStatus(service_handle, &service_status);
			ঘূৰাই;
		}
		strcat(conf_path, "\\mosquitto.conf");

		argv = mosquitto_malloc(পরিমাণ(ৱর্ণ *)*3);
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


শূন্য service_install(ৱর্ণ *name)
{
	SC_HANDLE sc_manager, svc_handle;
	ৱর্ণ service_string[MAX_PATH + 20];
	ৱর্ণ exe_path[MAX_PATH + 1];
	ৱর্ণ display_name[MAX_PATH+1];
	SERVICE_DESCRIPTION svc_desc;

	memset(exe_path, 0, পরিমাণ(exe_path));
	যদি(GetModuleFileName(রিক্ত, exe_path, MAX_PATH) == MAX_PATH){
		fprintf(stderr, "Error: Path too long.\n");
		ঘূৰাই;
	}
	snprintf(service_string, পরিমাণ(service_string), "\"%s\" run", exe_path);

	name = fix_name(name);

	sc_manager = OpenSCManager(রিক্ত, রিক্ত, SC_MANAGER_CREATE_SERVICE);
	যদি(sc_manager){
		যদি(!strcmp(name, "mosquitto")){
			snprintf(display_name, পরিমাণ(display_name), "Mosquitto Broker");
		}অন্যথা{
			snprintf(display_name, পরিমাণ(display_name), "Mosquitto Broker (%s.exe)", name);
		}

		svc_handle = CreateService(sc_manager, name, display_name,
				SERVICE_START | SERVICE_STOP | SERVICE_CHANGE_CONFIG,
				SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
				service_string, রিক্ত, রিক্ত, রিক্ত, রিক্ত, রিক্ত);

		যদি(svc_handle){
			memset(&svc_desc, 0, পরিমাণ(svc_desc));
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


শূন্য service_uninstall(ৱর্ণ *name)
{
	SC_HANDLE sc_manager, svc_handle;
	SERVICE_STATUS status;

	name = fix_name(name);

	sc_manager = OpenSCManager(রিক্ত, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CONNECT);
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


শূন্য service_run(ৱর্ণ *name)
{
	SERVICE_TABLE_ENTRY ste[] = {
		{ name, service_main },
		{ রিক্ত, রিক্ত }
	};
	name = fix_name(name);
	ste[0].lpServiceName = name;
	StartServiceCtrlDispatcher(ste);
}

#endif
