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

خارجی صحیح g_تسلسل;
SERVICE_حالت_HANDLE service_handle = 0;
ساکن SERVICE_حالت service_حالت;
صحیح مین(صحیح argc, حرف *argv[]);


ساکن حرف *fix_name(حرف *name)
{
	size_t len;

	اگر(strrchr(name, '\\')){
		name = strrchr(name, '\\') + 1;
	}
	len = لڑی_لمبائی(name);
	اگر(!strcasecmp(&name[len-4], ".exe")){
		name[len-4] = '\0';
	}

	واپس name;
}


ساکن خالی print_error(خالی)
{
	حرف *buf = تہی;

	Format_پیغام(FORMAT_پیغام_ALLOCATE_بفر | FORMAT_پیغام_FROM_SYSTEM,
			تہی, Get_آخری_Error(), LANG_NEUTRAL, (LPTSTR)&buf, 0, تہی);

	fprintf(stderr, "Error: %s\n", buf);
	LocalFree(buf);
}


/* Service control callback */
خالی __stdcall service_handler(DWORD fdwControl)
{
	انتخاب(fdwControl){
		حالت SERVICE_CONTROL_CONTINUE:
			/* Continue from Paused state. */
			توڑو;
		حالت SERVICE_CONTROL_PAUSE:
			/* Pause service. */
			توڑو;
		حالت SERVICE_CONTROL_SHUTDOWN:
		/* System is shutting down. */
		حالت SERVICE_CONTROL_STOP:
			/* Service should stop. */
			service_حالت.dwCurrentState = SERVICE_STOP_PENDING;
			سیٹ_Service_حالت(service_handle, &service_حالت);
			g_تسلسل = 0;
			توڑو;
	}
}


/* Function called when started as a service. */
خالی __stdcall service_main(DWORD dwArgc, LPTSTR *lpszArgv)
{
	حرف **argv;
	صحیح argc = 1;
	حرف conf_راستہ[MAX_راستہ + 20];
	صحیح rc;
	حرف *name;
	حرف env_name[MAX_راستہ];

	غیر_مستعمل(dwArgc);

	name = fix_name(lpszArgv[0]);
	snprintf(env_name, حجم(env_name), "%s_DIR", name);
	برائے(صحیح i=0; i<لڑی_لمبائی(env_name); i++){
		اگر(env_name[i]>='A' && env_name[i]<='Z'){
			/* Keep upper case letter */
		}ورنہ اگر(env_name[i]>='0' && env_name[i]<='9'){
			/* Keep number */
		}ورنہ اگر(env_name[i]>='a' && env_name[i]<='z'){
			/* Convert lower case to upper case */
			env_name[i] -= 32;
		}ورنہ{
			env_name[i] = '_';
		}
	}

	service_handle = رجسٹر_Service_Ctrl_Handler(name, service_handler);
	اگر(service_handle){
		memset(conf_راستہ, 0, حجم(conf_راستہ));
		rc = GetEnvironmentVariable(env_name, conf_راستہ, MAX_راستہ);
		اگر(!rc || rc == MAX_راستہ){
			service_حالت.dwCurrentState = SERVICE_STOPPED;
			سیٹ_Service_حالت(service_handle, &service_حالت);
			واپس;
		}
		strcat(conf_راستہ, "\\mosquitto.conf");

		argv = mosquitto_malloc(حجم(حرف *)*3);
		argv[0] = name;
		argv[1] = "-c";
		argv[2] = conf_راستہ;
		argc = 3;

		service_حالت.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		service_حالت.dwCurrentState = SERVICE_RUNNING;
		service_حالت.dwControlsAccepted = SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_STOP;
		service_حالت.dw_Win32_Exit_رمز = NO_ERROR;
		service_حالت.dw_Check_نقطہ = 0;
		سیٹ_Service_حالت(service_handle, &service_حالت);

		مین(argc, argv);
		mosquitto_FREE(argv);

		service_حالت.dwCurrentState = SERVICE_STOPPED;
		سیٹ_Service_حالت(service_handle, &service_حالت);
	}
}


خالی service_install(حرف *name)
{
	SC_HANDLE sc_manager, svc_handle;
	حرف service_string[MAX_راستہ + 20];
	حرف exe_راستہ[MAX_راستہ + 1];
	حرف display_name[MAX_راستہ+1];
	SERVICE_DESCRIPTION svc_desc;

	memset(exe_راستہ, 0, حجم(exe_راستہ));
	اگر(GetModuleFileName(تہی, exe_راستہ, MAX_راستہ) == MAX_راستہ){
		fprintf(stderr, "Error: Path too long.\n");
		واپس;
	}
	snprintf(service_string, حجم(service_string), "\"%s\" run", exe_راستہ);

	name = fix_name(name);

	sc_manager = OpenSCManager(تہی, تہی, SC_MANAGER_CREATE_SERVICE);
	اگر(sc_manager){
		اگر(!strcmp(name, "mosquitto")){
			snprintf(display_name, حجم(display_name), "Mosquitto Broker");
		}ورنہ{
			snprintf(display_name, حجم(display_name), "Mosquitto Broker (%s.exe)", name);
		}

		svc_handle = CreateService(sc_manager, name, display_name,
				SERVICE_آغاز | SERVICE_STOP | SERVICE_CHANGE_CONFIG,
				SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_آغاز, SERVICE_ERROR_معمول,
				service_string, تہی, تہی, تہی, تہی, تہی);

		اگر(svc_handle){
			memset(&svc_desc, 0, حجم(svc_desc));
			svc_desc.lpDescription = "Eclipse Mosquitto MQTT v5/v3.1.1 broker";
			ChangeServiceConfig2(svc_handle, SERVICE_CONFIG_DESCRIPTION, &svc_desc);
			CloseServiceHandle(svc_handle);
		}ورنہ{
			print_error();
		}
		CloseServiceHandle(sc_manager);
	}ورنہ{
		print_error();
	}
}


خالی service_uninstall(حرف *name)
{
	SC_HANDLE sc_manager, svc_handle;
	SERVICE_حالت حالت_2;

	name = fix_name(name);

	sc_manager = OpenSCManager(تہی, SERVICES_فعال_DATABASE, SC_MANAGER_CONNECT);
	اگر(sc_manager){
		svc_handle = OpenService(sc_manager, name, SERVICE_QUERY_حالت | DELETE);
		اگر(svc_handle){
			اگر(Query_Service_حالت(svc_handle, &حالت_2)){
				اگر(حالت_2.dwCurrentState == SERVICE_STOPPED){
					DeleteService(svc_handle);
				}
			}
			CloseServiceHandle(svc_handle);
		}ورنہ{
			print_error();
		}
		CloseServiceHandle(sc_manager);
	}ورنہ{
		print_error();
	}
}


خالی service_تسلسل(حرف *name)
{
	SERVICE_جدول_اندراج ste[] = {
		{ name, service_main },
		{ تہی, تہی }
	};
	name = fix_name(name);
	ste[0].lpServiceName = name;
	آغاز_Service_Ctrl_Dispatcher(ste);
}

#اگر_ختم
