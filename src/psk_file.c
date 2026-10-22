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

#அந்தர்பாவ "config.h"

#அந்தர்பாவ <ctype.h>
#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <string.h>

#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "mosquitto/mqtt_protocol.h"
#அந்தர்பாவ "send_mosq.h"
#அந்தர்பாவ "util_mosq.h"

நிலையான முழு psk__cleanup(கட்டமைப்பு mosquitto__psk **psk);
நிலையான முழு psk__file_parse(கட்டமைப்பு mosquitto__psk **psk_id, மாறிலி எழுத்து *psk_file);


நிலையான வெறுமை psk__free_item(கட்டமைப்பு mosquitto__psk *psk)
{
	mosquitto_FREE(psk->username);
	mosquitto_FREE(psk->password);
	mosquitto_FREE(psk);
}


முழு psk_file__init(வெறுமை)
{
	முழு rc;
	எழுத்து *pskf = வெற்று;

	/* Load psk data if required. */
	எனில்(db.config->per_listener_settings){
		க்கு(முழு i=0; i<db.config->listener_மொத்தம்; i++){
			pskf = db.config->listeners[i].security_options->psk_file;
			எனில்(pskf){
				rc = psk__file_parse(&db.config->listeners[i].security_options->psk_id, pskf);
				எனில்(rc){
					log__printf(வெற்று, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
					திரும்பு rc;
				}
			}
		}
	}இல்லையெனில்{
		pskf = db.config->security_options.psk_file;
		எனில்(pskf){
			rc = psk__file_parse(&db.config->security_options.psk_id, pskf);
			எனில்(rc){
				log__printf(வெற்று, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
				திரும்பு rc;
			}
		}
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு psk_file__cleanup(வெறுமை)
{
	முழு rc;

	rc = psk__cleanup(&db.config->security_options.psk_id);
	எனில்(rc != MOSQ_ERR_SUCCESS){
		திரும்பு rc;
	}

	க்கு(முழு i=0; i<db.config->listener_மொத்தம்; i++){
		எனில்(db.config->listeners[i].security_options->psk_id){
			rc = psk__cleanup(&db.config->listeners[i].security_options->psk_id);
			எனில்(rc != MOSQ_ERR_SUCCESS){
				திரும்பு rc;
			}
		}
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான முழு pwfile__parse(மாறிலி எழுத்து *file, கட்டமைப்பு mosquitto__psk **வேர்)
{
	FILE *pwfile;
	கட்டமைப்பு mosquitto__psk *psk;
	எழுத்து *username, *password;
	எழுத்து *saveptr = வெற்று;
	எழுத்து *buf;
	முழு buflen = 256;

	buf = mosquitto_malloc((size_t)buflen);
	எனில்(buf == வெற்று){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
		திரும்பு MOSQ_ERR_NOMEM;
	}

	pwfile = mosquitto_fopen(file, "rt", true);
	எனில்(!pwfile){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Unable to open pwfile \"%s\".", file);
		mosquitto_FREE(buf);
		திரும்பு MOSQ_ERR_UNKNOWN;
	}

	போது(!feof(pwfile)){
		எனில்(mosquitto_fgets(&buf, &buflen, pwfile)){
			எனில்(buf[0] == '#'){
				தொடர்;
			}
			எனில்(!strchr(buf, ':')){
				தொடர்;
			}

			username = strtok_r(buf, ":", &saveptr);
			எனில்(username){
				username = mosquitto_trimblanks(username);
				எனில்(strlen(username) > 65535){
					log__printf(வெற்று, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', username too long.", file);
					தொடர்;
				}
				எனில்(strlen(username) <= 0){
					log__printf(வெற்று, MOSQ_LOG_NOTICE, "Warning: Empty username in password file '%s', ingoring.", file);
					தொடர்;
				}

				HASH_FIND(hh, *வேர், username, strlen(username), psk);
				எனில்(psk){
					log__printf(வெற்று, MOSQ_LOG_NOTICE, "Error: Duplicate user '%s' in password file '%s', ignoring.", username, file);
					தொடர்;
				}

				psk = mosquitto_calloc(1, அளவு(கட்டமைப்பு mosquitto__psk));
				எனில்(!psk){
					fclose(pwfile);
					mosquitto_FREE(buf);
					திரும்பு MOSQ_ERR_NOMEM;
				}

				psk->username = mosquitto_strdup(username);
				எனில்(!psk->username){
					psk__free_item(psk);
					mosquitto_FREE(buf);
					fclose(pwfile);
					திரும்பு MOSQ_ERR_NOMEM;
				}
				password = strtok_r(வெற்று, ":", &saveptr);
				எனில்(password){
					password = mosquitto_trimblanks(password);

					எனில்(strlen(password) > 65535){
						log__printf(வெற்று, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', password too long.", file);
						psk__free_item(psk);
						தொடர்;
					}

					psk->password = mosquitto_strdup(password);
					எனில்(!psk->password){
						log__printf(வெற்று, MOSQ_LOG_NOTICE, "Warning: Unable to decode line in password file '%s'.", file);
						psk__free_item(psk);
						தொடர்;
					}

					HASH_ADD_KEYPTR(hh, *வேர், psk->username, strlen(psk->username), psk);
				}இல்லையெனில்{
					log__printf(வெற்று, MOSQ_LOG_NOTICE, "Warning: Invalid line in psk file '%s': %s", file, buf);
					psk__free_item(psk);
				}
			}
		}
	}
	fclose(pwfile);
	mosquitto_FREE(buf);

	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான முழு psk__file_parse(கட்டமைப்பு mosquitto__psk **psk_id, மாறிலி எழுத்து *psk_file)
{
	முழு rc;
	கட்டமைப்பு mosquitto__psk *psk, *tmp = வெற்று;

	எனில்(!db.config || !psk_id){
		திரும்பு MOSQ_ERR_INVAL;
	}

	/* We haven't been asked to parse a psk file. */
	எனில்(!psk_file){
		திரும்பு MOSQ_ERR_SUCCESS;
	}

	rc = pwfile__parse(psk_file, psk_id);
	எனில்(rc){
		திரும்பு rc;
	}

	HASH_ITER(hh, (*psk_id), psk, tmp){
		/* Check for hex only digits */
		எனில்(!psk->password){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Empty psk for identity \"%s\".", psk->username);
			திரும்பு MOSQ_ERR_INVAL;
		}
		எனில்(strspn(psk->password, "0123456789abcdefABCDEF") < strlen(psk->password)){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error: psk for identity \"%s\" contains non-hexadecimal characters.", psk->username);
			திரும்பு MOSQ_ERR_INVAL;
		}
	}
	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான முழு psk__cleanup(கட்டமைப்பு mosquitto__psk **வேர்)
{
	கட்டமைப்பு mosquitto__psk *psk, *tmp = வெற்று;

	எனில்(!வேர்){
		திரும்பு MOSQ_ERR_INVAL;
	}

	HASH_ITER(hh, *வேர், psk, tmp){
		HASH_DEL(*வேர், psk);
		psk__free_item(psk);
	}

	*வேர் = வெற்று;

	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு mosquitto_psk_சாவி_get_default(கட்டமைப்பு mosquitto *context, மாறிலி எழுத்து *hint, மாறிலி எழுத்து *identity, எழுத்து *சாவி, முழு max_சாவி_len)
{
	கட்டமைப்பு mosquitto__psk *psk;
	கட்டமைப்பு mosquitto__psk *psk_id_ref = வெற்று;

	எனில்(!hint || !identity || !சாவி){
		திரும்பு MOSQ_ERR_INVAL;
	}

	எனில்(db.config->per_listener_settings){
		எனில்(!context->listener){
			திரும்பு MOSQ_ERR_INVAL;
		}
		psk_id_ref = context->listener->security_options->psk_id;
	}இல்லையெனில்{
		psk_id_ref = db.config->security_options.psk_id;
	}
	எனில்(!psk_id_ref){
		திரும்பு MOSQ_ERR_PLUGIN_IGNORE;
	}

	HASH_FIND(hh, psk_id_ref, identity, strlen(identity), psk);
	எனில்(psk){
		strncpy(சாவி, psk->password, (size_t)max_சாவி_len);
		திரும்பு MOSQ_ERR_SUCCESS;
	}

	திரும்பு MOSQ_ERR_AUTH;
}
