/*
Copyright (c) 2009-2021 Roger Light <roger@atchoo.org>

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

#अन्तर्भाव "config.h"

#अन्तर्भाव <ctype.h>
#अन्तर्भाव <limits.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <errno.h>

#ifdef WIN32
#अन्यथा
#  include <dirent.h>
#endif

#ifndef WIN32
#  include <strings.h>
#  include <netdb.h>
#  include <sys/socket.h>
#अन्यथा
#  include <winsock2.h>
#  include <ws2tcpip.h>
#endif

#जर defined(__HAIKU__)
#  include <syslog.h>
#किंवाजर !defined(WIN32) && !defined(__CYGWIN__) && !defined(__QNX__)
#  include <sys/syslog.h>
#endif

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "tls_mosq.h"
#अन्तर्भाव "util_mosq.h"
#अन्तर्भाव "mosquitto/mqtt_protocol.h"


स्थिर पूर्णांक scmp_p(स्थिरांक शून्य *p1, स्थिरांक शून्य *p2)
{
	स्थिरांक अक्षर *s1 = *(स्थिरांक अक्षर **)p1;
	स्थिरांक अक्षर *s2 = *(स्थिरांक अक्षर **)p2;
	पूर्णांक result;

	पर्यंत(s1[0] && s2[0]){
		/* Sort by case insensitive part first */
		result = बड़े((अचिन्हित अक्षर)s1[0]) - बड़े((अचिन्हित अक्षर)s2[0]);
		जर(result == 0){
			/* Case insensitive part matched, now distinguish between case */
			result = s1[0] - s2[0];
			जर(result != 0){
				परत result;
			}
		}अन्यथा{
			/* Return case insensitive match fail */
			परत result;
		}
		s1++;
		s2++;
	}

	परत s1[0] - s2[0];
}

#ifdef WIN32


पूर्णांक config__get_dir_files(स्थिरांक अक्षर *include_dir, अक्षर ***files, पूर्णांक *file_मोजणी)
{
	माप_प्रकार len;
	अक्षर **l_files = रिक्त;
	पूर्णांक l_file_मोजणी = 0;
	अक्षर **files_tmp;

	HANDLE fh;
	अक्षर dirpath[MAX_PATH];
	WIN32_FIND_DATA find_data;

	snprintf(dirpath, MAX_PATH, "%s\\*.conf", include_dir);
	fh = FindFirstFile(dirpath, &find_data);
	जर(fh == INVALID_HANDLE_मूल्य){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to open include_dir '%s'.", include_dir);
		परत 1;
	}

	करा{
		len = म_माप(include_dir)+1+म_माप(find_data.cFileName)+1;

		l_file_मोजणी++;
		files_tmp = mosquitto_realloc(l_files, l_file_मोजणी*आकार(अक्षर *));
		जर(!files_tmp){
			साठी(पूर्णांक i=0; i<l_file_मोजणी-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			FindClose(fh);
			परत MOSQ_ERR_NOMEM;
		}
		l_files = files_tmp;

		l_files[l_file_मोजणी-1] = mosquitto_malloc(len+1);
		जर(!l_files[l_file_मोजणी-1]){
			साठी(पूर्णांक i=0; i<l_file_मोजणी-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			FindClose(fh);
			परत MOSQ_ERR_NOMEM;
		}
		snprintf(l_files[l_file_मोजणी-1], len, "%s/%s", include_dir, find_data.cFileName);
		l_files[l_file_मोजणी-1][len] = '\0';
	}पर्यंत(FindNextFile(fh, &find_data));

	FindClose(fh);

	जर(l_files){
		क्विक(l_files, l_file_मोजणी, आकार(अक्षर *), scmp_p);
	}
	*files = l_files;
	*file_मोजणी = l_file_मोजणी;

	परत 0;
}
#endif


#ifndef WIN32


पूर्णांक config__get_dir_files(स्थिरांक अक्षर *include_dir, अक्षर ***files, पूर्णांक *file_मोजणी)
{
	अक्षर **l_files = रिक्त;
	पूर्णांक l_file_मोजणी = 0;
	अक्षर **files_tmp;
	माप_प्रकार len;

	सूची *dh;
	संरचना dirent *de;

	dh = सूची_खोलो(include_dir);
	जर(!dh){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to open include_dir '%s'.", include_dir);
		परत 1;
	}
	पर्यंत((de = सूची_पढ़ो(dh)) != रिक्त){
		जर(म_माप(de->d_name) > 5){
			जर(!म_भेद(&de->d_name[म_माप(de->d_name)-5], ".conf")){
				len = म_माप(include_dir)+1+म_माप(de->d_name)+1;

				l_file_मोजणी++;
				files_tmp = mosquitto_realloc(l_files, (माप_प्रकार)l_file_मोजणी*आकार(अक्षर *));
				जर(!files_tmp){
					जा error;
				}
				l_files = files_tmp;

				l_files[l_file_मोजणी-1] = mosquitto_malloc(len+1);
				जर(!l_files[l_file_मोजणी-1]){
					जा error;
				}
				snprintf(l_files[l_file_मोजणी-1], len, "%s/%s", include_dir, de->d_name);
				l_files[l_file_मोजणी-1][len] = '\0';
			}
		}
	}
	बंद_सूची(dh);

	जर(l_files){
		क्विक(l_files, (माप_प्रकार)l_file_मोजणी, आकार(अक्षर *), scmp_p);
	}
	*files = l_files;
	*file_मोजणी = l_file_मोजणी;

	परत 0;
error:
	साठी(पूर्णांक i=0; i<l_file_मोजणी-1; i++){
		mosquitto_FREE(l_files[i]);
	}
	mosquitto_FREE(l_files);
	बंद_सूची(dh);
	परत MOSQ_ERR_NOMEM;
}
#endif


