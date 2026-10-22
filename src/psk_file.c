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

#অন্তর্ভাব "config.h"

#অন্তর্ভাব <ctype.h>
#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <string.h>

#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "mosquitto/mqtt_protocol.h"
#অন্তর্ভাব "send_mosq.h"
#অন্তর্ভাব "util_mosq.h"

স্থির পূর্ণসংখ্যা psk__cleanup(কাঠামো mosquitto__psk **psk);
স্থির পূর্ণসংখ্যা psk__file_parse(কাঠামো mosquitto__psk **psk_id, ধ্রুবক অক্ষর *psk_file);


স্থির শূন্য psk__free_item(কাঠামো mosquitto__psk *psk)
{
	mosquitto_FREE(psk->username);
	mosquitto_FREE(psk->password);
	mosquitto_FREE(psk);
}


পূর্ণসংখ্যা psk_file__init(শূন্য)
{
	পূর্ণসংখ্যা rc;
	অক্ষর *pskf = ফাঁকা;

	/* Load psk data if required. */
	যদি(db.config->per_listener_settings){
		জন্য(পূর্ণসংখ্যা i=0; i<db.config->listener_গুনতি; i++){
			pskf = db.config->listeners[i].security_options->psk_file;
			যদি(pskf){
				rc = psk__file_parse(&db.config->listeners[i].security_options->psk_id, pskf);
				যদি(rc){
					log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
					ফেরত rc;
				}
			}
		}
	}অন্যথা{
		pskf = db.config->security_options.psk_file;
		যদি(pskf){
			rc = psk__file_parse(&db.config->security_options.psk_id, pskf);
			যদি(rc){
				log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
				ফেরত rc;
			}
		}
	}

	ফেরত MOSQ_ERR_SUCCESS;
}


পূর্ণসংখ্যা psk_file__cleanup(শূন্য)
{
	পূর্ণসংখ্যা rc;

	rc = psk__cleanup(&db.config->security_options.psk_id);
	যদি(rc != MOSQ_ERR_SUCCESS){
		ফেরত rc;
	}

	জন্য(পূর্ণসংখ্যা i=0; i<db.config->listener_গুনতি; i++){
		যদি(db.config->listeners[i].security_options->psk_id){
			rc = psk__cleanup(&db.config->listeners[i].security_options->psk_id);
			যদি(rc != MOSQ_ERR_SUCCESS){
				ফেরত rc;
			}
		}
	}

	ফেরত MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণসংখ্যা pwfile__parse(ধ্রুবক অক্ষর *file, কাঠামো mosquitto__psk **মূল)
{
	FILE *pwfile;
	কাঠামো mosquitto__psk *psk;
	অক্ষর *username, *password;
	অক্ষর *saveptr = ফাঁকা;
	অক্ষর *buf;
	পূর্ণসংখ্যা buflen = 256;

	buf = mosquitto_malloc((size_t)buflen);
	যদি(buf == ফাঁকা){
		log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Out of memory.");
		ফেরত MOSQ_ERR_NOMEM;
	}

	pwfile = mosquitto_fopen(file, "rt", true);
	যদি(!pwfile){
		log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Unable to open pwfile \"%s\".", file);
		mosquitto_FREE(buf);
		ফেরত MOSQ_ERR_UNKNOWN;
	}

	যখন(!feof(pwfile)){
		যদি(mosquitto_fgets(&buf, &buflen, pwfile)){
			যদি(buf[0] == '#'){
				চালু;
			}
			যদি(!strchr(buf, ':')){
				চালু;
			}

			username = strtok_r(buf, ":", &saveptr);
			যদি(username){
				username = mosquitto_trimblanks(username);
				যদি(strlen(username) > 65535){
					log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', username too long.", file);
					চালু;
				}
				যদি(strlen(username) <= 0){
					log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Warning: Empty username in password file '%s', ingoring.", file);
					চালু;
				}

				HASH_FIND(hh, *মূল, username, strlen(username), psk);
				যদি(psk){
					log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Error: Duplicate user '%s' in password file '%s', ignoring.", username, file);
					চালু;
				}

				psk = mosquitto_calloc(1, আকার(কাঠামো mosquitto__psk));
				যদি(!psk){
					fclose(pwfile);
					mosquitto_FREE(buf);
					ফেরত MOSQ_ERR_NOMEM;
				}

				psk->username = mosquitto_strdup(username);
				যদি(!psk->username){
					psk__free_item(psk);
					mosquitto_FREE(buf);
					fclose(pwfile);
					ফেরত MOSQ_ERR_NOMEM;
				}
				password = strtok_r(ফাঁকা, ":", &saveptr);
				যদি(password){
					password = mosquitto_trimblanks(password);

					যদি(strlen(password) > 65535){
						log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', password too long.", file);
						psk__free_item(psk);
						চালু;
					}

					psk->password = mosquitto_strdup(password);
					যদি(!psk->password){
						log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Warning: Unable to decode line in password file '%s'.", file);
						psk__free_item(psk);
						চালু;
					}

					HASH_ADD_KEYPTR(hh, *মূল, psk->username, strlen(psk->username), psk);
				}অন্যথা{
					log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "Warning: Invalid line in psk file '%s': %s", file, buf);
					psk__free_item(psk);
				}
			}
		}
	}
	fclose(pwfile);
	mosquitto_FREE(buf);

	ফেরত MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণসংখ্যা psk__file_parse(কাঠামো mosquitto__psk **psk_id, ধ্রুবক অক্ষর *psk_file)
{
	পূর্ণসংখ্যা rc;
	কাঠামো mosquitto__psk *psk, *tmp = ফাঁকা;

	যদি(!db.config || !psk_id){
		ফেরত MOSQ_ERR_INVAL;
	}

	/* We haven't been asked to parse a psk file. */
	যদি(!psk_file){
		ফেরত MOSQ_ERR_SUCCESS;
	}

	rc = pwfile__parse(psk_file, psk_id);
	যদি(rc){
		ফেরত rc;
	}

	HASH_ITER(hh, (*psk_id), psk, tmp){
		/* Check for hex only digits */
		যদি(!psk->password){
			log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Empty psk for identity \"%s\".", psk->username);
			ফেরত MOSQ_ERR_INVAL;
		}
		যদি(strspn(psk->password, "0123456789abcdefABCDEF") < strlen(psk->password)){
			log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: psk for identity \"%s\" contains non-hexadecimal characters.", psk->username);
			ফেরত MOSQ_ERR_INVAL;
		}
	}
	ফেরত MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণসংখ্যা psk__cleanup(কাঠামো mosquitto__psk **মূল)
{
	কাঠামো mosquitto__psk *psk, *tmp = ফাঁকা;

	যদি(!মূল){
		ফেরত MOSQ_ERR_INVAL;
	}

	HASH_ITER(hh, *মূল, psk, tmp){
		HASH_DEL(*মূল, psk);
		psk__free_item(psk);
	}

	*মূল = ফাঁকা;

	ফেরত MOSQ_ERR_SUCCESS;
}


পূর্ণসংখ্যা mosquitto_psk_চাবি_get_default(কাঠামো mosquitto *context, ধ্রুবক অক্ষর *hint, ধ্রুবক অক্ষর *identity, অক্ষর *চাবি, পূর্ণসংখ্যা max_চাবি_len)
{
	কাঠামো mosquitto__psk *psk;
	কাঠামো mosquitto__psk *psk_id_ref = ফাঁকা;

	যদি(!hint || !identity || !চাবি){
		ফেরত MOSQ_ERR_INVAL;
	}

	যদি(db.config->per_listener_settings){
		যদি(!context->listener){
			ফেরত MOSQ_ERR_INVAL;
		}
		psk_id_ref = context->listener->security_options->psk_id;
	}অন্যথা{
		psk_id_ref = db.config->security_options.psk_id;
	}
	যদি(!psk_id_ref){
		ফেরত MOSQ_ERR_PLUGIN_IGNORE;
	}

	HASH_FIND(hh, psk_id_ref, identity, strlen(identity), psk);
	যদি(psk){
		strncpy(চাবি, psk->password, (size_t)max_চাবি_len);
		ফেরত MOSQ_ERR_SUCCESS;
	}

	ফেরত MOSQ_ERR_AUTH;
}
