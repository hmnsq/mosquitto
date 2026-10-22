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

static පූර්ණ_සංඛ්‍යා psk__cleanup(sa_nracanaa mosquitto__psk **psk);
static පූර්ණ_සංඛ්‍යා psk__file_parse(sa_nracanaa mosquitto__psk **psk_id, const var_na *psk_file);


static void psk__free_item(sa_nracanaa mosquitto__psk *psk)
{
	mosquitto_FREE(psk->username);
	mosquitto_FREE(psk->password);
	mosquitto_FREE(psk);
}


පූර්ණ_සංඛ්‍යා psk_file__init(void)
{
	පූර්ණ_සංඛ්‍යා rc;
	var_na *pskf = NULL;

	/* Load psk data if required. */
	නම්(db.config->per_listener_settings){
		සඳහා(පූර්ණ_සංඛ්‍යා i=0; i<db.config->listener_count; i++){
			pskf = db.config->listeners[i].security_options->psk_file;
			නම්(pskf){
				rc = psk__file_parse(&db.config->listeners[i].security_options->psk_id, pskf);
				නම්(rc){
					log__printf(NULL, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
					ආපසු rc;
				}
			}
		}
	}නැතහොත්{
		pskf = db.config->security_options.psk_file;
		නම්(pskf){
			rc = psk__file_parse(&db.config->security_options.psk_id, pskf);
			නම්(rc){
				log__printf(NULL, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
				ආපසු rc;
			}
		}
	}

	ආපසු MOSQ_ERR_SUCCESS;
}


පූර්ණ_සංඛ්‍යා psk_file__cleanup(void)
{
	පූර්ණ_සංඛ්‍යා rc;

	rc = psk__cleanup(&db.config->security_options.psk_id);
	නම්(rc != MOSQ_ERR_SUCCESS){
		ආපසු rc;
	}

	සඳහා(පූර්ණ_සංඛ්‍යා i=0; i<db.config->listener_count; i++){
		නම්(db.config->listeners[i].security_options->psk_id){
			rc = psk__cleanup(&db.config->listeners[i].security_options->psk_id);
			නම්(rc != MOSQ_ERR_SUCCESS){
				ආපසු rc;
			}
		}
	}

	ආපසු MOSQ_ERR_SUCCESS;
}


static පූර්ණ_සංඛ්‍යා pwfile__parse(const var_na *file, sa_nracanaa mosquitto__psk **root)
{
	FILE *pwfile;
	sa_nracanaa mosquitto__psk *psk;
	var_na *username, *password;
	var_na *saveptr = NULL;
	var_na *buf;
	පූර්ණ_සංඛ්‍යා buflen = 256;

	buf = mosquitto_malloc((size_t)buflen);
	නම්(buf == NULL){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
		ආපසු MOSQ_ERR_NOMEM;
	}

	pwfile = mosquitto_fopen(file, "rt", true);
	නම්(!pwfile){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to open pwfile \"%s\".", file);
		mosquitto_FREE(buf);
		ආපසු MOSQ_ERR_UNKNOWN;
	}

	අතරතුර(!feof(pwfile)){
		නම්(mosquitto_fgets(&buf, &buflen, pwfile)){
			නම්(buf[0] == '#'){
				continue;
			}
			නම්(!strchr(buf, ':')){
				continue;
			}

			username = strtok_r(buf, ":", &saveptr);
			නම්(username){
				username = mosquitto_trimblanks(username);
				නම්(strlen(username) > 65535){
					log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', username too long.", file);
					continue;
				}
				නම්(strlen(username) <= 0){
					log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Empty username in password file '%s', ingoring.", file);
					continue;
				}

				HASH_FIND(hh, *root, username, strlen(username), psk);
				නම්(psk){
					log__printf(NULL, MOSQ_LOG_NOTICE, "Error: Duplicate user '%s' in password file '%s', ignoring.", username, file);
					continue;
				}

				psk = mosquitto_calloc(1, parimaa_na(sa_nracanaa mosquitto__psk));
				නම්(!psk){
					fclose(pwfile);
					mosquitto_FREE(buf);
					ආපසු MOSQ_ERR_NOMEM;
				}

				psk->username = mosquitto_strdup(username);
				නම්(!psk->username){
					psk__free_item(psk);
					mosquitto_FREE(buf);
					fclose(pwfile);
					ආපසු MOSQ_ERR_NOMEM;
				}
				password = strtok_r(NULL, ":", &saveptr);
				නම්(password){
					password = mosquitto_trimblanks(password);

					නම්(strlen(password) > 65535){
						log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', password too long.", file);
						psk__free_item(psk);
						continue;
					}

					psk->password = mosquitto_strdup(password);
					නම්(!psk->password){
						log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Unable to decode line in password file '%s'.", file);
						psk__free_item(psk);
						continue;
					}

					HASH_ADD_KEYPTR(hh, *root, psk->username, strlen(psk->username), psk);
				}නැතහොත්{
					log__printf(NULL, MOSQ_LOG_NOTICE, "Warning: Invalid line in psk file '%s': %s", file, buf);
					psk__free_item(psk);
				}
			}
		}
	}
	fclose(pwfile);
	mosquitto_FREE(buf);

	ආපසු MOSQ_ERR_SUCCESS;
}


static පූර්ණ_සංඛ්‍යා psk__file_parse(sa_nracanaa mosquitto__psk **psk_id, const var_na *psk_file)
{
	පූර්ණ_සංඛ්‍යා rc;
	sa_nracanaa mosquitto__psk *psk, *tmp = NULL;

	නම්(!db.config || !psk_id){
		ආපසු MOSQ_ERR_INVAL;
	}

	/* We haven't been asked to parse a psk file. */
	නම්(!psk_file){
		ආපසු MOSQ_ERR_SUCCESS;
	}

	rc = pwfile__parse(psk_file, psk_id);
	නම්(rc){
		ආපසු rc;
	}

	HASH_ITER(hh, (*psk_id), psk, tmp){
		/* Check for hex only digits */
		නම්(!psk->password){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Empty psk for identity \"%s\".", psk->username);
			ආපසු MOSQ_ERR_INVAL;
		}
		නම්(strspn(psk->password, "0123456789abcdefABCDEF") < strlen(psk->password)){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: psk for identity \"%s\" contains non-hexadecimal characters.", psk->username);
			ආපසු MOSQ_ERR_INVAL;
		}
	}
	ආපසු MOSQ_ERR_SUCCESS;
}


static පූර්ණ_සංඛ්‍යා psk__cleanup(sa_nracanaa mosquitto__psk **root)
{
	sa_nracanaa mosquitto__psk *psk, *tmp = NULL;

	නම්(!root){
		ආපසු MOSQ_ERR_INVAL;
	}

	HASH_ITER(hh, *root, psk, tmp){
		HASH_DEL(*root, psk);
		psk__free_item(psk);
	}

	*root = NULL;

	ආපසු MOSQ_ERR_SUCCESS;
}


පූර්ණ_සංඛ්‍යා mosquitto_psk_key_get_default(sa_nracanaa mosquitto *context, const var_na *hint, const var_na *identity, var_na *key, පූර්ණ_සංඛ්‍යා max_key_len)
{
	sa_nracanaa mosquitto__psk *psk;
	sa_nracanaa mosquitto__psk *psk_id_ref = NULL;

	නම්(!hint || !identity || !key){
		ආපසු MOSQ_ERR_INVAL;
	}

	නම්(db.config->per_listener_settings){
		නම්(!context->listener){
			ආපසු MOSQ_ERR_INVAL;
		}
		psk_id_ref = context->listener->security_options->psk_id;
	}නැතහොත්{
		psk_id_ref = db.config->security_options.psk_id;
	}
	නම්(!psk_id_ref){
		ආපසු MOSQ_ERR_PLUGIN_IGNORE;
	}

	HASH_FIND(hh, psk_id_ref, identity, strlen(identity), psk);
	නම්(psk){
		strncpy(key, psk->password, (size_t)max_key_len);
		ආපසු MOSQ_ERR_SUCCESS;
	}

	ආපසු MOSQ_ERR_AUTH;
}
