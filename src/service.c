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

#यदि defined(WIN32) || defined(__CYGWIN__)

#अन्तर्भाव "config.h"

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव <windows.h>

बाह्य पूर्णाङ्क g_सततम्;
SERVICE_स्थितिः_HANDLE service_handle = 0;
स्थिर SERVICE_स्थितिः service_स्थितिः;
पूर्णाङ्क मुख्यम्(पूर्णाङ्क argc, वर्ण *argv[]);


स्थिर वर्ण *fix_name(वर्ण *name)
{
	size_t len;

	यदि(strrchr(name, '\\')){
		name = strrchr(name, '\\') + 1;
	}
	len = वाक्यदैर्घ्यम्(name);
	यदि(!strcasecmp(&name[len-4], ".exe")){
		name[len-4] = '\0';
	}

	प्रत्यागम name;
}


स्थिर शून्य print_error(शून्य)
{
	वर्ण *buf = रिक्त;

	Format_सन्देशः(FORMAT_सन्देशः_ALLOCATE_बफर | FORMAT_सन्देशः_FROM_SYSTEM,
			रिक्त, Get_चरमः_Error(), LANG_NEUTRAL, (LPTSTR)&buf, 0, रिक्त);

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
			service_स्थितिः.dwCurrentState = SERVICE_STOP_PENDING;
			समुच्चयः_Service_स्थितिः(service_handle, &service_स्थितिः);
			g_सततम् = 0;
			भङ्ग;
	}
}


/* Function called when started as a service. */
शून्य __stdcall service_main(DWORD dwArgc, LPTSTR *lpszArgv)
{
	वर्ण **argv;
	पूर्णाङ्क argc = 1;
	वर्ण conf_मार्गः[MAX_मार्गः + 20];
	पूर्णाङ्क rc;
	वर्ण *name;
	वर्ण env_name[MAX_मार्गः];

	अप्रयुक्तम्(dwArgc);

	name = fix_name(lpszArgv[0]);
	snprintf(env_name, परिमाण(env_name), "%s_DIR", name);
	प्रति(पूर्णाङ्क i=0; i<वाक्यदैर्घ्यम्(env_name); i++){
		यदि(env_name[i]>='A' && env_name[i]<='Z'){
			/* Keep upper case letter */
		}अन्यथा यदि(env_name[i]>='0' && env_name[i]<='9'){
			/* Keep number */
		}अन्यथा यदि(env_name[i]>='a' && env_name[i]<='z'){
			/* Convert lower case to upper case */
			env_name[i] -= 32;
		}अन्यथा{
			env_name[i] = '_';
		}
	}

	service_handle = पञ्जिकाकोशः_Service_Ctrl_Handler(name, service_handler);
	यदि(service_handle){
		स्मृतिं_पूरय(conf_मार्गः, 0, परिमाण(conf_मार्गः));
		rc = GetEnvironmentVariable(env_name, conf_मार्गः, MAX_मार्गः);
		यदि(!rc || rc == MAX_मार्गः){
			service_स्थितिः.dwCurrentState = SERVICE_STOPPED;
			समुच्चयः_Service_स्थितिः(service_handle, &service_स्थितिः);
			प्रत्यागम;
		}
		वाक्यं_योजय(conf_मार्गः, "\\mosquitto.conf");

		argv = mosquitto_malloc(परिमाण(वर्ण *)*3);
		argv[0] = name;
		argv[1] = "-c";
		argv[2] = conf_मार्गः;
		argc = 3;

		service_स्थितिः.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		service_स्थितिः.dwCurrentState = SERVICE_RUNNING;
		service_स्थितिः.dwControlsAccepted = SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_STOP;
		service_स्थितिः.dw_Win32_Exit_संहिता = NO_ERROR;
		service_स्थितिः.dwCheckPoint = 0;
		समुच्चयः_Service_स्थितिः(service_handle, &service_स्थितिः);

		मुख्यम्(argc, argv);
		mosquitto_FREE(argv);

		service_स्थितिः.dwCurrentState = SERVICE_STOPPED;
		समुच्चयः_Service_स्थितिः(service_handle, &service_स्थितिः);
	}
}


शून्य service_install(वर्ण *name)
{
	SC_HANDLE sc_manager, svc_handle;
	वर्ण service_string[MAX_मार्गः + 20];
	वर्ण exe_मार्गः[MAX_मार्गः + 1];
	वर्ण display_name[MAX_मार्गः+1];
	SERVICE_DESCRIPTION svc_desc;

	स्मृतिं_पूरय(exe_मार्गः, 0, परिमाण(exe_मार्गः));
	यदि(GetModuleFileName(रिक्त, exe_मार्गः, MAX_मार्गः) == MAX_मार्गः){
		fprintf(stderr, "Error: Path too long.\n");
		प्रत्यागम;
	}
	snprintf(service_string, परिमाण(service_string), "\"%s\" run", exe_मार्गः);

	name = fix_name(name);

	sc_manager = OpenSCManager(रिक्त, रिक्त, SC_MANAGER_CREATE_SERVICE);
	यदि(sc_manager){
		यदि(!वाक्यं_तुलय(name, "mosquitto")){
			snprintf(display_name, परिमाण(display_name), "Mosquitto Broker");
		}अन्यथा{
			snprintf(display_name, परिमाण(display_name), "Mosquitto Broker (%s.exe)", name);
		}

		svc_handle = CreateService(sc_manager, name, display_name,
				SERVICE_START | SERVICE_STOP | SERVICE_CHANGE_CONFIG,
				SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_प्रसामान्यम्,
				service_string, रिक्त, रिक्त, रिक्त, रिक्त, रिक्त);

		यदि(svc_handle){
			स्मृतिं_पूरय(&svc_desc, 0, परिमाण(svc_desc));
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


शून्य service_uninstall(वर्ण *name)
{
	SC_HANDLE sc_manager, svc_handle;
	SERVICE_स्थितिः स्थितिः;

	name = fix_name(name);

	sc_manager = OpenSCManager(रिक्त, SERVICES_सक्रिय_DATABASE, SC_MANAGER_CONNECT);
	यदि(sc_manager){
		svc_handle = OpenService(sc_manager, name, SERVICE_QUERY_स्थितिः | DELETE);
		यदि(svc_handle){
			यदि(Query_Service_स्थितिः(svc_handle, &स्थितिः)){
				यदि(स्थितिः.dwCurrentState == SERVICE_STOPPED){
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


शून्य service_सततम्(वर्ण *name)
{
	SERVICE_सारणी_प्रविष्टिः ste[] = {
		{ name, service_main },
		{ रिक्त, रिक्त }
	};
	name = fix_name(name);
	ste[0].lpServiceName = name;
	StartServiceCtrlDispatcher(ste);
}

#यदि_अन्तः
