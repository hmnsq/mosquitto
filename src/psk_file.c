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

static 整数 psk__cleanup(struct mosquitto__psk **psk);
static 整数 psk__file_parse(struct mosquitto__psk **psk_id, const char *psk_file);


static void psk__free_item(struct mosquitto__psk *psk)
{
	mosquitto_FREE(psk->username);
	mosquitto_FREE(psk->password);
	mosquitto_FREE(psk);
}


整数 psk_file__init(void)
{
	整数 rc;
	char *pskf = NULL;

	/* Load psk data if required. */
	如果(db.config->per_listener_settings){
		为(整数 i=0; i<db.config->listener_count; i++){
			pskf = db.config->listeners[i].security_options->psk_file;
			如果(pskf){
				rc = psk__file_parse(&db.config->listeners[i].security_options->psk_id, pskf);
				如果(rc){
					log__printf(NULL, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
					返回 rc;
				}
			}
		}
	}否则{
		pskf = db.config->security_options.psk_file;
		如果(pskf){
			rc = psk__file_parse(&db.config->security_options.psk_id, pskf);
			如果(rc){
				log__printf(NULL, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
				返回 rc;
			}
		}
	}

	返回 MOSQ_ERR_SUCCESS;
}


整数 psk_file__cleanup(void)
{
	整数 rc;

	rc = psk__cleanup(&db.config->security_options.psk_id);
	如果(rc != MOSQ_ERR_SUCCESS){
		返回 rc;
	}

	为(整数 i=0; i<db.config->listener_count; i++){
		如果(db.config->listeners[i].security_options->psk_id){
			rc = psk__cleanup(&db.config->listeners[i].security_options->psk_id);
			如果(rc != MOSQ_ERR_SUCCESS){
				返回 rc;
			}
		}
	}

	返回 MOSQ_ERR_SUCCESS;
}


static 整数 pwfile__parse(const char *file, struct mosquitto__psk **root)
{
	FILE *pwfile;
	struct mosquitto__psk *psk;
	char *username, *password;
	char *saveptr = NULL;
	char *buf;
	整数 buflen = 256;

	buf = mosquitto_malloc((size_t)buflen);
	如果(buf == NULL){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		返回 MOSQ_ERR_NOMEM;
	}

	pwfile = mosquitto_fopen(file, "rt", true);
	如果(!pwfile){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to open pwfile \"%s\".", file);
		mosquitto_FREE(buf);
		返回 MOSQ_ERR_UNKNOWN;
	}

	当(!feof(pwfile)){
		如果(mosquitto_fgets(&buf, &buflen, pwfile)){
			如果(buf[0] == '#'){
				continue;
			}
			如果(!strchr(buf, ':')){
				continue;
			}

			username = strtok_r(buf, ":", &saveptr);
			如果(username){
				username = mosquitto_trimblanks(username);
				如果(strlen(username) > 65535){
					log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', username too long.", file);
					continue;
				}
				如果(strlen(username) <= 0){
					log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Empty username in password file '%s', ingoring.", file);
					continue;
				}

				HASH_FIND(hh, *root, username, strlen(username), psk);
				如果(psk){
					log__printf(NULL, MOSQ_LOG_NOTICE, "Error: Duplicate user '%s' in password file '%s', ignoring.", username, file);
					continue;
				}

				psk = mosquitto_calloc(1, sizeof(struct mosquitto__psk));
				如果(!psk){
					fclose(pwfile);
					mosquitto_FREE(buf);
					返回 MOSQ_ERR_NOMEM;
				}

				psk->username = mosquitto_strdup(username);
				如果(!psk->username){
					psk__free_item(psk);
					mosquitto_FREE(buf);
					fclose(pwfile);
					返回 MOSQ_ERR_NOMEM;
				}
				password = strtok_r(NULL, ":", &saveptr);
				如果(password){
					password = mosquitto_trimblanks(password);

					如果(strlen(password) > 65535){
						log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', password too long.", file);
						psk__free_item(psk);
						continue;
					}

					psk->password = mosquitto_strdup(password);
					如果(!psk->password){
						log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Unable to decode line in password file '%s'.", file);
						psk__free_item(psk);
						continue;
					}

					HASH_ADD_KEYPTR(hh, *root, psk->username, strlen(psk->username), psk);
				}否则{
					log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Invalid line in psk file '%s': %s", file, buf);
					psk__free_item(psk);
				}
			}
		}
	}
	fclose(pwfile);
	mosquitto_FREE(buf);

	返回 MOSQ_ERR_SUCCESS;
}


static 整数 psk__file_parse(struct mosquitto__psk **psk_id, const char *psk_file)
{
	整数 rc;
	struct mosquitto__psk *psk, *tmp = NULL;

	如果(!db.config || !psk_id){
		返回 MOSQ_ERR_INVAL;
	}

	/* We haven't been asked to parse a psk file. */
	如果(!psk_file){
		返回 MOSQ_ERR_SUCCESS;
	}

	rc = pwfile__parse(psk_file, psk_id);
	如果(rc){
		返回 rc;
	}

	HASH_ITER(hh, (*psk_id), psk, tmp){
		/* Check for hex only digits */
		如果(!psk->password){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty psk for identity \"%s\".", psk->username);
			返回 MOSQ_ERR_INVAL;
		}
		如果(strspn(psk->password, "0123456789abcdefABCDEF") < strlen(psk->password)){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: psk for identity \"%s\" contains non-hexadecimal characters.", psk->username);
			返回 MOSQ_ERR_INVAL;
		}
	}
	返回 MOSQ_ERR_SUCCESS;
}


static 整数 psk__cleanup(struct mosquitto__psk **root)
{
	struct mosquitto__psk *psk, *tmp = NULL;

	如果(!root){
		返回 MOSQ_ERR_INVAL;
	}

	HASH_ITER(hh, *root, psk, tmp){
		HASH_DEL(*root, psk);
		psk__free_item(psk);
	}

	*root = NULL;

	返回 MOSQ_ERR_SUCCESS;
}


整数 mosquitto_psk_key_get_default(struct mosquitto *context, const char *hint, const char *identity, char *key, 整数 max_key_len)
{
	struct mosquitto__psk *psk;
	struct mosquitto__psk *psk_id_ref = NULL;

	如果(!hint || !identity || !key){
		返回 MOSQ_ERR_INVAL;
	}

	如果(db.config->per_listener_settings){
		如果(!context->listener){
			返回 MOSQ_ERR_INVAL;
		}
		psk_id_ref = context->listener->security_options->psk_id;
	}否则{
		psk_id_ref = db.config->security_options.psk_id;
	}
	如果(!psk_id_ref){
		返回 MOSQ_ERR_PLUGIN_IGNORE;
	}

	HASH_FIND(hh, psk_id_ref, identity, strlen(identity), psk);
	如果(psk){
		strncpy(key, psk->password, (size_t)max_key_len);
		返回 MOSQ_ERR_SUCCESS;
	}

	返回 MOSQ_ERR_AUTH;
}
