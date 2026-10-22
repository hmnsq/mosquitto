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

#ਜੇ defined(WIN32) || defined(__CYGWIN__)

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <windows.h>

ਬਾਹਰੀ ਪੂਰਨ g_ਲੜੀਵਾਰ;
SERVICE_ਸਥਿਤੀ_HANDLE service_handle = 0;
ਸਥਿਰ SERVICE_ਸਥਿਤੀ service_ਸਥਿਤੀ;
ਪੂਰਨ ਮੁੱਖ(ਪੂਰਨ argc, ਅੱਖਰ *argv[]);


ਸਥਿਰ ਅੱਖਰ *fix_name(ਅੱਖਰ *name)
{
	size_t len;

	ਜੇ(strrchr(name, '\\')){
		name = strrchr(name, '\\') + 1;
	}
	len = ਲੜੀ_ਲੰਬਾਈ(name);
	ਜੇ(!strcasecmp(&name[len-4], ".exe")){
		name[len-4] = '\0';
	}

	ਵਾਪਸ name;
}


ਸਥਿਰ ਖਾਲੀ print_error(ਖਾਲੀ)
{
	ਅੱਖਰ *buf = ਨਿਰਾ;

	Format_ਸੁਨੇਹਾ(FORMAT_ਸੁਨੇਹਾ_ALLOCATE_ਬਫਰ | FORMAT_ਸੁਨੇਹਾ_FROM_SYSTEM,
			ਨਿਰਾ, Get_ਆਖਰੀ_Error(), LANG_NEUTRAL, (LPTSTR)&buf, 0, ਨਿਰਾ);

	fprintf(stderr, "Error: %s\n", buf);
	LocalFree(buf);
}


/* Service control callback */
ਖਾਲੀ __stdcall service_handler(DWORD fdwControl)
{
	ਚੋਣ(fdwControl){
		ਹਾਲਤ SERVICE_CONTROL_CONTINUE:
			/* Continue from Paused state. */
			ਰੋਕੋ;
		ਹਾਲਤ SERVICE_CONTROL_PAUSE:
			/* Pause service. */
			ਰੋਕੋ;
		ਹਾਲਤ SERVICE_CONTROL_SHUTDOWN:
		/* System is shutting down. */
		ਹਾਲਤ SERVICE_CONTROL_STOP:
			/* Service should stop. */
			service_ਸਥਿਤੀ.dwCurrentState = SERVICE_STOP_PENDING;
			ਸੈੱਟ_Service_ਸਥਿਤੀ(service_handle, &service_ਸਥਿਤੀ);
			g_ਲੜੀਵਾਰ = 0;
			ਰੋਕੋ;
	}
}


/* Function called when started as a service. */
ਖਾਲੀ __stdcall service_main(DWORD dwArgc, LPTSTR *lpszArgv)
{
	ਅੱਖਰ **argv;
	ਪੂਰਨ argc = 1;
	ਅੱਖਰ conf_ਰਸਤਾ[MAX_ਰਸਤਾ + 20];
	ਪੂਰਨ rc;
	ਅੱਖਰ *name;
	ਅੱਖਰ env_name[MAX_ਰਸਤਾ];

	ਅਣਵਰਤਿਆ(dwArgc);

	name = fix_name(lpszArgv[0]);
	snprintf(env_name, ਆਕਾਰ(env_name), "%s_DIR", name);
	ਲਈ(ਪੂਰਨ i=0; i<ਲੜੀ_ਲੰਬਾਈ(env_name); i++){
		ਜੇ(env_name[i]>='A' && env_name[i]<='Z'){
			/* Keep upper case letter */
		}ਨਹੀਂਤਾਂ ਜੇ(env_name[i]>='0' && env_name[i]<='9'){
			/* Keep number */
		}ਨਹੀਂਤਾਂ ਜੇ(env_name[i]>='a' && env_name[i]<='z'){
			/* Convert lower case to upper case */
			env_name[i] -= 32;
		}ਨਹੀਂਤਾਂ{
			env_name[i] = '_';
		}
	}

	service_handle = ਰਜਿਸਟਰ_Service_Ctrl_Handler(name, service_handler);
	ਜੇ(service_handle){
		memset(conf_ਰਸਤਾ, 0, ਆਕਾਰ(conf_ਰਸਤਾ));
		rc = GetEnvironmentVariable(env_name, conf_ਰਸਤਾ, MAX_ਰਸਤਾ);
		ਜੇ(!rc || rc == MAX_ਰਸਤਾ){
			service_ਸਥਿਤੀ.dwCurrentState = SERVICE_STOPPED;
			ਸੈੱਟ_Service_ਸਥਿਤੀ(service_handle, &service_ਸਥਿਤੀ);
			ਵਾਪਸ;
		}
		strcat(conf_ਰਸਤਾ, "\\mosquitto.conf");

		argv = mosquitto_malloc(ਆਕਾਰ(ਅੱਖਰ *)*3);
		argv[0] = name;
		argv[1] = "-c";
		argv[2] = conf_ਰਸਤਾ;
		argc = 3;

		service_ਸਥਿਤੀ.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		service_ਸਥਿਤੀ.dwCurrentState = SERVICE_RUNNING;
		service_ਸਥਿਤੀ.dwControlsAccepted = SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_STOP;
		service_ਸਥਿਤੀ.dw_Win32_Exit_ਕੋਡ = NO_ERROR;
		service_ਸਥਿਤੀ.dw_Check_ਬਿੰਦੂ = 0;
		ਸੈੱਟ_Service_ਸਥਿਤੀ(service_handle, &service_ਸਥਿਤੀ);

		ਮੁੱਖ(argc, argv);
		mosquitto_FREE(argv);

		service_ਸਥਿਤੀ.dwCurrentState = SERVICE_STOPPED;
		ਸੈੱਟ_Service_ਸਥਿਤੀ(service_handle, &service_ਸਥਿਤੀ);
	}
}


ਖਾਲੀ service_install(ਅੱਖਰ *name)
{
	SC_HANDLE sc_manager, svc_handle;
	ਅੱਖਰ service_string[MAX_ਰਸਤਾ + 20];
	ਅੱਖਰ exe_ਰਸਤਾ[MAX_ਰਸਤਾ + 1];
	ਅੱਖਰ display_name[MAX_ਰਸਤਾ+1];
	SERVICE_DESCRIPTION svc_desc;

	memset(exe_ਰਸਤਾ, 0, ਆਕਾਰ(exe_ਰਸਤਾ));
	ਜੇ(GetModuleFileName(ਨਿਰਾ, exe_ਰਸਤਾ, MAX_ਰਸਤਾ) == MAX_ਰਸਤਾ){
		fprintf(stderr, "Error: Path too long.\n");
		ਵਾਪਸ;
	}
	snprintf(service_string, ਆਕਾਰ(service_string), "\"%s\" run", exe_ਰਸਤਾ);

	name = fix_name(name);

	sc_manager = OpenSCManager(ਨਿਰਾ, ਨਿਰਾ, SC_MANAGER_CREATE_SERVICE);
	ਜੇ(sc_manager){
		ਜੇ(!strcmp(name, "mosquitto")){
			snprintf(display_name, ਆਕਾਰ(display_name), "Mosquitto Broker");
		}ਨਹੀਂਤਾਂ{
			snprintf(display_name, ਆਕਾਰ(display_name), "Mosquitto Broker (%s.exe)", name);
		}

		svc_handle = CreateService(sc_manager, name, display_name,
				SERVICE_ਸ਼ੁਰੂ | SERVICE_STOP | SERVICE_CHANGE_CONFIG,
				SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_ਸ਼ੁਰੂ, SERVICE_ERROR_ਸਾਧਾਰਨ,
				service_string, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);

		ਜੇ(svc_handle){
			memset(&svc_desc, 0, ਆਕਾਰ(svc_desc));
			svc_desc.lpDescription = "Eclipse Mosquitto MQTT v5/v3.1.1 broker";
			ChangeServiceConfig2(svc_handle, SERVICE_CONFIG_DESCRIPTION, &svc_desc);
			CloseServiceHandle(svc_handle);
		}ਨਹੀਂਤਾਂ{
			print_error();
		}
		CloseServiceHandle(sc_manager);
	}ਨਹੀਂਤਾਂ{
		print_error();
	}
}


ਖਾਲੀ service_uninstall(ਅੱਖਰ *name)
{
	SC_HANDLE sc_manager, svc_handle;
	SERVICE_ਸਥਿਤੀ ਸਥਿਤੀ;

	name = fix_name(name);

	sc_manager = OpenSCManager(ਨਿਰਾ, SERVICES_ਸਰਗਰਮ_DATABASE, SC_MANAGER_CONNECT);
	ਜੇ(sc_manager){
		svc_handle = OpenService(sc_manager, name, SERVICE_QUERY_ਸਥਿਤੀ | DELETE);
		ਜੇ(svc_handle){
			ਜੇ(Query_Service_ਸਥਿਤੀ(svc_handle, &ਸਥਿਤੀ)){
				ਜੇ(ਸਥਿਤੀ.dwCurrentState == SERVICE_STOPPED){
					DeleteService(svc_handle);
				}
			}
			CloseServiceHandle(svc_handle);
		}ਨਹੀਂਤਾਂ{
			print_error();
		}
		CloseServiceHandle(sc_manager);
	}ਨਹੀਂਤਾਂ{
		print_error();
	}
}


ਖਾਲੀ service_ਲੜੀਵਾਰ(ਅੱਖਰ *name)
{
	SERVICE_ਸਾਰਣੀ_ਇੰਦਰਾਜ ste[] = {
		{ name, service_main },
		{ ਨਿਰਾ, ਨਿਰਾ }
	};
	name = fix_name(name);
	ste[0].lpServiceName = name;
	ਸ਼ੁਰੂ_Service_Ctrl_Dispatcher(ste);
}

#ਜੇ_ਅੰਤ
