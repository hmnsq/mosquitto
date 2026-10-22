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

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ psk__cleanup(ರಚನೆ mosquitto__psk **psk);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ psk__file_parse(ರಚನೆ mosquitto__psk **psk_id, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *psk_file);


ಸ್ಥಿರ ಶೂನ್ಯ psk_free_ಅಂಶ(ರಚನೆ mosquitto__psk *psk)
{
	mosquitto_FREE(psk->username);
	mosquitto_FREE(psk->password);
	mosquitto_FREE(psk);
}


ಪೂರ್ಣಾಂಕ psk_file__init(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಂಕ rc;
	ಅಕ್ಷರ *pskf = ಖಾಲಿ;

	/* Load psk data if required. */
	ಆದರೆ(db.config->ಪ್ರತಿ_listener_settings){
		ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<db.config->listener_ಎಣಿಕೆ; i++){
			pskf = db.config->listeners[i].security_options->psk_file;
			ಆದರೆ(pskf){
				rc = psk__file_parse(&db.config->listeners[i].security_options->psk_id, pskf);
				ಆದರೆ(rc){
					log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
					ಹಿಂತಿರುಗಿ rc;
				}
			}
		}
	}ಇಲ್ಲದಿದ್ದರೆ{
		pskf = db.config->security_options.psk_file;
		ಆದರೆ(pskf){
			rc = psk__file_parse(&db.config->security_options.psk_id, pskf);
			ಆದರೆ(rc){
				log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
				ಹಿಂತಿರುಗಿ rc;
			}
		}
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ psk_file__cleanup(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಂಕ rc;

	rc = psk__cleanup(&db.config->security_options.psk_id);
	ಆದರೆ(rc != MOSQ_ERR_SUCCESS){
		ಹಿಂತಿರುಗಿ rc;
	}

	ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<db.config->listener_ಎಣಿಕೆ; i++){
		ಆದರೆ(db.config->listeners[i].security_options->psk_id){
			rc = psk__cleanup(&db.config->listeners[i].security_options->psk_id);
			ಆದರೆ(rc != MOSQ_ERR_SUCCESS){
				ಹಿಂತಿರುಗಿ rc;
			}
		}
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pwfile__parse(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file, ರಚನೆ mosquitto__psk **ಬೇರು)
{
	FILE *pwfile;
	ರಚನೆ mosquitto__psk *psk;
	ಅಕ್ಷರ *username, *password;
	ಅಕ್ಷರ *saveptr = ಖಾಲಿ;
	ಅಕ್ಷರ *buf;
	ಪೂರ್ಣಾಂಕ buflen = 256;

	buf = mosquitto_malloc((size_t)buflen);
	ಆದರೆ(buf == ಖಾಲಿ){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Out of memory.");
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}

	pwfile = mosquitto_fopen(file, "rt", true);
	ಆದರೆ(!pwfile){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Unable to open pwfile \"%s\".", file);
		mosquitto_FREE(buf);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_UNKNOWN;
	}

	ಆಗ(!feof(pwfile)){
		ಆದರೆ(mosquitto_fgets(&buf, &buflen, pwfile)){
			ಆದರೆ(buf[0] == '#'){
				ಮುಂದುವರಿ;
			}
			ಆದರೆ(!strchr(buf, ':')){
				ಮುಂದುವರಿ;
			}

			username = strtok_r(buf, ":", &saveptr);
			ಆದರೆ(username){
				username = mosquitto_trimblanks(username);
				ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(username) > 65535){
					log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', username too long.", file);
					ಮುಂದುವರಿ;
				}
				ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(username) <= 0){
					log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Warning: Empty username in password file '%s', ingoring.", file);
					ಮುಂದುವರಿ;
				}

				HASH_ಕಂಡುಹಿಡಿ(hh, *ಬೇರು, username, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(username), psk);
				ಆದರೆ(psk){
					log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Error: Duplicate user '%s' in password file '%s', ignoring.", username, file);
					ಮುಂದುವರಿ;
				}

				psk = mosquitto_calloc(1, ಗಾತ್ರ(ರಚನೆ mosquitto__psk));
				ಆದರೆ(!psk){
					fclose(pwfile);
					mosquitto_FREE(buf);
					ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
				}

				psk->username = mosquitto_strdup(username);
				ಆದರೆ(!psk->username){
					psk_free_ಅಂಶ(psk);
					mosquitto_FREE(buf);
					fclose(pwfile);
					ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
				}
				password = strtok_r(ಖಾಲಿ, ":", &saveptr);
				ಆದರೆ(password){
					password = mosquitto_trimblanks(password);

					ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(password) > 65535){
						log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', password too long.", file);
						psk_free_ಅಂಶ(psk);
						ಮುಂದುವರಿ;
					}

					psk->password = mosquitto_strdup(password);
					ಆದರೆ(!psk->password){
						log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Warning: Unable to decode line in password file '%s'.", file);
						psk_free_ಅಂಶ(psk);
						ಮುಂದುವರಿ;
					}

					HASH_ಕೂಡಿಸು_KEYPTR(hh, *ಬೇರು, psk->username, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(psk->username), psk);
				}ಇಲ್ಲದಿದ್ದರೆ{
					log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "Warning: Invalid line in psk file '%s': %s", file, buf);
					psk_free_ಅಂಶ(psk);
				}
			}
		}
	}
	fclose(pwfile);
	mosquitto_FREE(buf);

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ psk__file_parse(ರಚನೆ mosquitto__psk **psk_id, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *psk_file)
{
	ಪೂರ್ಣಾಂಕ rc;
	ರಚನೆ mosquitto__psk *psk, *tmp = ಖಾಲಿ;

	ಆದರೆ(!db.config || !psk_id){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	/* We haven't been asked to parse a psk file. */
	ಆದರೆ(!psk_file){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}

	rc = pwfile__parse(psk_file, psk_id);
	ಆದರೆ(rc){
		ಹಿಂತಿರುಗಿ rc;
	}

	HASH_ITER(hh, (*psk_id), psk, tmp){
		/* Check for hex only digits */
		ಆದರೆ(!psk->password){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Empty psk for identity \"%s\".", psk->username);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
		}
		ಆದರೆ(strspn(psk->password, "0123456789abcdefABCDEF") < ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(psk->password)){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: psk for identity \"%s\" contains non-hexadecimal characters.", psk->username);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
		}
	}
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ psk__cleanup(ರಚನೆ mosquitto__psk **ಬೇರು)
{
	ರಚನೆ mosquitto__psk *psk, *tmp = ಖಾಲಿ;

	ಆದರೆ(!ಬೇರು){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	HASH_ITER(hh, *ಬೇರು, psk, tmp){
		HASH_DEL(*ಬೇರು, psk);
		psk_free_ಅಂಶ(psk);
	}

	*ಬೇರು = ಖಾಲಿ;

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ mosquitto_psk_ಕೀಲಿ_get_default(ರಚನೆ mosquitto *context, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hint, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ತಟಸ್ಥ, ಅಕ್ಷರ *ಕೀಲಿ, ಪೂರ್ಣಾಂಕ max_ಕೀಲಿ_len)
{
	ರಚನೆ mosquitto__psk *psk;
	ರಚನೆ mosquitto__psk *psk_id_ref = ಖಾಲಿ;

	ಆದರೆ(!hint || !ತಟಸ್ಥ || !ಕೀಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	ಆದರೆ(db.config->ಪ್ರತಿ_listener_settings){
		ಆದರೆ(!context->listener){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
		}
		psk_id_ref = context->listener->security_options->psk_id;
	}ಇಲ್ಲದಿದ್ದರೆ{
		psk_id_ref = db.config->security_options.psk_id;
	}
	ಆದರೆ(!psk_id_ref){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_PLUGIN_IGNORE;
	}

	HASH_ಕಂಡುಹಿಡಿ(hh, psk_id_ref, ತಟಸ್ಥ, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ತಟಸ್ಥ), psk);
	ಆದರೆ(psk){
		strncpy(ಕೀಲಿ, psk->password, (size_t)max_ಕೀಲಿ_len);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_AUTH;
}
