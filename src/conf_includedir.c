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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ <ctype.h>
#ಅನ್ತರ್ಭಾವ <limits.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <errno.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WIN32
#ಇಲ್ಲದಿದ್ದರೆ
#  include <dirent.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
#  include <strings.h>
#  include <netdb.h>
#  include <sys/socket.h>
#ಇಲ್ಲದಿದ್ದರೆ
#  include <winsock2.h>
#  include <ws2tcpip.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(__HAIKU__)
#  include <syslog.h>
#ಅಥವಾಆದರೆ !defined(WIN32) && !defined(__CYGWIN__) && !defined(__QNX__)
#  include <sys/syslog.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "tls_mosq.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/mqtt_protocol.h"


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ scmp_p(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p1, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p2)
{
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s1 = *(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **)p1;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s2 = *(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **)p2;
	ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ;

	ಆಗ(s1[0] && s2[0]){
		/* Sort by case insensitive part first */
		ಫಲಿತಾಂಶ = ದೊಡ್ಡ_ಅಕ್ಷರ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)s1[0]) - ದೊಡ್ಡ_ಅಕ್ಷರ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)s2[0]);
		ಆದರೆ(ಫಲಿತಾಂಶ == 0){
			/* Case insensitive part matched, now distinguish between case */
			ಫಲಿತಾಂಶ = s1[0] - s2[0];
			ಆದರೆ(ಫಲಿತಾಂಶ != 0){
				ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
			}
		}ಇಲ್ಲದಿದ್ದರೆ{
			/* Return case insensitive match fail */
			ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
		}
		s1++;
		s2++;
	}

	ಹಿಂತಿರುಗಿ s1[0] - s2[0];
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WIN32


ಪೂರ್ಣಾಂಕ config__get_dir_files(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *include_dir, ಅಕ್ಷರ ***files, ಪೂರ್ಣಾಂಕ *file_ಎಣಿಕೆ)
{
	size_t len;
	ಅಕ್ಷರ **l_files = ಖಾಲಿ;
	ಪೂರ್ಣಾಂಕ l_file_ಎಣಿಕೆ = 0;
	ಅಕ್ಷರ **files_tmp;

	HANDLE fh;
	ಅಕ್ಷರ dirpath[MAX_ಮಾರ್ಗ];
	WIN32_ಕಂಡುಹಿಡಿ_ದತ್ತಾಂಶ ಕಂಡುಹಿಡಿ_ದತ್ತಾಂಶ;

	snprintf(dirpath, MAX_ಮಾರ್ಗ, "%s\\*.conf", include_dir);
	fh = ಕಂಡುಹಿಡಿ_ಮೊದಲ_File(dirpath, &ಕಂಡುಹಿಡಿ_ದತ್ತಾಂಶ);
	ಆದರೆ(fh == INVALID_HANDLE_ಮೌಲ್ಯ){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Unable to open include_dir '%s'.", include_dir);
		ಹಿಂತಿರುಗಿ 1;
	}

	ಮಾಡು{
		len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(include_dir)+1+ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಕಂಡುಹಿಡಿ_ದತ್ತಾಂಶ.cFileName)+1;

		l_file_ಎಣಿಕೆ++;
		files_tmp = mosquitto_realloc(l_files, l_file_ಎಣಿಕೆ*ಗಾತ್ರ(ಅಕ್ಷರ *));
		ಆದರೆ(!files_tmp){
			ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<l_file_ಎಣಿಕೆ-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			ಕಂಡುಹಿಡಿ_Close(fh);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
		}
		l_files = files_tmp;

		l_files[l_file_ಎಣಿಕೆ-1] = mosquitto_malloc(len+1);
		ಆದರೆ(!l_files[l_file_ಎಣಿಕೆ-1]){
			ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<l_file_ಎಣಿಕೆ-1; i++){
				mosquitto_FREE(l_files[i]);
			}
			mosquitto_FREE(l_files);
			ಕಂಡುಹಿಡಿ_Close(fh);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
		}
		snprintf(l_files[l_file_ಎಣಿಕೆ-1], len, "%s/%s", include_dir, ಕಂಡುಹಿಡಿ_ದತ್ತಾಂಶ.cFileName);
		l_files[l_file_ಎಣಿಕೆ-1][len] = '\0';
	}ಆಗ(ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_File(fh, &ಕಂಡುಹಿಡಿ_ದತ್ತಾಂಶ));

	ಕಂಡುಹಿಡಿ_Close(fh);

	ಆದರೆ(l_files){
		qsort(l_files, l_file_ಎಣಿಕೆ, ಗಾತ್ರ(ಅಕ್ಷರ *), scmp_p);
	}
	*files = l_files;
	*file_ಎಣಿಕೆ = l_file_ಎಣಿಕೆ;

	ಹಿಂತಿರುಗಿ 0;
}
#ಆದರೆ_ಅಂತ್ಯ


#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32


ಪೂರ್ಣಾಂಕ config__get_dir_files(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *include_dir, ಅಕ್ಷರ ***files, ಪೂರ್ಣಾಂಕ *file_ಎಣಿಕೆ)
{
	ಅಕ್ಷರ **l_files = ಖಾಲಿ;
	ಪೂರ್ಣಾಂಕ l_file_ಎಣಿಕೆ = 0;
	ಅಕ್ಷರ **files_tmp;
	size_t len;

	DIR *dh;
	ರಚನೆ dirent *de;

	dh = opendir(include_dir);
	ಆದರೆ(!dh){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Unable to open include_dir '%s'.", include_dir);
		ಹಿಂತಿರುಗಿ 1;
	}
	ಆಗ((de = readdir(dh)) != ಖಾಲಿ){
		ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(de->d_name) > 5){
			ಆದರೆ(!strcmp(&de->d_name[ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(de->d_name)-5], ".conf")){
				len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(include_dir)+1+ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(de->d_name)+1;

				l_file_ಎಣಿಕೆ++;
				files_tmp = mosquitto_realloc(l_files, (size_t)l_file_ಎಣಿಕೆ*ಗಾತ್ರ(ಅಕ್ಷರ *));
				ಆದರೆ(!files_tmp){
					ಹೋಗು error;
				}
				l_files = files_tmp;

				l_files[l_file_ಎಣಿಕೆ-1] = mosquitto_malloc(len+1);
				ಆದರೆ(!l_files[l_file_ಎಣಿಕೆ-1]){
					ಹೋಗು error;
				}
				snprintf(l_files[l_file_ಎಣಿಕೆ-1], len, "%s/%s", include_dir, de->d_name);
				l_files[l_file_ಎಣಿಕೆ-1][len] = '\0';
			}
		}
	}
	closedir(dh);

	ಆದರೆ(l_files){
		qsort(l_files, (size_t)l_file_ಎಣಿಕೆ, ಗಾತ್ರ(ಅಕ್ಷರ *), scmp_p);
	}
	*files = l_files;
	*file_ಎಣಿಕೆ = l_file_ಎಣಿಕೆ;

	ಹಿಂತಿರುಗಿ 0;
error:
	ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<l_file_ಎಣಿಕೆ-1; i++){
		mosquitto_FREE(l_files[i]);
	}
	mosquitto_FREE(l_files);
	closedir(dh);
	ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
}
#ಆದರೆ_ಅಂತ್ಯ


