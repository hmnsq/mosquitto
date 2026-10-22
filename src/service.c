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

#अगर defined(WIN32) || defined(__CYGWIN__)

#समावेश "config.h"

#समावेश "mosquitto_broker_internal.h"
#समावेश <windows.h>

बाह्य पूर्णांक g_run;
SERVICE_STATUS_HANDLE service_handle = 0;
अटल SERVICE_STATUS service_status;
पूर्णांक मुख्य(पूर्णांक argc, अक्षर *argv[]);


अटल अक्षर *fix_name(अक्षर *name)
{
	माप_प्रकार len;

	अगर(म_खोजप(name, '\\')){
		name = म_खोजप(name, '\\') + 1;
	}
	len = म_माप(name);
	अगर(!strcasecmp(&name[len-4], ".exe")){
		name[len-4] = '\0';
	}

	वापस name;
}


अटल व्योम print_error(व्योम)
{
	अक्षर *buf = शून्य;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_बफर | FORMAT_MESSAGE_FROM_SYSTEM,
			शून्य, GetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, शून्य);

	ख_लिखो(मानक_त्रुटि, "Error: %s\n", buf);
	LocalFree(buf);
}


/* Service control callback */
व्योम __stdcall service_handler(DWORD fdwControl)
{
	चयन(fdwControl){
		हाल SERVICE_CONTROL_CONTINUE:
			/* Continue from Paused state. */
			अवरोध;
		हाल SERVICE_CONTROL_PAUSE:
			/* Pause service. */
			अवरोध;
		हाल SERVICE_CONTROL_SHUTDOWN:
		/* System is shutting down. */
		हाल SERVICE_CONTROL_STOP:
			/* Service should stop. */
			service_status.dwCurrentState = SERVICE_STOP_PENDING;
			SetServiceStatus(service_handle, &service_status);
			g_run = 0;
			अवरोध;
	}
}


/* Function called when started as a service. */
व्योम __stdcall service_main(DWORD dwArgc, LPTSTR *lpszArgv)
{
	अक्षर **argv;
	पूर्णांक argc = 1;
	अक्षर conf_path[MAX_PATH + 20];
	पूर्णांक rc;
	अक्षर *name;
	अक्षर env_name[MAX_PATH];

	UNUSED(dwArgc);

	name = fix_name(lpszArgv[0]);
	snprintf(env_name, माप(env_name), "%s_DIR", name);
	क्रम(पूर्णांक i=0; i<म_माप(env_name); i++){
		अगर(env_name[i]>='A' && env_name[i]<='Z'){
			/* Keep upper case letter */
		}अन्यथा अगर(env_name[i]>='0' && env_name[i]<='9'){
			/* Keep number */
		}अन्यथा अगर(env_name[i]>='a' && env_name[i]<='z'){
			/* Convert lower case to upper case */
			env_name[i] -= 32;
		}अन्यथा{
			env_name[i] = '_';
		}
	}

	service_handle = RegisterServiceCtrlHandler(name, service_handler);
	अगर(service_handle){
		स_रखो(conf_path, 0, माप(conf_path));
		rc = GetEnvironmentVariable(env_name, conf_path, MAX_PATH);
		अगर(!rc || rc == MAX_PATH){
			service_status.dwCurrentState = SERVICE_STOPPED;
			SetServiceStatus(service_handle, &service_status);
			वापस;
		}
		म_जोड़ो(conf_path, "\\mosquitto.conf");

		argv = mosquitto_malloc(माप(अक्षर *)*3);
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

		मुख्य(argc, argv);
		mosquitto_FREE(argv);

		service_status.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(service_handle, &service_status);
	}
}


व्योम service_install(अक्षर *name)
{
	SC_HANDLE sc_manager, svc_handle;
	अक्षर service_string[MAX_PATH + 20];
	अक्षर exe_path[MAX_PATH + 1];
	अक्षर display_name[MAX_PATH+1];
	SERVICE_DESCRIPTION svc_desc;

	स_रखो(exe_path, 0, माप(exe_path));
	अगर(GetModuleFileName(शून्य, exe_path, MAX_PATH) == MAX_PATH){
		ख_लिखो(मानक_त्रुटि, "Error: Path too long.\n");
		वापस;
	}
	snprintf(service_string, माप(service_string), "\"%s\" run", exe_path);

	name = fix_name(name);

	sc_manager = OpenSCManager(शून्य, शून्य, SC_MANAGER_CREATE_SERVICE);
	अगर(sc_manager){
		अगर(!म_भेद(name, "mosquitto")){
			snprintf(display_name, माप(display_name), "Mosquitto Broker");
		}अन्यथा{
			snprintf(display_name, माप(display_name), "Mosquitto Broker (%s.exe)", name);
		}

		svc_handle = CreateService(sc_manager, name, display_name,
				SERVICE_START | SERVICE_STOP | SERVICE_CHANGE_CONFIG,
				SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
				service_string, शून्य, शून्य, शून्य, शून्य, शून्य);

		अगर(svc_handle){
			स_रखो(&svc_desc, 0, माप(svc_desc));
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


व्योम service_uninstall(अक्षर *name)
{
	SC_HANDLE sc_manager, svc_handle;
	SERVICE_STATUS status;

	name = fix_name(name);

	sc_manager = OpenSCManager(शून्य, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CONNECT);
	अगर(sc_manager){
		svc_handle = OpenService(sc_manager, name, SERVICE_QUERY_STATUS | DELETE);
		अगर(svc_handle){
			अगर(QueryServiceStatus(svc_handle, &status)){
				अगर(status.dwCurrentState == SERVICE_STOPPED){
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


व्योम service_run(अक्षर *name)
{
	SERVICE_TABLE_ENTRY ste[] = {
		{ name, service_main },
		{ शून्य, शून्य }
	};
	name = fix_name(name);
	ste[0].lpServiceName = name;
	StartServiceCtrlDispatcher(ste);
}

#पूर्ण_अगर
