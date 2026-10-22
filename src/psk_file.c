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

static ကိန်းပြည့် psk__cleanup(sa_nracanaa mosquitto__psk **psk);
static ကိန်းပြည့် psk__file_parse(sa_nracanaa mosquitto__psk **psk_id, const var_na *psk_file);


static void psk__free_item(sa_nracanaa mosquitto__psk *psk)
{
	mosquitto_FREE(psk->username);
	mosquitto_FREE(psk->password);
	mosquitto_FREE(psk);
}


ကိန်းပြည့် psk_file__init(void)
{
	ကိန်းပြည့် rc;
	var_na *pskf = NULL;

	/* Load psk data if required. */
	အကယ်၍(db.config->per_listener_settings){
		အတွက်(ကိန်းပြည့် i=0; i<db.config->listener_count; i++){
			pskf = db.config->listeners[i].security_options->psk_file;
			အကယ်၍(pskf){
				rc = psk__file_parse(&db.config->listeners[i].security_options->psk_id, pskf);
				အကယ်၍(rc){
					log__printf(NULL, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
					ပြန်ပေး rc;
				}
			}
		}
	}မဟုတ်ပါက{
		pskf = db.config->security_options.psk_file;
		အကယ်၍(pskf){
			rc = psk__file_parse(&db.config->security_options.psk_id, pskf);
			အကယ်၍(rc){
				log__printf(NULL, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
				ပြန်ပေး rc;
			}
		}
	}

	ပြန်ပေး MOSQ_ERR_SUCCESS;
}


ကိန်းပြည့် psk_file__cleanup(void)
{
	ကိန်းပြည့် rc;

	rc = psk__cleanup(&db.config->security_options.psk_id);
	အကယ်၍(rc != MOSQ_ERR_SUCCESS){
		ပြန်ပေး rc;
	}

	အတွက်(ကိန်းပြည့် i=0; i<db.config->listener_count; i++){
		အကယ်၍(db.config->listeners[i].security_options->psk_id){
			rc = psk__cleanup(&db.config->listeners[i].security_options->psk_id);
			အကယ်၍(rc != MOSQ_ERR_SUCCESS){
				ပြန်ပေး rc;
			}
		}
	}

	ပြန်ပေး MOSQ_ERR_SUCCESS;
}


static ကိန်းပြည့် pwfile__parse(const var_na *file, sa_nracanaa mosquitto__psk **root)
{
	FILE *pwfile;
	sa_nracanaa mosquitto__psk *psk;
	var_na *username, *password;
	var_na *saveptr = NULL;
	var_na *buf;
	ကိန်းပြည့် buflen = 256;

	buf = mosquitto_malloc((size_t)buflen);
	အကယ်၍(buf == NULL){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		ပြန်ပေး MOSQ_ERR_NOMEM;
	}

	pwfile = mosquitto_fopen(file, "rt", true);
	အကယ်၍(!pwfile){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to open pwfile \"%s\".", file);
		mosquitto_FREE(buf);
		ပြန်ပေး MOSQ_ERR_UNKNOWN;
	}

	နေစဉ်(!feof(pwfile)){
		အကယ်၍(mosquitto_fgets(&buf, &buflen, pwfile)){
			အကယ်၍(buf[0] == '#'){
				continue;
			}
			အကယ်၍(!strchr(buf, ':')){
				continue;
			}

			username = strtok_r(buf, ":", &saveptr);
			အကယ်၍(username){
				username = mosquitto_trimblanks(username);
				အကယ်၍(strlen(username) > 65535){
					log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', username too long.", file);
					continue;
				}
				အကယ်၍(strlen(username) <= 0){
					log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Empty username in password file '%s', ingoring.", file);
					continue;
				}

				HASH_FIND(hh, *root, username, strlen(username), psk);
				အကယ်၍(psk){
					log__printf(NULL, MOSQ_LOG_NOTICE, "Error: Duplicate user '%s' in password file '%s', ignoring.", username, file);
					continue;
				}

				psk = mosquitto_calloc(1, parimaa_na(sa_nracanaa mosquitto__psk));
				အကယ်၍(!psk){
					fclose(pwfile);
					mosquitto_FREE(buf);
					ပြန်ပေး MOSQ_ERR_NOMEM;
				}

				psk->username = mosquitto_strdup(username);
				အကယ်၍(!psk->username){
					psk__free_item(psk);
					mosquitto_FREE(buf);
					fclose(pwfile);
					ပြန်ပေး MOSQ_ERR_NOMEM;
				}
				password = strtok_r(NULL, ":", &saveptr);
				အကယ်၍(password){
					password = mosquitto_trimblanks(password);

					အကယ်၍(strlen(password) > 65535){
						log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', password too long.", file);
						psk__free_item(psk);
						continue;
					}

					psk->password = mosquitto_strdup(password);
					အကယ်၍(!psk->password){
						log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Unable to decode line in password file '%s'.", file);
						psk__free_item(psk);
						continue;
					}

					HASH_ADD_KEYPTR(hh, *root, psk->username, strlen(psk->username), psk);
				}မဟုတ်ပါက{
					log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Invalid line in psk file '%s': %s", file, buf);
					psk__free_item(psk);
				}
			}
		}
	}
	fclose(pwfile);
	mosquitto_FREE(buf);

	ပြန်ပေး MOSQ_ERR_SUCCESS;
}


static ကိန်းပြည့် psk__file_parse(sa_nracanaa mosquitto__psk **psk_id, const var_na *psk_file)
{
	ကိန်းပြည့် rc;
	sa_nracanaa mosquitto__psk *psk, *tmp = NULL;

	အကယ်၍(!db.config || !psk_id){
		ပြန်ပေး MOSQ_ERR_INVAL;
	}

	/* We haven't been asked to parse a psk file. */
	အကယ်၍(!psk_file){
		ပြန်ပေး MOSQ_ERR_SUCCESS;
	}

	rc = pwfile__parse(psk_file, psk_id);
	အကယ်၍(rc){
		ပြန်ပေး rc;
	}

	HASH_ITER(hh, (*psk_id), psk, tmp){
		/* Check for hex only digits */
		အကယ်၍(!psk->password){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty psk for identity \"%s\".", psk->username);
			ပြန်ပေး MOSQ_ERR_INVAL;
		}
		အကယ်၍(strspn(psk->password, "0123456789abcdefABCDEF") < strlen(psk->password)){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: psk for identity \"%s\" contains non-hexadecimal characters.", psk->username);
			ပြန်ပေး MOSQ_ERR_INVAL;
		}
	}
	ပြန်ပေး MOSQ_ERR_SUCCESS;
}


static ကိန်းပြည့် psk__cleanup(sa_nracanaa mosquitto__psk **root)
{
	sa_nracanaa mosquitto__psk *psk, *tmp = NULL;

	အကယ်၍(!root){
		ပြန်ပေး MOSQ_ERR_INVAL;
	}

	HASH_ITER(hh, *root, psk, tmp){
		HASH_DEL(*root, psk);
		psk__free_item(psk);
	}

	*root = NULL;

	ပြန်ပေး MOSQ_ERR_SUCCESS;
}


ကိန်းပြည့် mosquitto_psk_key_get_default(sa_nracanaa mosquitto *context, const var_na *hint, const var_na *identity, var_na *key, ကိန်းပြည့် max_key_len)
{
	sa_nracanaa mosquitto__psk *psk;
	sa_nracanaa mosquitto__psk *psk_id_ref = NULL;

	အကယ်၍(!hint || !identity || !key){
		ပြန်ပေး MOSQ_ERR_INVAL;
	}

	အကယ်၍(db.config->per_listener_settings){
		အကယ်၍(!context->listener){
			ပြန်ပေး MOSQ_ERR_INVAL;
		}
		psk_id_ref = context->listener->security_options->psk_id;
	}မဟုတ်ပါက{
		psk_id_ref = db.config->security_options.psk_id;
	}
	အကယ်၍(!psk_id_ref){
		ပြန်ပေး MOSQ_ERR_PLUGIN_IGNORE;
	}

	HASH_FIND(hh, psk_id_ref, identity, strlen(identity), psk);
	အကယ်၍(psk){
		strncpy(key, psk->password, (size_t)max_key_len);
		ပြန်ပေး MOSQ_ERR_SUCCESS;
	}

	ပြန်ပေး MOSQ_ERR_AUTH;
}
