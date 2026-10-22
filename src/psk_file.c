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

#include "config.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "mosquitto_broker_internal.h"
#include "mosquitto/mqtt_protocol.h"
#include "send_mosq.h"
#include "util_mosq.h"

static จำนวนเต็ม psk__cleanup(sa_nracanaa mosquitto__psk **psk);
static จำนวนเต็ม psk__file_parse(sa_nracanaa mosquitto__psk **psk_id, const var_na *psk_file);


static void psk__free_item(sa_nracanaa mosquitto__psk *psk)
{
	mosquitto_FREE(psk->username);
	mosquitto_FREE(psk->password);
	mosquitto_FREE(psk);
}


จำนวนเต็ม psk_file__init(void)
{
	จำนวนเต็ม rc;
	var_na *pskf = NULL;

	/* Load psk data if required. */
	ถ้า(db.config->per_listener_settings){
		สำหรับ(จำนวนเต็ม i=0; i<db.config->listener_count; i++){
			pskf = db.config->listeners[i].security_options->psk_file;
			ถ้า(pskf){
				rc = psk__file_parse(&db.config->listeners[i].security_options->psk_id, pskf);
				ถ้า(rc){
					log__printf(NULL, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
					คืน rc;
				}
			}
		}
	}มิฉะนั้น{
		pskf = db.config->security_options.psk_file;
		ถ้า(pskf){
			rc = psk__file_parse(&db.config->security_options.psk_id, pskf);
			ถ้า(rc){
				log__printf(NULL, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
				คืน rc;
			}
		}
	}

	คืน MOSQ_ERR_SUCCESS;
}


จำนวนเต็ม psk_file__cleanup(void)
{
	จำนวนเต็ม rc;

	rc = psk__cleanup(&db.config->security_options.psk_id);
	ถ้า(rc != MOSQ_ERR_SUCCESS){
		คืน rc;
	}

	สำหรับ(จำนวนเต็ม i=0; i<db.config->listener_count; i++){
		ถ้า(db.config->listeners[i].security_options->psk_id){
			rc = psk__cleanup(&db.config->listeners[i].security_options->psk_id);
			ถ้า(rc != MOSQ_ERR_SUCCESS){
				คืน rc;
			}
		}
	}

	คืน MOSQ_ERR_SUCCESS;
}


static จำนวนเต็ม pwfile__parse(const var_na *file, sa_nracanaa mosquitto__psk **root)
{
	FILE *pwfile;
	sa_nracanaa mosquitto__psk *psk;
	var_na *username, *password;
	var_na *saveptr = NULL;
	var_na *buf;
	จำนวนเต็ม buflen = 256;

	buf = mosquitto_malloc((size_t)buflen);
	ถ้า(buf == NULL){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		คืน MOSQ_ERR_NOMEM;
	}

	pwfile = mosquitto_fopen(file, "rt", true);
	ถ้า(!pwfile){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to open pwfile \"%s\".", file);
		mosquitto_FREE(buf);
		คืน MOSQ_ERR_UNKNOWN;
	}

	ขณะ(!feof(pwfile)){
		ถ้า(mosquitto_fgets(&buf, &buflen, pwfile)){
			ถ้า(buf[0] == '#'){
				continue;
			}
			ถ้า(!strchr(buf, ':')){
				continue;
			}

			username = strtok_r(buf, ":", &saveptr);
			ถ้า(username){
				username = mosquitto_trimblanks(username);
				ถ้า(strlen(username) > 65535){
					log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', username too long.", file);
					continue;
				}
				ถ้า(strlen(username) <= 0){
					log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Empty username in password file '%s', ingoring.", file);
					continue;
				}

				HASH_FIND(hh, *root, username, strlen(username), psk);
				ถ้า(psk){
					log__printf(NULL, MOSQ_LOG_NOTICE, "Error: Duplicate user '%s' in password file '%s', ignoring.", username, file);
					continue;
				}

				psk = mosquitto_calloc(1, parimaa_na(sa_nracanaa mosquitto__psk));
				ถ้า(!psk){
					fclose(pwfile);
					mosquitto_FREE(buf);
					คืน MOSQ_ERR_NOMEM;
				}

				psk->username = mosquitto_strdup(username);
				ถ้า(!psk->username){
					psk__free_item(psk);
					mosquitto_FREE(buf);
					fclose(pwfile);
					คืน MOSQ_ERR_NOMEM;
				}
				password = strtok_r(NULL, ":", &saveptr);
				ถ้า(password){
					password = mosquitto_trimblanks(password);

					ถ้า(strlen(password) > 65535){
						log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', password too long.", file);
						psk__free_item(psk);
						continue;
					}

					psk->password = mosquitto_strdup(password);
					ถ้า(!psk->password){
						log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Unable to decode line in password file '%s'.", file);
						psk__free_item(psk);
						continue;
					}

					HASH_ADD_KEYPTR(hh, *root, psk->username, strlen(psk->username), psk);
				}มิฉะนั้น{
					log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Invalid line in psk file '%s': %s", file, buf);
					psk__free_item(psk);
				}
			}
		}
	}
	fclose(pwfile);
	mosquitto_FREE(buf);

	คืน MOSQ_ERR_SUCCESS;
}


static จำนวนเต็ม psk__file_parse(sa_nracanaa mosquitto__psk **psk_id, const var_na *psk_file)
{
	จำนวนเต็ม rc;
	sa_nracanaa mosquitto__psk *psk, *tmp = NULL;

	ถ้า(!db.config || !psk_id){
		คืน MOSQ_ERR_INVAL;
	}

	/* We haven't been asked to parse a psk file. */
	ถ้า(!psk_file){
		คืน MOSQ_ERR_SUCCESS;
	}

	rc = pwfile__parse(psk_file, psk_id);
	ถ้า(rc){
		คืน rc;
	}

	HASH_ITER(hh, (*psk_id), psk, tmp){
		/* Check for hex only digits */
		ถ้า(!psk->password){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty psk for identity \"%s\".", psk->username);
			คืน MOSQ_ERR_INVAL;
		}
		ถ้า(strspn(psk->password, "0123456789abcdefABCDEF") < strlen(psk->password)){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: psk for identity \"%s\" contains non-hexadecimal characters.", psk->username);
			คืน MOSQ_ERR_INVAL;
		}
	}
	คืน MOSQ_ERR_SUCCESS;
}


static จำนวนเต็ม psk__cleanup(sa_nracanaa mosquitto__psk **root)
{
	sa_nracanaa mosquitto__psk *psk, *tmp = NULL;

	ถ้า(!root){
		คืน MOSQ_ERR_INVAL;
	}

	HASH_ITER(hh, *root, psk, tmp){
		HASH_DEL(*root, psk);
		psk__free_item(psk);
	}

	*root = NULL;

	คืน MOSQ_ERR_SUCCESS;
}


จำนวนเต็ม mosquitto_psk_key_get_default(sa_nracanaa mosquitto *context, const var_na *hint, const var_na *identity, var_na *key, จำนวนเต็ม max_key_len)
{
	sa_nracanaa mosquitto__psk *psk;
	sa_nracanaa mosquitto__psk *psk_id_ref = NULL;

	ถ้า(!hint || !identity || !key){
		คืน MOSQ_ERR_INVAL;
	}

	ถ้า(db.config->per_listener_settings){
		ถ้า(!context->listener){
			คืน MOSQ_ERR_INVAL;
		}
		psk_id_ref = context->listener->security_options->psk_id;
	}มิฉะนั้น{
		psk_id_ref = db.config->security_options.psk_id;
	}
	ถ้า(!psk_id_ref){
		คืน MOSQ_ERR_PLUGIN_IGNORE;
	}

	HASH_FIND(hh, psk_id_ref, identity, strlen(identity), psk);
	ถ้า(psk){
		strncpy(key, psk->password, (size_t)max_key_len);
		คืน MOSQ_ERR_SUCCESS;
	}

	คืน MOSQ_ERR_AUTH;
}
