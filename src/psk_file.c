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

#અન્તર્ભાવ "config.h"

#અન્તર્ભાવ <ctype.h>
#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <string.h>

#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "mosquitto/mqtt_protocol.h"
#અન્તર્ભાવ "send_mosq.h"
#અન્તર્ભાવ "util_mosq.h"

સ્થિર પૂર્ણાંક psk__cleanup(સંરચના mosquitto__psk **psk);
સ્થિર પૂર્ણાંક psk__file_parse(સંરચના mosquitto__psk **psk_id, અચલ અક્ષર *psk_file);


સ્થિર શૂન્ય psk__free_item(સંરચના mosquitto__psk *psk)
{
	mosquitto_FREE(psk->username);
	mosquitto_FREE(psk->password);
	mosquitto_FREE(psk);
}


પૂર્ણાંક psk_file__init(શૂન્ય)
{
	પૂર્ણાંક rc;
	અક્ષર *pskf = ખાલી;

	/* Load psk data if required. */
	જો(db.config->per_listener_settings){
		માટે(પૂર્ણાંક i=0; i<db.config->listener_ગણતરી; i++){
			pskf = db.config->listeners[i].security_options->psk_file;
			જો(pskf){
				rc = psk__file_parse(&db.config->listeners[i].security_options->psk_id, pskf);
				જો(rc){
					log__printf(ખાલી, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
					પાછા rc;
				}
			}
		}
	}નહીંતર{
		pskf = db.config->security_options.psk_file;
		જો(pskf){
			rc = psk__file_parse(&db.config->security_options.psk_id, pskf);
			જો(rc){
				log__printf(ખાલી, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
				પાછા rc;
			}
		}
	}

	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક psk_file__cleanup(શૂન્ય)
{
	પૂર્ણાંક rc;

	rc = psk__cleanup(&db.config->security_options.psk_id);
	જો(rc != MOSQ_ERR_SUCCESS){
		પાછા rc;
	}

	માટે(પૂર્ણાંક i=0; i<db.config->listener_ગણતરી; i++){
		જો(db.config->listeners[i].security_options->psk_id){
			rc = psk__cleanup(&db.config->listeners[i].security_options->psk_id);
			જો(rc != MOSQ_ERR_SUCCESS){
				પાછા rc;
			}
		}
	}

	પાછા MOSQ_ERR_SUCCESS;
}


સ્થિર પૂર્ણાંક pwfile__parse(અચલ અક્ષર *file, સંરચના mosquitto__psk **મૂળ)
{
	FILE *pwfile;
	સંરચના mosquitto__psk *psk;
	અક્ષર *username, *password;
	અક્ષર *saveptr = ખાલી;
	અક્ષર *buf;
	પૂર્ણાંક buflen = 256;

	buf = mosquitto_malloc((size_t)buflen);
	જો(buf == ખાલી){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
		પાછા MOSQ_ERR_NOMEM;
	}

	pwfile = mosquitto_fopen(file, "rt", true);
	જો(!pwfile){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Unable to open pwfile \"%s\".", file);
		mosquitto_FREE(buf);
		પાછા MOSQ_ERR_UNKNOWN;
	}

	જ્યારે(!feof(pwfile)){
		જો(mosquitto_fgets(&buf, &buflen, pwfile)){
			જો(buf[0] == '#'){
				ચાલુ;
			}
			જો(!strchr(buf, ':')){
				ચાલુ;
			}

			username = strtok_r(buf, ":", &saveptr);
			જો(username){
				username = mosquitto_trimblanks(username);
				જો(strlen(username) > 65535){
					log__printf(ખાલી, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', username too long.", file);
					ચાલુ;
				}
				જો(strlen(username) <= 0){
					log__printf(ખાલી, MOSQ_LOG_NOTICE, "Warning: Empty username in password file '%s', ingoring.", file);
					ચાલુ;
				}

				HASH_FIND(hh, *મૂળ, username, strlen(username), psk);
				જો(psk){
					log__printf(ખાલી, MOSQ_LOG_NOTICE, "Error: Duplicate user '%s' in password file '%s', ignoring.", username, file);
					ચાલુ;
				}

				psk = mosquitto_calloc(1, કદ(સંરચના mosquitto__psk));
				જો(!psk){
					fclose(pwfile);
					mosquitto_FREE(buf);
					પાછા MOSQ_ERR_NOMEM;
				}

				psk->username = mosquitto_strdup(username);
				જો(!psk->username){
					psk__free_item(psk);
					mosquitto_FREE(buf);
					fclose(pwfile);
					પાછા MOSQ_ERR_NOMEM;
				}
				password = strtok_r(ખાલી, ":", &saveptr);
				જો(password){
					password = mosquitto_trimblanks(password);

					જો(strlen(password) > 65535){
						log__printf(ખાલી, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', password too long.", file);
						psk__free_item(psk);
						ચાલુ;
					}

					psk->password = mosquitto_strdup(password);
					જો(!psk->password){
						log__printf(ખાલી, MOSQ_LOG_NOTICE, "Warning: Unable to decode line in password file '%s'.", file);
						psk__free_item(psk);
						ચાલુ;
					}

					HASH_ADD_KEYPTR(hh, *મૂળ, psk->username, strlen(psk->username), psk);
				}નહીંતર{
					log__printf(ખાલી, MOSQ_LOG_NOTICE, "Warning: Invalid line in psk file '%s': %s", file, buf);
					psk__free_item(psk);
				}
			}
		}
	}
	fclose(pwfile);
	mosquitto_FREE(buf);

	પાછા MOSQ_ERR_SUCCESS;
}


સ્થિર પૂર્ણાંક psk__file_parse(સંરચના mosquitto__psk **psk_id, અચલ અક્ષર *psk_file)
{
	પૂર્ણાંક rc;
	સંરચના mosquitto__psk *psk, *tmp = ખાલી;

	જો(!db.config || !psk_id){
		પાછા MOSQ_ERR_INVAL;
	}

	/* We haven't been asked to parse a psk file. */
	જો(!psk_file){
		પાછા MOSQ_ERR_SUCCESS;
	}

	rc = pwfile__parse(psk_file, psk_id);
	જો(rc){
		પાછા rc;
	}

	HASH_ITER(hh, (*psk_id), psk, tmp){
		/* Check for hex only digits */
		જો(!psk->password){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Empty psk for identity \"%s\".", psk->username);
			પાછા MOSQ_ERR_INVAL;
		}
		જો(strspn(psk->password, "0123456789abcdefABCDEF") < strlen(psk->password)){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error: psk for identity \"%s\" contains non-hexadecimal characters.", psk->username);
			પાછા MOSQ_ERR_INVAL;
		}
	}
	પાછા MOSQ_ERR_SUCCESS;
}


સ્થિર પૂર્ણાંક psk__cleanup(સંરચના mosquitto__psk **મૂળ)
{
	સંરચના mosquitto__psk *psk, *tmp = ખાલી;

	જો(!મૂળ){
		પાછા MOSQ_ERR_INVAL;
	}

	HASH_ITER(hh, *મૂળ, psk, tmp){
		HASH_DEL(*મૂળ, psk);
		psk__free_item(psk);
	}

	*મૂળ = ખાલી;

	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક mosquitto_psk_ચાવી_get_default(સંરચના mosquitto *context, અચલ અક્ષર *hint, અચલ અક્ષર *identity, અક્ષર *ચાવી, પૂર્ણાંક max_ચાવી_len)
{
	સંરચના mosquitto__psk *psk;
	સંરચના mosquitto__psk *psk_id_ref = ખાલી;

	જો(!hint || !identity || !ચાવી){
		પાછા MOSQ_ERR_INVAL;
	}

	જો(db.config->per_listener_settings){
		જો(!context->listener){
			પાછા MOSQ_ERR_INVAL;
		}
		psk_id_ref = context->listener->security_options->psk_id;
	}નહીંતર{
		psk_id_ref = db.config->security_options.psk_id;
	}
	જો(!psk_id_ref){
		પાછા MOSQ_ERR_PLUGIN_IGNORE;
	}

	HASH_FIND(hh, psk_id_ref, identity, strlen(identity), psk);
	જો(psk){
		strncpy(ચાવી, psk->password, (size_t)max_ચાવી_len);
		પાછા MOSQ_ERR_SUCCESS;
	}

	પાછા MOSQ_ERR_AUTH;
}
