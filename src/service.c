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

#ಆದರೆ defined(WIN32) || defined(__CYGWIN__)

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ <windows.h>

ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ g_ಸತತ;
SERVICE_ಸ್ಥಿತಿ_HANDLE service_handle = 0;
ಸ್ಥಿರ SERVICE_ಸ್ಥಿತಿ service_ಸ್ಥಿತಿ;
ಪೂರ್ಣಾಂಕ ಮುಖ್ಯ(ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ *argv[]);


ಸ್ಥಿರ ಅಕ್ಷರ *fix_name(ಅಕ್ಷರ *name)
{
	size_t len;

	ಆದರೆ(strrchr(name, '\\')){
		name = strrchr(name, '\\') + 1;
	}
	len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name);
	ಆದರೆ(!strcasecmp(&name[len-4], ".exe")){
		name[len-4] = '\0';
	}

	ಹಿಂತಿರುಗಿ name;
}


ಸ್ಥಿರ ಶೂನ್ಯ print_error(ಶೂನ್ಯ)
{
	ಅಕ್ಷರ *buf = ಖಾಲಿ;

	Format_ಸಂದೇಶ(FORMAT_ಸಂದೇಶ_ALLOCATE_ಬಫರ್ | FORMAT_ಸಂದೇಶ_FROM_SYSTEM,
			ಖಾಲಿ, Get_ಕೊನೆಯ_Error(), LANG_NEUTRAL, (LPTSTR)&buf, 0, ಖಾಲಿ);

	fprintf(stderr, "Error: %s\n", buf);
	LocalFree(buf);
}


/* Service control callback */
ಶೂನ್ಯ __stdcall service_handler(DWORD fdwControl)
{
	ಆಯ್ಕೆ(fdwControl){
		ಸ್ಥಿತಿ SERVICE_CONTROL_CONTINUE:
			/* Continue from Paused state. */
			ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ SERVICE_CONTROL_PAUSE:
			/* Pause service. */
			ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ SERVICE_CONTROL_SHUTDOWN:
		/* System is shutting down. */
		ಸ್ಥಿತಿ SERVICE_CONTROL_STOP:
			/* Service should stop. */
			service_ಸ್ಥಿತಿ.dwCurrentState = SERVICE_STOP_PENDING;
			ಗಣ_Service_ಸ್ಥಿತಿ(service_handle, &service_ಸ್ಥಿತಿ);
			g_ಸತತ = 0;
			ನಿಲ್ಲಿಸು;
	}
}


/* Function called when started as a service. */
ಶೂನ್ಯ __stdcall service_main(DWORD dwArgc, LPTSTR *lpszArgv)
{
	ಅಕ್ಷರ **argv;
	ಪೂರ್ಣಾಂಕ argc = 1;
	ಅಕ್ಷರ conf_ಮಾರ್ಗ[MAX_ಮಾರ್ಗ + 20];
	ಪೂರ್ಣಾಂಕ rc;
	ಅಕ್ಷರ *name;
	ಅಕ್ಷರ env_name[MAX_ಮಾರ್ಗ];

	ಬಳಸದ(dwArgc);

	name = fix_name(lpszArgv[0]);
	snprintf(env_name, ಗಾತ್ರ(env_name), "%s_DIR", name);
	ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(env_name); i++){
		ಆದರೆ(env_name[i]>='A' && env_name[i]<='Z'){
			/* Keep upper case letter */
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(env_name[i]>='0' && env_name[i]<='9'){
			/* Keep number */
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(env_name[i]>='a' && env_name[i]<='z'){
			/* Convert lower case to upper case */
			env_name[i] -= 32;
		}ಇಲ್ಲದಿದ್ದರೆ{
			env_name[i] = '_';
		}
	}

	service_handle = ರಿಜಿಸ್ಟರ್_Service_Ctrl_Handler(name, service_handler);
	ಆದರೆ(service_handle){
		memset(conf_ಮಾರ್ಗ, 0, ಗಾತ್ರ(conf_ಮಾರ್ಗ));
		rc = GetEnvironmentVariable(env_name, conf_ಮಾರ್ಗ, MAX_ಮಾರ್ಗ);
		ಆದರೆ(!rc || rc == MAX_ಮಾರ್ಗ){
			service_ಸ್ಥಿತಿ.dwCurrentState = SERVICE_STOPPED;
			ಗಣ_Service_ಸ್ಥಿತಿ(service_handle, &service_ಸ್ಥಿತಿ);
			ಹಿಂತಿರುಗಿ;
		}
		strcat(conf_ಮಾರ್ಗ, "\\mosquitto.conf");

		argv = mosquitto_malloc(ಗಾತ್ರ(ಅಕ್ಷರ *)*3);
		argv[0] = name;
		argv[1] = "-c";
		argv[2] = conf_ಮಾರ್ಗ;
		argc = 3;

		service_ಸ್ಥಿತಿ.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		service_ಸ್ಥಿತಿ.dwCurrentState = SERVICE_RUNNING;
		service_ಸ್ಥಿತಿ.dwControlsAccepted = SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_STOP;
		service_ಸ್ಥಿತಿ.dw_Win32_Exit_ಸಂಕೇತ = NO_ERROR;
		service_ಸ್ಥಿತಿ.dw_Check_ಬಿಂದು = 0;
		ಗಣ_Service_ಸ್ಥಿತಿ(service_handle, &service_ಸ್ಥಿತಿ);

		ಮುಖ್ಯ(argc, argv);
		mosquitto_FREE(argv);

		service_ಸ್ಥಿತಿ.dwCurrentState = SERVICE_STOPPED;
		ಗಣ_Service_ಸ್ಥಿತಿ(service_handle, &service_ಸ್ಥಿತಿ);
	}
}


ಶೂನ್ಯ service_install(ಅಕ್ಷರ *name)
{
	SC_HANDLE sc_manager, svc_handle;
	ಅಕ್ಷರ service_string[MAX_ಮಾರ್ಗ + 20];
	ಅಕ್ಷರ exe_ಮಾರ್ಗ[MAX_ಮಾರ್ಗ + 1];
	ಅಕ್ಷರ display_name[MAX_ಮಾರ್ಗ+1];
	SERVICE_DESCRIPTION svc_desc;

	memset(exe_ಮಾರ್ಗ, 0, ಗಾತ್ರ(exe_ಮಾರ್ಗ));
	ಆದರೆ(GetModuleFileName(ಖಾಲಿ, exe_ಮಾರ್ಗ, MAX_ಮಾರ್ಗ) == MAX_ಮಾರ್ಗ){
		fprintf(stderr, "Error: Path too long.\n");
		ಹಿಂತಿರುಗಿ;
	}
	snprintf(service_string, ಗಾತ್ರ(service_string), "\"%s\" run", exe_ಮಾರ್ಗ);

	name = fix_name(name);

	sc_manager = OpenSCManager(ಖಾಲಿ, ಖಾಲಿ, SC_MANAGER_CREATE_SERVICE);
	ಆದರೆ(sc_manager){
		ಆದರೆ(!strcmp(name, "mosquitto")){
			snprintf(display_name, ಗಾತ್ರ(display_name), "Mosquitto Broker");
		}ಇಲ್ಲದಿದ್ದರೆ{
			snprintf(display_name, ಗಾತ್ರ(display_name), "Mosquitto Broker (%s.exe)", name);
		}

		svc_handle = CreateService(sc_manager, name, display_name,
				SERVICE_ಆರಂಭ | SERVICE_STOP | SERVICE_CHANGE_CONFIG,
				SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_ಆರಂಭ, SERVICE_ERROR_ಸಾಮಾನ್ಯ,
				service_string, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);

		ಆದರೆ(svc_handle){
			memset(&svc_desc, 0, ಗಾತ್ರ(svc_desc));
			svc_desc.lpDescription = "Eclipse Mosquitto MQTT v5/v3.1.1 broker";
			ChangeServiceConfig2(svc_handle, SERVICE_CONFIG_DESCRIPTION, &svc_desc);
			CloseServiceHandle(svc_handle);
		}ಇಲ್ಲದಿದ್ದರೆ{
			print_error();
		}
		CloseServiceHandle(sc_manager);
	}ಇಲ್ಲದಿದ್ದರೆ{
		print_error();
	}
}


ಶೂನ್ಯ service_uninstall(ಅಕ್ಷರ *name)
{
	SC_HANDLE sc_manager, svc_handle;
	SERVICE_ಸ್ಥಿತಿ ಸ್ಥಿತಿ_2;

	name = fix_name(name);

	sc_manager = OpenSCManager(ಖಾಲಿ, SERVICES_ಸಕ್ರಿಯ_DATABASE, SC_MANAGER_CONNECT);
	ಆದರೆ(sc_manager){
		svc_handle = OpenService(sc_manager, name, SERVICE_QUERY_ಸ್ಥಿತಿ | DELETE);
		ಆದರೆ(svc_handle){
			ಆದರೆ(Query_Service_ಸ್ಥಿತಿ(svc_handle, &ಸ್ಥಿತಿ_2)){
				ಆದರೆ(ಸ್ಥಿತಿ_2.dwCurrentState == SERVICE_STOPPED){
					DeleteService(svc_handle);
				}
			}
			CloseServiceHandle(svc_handle);
		}ಇಲ್ಲದಿದ್ದರೆ{
			print_error();
		}
		CloseServiceHandle(sc_manager);
	}ಇಲ್ಲದಿದ್ದರೆ{
		print_error();
	}
}


ಶೂನ್ಯ service_ಸತತ(ಅಕ್ಷರ *name)
{
	SERVICE_ಕೋಷ್ಟಕ_ನಮೂದು ste[] = {
		{ name, service_main },
		{ ಖಾಲಿ, ಖಾಲಿ }
	};
	name = fix_name(name);
	ste[0].lpServiceName = name;
	ಆರಂಭ_Service_Ctrl_Dispatcher(ste);
}

#ಆದರೆ_ಅಂತ್ಯ
