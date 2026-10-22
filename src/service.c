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

#ఒకవేళ defined(WIN32) || defined(__CYGWIN__)

#అన్తర్భావ "config.h"

#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ <windows.h>

బాహ్య పూర్ణాంకం g_నిరంతరం;
SERVICE_స్థితి_HANDLE service_handle = 0;
స్థిర SERVICE_స్థితి service_స్థితి;
పూర్ణాంకం ప్రధాన(పూర్ణాంకం argc, అక్షరం *argv[]);


స్థిర అక్షరం *fix_name(అక్షరం *name)
{
	size_t len;

	ఒకవేళ(strrchr(name, '\\')){
		name = strrchr(name, '\\') + 1;
	}
	len = అక్షరశ్రేణి_పొడవు(name);
	ఒకవేళ(!strcasecmp(&name[len-4], ".exe")){
		name[len-4] = '\0';
	}

	తిరిగి name;
}


స్థిర శూన్య print_error(శూన్య)
{
	అక్షరం *buf = ఖాళీ;

	Format_సందేశం(FORMAT_సందేశం_ALLOCATE_బఫరు | FORMAT_సందేశం_FROM_SYSTEM,
			ఖాళీ, Get_చివరి_Error(), LANG_NEUTRAL, (LPTSTR)&buf, 0, ఖాళీ);

	fprintf(stderr, "Error: %s\n", buf);
	LocalFree(buf);
}


/* Service control callback */
శూన్య __stdcall service_handler(DWORD fdwControl)
{
	ఎంపిక(fdwControl){
		స్థితి SERVICE_CONTROL_CONTINUE:
			/* Continue from Paused state. */
			ఆపు;
		స్థితి SERVICE_CONTROL_PAUSE:
			/* Pause service. */
			ఆపు;
		స్థితి SERVICE_CONTROL_SHUTDOWN:
		/* System is shutting down. */
		స్థితి SERVICE_CONTROL_STOP:
			/* Service should stop. */
			service_స్థితి.dwCurrentState = SERVICE_STOP_PENDING;
			సమితి_Service_స్థితి(service_handle, &service_స్థితి);
			g_నిరంతరం = 0;
			ఆపు;
	}
}


/* Function called when started as a service. */
శూన్య __stdcall service_main(DWORD dwArgc, LPTSTR *lpszArgv)
{
	అక్షరం **argv;
	పూర్ణాంకం argc = 1;
	అక్షరం conf_మార్గం[MAX_మార్గం + 20];
	పూర్ణాంకం rc;
	అక్షరం *name;
	అక్షరం env_name[MAX_మార్గం];

	వాడని(dwArgc);

	name = fix_name(lpszArgv[0]);
	snprintf(env_name, పరిమాణం(env_name), "%s_DIR", name);
	కొరకు(పూర్ణాంకం i=0; i<అక్షరశ్రేణి_పొడవు(env_name); i++){
		ఒకవేళ(env_name[i]>='A' && env_name[i]<='Z'){
			/* Keep upper case letter */
		}లేకపోతే ఒకవేళ(env_name[i]>='0' && env_name[i]<='9'){
			/* Keep number */
		}లేకపోతే ఒకవేళ(env_name[i]>='a' && env_name[i]<='z'){
			/* Convert lower case to upper case */
			env_name[i] -= 32;
		}లేకపోతే{
			env_name[i] = '_';
		}
	}

	service_handle = రిజిస్టరు_Service_Ctrl_Handler(name, service_handler);
	ఒకవేళ(service_handle){
		memset(conf_మార్గం, 0, పరిమాణం(conf_మార్గం));
		rc = GetEnvironmentVariable(env_name, conf_మార్గం, MAX_మార్గం);
		ఒకవేళ(!rc || rc == MAX_మార్గం){
			service_స్థితి.dwCurrentState = SERVICE_STOPPED;
			సమితి_Service_స్థితి(service_handle, &service_స్థితి);
			తిరిగి;
		}
		strcat(conf_మార్గం, "\\mosquitto.conf");

		argv = mosquitto_malloc(పరిమాణం(అక్షరం *)*3);
		argv[0] = name;
		argv[1] = "-c";
		argv[2] = conf_మార్గం;
		argc = 3;

		service_స్థితి.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		service_స్థితి.dwCurrentState = SERVICE_RUNNING;
		service_స్థితి.dwControlsAccepted = SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_STOP;
		service_స్థితి.dw_Win32_Exit_సంకేతం = NO_ERROR;
		service_స్థితి.dw_Check_బిందువు = 0;
		సమితి_Service_స్థితి(service_handle, &service_స్థితి);

		ప్రధాన(argc, argv);
		mosquitto_FREE(argv);

		service_స్థితి.dwCurrentState = SERVICE_STOPPED;
		సమితి_Service_స్థితి(service_handle, &service_స్థితి);
	}
}


శూన్య service_install(అక్షరం *name)
{
	SC_HANDLE sc_manager, svc_handle;
	అక్షరం service_string[MAX_మార్గం + 20];
	అక్షరం exe_మార్గం[MAX_మార్గం + 1];
	అక్షరం display_name[MAX_మార్గం+1];
	SERVICE_DESCRIPTION svc_desc;

	memset(exe_మార్గం, 0, పరిమాణం(exe_మార్గం));
	ఒకవేళ(GetModuleFileName(ఖాళీ, exe_మార్గం, MAX_మార్గం) == MAX_మార్గం){
		fprintf(stderr, "Error: Path too long.\n");
		తిరిగి;
	}
	snprintf(service_string, పరిమాణం(service_string), "\"%s\" run", exe_మార్గం);

	name = fix_name(name);

	sc_manager = OpenSCManager(ఖాళీ, ఖాళీ, SC_MANAGER_CREATE_SERVICE);
	ఒకవేళ(sc_manager){
		ఒకవేళ(!strcmp(name, "mosquitto")){
			snprintf(display_name, పరిమాణం(display_name), "Mosquitto Broker");
		}లేకపోతే{
			snprintf(display_name, పరిమాణం(display_name), "Mosquitto Broker (%s.exe)", name);
		}

		svc_handle = CreateService(sc_manager, name, display_name,
				SERVICE_ప్రారంభం | SERVICE_STOP | SERVICE_CHANGE_CONFIG,
				SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_ప్రారంభం, SERVICE_ERROR_సాధారణ,
				service_string, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ);

		ఒకవేళ(svc_handle){
			memset(&svc_desc, 0, పరిమాణం(svc_desc));
			svc_desc.lpDescription = "Eclipse Mosquitto MQTT v5/v3.1.1 broker";
			ChangeServiceConfig2(svc_handle, SERVICE_CONFIG_DESCRIPTION, &svc_desc);
			CloseServiceHandle(svc_handle);
		}లేకపోతే{
			print_error();
		}
		CloseServiceHandle(sc_manager);
	}లేకపోతే{
		print_error();
	}
}


శూన్య service_uninstall(అక్షరం *name)
{
	SC_HANDLE sc_manager, svc_handle;
	SERVICE_స్థితి స్థితి_2;

	name = fix_name(name);

	sc_manager = OpenSCManager(ఖాళీ, SERVICES_చురుకైన_DATABASE, SC_MANAGER_CONNECT);
	ఒకవేళ(sc_manager){
		svc_handle = OpenService(sc_manager, name, SERVICE_QUERY_స్థితి | DELETE);
		ఒకవేళ(svc_handle){
			ఒకవేళ(Query_Service_స్థితి(svc_handle, &స్థితి_2)){
				ఒకవేళ(స్థితి_2.dwCurrentState == SERVICE_STOPPED){
					DeleteService(svc_handle);
				}
			}
			CloseServiceHandle(svc_handle);
		}లేకపోతే{
			print_error();
		}
		CloseServiceHandle(sc_manager);
	}లేకపోతే{
		print_error();
	}
}


శూన్య service_నిరంతరం(అక్షరం *name)
{
	SERVICE_పట్టిక_నమోదు ste[] = {
		{ name, service_main },
		{ ఖాళీ, ఖాళీ }
	};
	name = fix_name(name);
	ste[0].lpServiceName = name;
	ప్రారంభం_Service_Ctrl_Dispatcher(ste);
}

#అయితే_ముగింపు
