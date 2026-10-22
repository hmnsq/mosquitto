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

#समावेश "config.h"

#समावेश <ctype.h>
#समावेश <limits.h>
#समावेश <stdio.h>
#समावेश <stdlib.h>
#समावेश <string.h>
#समावेश <errno.h>

#अगर_घोषित WIN32
#अन्यथा
#  include <dirent.h>
#पूर्ण_अगर

#अगर_अघोषित WIN32
#  include <strings.h>
#  include <netdb.h>
#  include <sys/socket.h>
#अन्यथा
#  include <winsock2.h>
#  include <ws2tcpip.h>
#पूर्ण_अगर

#अगर defined(__HAIKU__)
#  include <syslog.h>
#या_अगर !defined(WIN32) && !defined(__CYGWIN__) && !defined(__QNX__)
#  include <sys/syslog.h>
#पूर्ण_अगर

#समावेश "mosquitto_broker_internal.h"
#समावेश "tls_mosq.h"
#समावेश "util_mosq.h"
#समावेश "mosquitto/mqtt_protocol.h"


अटल पूर्णांक scmp_p(स्थिर व्योम *p1, स्थिर व्योम *p2)
{
	स्थिर अक्षर *s1 = *(स्थिर अक्षर **)p1;
	स्थिर अक्षर *s2 = *(स्थिर अक्षर **)p2;
	पूर्णांक परिणाम;

	जबतक(s1[0] && s2[0]){
		/* Sort by case insensitive part first */
		परिणाम = बड़े((अचिन्हित अक्षर)s1[0]) - बड़े((अचिन्हित अक्षर)s2[0]);
		अगर(परिणाम == 0){
			/* Case insensitive part matched, now distinguish between case */
			परिणाम = s1[0] - s2[0];
			अगर(परिणाम != 0){
				वापस परिणाम;
			}
		}अन्यथा{
			/* Return case insensitive match fail */
			वापस परिणाम;
		}
		s1++;
		s2++;
	}

	वापस s1[0] - s2[0];
}

#अगर_घोषित WIN32


पूर्णांक config__get_dir_files(स्थिर अक्षर *include_dir, अक्षर ***files, पूर्णांक *file_गिनती)
{
	माप_प्रकार len;
	अक्षर **l_files = शून्य;
	पूर्णांक l_file_गिनती = 0;
	अक्षर **files_tmp;

	HANDLE fh;
	अक्षर dirpath[MAX_PATH];
	WIN32_FIND_तथ्य find_तथ्य;

	snprintf(dirpath, MAX_PATH, "%s\\*.conf", include_dir);
	fh = FindFirstFile(dirpath, &find_तथ्य);
	अगर(fh == INVALID_HANDLE_मूल्य){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: Unable to open include_dir '%s'.", include_dir);
		वापस 1;
	}

	करो{
		len = म_माप(include_dir)+1+म_माप(find_तथ्य.cFileName)+1;

		l_file_गिनती++;
		files_tmp = mosquitto_realloc(l_files, l_file_गिनती*माप(अक्षर *));
		अगर(!files_tmp){
			क्रम(पूर्णांक i=0; i<l_file_गिनती-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			FindClose(fh);
			वापस MOSQ_ERR_NOMEM;
		}
		l_files = files_tmp;

		l_files[l_file_गिनती-1] = mosquitto_malloc(len+1);
		अगर(!l_files[l_file_गिनती-1]){
			क्रम(पूर्णांक i=0; i<l_file_गिनती-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			FindClose(fh);
			वापस MOSQ_ERR_NOMEM;
		}
		snprintf(l_files[l_file_गिनती-1], len, "%s/%s", include_dir, find_तथ्य.cFileName);
		l_files[l_file_गिनती-1][len] = '\0';
	}जबतक(Find_अग्र_File(fh, &find_तथ्य));

	FindClose(fh);

	अगर(l_files){
		क्विक(l_files, l_file_गिनती, माप(अक्षर *), scmp_p);
	}
	*files = l_files;
	*file_गिनती = l_file_गिनती;

	वापस 0;
}
#पूर्ण_अगर


#अगर_अघोषित WIN32


पूर्णांक config__get_dir_files(स्थिर अक्षर *include_dir, अक्षर ***files, पूर्णांक *file_गिनती)
{
	अक्षर **l_files = शून्य;
	पूर्णांक l_file_गिनती = 0;
	अक्षर **files_tmp;
	माप_प्रकार len;

	सूची *dh;
	काष्ठा dirent *de;

	dh = सूची_खोलो(include_dir);
	अगर(!dh){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: Unable to open include_dir '%s'.", include_dir);
		वापस 1;
	}
	जबतक((de = सूची_पढ़ो(dh)) != शून्य){
		अगर(म_माप(de->d_name) > 5){
			अगर(!म_भेद(&de->d_name[म_माप(de->d_name)-5], ".conf")){
				len = म_माप(include_dir)+1+म_माप(de->d_name)+1;

				l_file_गिनती++;
				files_tmp = mosquitto_realloc(l_files, (माप_प्रकार)l_file_गिनती*माप(अक्षर *));
				अगर(!files_tmp){
					जाओ error;
				}
				l_files = files_tmp;

				l_files[l_file_गिनती-1] = mosquitto_malloc(len+1);
				अगर(!l_files[l_file_गिनती-1]){
					जाओ error;
				}
				snprintf(l_files[l_file_गिनती-1], len, "%s/%s", include_dir, de->d_name);
				l_files[l_file_गिनती-1][len] = '\0';
			}
		}
	}
	बंद_सूची(dh);

	अगर(l_files){
		क्विक(l_files, (माप_प्रकार)l_file_गिनती, माप(अक्षर *), scmp_p);
	}
	*files = l_files;
	*file_गिनती = l_file_गिनती;

	वापस 0;
error:
	क्रम(पूर्णांक i=0; i<l_file_गिनती-1; i++){
		mosquitto_FREE(l_files[i]);
	}
	mosquitto_FREE(l_files);
	बंद_सूची(dh);
	वापस MOSQ_ERR_NOMEM;
}
#पूर्ण_अगर


