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

#إذا defined(WIN32) || defined(__CYGWIN__)

#ضمن "config.h"

#ضمن "mosquitto_broker_internal.h"
#ضمن <windows.h>

خارجي صحيح g_پیاپی;
SERVICE_وضعیت_HANDLE service_handle = 0;
ساكن SERVICE_وضعیت service_وضعیت;
صحيح اصلی(صحيح argc, حرف *argv[]);


ساكن حرف *fix_name(حرف *name)
{
	size_t len;

	إذا(strrchr(name, '\\')){
		name = strrchr(name, '\\') + 1;
	}
	len = طول_رشته(name);
	إذا(!strcasecmp(&name[len-4], ".exe")){
		name[len-4] = '\0';
	}

	أرجع name;
}


ساكن فراغ print_error(فراغ)
{
	حرف *buf = عدم;

	Format_پیام(FORMAT_پیام_ALLOCATE_میانگیر | FORMAT_پیام_FROM_SYSTEM,
			عدم, Get_آخرین_Error(), LANG_NEUTRAL, (LPTSTR)&buf, 0, عدم);

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
			service_وضعیت.dwCurrentState = SERVICE_STOP_PENDING;
			مجموعه_Service_وضعیت(service_handle, &service_وضعیت);
			g_پیاپی = 0;
			اكسر;
	}
}


/* Function called when started as a service. */
فراغ __stdcall service_main(DWORD dwArgc, LPTSTR *lpszArgv)
{
	حرف **argv;
	صحيح argc = 1;
	حرف conf_مسیر[MAX_مسیر + 20];
	صحيح rc;
	حرف *name;
	حرف env_name[MAX_مسیر];

	استفادهنشده(dwArgc);

	name = fix_name(lpszArgv[0]);
	snprintf(env_name, حجم(env_name), "%s_DIR", name);
	لكل(صحيح i=0; i<طول_رشته(env_name); i++){
		إذا(env_name[i]>='A' && env_name[i]<='Z'){
			/* Keep upper case letter */
		}وإلا إذا(env_name[i]>='0' && env_name[i]<='9'){
			/* Keep number */
		}وإلا إذا(env_name[i]>='a' && env_name[i]<='z'){
			/* Convert lower case to upper case */
			env_name[i] -= 32;
		}وإلا{
			env_name[i] = '_';
		}
	}

	service_handle = ثبات_Service_Ctrl_Handler(name, service_handler);
	إذا(service_handle){
		memset(conf_مسیر, 0, حجم(conf_مسیر));
		rc = GetEnvironmentVariable(env_name, conf_مسیر, MAX_مسیر);
		إذا(!rc || rc == MAX_مسیر){
			service_وضعیت.dwCurrentState = SERVICE_STOPPED;
			مجموعه_Service_وضعیت(service_handle, &service_وضعیت);
			أرجع;
		}
		strcat(conf_مسیر, "\\mosquitto.conf");

		argv = mosquitto_malloc(حجم(حرف *)*3);
		argv[0] = name;
		argv[1] = "-c";
		argv[2] = conf_مسیر;
		argc = 3;

		service_وضعیت.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		service_وضعیت.dwCurrentState = SERVICE_RUNNING;
		service_وضعیت.dwControlsAccepted = SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_STOP;
		service_وضعیت.dw_Win32_Exit_کد = NO_ERROR;
		service_وضعیت.dw_Check_نقطه = 0;
		مجموعه_Service_وضعیت(service_handle, &service_وضعیت);

		اصلی(argc, argv);
		mosquitto_FREE(argv);

		service_وضعیت.dwCurrentState = SERVICE_STOPPED;
		مجموعه_Service_وضعیت(service_handle, &service_وضعیت);
	}
}


فراغ service_install(حرف *name)
{
	SC_HANDLE sc_manager, svc_handle;
	حرف service_string[MAX_مسیر + 20];
	حرف exe_مسیر[MAX_مسیر + 1];
	حرف display_name[MAX_مسیر+1];
	SERVICE_DESCRIPTION svc_desc;

	memset(exe_مسیر, 0, حجم(exe_مسیر));
	إذا(GetModuleFileName(عدم, exe_مسیر, MAX_مسیر) == MAX_مسیر){
		fprintf(stderr, "Error: Path too long.\n");
		أرجع;
	}
	snprintf(service_string, حجم(service_string), "\"%s\" run", exe_مسیر);

	name = fix_name(name);

	sc_manager = OpenSCManager(عدم, عدم, SC_MANAGER_CREATE_SERVICE);
	إذا(sc_manager){
		إذا(!strcmp(name, "mosquitto")){
			snprintf(display_name, حجم(display_name), "Mosquitto Broker");
		}وإلا{
			snprintf(display_name, حجم(display_name), "Mosquitto Broker (%s.exe)", name);
		}

		svc_handle = CreateService(sc_manager, name, display_name,
				SERVICE_شروع | SERVICE_STOP | SERVICE_CHANGE_CONFIG,
				SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_شروع, SERVICE_ERROR_نرمال,
				service_string, عدم, عدم, عدم, عدم, عدم);

		إذا(svc_handle){
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
	SERVICE_وضعیت وضعیت;

	name = fix_name(name);

	sc_manager = OpenSCManager(عدم, SERVICES_فعال_DATABASE, SC_MANAGER_CONNECT);
	إذا(sc_manager){
		svc_handle = OpenService(sc_manager, name, SERVICE_QUERY_وضعیت | DELETE);
		إذا(svc_handle){
			إذا(Query_Service_وضعیت(svc_handle, &وضعیت)){
				إذا(وضعیت.dwCurrentState == SERVICE_STOPPED){
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


فراغ service_پیاپی(حرف *name)
{
	SERVICE_جدول_مدخل ste[] = {
		{ name, service_main },
		{ عدم, عدم }
	};
	name = fix_name(name);
	ste[0].lpServiceName = name;
	شروع_Service_Ctrl_Dispatcher(ste);
}

#پایان_اگر
