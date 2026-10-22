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

#यदि_परिभाषितम् WIN32
#अन्यथा
#  include <dirent.h>
#यदि_अन्तः

#यदि_अपरिभाषितम् WIN32
#  include <strings.h>
#  include <netdb.h>
#  include <sys/socket.h>
#अन्यथा
#  include <winsock2.h>
#  include <ws2tcpip.h>
#यदि_अन्तः

#यदि defined(__HAIKU__)
#  include <syslog.h>
#नोचेद्यदि !defined(WIN32) && !defined(__CYGWIN__) && !defined(__QNX__)
#  include <sys/syslog.h>
#यदि_अन्तः

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "tls_mosq.h"
#अन्तर्भाव "util_mosq.h"
#अन्तर्भाव "mosquitto/mqtt_protocol.h"


स्थिर पूर्णाङ्क scmp_p(ध्रुव शून्य *p1, ध्रुव शून्य *p2)
{
	ध्रुव वर्ण *s1 = *(ध्रुव वर्ण **)p1;
	ध्रुव वर्ण *s2 = *(ध्रुव वर्ण **)p2;
	पूर्णाङ्क परिणाम;

	यावत्(s1[0] && s2[0]){
		/* Sort by case insensitive part first */
		परिणाम = महाक्षरम्((अचिह्नित वर्ण)s1[0]) - महाक्षरम्((अचिह्नित वर्ण)s2[0]);
		यदि(परिणाम == 0){
			/* Case insensitive part matched, now distinguish between case */
			परिणाम = s1[0] - s2[0];
			यदि(परिणाम != 0){
				प्रत्यागम परिणाम;
			}
		}अन्यथा{
			/* Return case insensitive match fail */
			प्रत्यागम परिणाम;
		}
		s1++;
		s2++;
	}

	प्रत्यागम s1[0] - s2[0];
}

#यदि_परिभाषितम् WIN32


पूर्णाङ्क config__get_dir_files(ध्रुव वर्ण *include_dir, वर्ण ***files, पूर्णाङ्क *file_गणनम्)
{
	size_t len;
	वर्ण **l_files = रिक्त;
	पूर्णाङ्क l_file_गणनम् = 0;
	वर्ण **files_tmp;

	HANDLE fh;
	वर्ण dirpath[MAX_मार्गः];
	WIN32_FIND_दत्त find_दत्त;

	snprintf(dirpath, MAX_मार्गः, "%s\\*.conf", include_dir);
	fh = FindFirstFile(dirpath, &find_दत्त);
	यदि(fh == INVALID_HANDLE_मानम्){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to open include_dir '%s'.", include_dir);
		प्रत्यागम 1;
	}

	कुरु{
		len = वाक्यदैर्घ्यम्(include_dir)+1+वाक्यदैर्घ्यम्(find_दत्त.cFileName)+1;

		l_file_गणनम्++;
		files_tmp = mosquitto_realloc(l_files, l_file_गणनम्*परिमाण(वर्ण *));
		यदि(!files_tmp){
			प्रति(पूर्णाङ्क i=0; i<l_file_गणनम्-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			FindClose(fh);
			प्रत्यागम MOSQ_ERR_NOMEM;
		}
		l_files = files_tmp;

		l_files[l_file_गणनम्-1] = mosquitto_malloc(len+1);
		यदि(!l_files[l_file_गणनम्-1]){
			प्रति(पूर्णाङ्क i=0; i<l_file_गणनम्-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			FindClose(fh);
			प्रत्यागम MOSQ_ERR_NOMEM;
		}
		snprintf(l_files[l_file_गणनम्-1], len, "%s/%s", include_dir, find_दत्त.cFileName);
		l_files[l_file_गणनम्-1][len] = '\0';
	}यावत्(Find_अग्र_File(fh, &find_दत्त));

	FindClose(fh);

	यदि(l_files){
		शीघ्रं_क्रमय(l_files, l_file_गणनम्, परिमाण(वर्ण *), scmp_p);
	}
	*files = l_files;
	*file_गणनम् = l_file_गणनम्;

	प्रत्यागम 0;
}
#यदि_अन्तः


#यदि_अपरिभाषितम् WIN32


पूर्णाङ्क config__get_dir_files(ध्रुव वर्ण *include_dir, वर्ण ***files, पूर्णाङ्क *file_गणनम्)
{
	वर्ण **l_files = रिक्त;
	पूर्णाङ्क l_file_गणनम् = 0;
	वर्ण **files_tmp;
	size_t len;

	DIR *dh;
	संरचना dirent *de;

	dh = opendir(include_dir);
	यदि(!dh){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to open include_dir '%s'.", include_dir);
		प्रत्यागम 1;
	}
	यावत्((de = readdir(dh)) != रिक्त){
		यदि(वाक्यदैर्घ्यम्(de->d_name) > 5){
			यदि(!वाक्यं_तुलय(&de->d_name[वाक्यदैर्घ्यम्(de->d_name)-5], ".conf")){
				len = वाक्यदैर्घ्यम्(include_dir)+1+वाक्यदैर्घ्यम्(de->d_name)+1;

				l_file_गणनम्++;
				files_tmp = mosquitto_realloc(l_files, (size_t)l_file_गणनम्*परिमाण(वर्ण *));
				यदि(!files_tmp){
					गच्छ error;
				}
				l_files = files_tmp;

				l_files[l_file_गणनम्-1] = mosquitto_malloc(len+1);
				यदि(!l_files[l_file_गणनम्-1]){
					गच्छ error;
				}
				snprintf(l_files[l_file_गणनम्-1], len, "%s/%s", include_dir, de->d_name);
				l_files[l_file_गणनम्-1][len] = '\0';
			}
		}
	}
	closedir(dh);

	यदि(l_files){
		शीघ्रं_क्रमय(l_files, (size_t)l_file_गणनम्, परिमाण(वर्ण *), scmp_p);
	}
	*files = l_files;
	*file_गणनम् = l_file_गणनम्;

	प्रत्यागम 0;
error:
	प्रति(पूर्णाङ्क i=0; i<l_file_गणनम्-1; i++){
		mosquitto_FREE(l_files[i]);
	}
	mosquitto_FREE(l_files);
	closedir(dh);
	प्रत्यागम MOSQ_ERR_NOMEM;
}
#यदि_अन्तः


