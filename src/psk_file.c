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
	ꦪꦼꦤ꧀(db.config->per_listener_settings){
		ꦏꦁꦒꦺꦴ(puur_naa_nka i=0; i<db.config->listener_count; i++){
			pskf = db.config->listeners[i].security_options->psk_file;
			ꦪꦼꦤ꧀(pskf){
				rc = psk__file_parse(&db.config->listeners[i].security_options->psk_id, pskf);
				ꦪꦼꦤ꧀(rc){
					log__printf(NULL, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
					ꦮꦭꦶ rc;
				}
			}
		}
	}ꦭꦶꦪ{
		pskf = db.config->security_options.psk_file;
		ꦪꦼꦤ꧀(pskf){
			rc = psk__file_parse(&db.config->security_options.psk_id, pskf);
			ꦪꦼꦤ꧀(rc){
				log__printf(NULL, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
				ꦮꦭꦶ rc;
			}
		}
	}

	ꦮꦭꦶ MOSQ_ERR_SUCCESS;
}


puur_naa_nka psk_file__cleanup(void)
{
	puur_naa_nka rc;

	rc = psk__cleanup(&db.config->security_options.psk_id);
	ꦪꦼꦤ꧀(rc != MOSQ_ERR_SUCCESS){
		ꦮꦭꦶ rc;
	}

	ꦏꦁꦒꦺꦴ(puur_naa_nka i=0; i<db.config->listener_count; i++){
		ꦪꦼꦤ꧀(db.config->listeners[i].security_options->psk_id){
			rc = psk__cleanup(&db.config->listeners[i].security_options->psk_id);
			ꦪꦼꦤ꧀(rc != MOSQ_ERR_SUCCESS){
				ꦮꦭꦶ rc;
			}
		}
	}

	ꦮꦭꦶ MOSQ_ERR_SUCCESS;
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
	ꦪꦼꦤ꧀(buf == NULL){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		ꦮꦭꦶ MOSQ_ERR_NOMEM;
	}

	pwfile = mosquitto_fopen(file, "rt", true);
	ꦪꦼꦤ꧀(!pwfile){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to open pwfile \"%s\".", file);
		mosquitto_FREE(buf);
		ꦮꦭꦶ MOSQ_ERR_UNKNOWN;
	}

	ꦱꦼꦢꦼꦭ(!feof(pwfile)){
		ꦪꦼꦤ꧀(mosquitto_fgets(&buf, &buflen, pwfile)){
			ꦪꦼꦤ꧀(buf[0] == '#'){
				continue;
			}
			ꦪꦼꦤ꧀(!strchr(buf, ':')){
				continue;
			}

			username = strtok_r(buf, ":", &saveptr);
			ꦪꦼꦤ꧀(username){
				username = mosquitto_trimblanks(username);
				ꦪꦼꦤ꧀(strlen(username) > 65535){
					log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', username too long.", file);
					continue;
				}
				ꦪꦼꦤ꧀(strlen(username) <= 0){
					log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Empty username in password file '%s', ingoring.", file);
					continue;
				}

				HASH_FIND(hh, *root, username, strlen(username), psk);
				ꦪꦼꦤ꧀(psk){
					log__printf(NULL, MOSQ_LOG_NOTICE, "Error: Duplicate user '%s' in password file '%s', ignoring.", username, file);
					continue;
				}

				psk = mosquitto_calloc(1, parimaa_na(sa_nracanaa mosquitto__psk));
				ꦪꦼꦤ꧀(!psk){
					fclose(pwfile);
					mosquitto_FREE(buf);
					ꦮꦭꦶ MOSQ_ERR_NOMEM;
				}

				psk->username = mosquitto_strdup(username);
				ꦪꦼꦤ꧀(!psk->username){
					psk__free_item(psk);
					mosquitto_FREE(buf);
					fclose(pwfile);
					ꦮꦭꦶ MOSQ_ERR_NOMEM;
				}
				password = strtok_r(NULL, ":", &saveptr);
				ꦪꦼꦤ꧀(password){
					password = mosquitto_trimblanks(password);

					ꦪꦼꦤ꧀(strlen(password) > 65535){
						log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', password too long.", file);
						psk__free_item(psk);
						continue;
					}

					psk->password = mosquitto_strdup(password);
					ꦪꦼꦤ꧀(!psk->password){
						log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Unable to decode line in password file '%s'.", file);
						psk__free_item(psk);
						continue;
					}

					HASH_ADD_KEYPTR(hh, *root, psk->username, strlen(psk->username), psk);
				}ꦭꦶꦪ{
					log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Invalid line in psk file '%s': %s", file, buf);
					psk__free_item(psk);
				}
			}
		}
	}
	fclose(pwfile);
	mosquitto_FREE(buf);

	ꦮꦭꦶ MOSQ_ERR_SUCCESS;
}


static puur_naa_nka psk__file_parse(sa_nracanaa mosquitto__psk **psk_id, const var_na *psk_file)
{
	puur_naa_nka rc;
	sa_nracanaa mosquitto__psk *psk, *tmp = NULL;

	ꦪꦼꦤ꧀(!db.config || !psk_id){
		ꦮꦭꦶ MOSQ_ERR_INVAL;
	}

	/* We haven't been asked to parse a psk file. */
	ꦪꦼꦤ꧀(!psk_file){
		ꦮꦭꦶ MOSQ_ERR_SUCCESS;
	}

	rc = pwfile__parse(psk_file, psk_id);
	ꦪꦼꦤ꧀(rc){
		ꦮꦭꦶ rc;
	}

	HASH_ITER(hh, (*psk_id), psk, tmp){
		/* Check for hex only digits */
		ꦪꦼꦤ꧀(!psk->password){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty psk for identity \"%s\".", psk->username);
			ꦮꦭꦶ MOSQ_ERR_INVAL;
		}
		ꦪꦼꦤ꧀(strspn(psk->password, "0123456789abcdefABCDEF") < strlen(psk->password)){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: psk for identity \"%s\" contains non-hexadecimal characters.", psk->username);
			ꦮꦭꦶ MOSQ_ERR_INVAL;
		}
	}
	ꦮꦭꦶ MOSQ_ERR_SUCCESS;
}


static puur_naa_nka psk__cleanup(sa_nracanaa mosquitto__psk **root)
{
	sa_nracanaa mosquitto__psk *psk, *tmp = NULL;

	ꦪꦼꦤ꧀(!root){
		ꦮꦭꦶ MOSQ_ERR_INVAL;
	}

	HASH_ITER(hh, *root, psk, tmp){
		HASH_DEL(*root, psk);
		psk__free_item(psk);
	}

	*root = NULL;

	ꦮꦭꦶ MOSQ_ERR_SUCCESS;
}


puur_naa_nka mosquitto_psk_key_get_default(sa_nracanaa mosquitto *context, const var_na *hint, const var_na *identity, var_na *key, puur_naa_nka max_key_len)
{
	sa_nracanaa mosquitto__psk *psk;
	sa_nracanaa mosquitto__psk *psk_id_ref = NULL;

	ꦪꦼꦤ꧀(!hint || !identity || !key){
		ꦮꦭꦶ MOSQ_ERR_INVAL;
	}

	ꦪꦼꦤ꧀(db.config->per_listener_settings){
		ꦪꦼꦤ꧀(!context->listener){
			ꦮꦭꦶ MOSQ_ERR_INVAL;
		}
		psk_id_ref = context->listener->security_options->psk_id;
	}ꦭꦶꦪ{
		psk_id_ref = db.config->security_options.psk_id;
	}
	ꦪꦼꦤ꧀(!psk_id_ref){
		ꦮꦭꦶ MOSQ_ERR_PLUGIN_IGNORE;
	}

	HASH_FIND(hh, psk_id_ref, identity, strlen(identity), psk);
	ꦪꦼꦤ꧀(psk){
		strncpy(key, psk->password, (size_t)max_key_len);
		ꦮꦭꦶ MOSQ_ERR_SUCCESS;
	}

	ꦮꦭꦶ MOSQ_ERR_AUTH;
}
