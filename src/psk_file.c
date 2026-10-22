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

static puur_naa_nka psk__cleanup(sa_nracanaa mosquitto__psk **psk);
static puur_naa_nka psk__file_parse(sa_nracanaa mosquitto__psk **psk_id, const var_na *psk_file);


static void psk__free_item(sa_nracanaa mosquitto__psk *psk)
{
	mosquitto_FREE(psk->username);
	mosquitto_FREE(psk->password);
	mosquitto_FREE(psk);
}


puur_naa_nka psk_file__init(void)
{
	puur_naa_nka rc;
	var_na *pskf = NULL;

	/* Load psk data if required. */
	ᯀᯮᯝ(db.config->per_listener_settings){
		for(puur_naa_nka i=0; i<db.config->listener_count; i++){
			pskf = db.config->listeners[i].security_options->psk_file;
			ᯀᯮᯝ(pskf){
				rc = psk__file_parse(&db.config->listeners[i].security_options->psk_id, pskf);
				ᯀᯮᯝ(rc){
					log__printf(NULL, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
					ᯀᯮᯞ rc;
				}
			}
		}
	}ᯀᯮᯝᯂᯪ{
		pskf = db.config->security_options.psk_file;
		ᯀᯮᯝ(pskf){
			rc = psk__file_parse(&db.config->security_options.psk_id, pskf);
			ᯀᯮᯝ(rc){
				log__printf(NULL, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
				ᯀᯮᯞ rc;
			}
		}
	}

	ᯀᯮᯞ MOSQ_ERR_SUCCESS;
}


puur_naa_nka psk_file__cleanup(void)
{
	puur_naa_nka rc;

	rc = psk__cleanup(&db.config->security_options.psk_id);
	ᯀᯮᯝ(rc != MOSQ_ERR_SUCCESS){
		ᯀᯮᯞ rc;
	}

	for(puur_naa_nka i=0; i<db.config->listener_count; i++){
		ᯀᯮᯝ(db.config->listeners[i].security_options->psk_id){
			rc = psk__cleanup(&db.config->listeners[i].security_options->psk_id);
			ᯀᯮᯝ(rc != MOSQ_ERR_SUCCESS){
				ᯀᯮᯞ rc;
			}
		}
	}

	ᯀᯮᯞ MOSQ_ERR_SUCCESS;
}


static puur_naa_nka pwfile__parse(const var_na *file, sa_nracanaa mosquitto__psk **root)
{
	FILE *pwfile;
	sa_nracanaa mosquitto__psk *psk;
	var_na *username, *password;
	var_na *saveptr = NULL;
	var_na *buf;
	puur_naa_nka buflen = 256;

	buf = mosquitto_malloc((size_t)buflen);
	ᯀᯮᯝ(buf == NULL){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		ᯀᯮᯞ MOSQ_ERR_NOMEM;
	}

	pwfile = mosquitto_fopen(file, "rt", true);
	ᯀᯮᯝ(!pwfile){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to open pwfile \"%s\".", file);
		mosquitto_FREE(buf);
		ᯀᯮᯞ MOSQ_ERR_UNKNOWN;
	}

	ᯀᯬᯝ(!feof(pwfile)){
		ᯀᯮᯝ(mosquitto_fgets(&buf, &buflen, pwfile)){
			ᯀᯮᯝ(buf[0] == '#'){
				continue;
			}
			ᯀᯮᯝ(!strchr(buf, ':')){
				continue;
			}

			username = strtok_r(buf, ":", &saveptr);
			ᯀᯮᯝ(username){
				username = mosquitto_trimblanks(username);
				ᯀᯮᯝ(strlen(username) > 65535){
					log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', username too long.", file);
					continue;
				}
				ᯀᯮᯝ(strlen(username) <= 0){
					log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Empty username in password file '%s', ingoring.", file);
					continue;
				}

				HASH_FIND(hh, *root, username, strlen(username), psk);
				ᯀᯮᯝ(psk){
					log__printf(NULL, MOSQ_LOG_NOTICE, "Error: Duplicate user '%s' in password file '%s', ignoring.", username, file);
					continue;
				}

				psk = mosquitto_calloc(1, parimaa_na(sa_nracanaa mosquitto__psk));
				ᯀᯮᯝ(!psk){
					fclose(pwfile);
					mosquitto_FREE(buf);
					ᯀᯮᯞ MOSQ_ERR_NOMEM;
				}

				psk->username = mosquitto_strdup(username);
				ᯀᯮᯝ(!psk->username){
					psk__free_item(psk);
					mosquitto_FREE(buf);
					fclose(pwfile);
					ᯀᯮᯞ MOSQ_ERR_NOMEM;
				}
				password = strtok_r(NULL, ":", &saveptr);
				ᯀᯮᯝ(password){
					password = mosquitto_trimblanks(password);

					ᯀᯮᯝ(strlen(password) > 65535){
						log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', password too long.", file);
						psk__free_item(psk);
						continue;
					}

					psk->password = mosquitto_strdup(password);
					ᯀᯮᯝ(!psk->password){
						log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Unable to decode line in password file '%s'.", file);
						psk__free_item(psk);
						continue;
					}

					HASH_ADD_KEYPTR(hh, *root, psk->username, strlen(psk->username), psk);
				}ᯀᯮᯝᯂᯪ{
					log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Invalid line in psk file '%s': %s", file, buf);
					psk__free_item(psk);
				}
			}
		}
	}
	fclose(pwfile);
	mosquitto_FREE(buf);

	ᯀᯮᯞ MOSQ_ERR_SUCCESS;
}


static puur_naa_nka psk__file_parse(sa_nracanaa mosquitto__psk **psk_id, const var_na *psk_file)
{
	puur_naa_nka rc;
	sa_nracanaa mosquitto__psk *psk, *tmp = NULL;

	ᯀᯮᯝ(!db.config || !psk_id){
		ᯀᯮᯞ MOSQ_ERR_INVAL;
	}

	/* We haven't been asked to parse a psk file. */
	ᯀᯮᯝ(!psk_file){
		ᯀᯮᯞ MOSQ_ERR_SUCCESS;
	}

	rc = pwfile__parse(psk_file, psk_id);
	ᯀᯮᯝ(rc){
		ᯀᯮᯞ rc;
	}

	HASH_ITER(hh, (*psk_id), psk, tmp){
		/* Check for hex only digits */
		ᯀᯮᯝ(!psk->password){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty psk for identity \"%s\".", psk->username);
			ᯀᯮᯞ MOSQ_ERR_INVAL;
		}
		ᯀᯮᯝ(strspn(psk->password, "0123456789abcdefABCDEF") < strlen(psk->password)){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: psk for identity \"%s\" contains non-hexadecimal characters.", psk->username);
			ᯀᯮᯞ MOSQ_ERR_INVAL;
		}
	}
	ᯀᯮᯞ MOSQ_ERR_SUCCESS;
}


static puur_naa_nka psk__cleanup(sa_nracanaa mosquitto__psk **root)
{
	sa_nracanaa mosquitto__psk *psk, *tmp = NULL;

	ᯀᯮᯝ(!root){
		ᯀᯮᯞ MOSQ_ERR_INVAL;
	}

	HASH_ITER(hh, *root, psk, tmp){
		HASH_DEL(*root, psk);
		psk__free_item(psk);
	}

	*root = NULL;

	ᯀᯮᯞ MOSQ_ERR_SUCCESS;
}


puur_naa_nka mosquitto_psk_key_get_default(sa_nracanaa mosquitto *context, const var_na *hint, const var_na *identity, var_na *key, puur_naa_nka max_key_len)
{
	sa_nracanaa mosquitto__psk *psk;
	sa_nracanaa mosquitto__psk *psk_id_ref = NULL;

	ᯀᯮᯝ(!hint || !identity || !key){
		ᯀᯮᯞ MOSQ_ERR_INVAL;
	}

	ᯀᯮᯝ(db.config->per_listener_settings){
		ᯀᯮᯝ(!context->listener){
			ᯀᯮᯞ MOSQ_ERR_INVAL;
		}
		psk_id_ref = context->listener->security_options->psk_id;
	}ᯀᯮᯝᯂᯪ{
		psk_id_ref = db.config->security_options.psk_id;
	}
	ᯀᯮᯝ(!psk_id_ref){
		ᯀᯮᯞ MOSQ_ERR_PLUGIN_IGNORE;
	}

	HASH_FIND(hh, psk_id_ref, identity, strlen(identity), psk);
	ᯀᯮᯝ(psk){
		strncpy(key, psk->password, (size_t)max_key_len);
		ᯀᯮᯞ MOSQ_ERR_SUCCESS;
	}

	ᯀᯮᯞ MOSQ_ERR_AUTH;
}
