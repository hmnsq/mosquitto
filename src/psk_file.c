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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ <ctype.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/mqtt_protocol.h"
#ಅನ್ತರ್ಭಾವ "send_mosq.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ psk__cleanup(ಸಂರಚನಾ mosquitto__psk **psk);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ psk__file_parse(ಸಂರಚನಾ mosquitto__psk **psk_id, ಧ್ರುವ ವರ್ಣ *psk_file);


ಸ್ಥಿರ ಶೂನ್ಯ psk__free_item(ಸಂರಚನಾ mosquitto__psk *psk)
{
	mosquitto_FREE(psk->username);
	mosquitto_FREE(psk->password);
	mosquitto_FREE(psk);
}


ಪೂರ್ಣಾಙ್ಕ psk_file__init(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	ವರ್ಣ *pskf = ರಿಕ್ತ;

	/* Load psk data if required. */
	ಯದಿ(db.config->per_listener_settings){
		ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<db.config->listener_ಎಣಿಕೆ; i++){
			pskf = db.config->listeners[i].security_options->psk_file;
			ಯದಿ(pskf){
				rc = psk__file_parse(&db.config->listeners[i].security_options->psk_id, pskf);
				ಯದಿ(rc){
					log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
					ಪ್ರತ್ಯಾಗಮ rc;
				}
			}
		}
	}ಅನ್ಯಥಾ{
		pskf = db.config->security_options.psk_file;
		ಯದಿ(pskf){
			rc = psk__file_parse(&db.config->security_options.psk_id, pskf);
			ಯದಿ(rc){
				log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
				ಪ್ರತ್ಯಾಗಮ rc;
			}
		}
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ psk_file__cleanup(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಙ್ಕ rc;

	rc = psk__cleanup(&db.config->security_options.psk_id);
	ಯದಿ(rc != MOSQ_ERR_SUCCESS){
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<db.config->listener_ಎಣಿಕೆ; i++){
		ಯದಿ(db.config->listeners[i].security_options->psk_id){
			rc = psk__cleanup(&db.config->listeners[i].security_options->psk_id);
			ಯದಿ(rc != MOSQ_ERR_SUCCESS){
				ಪ್ರತ್ಯಾಗಮ rc;
			}
		}
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ pwfile__parse(ಧ್ರುವ ವರ್ಣ *file, ಸಂರಚನಾ mosquitto__psk **ಬೇರು)
{
	FILE *pwfile;
	ಸಂರಚನಾ mosquitto__psk *psk;
	ವರ್ಣ *username, *password;
	ವರ್ಣ *saveptr = ರಿಕ್ತ;
	ವರ್ಣ *buf;
	ಪೂರ್ಣಾಙ್ಕ buflen = 256;

	buf = mosquitto_malloc((size_t)buflen);
	ಯದಿ(buf == ರಿಕ್ತ){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}

	pwfile = mosquitto_fopen(file, "rt", true);
	ಯದಿ(!pwfile){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Unable to open pwfile \"%s\".", file);
		mosquitto_FREE(buf);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_UNKNOWN;
	}

	ಯಾವತ್(!feof(pwfile)){
		ಯದಿ(mosquitto_fgets(&buf, &buflen, pwfile)){
			ಯದಿ(buf[0] == '#'){
				ಅನುವರ್ತ;
			}
			ಯದಿ(!strchr(buf, ':')){
				ಅನುವರ್ತ;
			}

			username = strtok_r(buf, ":", &saveptr);
			ಯದಿ(username){
				username = mosquitto_trimblanks(username);
				ಯದಿ(strlen(username) > 65535){
					log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', username too long.", file);
					ಅನುವರ್ತ;
				}
				ಯದಿ(strlen(username) <= 0){
					log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Warning: Empty username in password file '%s', ingoring.", file);
					ಅನುವರ್ತ;
				}

				HASH_FIND(hh, *ಬೇರು, username, strlen(username), psk);
				ಯದಿ(psk){
					log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Error: Duplicate user '%s' in password file '%s', ignoring.", username, file);
					ಅನುವರ್ತ;
				}

				psk = mosquitto_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto__psk));
				ಯದಿ(!psk){
					fclose(pwfile);
					mosquitto_FREE(buf);
					ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
				}

				psk->username = mosquitto_strdup(username);
				ಯದಿ(!psk->username){
					psk__free_item(psk);
					mosquitto_FREE(buf);
					fclose(pwfile);
					ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
				}
				password = strtok_r(ರಿಕ್ತ, ":", &saveptr);
				ಯದಿ(password){
					password = mosquitto_trimblanks(password);

					ಯದಿ(strlen(password) > 65535){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', password too long.", file);
						psk__free_item(psk);
						ಅನುವರ್ತ;
					}

					psk->password = mosquitto_strdup(password);
					ಯದಿ(!psk->password){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Warning: Unable to decode line in password file '%s'.", file);
						psk__free_item(psk);
						ಅನುವರ್ತ;
					}

					HASH_ADD_KEYPTR(hh, *ಬೇರು, psk->username, strlen(psk->username), psk);
				}ಅನ್ಯಥಾ{
					log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "Warning: Invalid line in psk file '%s': %s", file, buf);
					psk__free_item(psk);
				}
			}
		}
	}
	fclose(pwfile);
	mosquitto_FREE(buf);

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ psk__file_parse(ಸಂರಚನಾ mosquitto__psk **psk_id, ಧ್ರುವ ವರ್ಣ *psk_file)
{
	ಪೂರ್ಣಾಙ್ಕ rc;
	ಸಂರಚನಾ mosquitto__psk *psk, *tmp = ರಿಕ್ತ;

	ಯದಿ(!db.config || !psk_id){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	/* We haven't been asked to parse a psk file. */
	ಯದಿ(!psk_file){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}

	rc = pwfile__parse(psk_file, psk_id);
	ಯದಿ(rc){
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	HASH_ITER(hh, (*psk_id), psk, tmp){
		/* Check for hex only digits */
		ಯದಿ(!psk->password){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Empty psk for identity \"%s\".", psk->username);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
		}
		ಯದಿ(strspn(psk->password, "0123456789abcdefABCDEF") < strlen(psk->password)){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: psk for identity \"%s\" contains non-hexadecimal characters.", psk->username);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
		}
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ psk__cleanup(ಸಂರಚನಾ mosquitto__psk **ಬೇರು)
{
	ಸಂರಚನಾ mosquitto__psk *psk, *tmp = ರಿಕ್ತ;

	ಯದಿ(!ಬೇರು){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	HASH_ITER(hh, *ಬೇರು, psk, tmp){
		HASH_DEL(*ಬೇರು, psk);
		psk__free_item(psk);
	}

	*ಬೇರು = ರಿಕ್ತ;

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ mosquitto_psk_ಕೀಲಿ_get_default(ಸಂರಚನಾ mosquitto *context, ಧ್ರುವ ವರ್ಣ *hint, ಧ್ರುವ ವರ್ಣ *identity, ವರ್ಣ *ಕೀಲಿ, ಪೂರ್ಣಾಙ್ಕ max_ಕೀಲಿ_len)
{
	ಸಂರಚನಾ mosquitto__psk *psk;
	ಸಂರಚನಾ mosquitto__psk *psk_id_ref = ರಿಕ್ತ;

	ಯದಿ(!hint || !identity || !ಕೀಲಿ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	ಯದಿ(db.config->per_listener_settings){
		ಯದಿ(!context->listener){
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
		}
		psk_id_ref = context->listener->security_options->psk_id;
	}ಅನ್ಯಥಾ{
		psk_id_ref = db.config->security_options.psk_id;
	}
	ಯದಿ(!psk_id_ref){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PLUGIN_IGNORE;
	}

	HASH_FIND(hh, psk_id_ref, identity, strlen(identity), psk);
	ಯದಿ(psk){
		strncpy(ಕೀಲಿ, psk->password, (size_t)max_ಕೀಲಿ_len);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_AUTH;
}
