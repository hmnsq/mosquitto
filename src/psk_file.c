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

static ចំនួនគត់ psk__cleanup(sa_nracanaa mosquitto__psk **psk);
static ចំនួនគត់ psk__file_parse(sa_nracanaa mosquitto__psk **psk_id, const var_na *psk_file);


static void psk__free_item(sa_nracanaa mosquitto__psk *psk)
{
	mosquitto_FREE(psk->username);
	mosquitto_FREE(psk->password);
	mosquitto_FREE(psk);
}


ចំនួនគត់ psk_file__init(void)
{
	ចំនួនគត់ rc;
	var_na *pskf = NULL;

	/* Load psk data if required. */
	ប្រសិនបើ(db.config->per_listener_settings){
		សម្រាប់(ចំនួនគត់ i=0; i<db.config->listener_count; i++){
			pskf = db.config->listeners[i].security_options->psk_file;
			ប្រសិនបើ(pskf){
				rc = psk__file_parse(&db.config->listeners[i].security_options->psk_id, pskf);
				ប្រសិនបើ(rc){
					log__printf(NULL, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
					ត្រឡប់ rc;
				}
			}
		}
	}បើមិនដូច្នេះ{
		pskf = db.config->security_options.psk_file;
		ប្រសិនបើ(pskf){
			rc = psk__file_parse(&db.config->security_options.psk_id, pskf);
			ប្រសិនបើ(rc){
				log__printf(NULL, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
				ត្រឡប់ rc;
			}
		}
	}

	ត្រឡប់ MOSQ_ERR_SUCCESS;
}


ចំនួនគត់ psk_file__cleanup(void)
{
	ចំនួនគត់ rc;

	rc = psk__cleanup(&db.config->security_options.psk_id);
	ប្រសិនបើ(rc != MOSQ_ERR_SUCCESS){
		ត្រឡប់ rc;
	}

	សម្រាប់(ចំនួនគត់ i=0; i<db.config->listener_count; i++){
		ប្រសិនបើ(db.config->listeners[i].security_options->psk_id){
			rc = psk__cleanup(&db.config->listeners[i].security_options->psk_id);
			ប្រសិនបើ(rc != MOSQ_ERR_SUCCESS){
				ត្រឡប់ rc;
			}
		}
	}

	ត្រឡប់ MOSQ_ERR_SUCCESS;
}


static ចំនួនគត់ pwfile__parse(const var_na *file, sa_nracanaa mosquitto__psk **root)
{
	FILE *pwfile;
	sa_nracanaa mosquitto__psk *psk;
	var_na *username, *password;
	var_na *saveptr = NULL;
	var_na *buf;
	ចំនួនគត់ buflen = 256;

	buf = mosquitto_malloc((size_t)buflen);
	ប្រសិនបើ(buf == NULL){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		ត្រឡប់ MOSQ_ERR_NOMEM;
	}

	pwfile = mosquitto_fopen(file, "rt", true);
	ប្រសិនបើ(!pwfile){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to open pwfile \"%s\".", file);
		mosquitto_FREE(buf);
		ត្រឡប់ MOSQ_ERR_UNKNOWN;
	}

	ខណៈពេល(!feof(pwfile)){
		ប្រសិនបើ(mosquitto_fgets(&buf, &buflen, pwfile)){
			ប្រសិនបើ(buf[0] == '#'){
				continue;
			}
			ប្រសិនបើ(!strchr(buf, ':')){
				continue;
			}

			username = strtok_r(buf, ":", &saveptr);
			ប្រសិនបើ(username){
				username = mosquitto_trimblanks(username);
				ប្រសិនបើ(strlen(username) > 65535){
					log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', username too long.", file);
					continue;
				}
				ប្រសិនបើ(strlen(username) <= 0){
					log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Empty username in password file '%s', ingoring.", file);
					continue;
				}

				HASH_FIND(hh, *root, username, strlen(username), psk);
				ប្រសិនបើ(psk){
					log__printf(NULL, MOSQ_LOG_NOTICE, "Error: Duplicate user '%s' in password file '%s', ignoring.", username, file);
					continue;
				}

				psk = mosquitto_calloc(1, parimaa_na(sa_nracanaa mosquitto__psk));
				ប្រសិនបើ(!psk){
					fclose(pwfile);
					mosquitto_FREE(buf);
					ត្រឡប់ MOSQ_ERR_NOMEM;
				}

				psk->username = mosquitto_strdup(username);
				ប្រសិនបើ(!psk->username){
					psk__free_item(psk);
					mosquitto_FREE(buf);
					fclose(pwfile);
					ត្រឡប់ MOSQ_ERR_NOMEM;
				}
				password = strtok_r(NULL, ":", &saveptr);
				ប្រសិនបើ(password){
					password = mosquitto_trimblanks(password);

					ប្រសិនបើ(strlen(password) > 65535){
						log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', password too long.", file);
						psk__free_item(psk);
						continue;
					}

					psk->password = mosquitto_strdup(password);
					ប្រសិនបើ(!psk->password){
						log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Unable to decode line in password file '%s'.", file);
						psk__free_item(psk);
						continue;
					}

					HASH_ADD_KEYPTR(hh, *root, psk->username, strlen(psk->username), psk);
				}បើមិនដូច្នេះ{
					log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Invalid line in psk file '%s': %s", file, buf);
					psk__free_item(psk);
				}
			}
		}
	}
	fclose(pwfile);
	mosquitto_FREE(buf);

	ត្រឡប់ MOSQ_ERR_SUCCESS;
}


static ចំនួនគត់ psk__file_parse(sa_nracanaa mosquitto__psk **psk_id, const var_na *psk_file)
{
	ចំនួនគត់ rc;
	sa_nracanaa mosquitto__psk *psk, *tmp = NULL;

	ប្រសិនបើ(!db.config || !psk_id){
		ត្រឡប់ MOSQ_ERR_INVAL;
	}

	/* We haven't been asked to parse a psk file. */
	ប្រសិនបើ(!psk_file){
		ត្រឡប់ MOSQ_ERR_SUCCESS;
	}

	rc = pwfile__parse(psk_file, psk_id);
	ប្រសិនបើ(rc){
		ត្រឡប់ rc;
	}

	HASH_ITER(hh, (*psk_id), psk, tmp){
		/* Check for hex only digits */
		ប្រសិនបើ(!psk->password){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty psk for identity \"%s\".", psk->username);
			ត្រឡប់ MOSQ_ERR_INVAL;
		}
		ប្រសិនបើ(strspn(psk->password, "0123456789abcdefABCDEF") < strlen(psk->password)){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: psk for identity \"%s\" contains non-hexadecimal characters.", psk->username);
			ត្រឡប់ MOSQ_ERR_INVAL;
		}
	}
	ត្រឡប់ MOSQ_ERR_SUCCESS;
}


static ចំនួនគត់ psk__cleanup(sa_nracanaa mosquitto__psk **root)
{
	sa_nracanaa mosquitto__psk *psk, *tmp = NULL;

	ប្រសិនបើ(!root){
		ត្រឡប់ MOSQ_ERR_INVAL;
	}

	HASH_ITER(hh, *root, psk, tmp){
		HASH_DEL(*root, psk);
		psk__free_item(psk);
	}

	*root = NULL;

	ត្រឡប់ MOSQ_ERR_SUCCESS;
}


ចំនួនគត់ mosquitto_psk_key_get_default(sa_nracanaa mosquitto *context, const var_na *hint, const var_na *identity, var_na *key, ចំនួនគត់ max_key_len)
{
	sa_nracanaa mosquitto__psk *psk;
	sa_nracanaa mosquitto__psk *psk_id_ref = NULL;

	ប្រសិនបើ(!hint || !identity || !key){
		ត្រឡប់ MOSQ_ERR_INVAL;
	}

	ប្រសិនបើ(db.config->per_listener_settings){
		ប្រសិនបើ(!context->listener){
			ត្រឡប់ MOSQ_ERR_INVAL;
		}
		psk_id_ref = context->listener->security_options->psk_id;
	}បើមិនដូច្នេះ{
		psk_id_ref = db.config->security_options.psk_id;
	}
	ប្រសិនបើ(!psk_id_ref){
		ត្រឡប់ MOSQ_ERR_PLUGIN_IGNORE;
	}

	HASH_FIND(hh, psk_id_ref, identity, strlen(identity), psk);
	ប្រសិនបើ(psk){
		strncpy(key, psk->password, (size_t)max_key_len);
		ត្រឡប់ MOSQ_ERR_SUCCESS;
	}

	ត្រឡប់ MOSQ_ERR_AUTH;
}
