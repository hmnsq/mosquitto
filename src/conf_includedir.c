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
#अञ्ञथा
#  include <dirent.h>
#endif

#ifndef WIN32
#  include <strings.h>
#  include <netdb.h>
#  include <sys/socket.h>
#अञ्ञथा
#  include <winsock2.h>
#  include <ws2tcpip.h>
#endif

#सचे defined(__HAIKU__)
#  include <syslog.h>
#नोचेद्यदि !defined(WIN32) && !defined(__CYGWIN__) && !defined(__QNX__)
#  include <sys/syslog.h>
#endif

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "tls_mosq.h"
#अन्तर्भाव "util_mosq.h"
#अन्तर्भाव "mosquitto/mqtt_protocol.h"


स्थिर पूर्णाङ्क scmp_p(ध्रुव शून्य *p1, ध्रुव शून्य *p2)
{
	ध्रुव वर्ण *s1 = *(ध्रुव वर्ण **)p1;
	ध्रुव वर्ण *s2 = *(ध्रुव वर्ण **)p2;
	पूर्णाङ्क result;

	याव(s1[0] && s2[0]){
		/* Sort by case insensitive part first */
		result = toupper((unsigned वर्ण)s1[0]) - toupper((unsigned वर्ण)s2[0]);
		सचे(result == 0){
			/* Case insensitive part matched, now distinguish between case */
			result = s1[0] - s2[0];
			सचे(result != 0){
				पटिनिवत्त result;
			}
		}अञ्ञथा{
			/* Return case insensitive match fail */
			पटिनिवत्त result;
		}
		s1++;
		s2++;
	}

	पटिनिवत्त s1[0] - s2[0];
}

#ifdef WIN32


पूर्णाङ्क config__get_dir_files(ध्रुव वर्ण *include_dir, वर्ण ***files, पूर्णाङ्क *file_गणनम्)
{
	size_t len;
	वर्ण **l_files = रिक्त;
	पूर्णाङ्क l_file_गणनम् = 0;
	वर्ण **files_tmp;

	HANDLE fh;
	वर्ण dirpath[MAX_PATH];
	WIN32_FIND_DATA find_data;

	snprintf(dirpath, MAX_PATH, "%s\\*.conf", include_dir);
	fh = FindFirstFile(dirpath, &find_data);
	सचे(fh == INVALID_HANDLE_मानम्){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to open include_dir '%s'.", include_dir);
		पटिनिवत्त 1;
	}

	करोहि{
		len = strlen(include_dir)+1+strlen(find_data.cFileName)+1;

		l_file_गणनम्++;
		files_tmp = mosquitto_realloc(l_files, l_file_गणनम्*परिमाण(वर्ण *));
		सचे(!files_tmp){
			अत्थाय(पूर्णाङ्क i=0; i<l_file_गणनम्-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			FindClose(fh);
			पटिनिवत्त MOSQ_ERR_NOMEM;
		}
		l_files = files_tmp;

		l_files[l_file_गणनम्-1] = mosquitto_malloc(len+1);
		सचे(!l_files[l_file_गणनम्-1]){
			अत्थाय(पूर्णाङ्क i=0; i<l_file_गणनम्-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			FindClose(fh);
			पटिनिवत्त MOSQ_ERR_NOMEM;
		}
		snprintf(l_files[l_file_गणनम्-1], len, "%s/%s", include_dir, find_data.cFileName);
		l_files[l_file_गणनम्-1][len] = '\0';
	}याव(FindNextFile(fh, &find_data));

	FindClose(fh);

	सचे(l_files){
		qsort(l_files, l_file_गणनम्, परिमाण(वर्ण *), scmp_p);
	}
	*files = l_files;
	*file_गणनम् = l_file_गणनम्;

	पटिनिवत्त 0;
}
#endif


#ifndef WIN32


पूर्णाङ्क config__get_dir_files(ध्रुव वर्ण *include_dir, वर्ण ***files, पूर्णाङ्क *file_गणनम्)
{
	वर्ण **l_files = रिक्त;
	पूर्णाङ्क l_file_गणनम् = 0;
	वर्ण **files_tmp;
	size_t len;

	DIR *dh;
	संरचना dirent *de;

	dh = opendir(include_dir);
	सचे(!dh){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to open include_dir '%s'.", include_dir);
		पटिनिवत्त 1;
	}
	याव((de = readdir(dh)) != रिक्त){
		सचे(strlen(de->d_name) > 5){
			सचे(!strcmp(&de->d_name[strlen(de->d_name)-5], ".conf")){
				len = strlen(include_dir)+1+strlen(de->d_name)+1;

				l_file_गणनम्++;
				files_tmp = mosquitto_realloc(l_files, (size_t)l_file_गणनम्*परिमाण(वर्ण *));
				सचे(!files_tmp){
					गच्छ error;
				}
				l_files = files_tmp;

				l_files[l_file_गणनम्-1] = mosquitto_malloc(len+1);
				सचे(!l_files[l_file_गणनम्-1]){
					गच्छ error;
				}
				snprintf(l_files[l_file_गणनम्-1], len, "%s/%s", include_dir, de->d_name);
				l_files[l_file_गणनम्-1][len] = '\0';
			}
		}
	}
	closedir(dh);

	सचे(l_files){
		qsort(l_files, (size_t)l_file_गणनम्, परिमाण(वर्ण *), scmp_p);
	}
	*files = l_files;
	*file_गणनम् = l_file_गणनम्;

	पटिनिवत्त 0;
error:
	अत्थाय(पूर्णाङ्क i=0; i<l_file_गणनम्-1; i++){
		mosquitto_FREE(l_files[i]);
	}
	mosquitto_FREE(l_files);
	closedir(dh);
	पटिनिवत्त MOSQ_ERR_NOMEM;
}
#endif


