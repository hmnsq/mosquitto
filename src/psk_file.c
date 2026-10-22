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

static ཧྲིལ psk__cleanup(sa_nracanaa mosquitto__psk **psk);
static ཧྲིལ psk__file_parse(sa_nracanaa mosquitto__psk **psk_id, const var_na *psk_file);


static void psk__free_item(sa_nracanaa mosquitto__psk *psk)
{
	mosquitto_FREE(psk->username);
	mosquitto_FREE(psk->password);
	mosquitto_FREE(psk);
}


ཧྲིལ psk_file__init(void)
{
	ཧྲིལ rc;
	var_na *pskf = NULL;

	/* Load psk data if required. */
	གལ་སྲིད(db.config->per_listener_settings){
		དོན་ལུ(ཧྲིལ i=0; i<db.config->listener_count; i++){
			pskf = db.config->listeners[i].security_options->psk_file;
			གལ་སྲིད(pskf){
				rc = psk__file_parse(&db.config->listeners[i].security_options->psk_id, pskf);
				གལ་སྲིད(rc){
					log__printf(NULL, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
					ལོག rc;
				}
			}
		}
	}དེ་མིན{
		pskf = db.config->security_options.psk_file;
		གལ་སྲིད(pskf){
			rc = psk__file_parse(&db.config->security_options.psk_id, pskf);
			གལ་སྲིད(rc){
				log__printf(NULL, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
				ལོག rc;
			}
		}
	}

	ལོག MOSQ_ERR_SUCCESS;
}


ཧྲིལ psk_file__cleanup(void)
{
	ཧྲིལ rc;

	rc = psk__cleanup(&db.config->security_options.psk_id);
	གལ་སྲིད(rc != MOSQ_ERR_SUCCESS){
		ལོག rc;
	}

	དོན་ལུ(ཧྲིལ i=0; i<db.config->listener_count; i++){
		གལ་སྲིད(db.config->listeners[i].security_options->psk_id){
			rc = psk__cleanup(&db.config->listeners[i].security_options->psk_id);
			གལ་སྲིད(rc != MOSQ_ERR_SUCCESS){
				ལོག rc;
			}
		}
	}

	ལོག MOSQ_ERR_SUCCESS;
}


static ཧྲིལ pwfile__parse(const var_na *file, sa_nracanaa mosquitto__psk **root)
{
	FILE *pwfile;
	sa_nracanaa mosquitto__psk *psk;
	var_na *username, *password;
	var_na *saveptr = NULL;
	var_na *buf;
	ཧྲིལ buflen = 256;

	buf = mosquitto_malloc((size_t)buflen);
	གལ་སྲིད(buf == NULL){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		ལོག MOSQ_ERR_NOMEM;
	}

	pwfile = mosquitto_fopen(file, "rt", true);
	གལ་སྲིད(!pwfile){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to open pwfile \"%s\".", file);
		mosquitto_FREE(buf);
		ལོག MOSQ_ERR_UNKNOWN;
	}

	སྐབས(!feof(pwfile)){
		གལ་སྲིད(mosquitto_fgets(&buf, &buflen, pwfile)){
			གལ་སྲིད(buf[0] == '#'){
				continue;
			}
			གལ་སྲིད(!strchr(buf, ':')){
				continue;
			}

			username = strtok_r(buf, ":", &saveptr);
			གལ་སྲིད(username){
				username = mosquitto_trimblanks(username);
				གལ་སྲིད(strlen(username) > 65535){
					log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', username too long.", file);
					continue;
				}
				གལ་སྲིད(strlen(username) <= 0){
					log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Empty username in password file '%s', ingoring.", file);
					continue;
				}

				HASH_FIND(hh, *root, username, strlen(username), psk);
				གལ་སྲིད(psk){
					log__printf(NULL, MOSQ_LOG_NOTICE, "Error: Duplicate user '%s' in password file '%s', ignoring.", username, file);
					continue;
				}

				psk = mosquitto_calloc(1, parimaa_na(sa_nracanaa mosquitto__psk));
				གལ་སྲིད(!psk){
					fclose(pwfile);
					mosquitto_FREE(buf);
					ལོག MOSQ_ERR_NOMEM;
				}

				psk->username = mosquitto_strdup(username);
				གལ་སྲིད(!psk->username){
					psk__free_item(psk);
					mosquitto_FREE(buf);
					fclose(pwfile);
					ལོག MOSQ_ERR_NOMEM;
				}
				password = strtok_r(NULL, ":", &saveptr);
				གལ་སྲིད(password){
					password = mosquitto_trimblanks(password);

					གལ་སྲིད(strlen(password) > 65535){
						log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', password too long.", file);
						psk__free_item(psk);
						continue;
					}

					psk->password = mosquitto_strdup(password);
					གལ་སྲིད(!psk->password){
						log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Unable to decode line in password file '%s'.", file);
						psk__free_item(psk);
						continue;
					}

					HASH_ADD_KEYPTR(hh, *root, psk->username, strlen(psk->username), psk);
				}དེ་མིན{
					log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Invalid line in psk file '%s': %s", file, buf);
					psk__free_item(psk);
				}
			}
		}
	}
	fclose(pwfile);
	mosquitto_FREE(buf);

	ལོག MOSQ_ERR_SUCCESS;
}


static ཧྲིལ psk__file_parse(sa_nracanaa mosquitto__psk **psk_id, const var_na *psk_file)
{
	ཧྲིལ rc;
	sa_nracanaa mosquitto__psk *psk, *tmp = NULL;

	གལ་སྲིད(!db.config || !psk_id){
		ལོག MOSQ_ERR_INVAL;
	}

	/* We haven't been asked to parse a psk file. */
	གལ་སྲིད(!psk_file){
		ལོག MOSQ_ERR_SUCCESS;
	}

	rc = pwfile__parse(psk_file, psk_id);
	གལ་སྲིད(rc){
		ལོག rc;
	}

	HASH_ITER(hh, (*psk_id), psk, tmp){
		/* Check for hex only digits */
		གལ་སྲིད(!psk->password){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty psk for identity \"%s\".", psk->username);
			ལོག MOSQ_ERR_INVAL;
		}
		གལ་སྲིད(strspn(psk->password, "0123456789abcdefABCDEF") < strlen(psk->password)){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: psk for identity \"%s\" contains non-hexadecimal characters.", psk->username);
			ལོག MOSQ_ERR_INVAL;
		}
	}
	ལོག MOSQ_ERR_SUCCESS;
}


static ཧྲིལ psk__cleanup(sa_nracanaa mosquitto__psk **root)
{
	sa_nracanaa mosquitto__psk *psk, *tmp = NULL;

	གལ་སྲིད(!root){
		ལོག MOSQ_ERR_INVAL;
	}

	HASH_ITER(hh, *root, psk, tmp){
		HASH_DEL(*root, psk);
		psk__free_item(psk);
	}

	*root = NULL;

	ལོག MOSQ_ERR_SUCCESS;
}


ཧྲིལ mosquitto_psk_key_get_default(sa_nracanaa mosquitto *context, const var_na *hint, const var_na *identity, var_na *key, ཧྲིལ max_key_len)
{
	sa_nracanaa mosquitto__psk *psk;
	sa_nracanaa mosquitto__psk *psk_id_ref = NULL;

	གལ་སྲིད(!hint || !identity || !key){
		ལོག MOSQ_ERR_INVAL;
	}

	གལ་སྲིད(db.config->per_listener_settings){
		གལ་སྲིད(!context->listener){
			ལོག MOSQ_ERR_INVAL;
		}
		psk_id_ref = context->listener->security_options->psk_id;
	}དེ་མིན{
		psk_id_ref = db.config->security_options.psk_id;
	}
	གལ་སྲིད(!psk_id_ref){
		ལོག MOSQ_ERR_PLUGIN_IGNORE;
	}

	HASH_FIND(hh, psk_id_ref, identity, strlen(identity), psk);
	གལ་སྲིད(psk){
		strncpy(key, psk->password, (size_t)max_key_len);
		ལོག MOSQ_ERR_SUCCESS;
	}

	ལོག MOSQ_ERR_AUTH;
}
