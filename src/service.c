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

#जर defined(WIN32) || defined(__CYGWIN__)

#अन्तर्भाव "config.h"

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव <windows.h>

बाह्य पूर्णांक g_run;
SERVICE_STATUS_HANDLE service_handle = 0;
स्थिर SERVICE_STATUS service_status;
पूर्णांक main(पूर्णांक argc, अक्षर *argv[]);


स्थिर अक्षर *fix_name(अक्षर *name)
{
	माप_प्रकार len;

	जर(म_खोजप(name, '\\')){
		name = म_खोजप(name, '\\') + 1;
	}
	len = म_माप(name);
	जर(!strcasecmp(&name[len-4], ".exe")){
		name[len-4] = '\0';
	}

	परत name;
}


स्थिर शून्य print_error(शून्य)
{
	अक्षर *buf = रिक्त;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			रिक्त, GetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, रिक्त);

	ख_लिखो(मानक_त्रुटि, "Error: %s\n", buf);
	LocalFree(buf);
}


/* Service control callback */
शून्य __stdcall service_handler(DWORD fdwControl)
{
	निवड(fdwControl){
		स्थिती SERVICE_CONTROL_CONTINUE:
			/* Continue from Paused state. */
			थांबा;
		स्थिती SERVICE_CONTROL_PAUSE:
			/* Pause service. */
			थांबा;
		स्थिती SERVICE_CONTROL_SHUTDOWN:
		/* System is shutting down. */
		स्थिती SERVICE_CONTROL_STOP:
			/* Service should stop. */
			service_status.dwCurrentState = SERVICE_STOP_PENDING;
			SetServiceStatus(service_handle, &service_status);
			g_run = 0;
			थांबा;
	}
}


/* Function called when started as a service. */
शून्य __stdcall service_main(DWORD dwArgc, LPTSTR *lpszArgv)
{
	अक्षर **argv;
	पूर्णांक argc = 1;
	अक्षर conf_path[MAX_PATH + 20];
	पूर्णांक rc;
	अक्षर *name;
	अक्षर env_name[MAX_PATH];

	UNUSED(dwArgc);

	name = fix_name(lpszArgv[0]);
	snprintf(env_name, आकार(env_name), "%s_DIR", name);
	साठी(पूर्णांक i=0; i<म_माप(env_name); i++){
		जर(env_name[i]>='A' && env_name[i]<='Z'){
			/* Keep upper case letter */
		}अन्यथा जर(env_name[i]>='0' && env_name[i]<='9'){
			/* Keep number */
		}अन्यथा जर(env_name[i]>='a' && env_name[i]<='z'){
			/* Convert lower case to upper case */
			env_name[i] -= 32;
		}अन्यथा{
			env_name[i] = '_';
		}
	}

	service_handle = RegisterServiceCtrlHandler(name, service_handler);
	जर(service_handle){
		स_रखो(conf_path, 0, आकार(conf_path));
		rc = GetEnvironmentVariable(env_name, conf_path, MAX_PATH);
		जर(!rc || rc == MAX_PATH){
			service_status.dwCurrentState = SERVICE_STOPPED;
			SetServiceStatus(service_handle, &service_status);
			परत;
		}
		म_जोड़ो(conf_path, "\\mosquitto.conf");

		argv = mosquitto_malloc(आकार(अक्षर *)*3);
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


शून्य service_install(अक्षर *name)
{
	SC_HANDLE sc_manager, svc_handle;
	अक्षर service_string[MAX_PATH + 20];
	अक्षर exe_path[MAX_PATH + 1];
	अक्षर display_name[MAX_PATH+1];
	SERVICE_DESCRIPTION svc_desc;

	स_रखो(exe_path, 0, आकार(exe_path));
	जर(GetModuleFileName(रिक्त, exe_path, MAX_PATH) == MAX_PATH){
		ख_लिखो(मानक_त्रुटि, "Error: Path too long.\n");
		परत;
	}
	snprintf(service_string, आकार(service_string), "\"%s\" run", exe_path);

	name = fix_name(name);

	sc_manager = OpenSCManager(रिक्त, रिक्त, SC_MANAGER_CREATE_SERVICE);
	जर(sc_manager){
		जर(!म_भेद(name, "mosquitto")){
			snprintf(display_name, आकार(display_name), "Mosquitto Broker");
		}अन्यथा{
			snprintf(display_name, आकार(display_name), "Mosquitto Broker (%s.exe)", name);
		}

		svc_handle = CreateService(sc_manager, name, display_name,
				SERVICE_START | SERVICE_STOP | SERVICE_CHANGE_CONFIG,
				SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
				service_string, रिक्त, रिक्त, रिक्त, रिक्त, रिक्त);

		जर(svc_handle){
			स_रखो(&svc_desc, 0, आकार(svc_desc));
			svc_desc.lpDescription = "Eclipse Mosquitto MQTT v5/v3.1.1 broker";
			ChangeServiceConfig2(svc_handle, SERVICE_CONFIG_DESCRIPTION, &svc_desc);
			CloseServiceHandle(svc_handle);
		}अन्यथा{
			print_error();
		}
		CloseServiceHandle(sc_manager);
	}अन्यथा{
		print_error();
	}
}


शून्य service_uninstall(अक्षर *name)
{
	SC_HANDLE sc_manager, svc_handle;
	SERVICE_STATUS status;

	name = fix_name(name);

	sc_manager = OpenSCManager(रिक्त, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CONNECT);
	जर(sc_manager){
		svc_handle = OpenService(sc_manager, name, SERVICE_QUERY_STATUS | DELETE);
		जर(svc_handle){
			जर(QueryServiceStatus(svc_handle, &status)){
				जर(status.dwCurrentState == SERVICE_STOPPED){
					DeleteService(svc_handle);
				}
			}
			CloseServiceHandle(svc_handle);
		}अन्यथा{
			print_error();
		}
		CloseServiceHandle(sc_manager);
	}अन्यथा{
		print_error();
	}
}


शून्य service_run(अक्षर *name)
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
