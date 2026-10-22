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

خارجي صحيح g_لگاتار;
SERVICE_حالت_HANDLE service_handle = 0;
ساكن SERVICE_حالت service_حالت;
صحيح مکھ(صحيح argc, حرف *argv[]);


ساكن حرف *fix_name(حرف *name)
{
	size_t len;

	إذا(strrchr(name, '\\')){
		name = strrchr(name, '\\') + 1;
	}
	len = لڑی_لمبائی(name);
	إذا(!strcasecmp(&name[len-4], ".exe")){
		name[len-4] = '\0';
	}

	أرجع name;
}


ساكن فراغ print_error(فراغ)
{
	حرف *buf = عدم;

	Format_سنیہا(FORMAT_سنیہا_ALLOCATE_بفر | FORMAT_سنیہا_FROM_SYSTEM,
			عدم, Get_آخری_Error(), LANG_NEUTRAL, (LPTSTR)&buf, 0, عدم);

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
			service_حالت.dwCurrentState = SERVICE_STOP_PENDING;
			سیٹ_Service_حالت(service_handle, &service_حالت);
			g_لگاتار = 0;
			اكسر;
	}
}


/* Function called when started as a service. */
فراغ __stdcall service_main(DWORD dwArgc, LPTSTR *lpszArgv)
{
	حرف **argv;
	صحيح argc = 1;
	حرف conf_رستہ[MAX_رستہ + 20];
	صحيح rc;
	حرف *name;
	حرف env_name[MAX_رستہ];

	بن_ورتیا(dwArgc);

	name = fix_name(lpszArgv[0]);
	snprintf(env_name, حجم(env_name), "%s_DIR", name);
	لكل(صحيح i=0; i<لڑی_لمبائی(env_name); i++){
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

	service_handle = رجسٹر_Service_Ctrl_Handler(name, service_handler);
	إذا(service_handle){
		memset(conf_رستہ, 0, حجم(conf_رستہ));
		rc = GetEnvironmentVariable(env_name, conf_رستہ, MAX_رستہ);
		إذا(!rc || rc == MAX_رستہ){
			service_حالت.dwCurrentState = SERVICE_STOPPED;
			سیٹ_Service_حالت(service_handle, &service_حالت);
			أرجع;
		}
		strcat(conf_رستہ, "\\mosquitto.conf");

		argv = mosquitto_malloc(حجم(حرف *)*3);
		argv[0] = name;
		argv[1] = "-c";
		argv[2] = conf_رستہ;
		argc = 3;

		service_حالت.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		service_حالت.dwCurrentState = SERVICE_RUNNING;
		service_حالت.dwControlsAccepted = SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_STOP;
		service_حالت.dw_Win32_Exit_کوڈ = NO_ERROR;
		service_حالت.dw_Check_نقطہ = 0;
		سیٹ_Service_حالت(service_handle, &service_حالت);

		مکھ(argc, argv);
		mosquitto_FREE(argv);

		service_حالت.dwCurrentState = SERVICE_STOPPED;
		سیٹ_Service_حالت(service_handle, &service_حالت);
	}
}


فراغ service_install(حرف *name)
{
	SC_HANDLE sc_manager, svc_handle;
	حرف service_string[MAX_رستہ + 20];
	حرف exe_رستہ[MAX_رستہ + 1];
	حرف display_name[MAX_رستہ+1];
	SERVICE_DESCRIPTION svc_desc;

	memset(exe_رستہ, 0, حجم(exe_رستہ));
	إذا(GetModuleFileName(عدم, exe_رستہ, MAX_رستہ) == MAX_رستہ){
		fprintf(stderr, "Error: Path too long.\n");
		أرجع;
	}
	snprintf(service_string, حجم(service_string), "\"%s\" run", exe_رستہ);

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
				SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_شروع, SERVICE_ERROR_عام,
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
	SERVICE_حالت حالت;

	name = fix_name(name);

	sc_manager = OpenSCManager(عدم, SERVICES_سرگرم_DATABASE, SC_MANAGER_CONNECT);
	إذا(sc_manager){
		svc_handle = OpenService(sc_manager, name, SERVICE_QUERY_حالت | DELETE);
		إذا(svc_handle){
			إذا(Query_Service_حالت(svc_handle, &حالت)){
				إذا(حالت.dwCurrentState == SERVICE_STOPPED){
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


فراغ service_لگاتار(حرف *name)
{
	SERVICE_جدول_اندراج ste[] = {
		{ name, service_main },
		{ عدم, عدم }
	};
	name = fix_name(name);
	ste[0].lpServiceName = name;
	شروع_Service_Ctrl_Dispatcher(ste);
}

#جے_انت
