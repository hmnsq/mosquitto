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

#सचे defined(WIN32) || defined(__CYGWIN__)

#अन्तर्भाव "config.h"

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव <windows.h>

बाह्य पूर्णाङ्क g_run;
SERVICE_STATUS_HANDLE service_handle = 0;
स्थिर SERVICE_STATUS service_status;
पूर्णाङ्क main(पूर्णाङ्क argc, वर्ण *argv[]);


स्थिर वर्ण *fix_name(वर्ण *name)
{
	size_t len;

	सचे(strrchr(name, '\\')){
		name = strrchr(name, '\\') + 1;
	}
	len = strlen(name);
	सचे(!strcasecmp(&name[len-4], ".exe")){
		name[len-4] = '\0';
	}

	पटिनिवत्त name;
}


स्थिर शून्य print_error(शून्य)
{
	वर्ण *buf = रिक्त;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			रिक्त, GetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, रिक्त);

	fprintf(stderr, "Error: %s\n", buf);
	LocalFree(buf);
}


/* Service control callback */
शून्य __stdcall service_handler(DWORD fdwControl)
{
	विकल्प(fdwControl){
		पक्ष SERVICE_CONTROL_CONTINUE:
			/* Continue from Paused state. */
			भङ्ग;
		पक्ष SERVICE_CONTROL_PAUSE:
			/* Pause service. */
			भङ्ग;
		पक्ष SERVICE_CONTROL_SHUTDOWN:
		/* System is shutting down. */
		पक्ष SERVICE_CONTROL_STOP:
			/* Service should stop. */
			service_status.dwCurrentState = SERVICE_STOP_PENDING;
			SetServiceStatus(service_handle, &service_status);
			g_run = 0;
			भङ्ग;
	}
}


/* Function called when started as a service. */
शून्य __stdcall service_main(DWORD dwArgc, LPTSTR *lpszArgv)
{
	वर्ण **argv;
	पूर्णाङ्क argc = 1;
	वर्ण conf_path[MAX_PATH + 20];
	पूर्णाङ्क rc;
	वर्ण *name;
	वर्ण env_name[MAX_PATH];

	UNUSED(dwArgc);

	name = fix_name(lpszArgv[0]);
	snprintf(env_name, परिमाण(env_name), "%s_DIR", name);
	अत्थाय(पूर्णाङ्क i=0; i<strlen(env_name); i++){
		सचे(env_name[i]>='A' && env_name[i]<='Z'){
			/* Keep upper case letter */
		}अञ्ञथा सचे(env_name[i]>='0' && env_name[i]<='9'){
			/* Keep number */
		}अञ्ञथा सचे(env_name[i]>='a' && env_name[i]<='z'){
			/* Convert lower case to upper case */
			env_name[i] -= 32;
		}अञ्ञथा{
			env_name[i] = '_';
		}
	}

	service_handle = RegisterServiceCtrlHandler(name, service_handler);
	सचे(service_handle){
		memset(conf_path, 0, परिमाण(conf_path));
		rc = GetEnvironmentVariable(env_name, conf_path, MAX_PATH);
		सचे(!rc || rc == MAX_PATH){
			service_status.dwCurrentState = SERVICE_STOPPED;
			SetServiceStatus(service_handle, &service_status);
			पटिनिवत्त;
		}
		strcat(conf_path, "\\mosquitto.conf");

		argv = mosquitto_malloc(परिमाण(वर्ण *)*3);
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


शून्य service_install(वर्ण *name)
{
	SC_HANDLE sc_manager, svc_handle;
	वर्ण service_string[MAX_PATH + 20];
	वर्ण exe_path[MAX_PATH + 1];
	वर्ण display_name[MAX_PATH+1];
	SERVICE_DESCRIPTION svc_desc;

	memset(exe_path, 0, परिमाण(exe_path));
	सचे(GetModuleFileName(रिक्त, exe_path, MAX_PATH) == MAX_PATH){
		fprintf(stderr, "Error: Path too long.\n");
		पटिनिवत्त;
	}
	snprintf(service_string, परिमाण(service_string), "\"%s\" run", exe_path);

	name = fix_name(name);

	sc_manager = OpenSCManager(रिक्त, रिक्त, SC_MANAGER_CREATE_SERVICE);
	सचे(sc_manager){
		सचे(!strcmp(name, "mosquitto")){
			snprintf(display_name, परिमाण(display_name), "Mosquitto Broker");
		}अञ्ञथा{
			snprintf(display_name, परिमाण(display_name), "Mosquitto Broker (%s.exe)", name);
		}

		svc_handle = CreateService(sc_manager, name, display_name,
				SERVICE_START | SERVICE_STOP | SERVICE_CHANGE_CONFIG,
				SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
				service_string, रिक्त, रिक्त, रिक्त, रिक्त, रिक्त);

		सचे(svc_handle){
			memset(&svc_desc, 0, परिमाण(svc_desc));
			svc_desc.lpDescription = "Eclipse Mosquitto MQTT v5/v3.1.1 broker";
			ChangeServiceConfig2(svc_handle, SERVICE_CONFIG_DESCRIPTION, &svc_desc);
			CloseServiceHandle(svc_handle);
		}अञ्ञथा{
			print_error();
		}
		CloseServiceHandle(sc_manager);
	}अञ्ञथा{
		print_error();
	}
}


शून्य service_uninstall(वर्ण *name)
{
	SC_HANDLE sc_manager, svc_handle;
	SERVICE_STATUS status;

	name = fix_name(name);

	sc_manager = OpenSCManager(रिक्त, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CONNECT);
	सचे(sc_manager){
		svc_handle = OpenService(sc_manager, name, SERVICE_QUERY_STATUS | DELETE);
		सचे(svc_handle){
			सचे(QueryServiceStatus(svc_handle, &status)){
				सचे(status.dwCurrentState == SERVICE_STOPPED){
					DeleteService(svc_handle);
				}
			}
			CloseServiceHandle(svc_handle);
		}अञ्ञथा{
			print_error();
		}
		CloseServiceHandle(sc_manager);
	}अञ्ञथा{
		print_error();
	}
}


शून्य service_run(वर्ण *name)
{
	SERVICE_TABLE_ENTRY ste[] = {
		{ name, service_main },
		{ रिक्त, रिक्त }
	};
	name = fix_name(name);
	ste[0].lpServiceName = name;
	StartServiceCtrlDispatcher(ste);
}

#endif
