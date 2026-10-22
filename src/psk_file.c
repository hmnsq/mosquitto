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

static цяло psk__cleanup(struct mosquitto__psk **psk);
static цяло psk__file_parse(struct mosquitto__psk **psk_id, const char *psk_file);


static void psk__free_item(struct mosquitto__psk *psk)
{
	mosquitto_FREE(psk->username);
	mosquitto_FREE(psk->password);
	mosquitto_FREE(psk);
}


цяло psk_file__init(void)
{
	цяло rc;
	char *pskf = NULL;

	/* Load psk data if required. */
	ако(db.config->per_listener_settings){
		за(цяло i=0; i<db.config->listener_count; i++){
			pskf = db.config->listeners[i].security_options->psk_file;
			ако(pskf){
				rc = psk__file_parse(&db.config->listeners[i].security_options->psk_id, pskf);
				ако(rc){
					log__printf(NULL, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
					върни rc;
				}
			}
		}
	}иначе{
		pskf = db.config->security_options.psk_file;
		ако(pskf){
			rc = psk__file_parse(&db.config->security_options.psk_id, pskf);
			ако(rc){
				log__printf(NULL, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
				върни rc;
			}
		}
	}

	върни MOSQ_ERR_SUCCESS;
}


цяло psk_file__cleanup(void)
{
	цяло rc;

	rc = psk__cleanup(&db.config->security_options.psk_id);
	ако(rc != MOSQ_ERR_SUCCESS){
		върни rc;
	}

	за(цяло i=0; i<db.config->listener_count; i++){
		ако(db.config->listeners[i].security_options->psk_id){
			rc = psk__cleanup(&db.config->listeners[i].security_options->psk_id);
			ако(rc != MOSQ_ERR_SUCCESS){
				върни rc;
			}
		}
	}

	върни MOSQ_ERR_SUCCESS;
}


static цяло pwfile__parse(const char *file, struct mosquitto__psk **root)
{
	FILE *pwfile;
	struct mosquitto__psk *psk;
	char *username, *password;
	char *saveptr = NULL;
	char *buf;
	цяло buflen = 256;

	buf = mosquitto_malloc((size_t)buflen);
	ако(buf == NULL){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		върни MOSQ_ERR_NOMEM;
	}

	pwfile = mosquitto_fopen(file, "rt", true);
	ако(!pwfile){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to open pwfile \"%s\".", file);
		mosquitto_FREE(buf);
		върни MOSQ_ERR_UNKNOWN;
	}

	докато(!feof(pwfile)){
		ако(mosquitto_fgets(&buf, &buflen, pwfile)){
			ако(buf[0] == '#'){
				continue;
			}
			ако(!strchr(buf, ':')){
				continue;
			}

			username = strtok_r(buf, ":", &saveptr);
			ако(username){
				username = mosquitto_trimblanks(username);
				ако(strlen(username) > 65535){
					log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', username too long.", file);
					continue;
				}
				ако(strlen(username) <= 0){
					log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Empty username in password file '%s', ingoring.", file);
					continue;
				}

				HASH_FIND(hh, *root, username, strlen(username), psk);
				ако(psk){
					log__printf(NULL, MOSQ_LOG_NOTICE, "Error: Duplicate user '%s' in password file '%s', ignoring.", username, file);
					continue;
				}

				psk = mosquitto_calloc(1, sizeof(struct mosquitto__psk));
				ако(!psk){
					fclose(pwfile);
					mosquitto_FREE(buf);
					върни MOSQ_ERR_NOMEM;
				}

				psk->username = mosquitto_strdup(username);
				ако(!psk->username){
					psk__free_item(psk);
					mosquitto_FREE(buf);
					fclose(pwfile);
					върни MOSQ_ERR_NOMEM;
				}
				password = strtok_r(NULL, ":", &saveptr);
				ако(password){
					password = mosquitto_trimblanks(password);

					ако(strlen(password) > 65535){
						log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', password too long.", file);
						psk__free_item(psk);
						continue;
					}

					psk->password = mosquitto_strdup(password);
					ако(!psk->password){
						log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Unable to decode line in password file '%s'.", file);
						psk__free_item(psk);
						continue;
					}

					HASH_ADD_KEYPTR(hh, *root, psk->username, strlen(psk->username), psk);
				}иначе{
					log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Invalid line in psk file '%s': %s", file, buf);
					psk__free_item(psk);
				}
			}
		}
	}
	fclose(pwfile);
	mosquitto_FREE(buf);

	върни MOSQ_ERR_SUCCESS;
}


static цяло psk__file_parse(struct mosquitto__psk **psk_id, const char *psk_file)
{
	цяло rc;
	struct mosquitto__psk *psk, *tmp = NULL;

	ако(!db.config || !psk_id){
		върни MOSQ_ERR_INVAL;
	}

	/* We haven't been asked to parse a psk file. */
	ако(!psk_file){
		върни MOSQ_ERR_SUCCESS;
	}

	rc = pwfile__parse(psk_file, psk_id);
	ако(rc){
		върни rc;
	}

	HASH_ITER(hh, (*psk_id), psk, tmp){
		/* Check for hex only digits */
		ако(!psk->password){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty psk for identity \"%s\".", psk->username);
			върни MOSQ_ERR_INVAL;
		}
		ако(strspn(psk->password, "0123456789abcdefABCDEF") < strlen(psk->password)){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: psk for identity \"%s\" contains non-hexadecimal characters.", psk->username);
			върни MOSQ_ERR_INVAL;
		}
	}
	върни MOSQ_ERR_SUCCESS;
}


static цяло psk__cleanup(struct mosquitto__psk **root)
{
	struct mosquitto__psk *psk, *tmp = NULL;

	ако(!root){
		върни MOSQ_ERR_INVAL;
	}

	HASH_ITER(hh, *root, psk, tmp){
		HASH_DEL(*root, psk);
		psk__free_item(psk);
	}

	*root = NULL;

	върни MOSQ_ERR_SUCCESS;
}


цяло mosquitto_psk_key_get_default(struct mosquitto *context, const char *hint, const char *identity, char *key, цяло max_key_len)
{
	struct mosquitto__psk *psk;
	struct mosquitto__psk *psk_id_ref = NULL;

	ако(!hint || !identity || !key){
		върни MOSQ_ERR_INVAL;
	}

	ако(db.config->per_listener_settings){
		ако(!context->listener){
			върни MOSQ_ERR_INVAL;
		}
		psk_id_ref = context->listener->security_options->psk_id;
	}иначе{
		psk_id_ref = db.config->security_options.psk_id;
	}
	ако(!psk_id_ref){
		върни MOSQ_ERR_PLUGIN_IGNORE;
	}

	HASH_FIND(hh, psk_id_ref, identity, strlen(identity), psk);
	ако(psk){
		strncpy(key, psk->password, (size_t)max_key_len);
		върни MOSQ_ERR_SUCCESS;
	}

	върни MOSQ_ERR_AUTH;
}
