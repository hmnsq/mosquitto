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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ଅନ୍ତର୍ଭାଵ <ctype.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto/mqtt_protocol.h"
#ଅନ୍ତର୍ଭାଵ "send_mosq.h"
#ଅନ୍ତର୍ଭାଵ "util_mosq.h"

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା psk__cleanup(ସଂରଚନା mosquitto__psk **psk);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା psk__file_parse(ସଂରଚନା mosquitto__psk **psk_id, ଅଚଳ ଅକ୍ଷର *psk_file);


ସ୍ଥିର ଶୂନ୍ୟ psk__free_item(ସଂରଚନା mosquitto__psk *psk)
{
	mosquitto_FREE(psk->username);
	mosquitto_FREE(psk->password);
	mosquitto_FREE(psk);
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା psk_file__init(ଶୂନ୍ୟ)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ଅକ୍ଷର *pskf = ଖାଲି;

	/* Load psk data if required. */
	ଯଦି(db.config->per_listener_settings){
		ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<db.config->listener_ଗଣନା; i++){
			pskf = db.config->listeners[i].security_options->psk_file;
			ଯଦି(pskf){
				rc = psk__file_parse(&db.config->listeners[i].security_options->psk_id, pskf);
				ଯଦି(rc){
					log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
					ଫେରନ୍ତୁ rc;
				}
			}
		}
	}ଅନ୍ୟଥା{
		pskf = db.config->security_options.psk_file;
		ଯଦି(pskf){
			rc = psk__file_parse(&db.config->security_options.psk_id, pskf);
			ଯଦି(rc){
				log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
				ଫେରନ୍ତୁ rc;
			}
		}
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା psk_file__cleanup(ଶୂନ୍ୟ)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	rc = psk__cleanup(&db.config->security_options.psk_id);
	ଯଦି(rc != MOSQ_ERR_SUCCESS){
		ଫେରନ୍ତୁ rc;
	}

	ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<db.config->listener_ଗଣନା; i++){
		ଯଦି(db.config->listeners[i].security_options->psk_id){
			rc = psk__cleanup(&db.config->listeners[i].security_options->psk_id);
			ଯଦି(rc != MOSQ_ERR_SUCCESS){
				ଫେରନ୍ତୁ rc;
			}
		}
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pwfile__parse(ଅଚଳ ଅକ୍ଷର *file, ସଂରଚନା mosquitto__psk **ମୂଳ)
{
	FILE *pwfile;
	ସଂରଚନା mosquitto__psk *psk;
	ଅକ୍ଷର *username, *password;
	ଅକ୍ଷର *saveptr = ଖାଲି;
	ଅକ୍ଷର *buf;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା buflen = 256;

	buf = mosquitto_malloc((size_t)buflen);
	ଯଦି(buf == ଖାଲି){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}

	pwfile = mosquitto_fopen(file, "rt", true);
	ଯଦି(!pwfile){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Unable to open pwfile \"%s\".", file);
		mosquitto_FREE(buf);
		ଫେରନ୍ତୁ MOSQ_ERR_UNKNOWN;
	}

	ଯେତେବେଳେ(!feof(pwfile)){
		ଯଦି(mosquitto_fgets(&buf, &buflen, pwfile)){
			ଯଦି(buf[0] == '#'){
				ଜାରି;
			}
			ଯଦି(!strchr(buf, ':')){
				ଜାରି;
			}

			username = strtok_r(buf, ":", &saveptr);
			ଯଦି(username){
				username = mosquitto_trimblanks(username);
				ଯଦି(strlen(username) > 65535){
					log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', username too long.", file);
					ଜାରି;
				}
				ଯଦି(strlen(username) <= 0){
					log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Warning: Empty username in password file '%s', ingoring.", file);
					ଜାରି;
				}

				HASH_FIND(hh, *ମୂଳ, username, strlen(username), psk);
				ଯଦି(psk){
					log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Error: Duplicate user '%s' in password file '%s', ignoring.", username, file);
					ଜାରି;
				}

				psk = mosquitto_calloc(1, ଆକାର(ସଂରଚନା mosquitto__psk));
				ଯଦି(!psk){
					fclose(pwfile);
					mosquitto_FREE(buf);
					ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
				}

				psk->username = mosquitto_strdup(username);
				ଯଦି(!psk->username){
					psk__free_item(psk);
					mosquitto_FREE(buf);
					fclose(pwfile);
					ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
				}
				password = strtok_r(ଖାଲି, ":", &saveptr);
				ଯଦି(password){
					password = mosquitto_trimblanks(password);

					ଯଦି(strlen(password) > 65535){
						log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', password too long.", file);
						psk__free_item(psk);
						ଜାରି;
					}

					psk->password = mosquitto_strdup(password);
					ଯଦି(!psk->password){
						log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Warning: Unable to decode line in password file '%s'.", file);
						psk__free_item(psk);
						ଜାରି;
					}

					HASH_ADD_KEYPTR(hh, *ମୂଳ, psk->username, strlen(psk->username), psk);
				}ଅନ୍ୟଥା{
					log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "Warning: Invalid line in psk file '%s': %s", file, buf);
					psk__free_item(psk);
				}
			}
		}
	}
	fclose(pwfile);
	mosquitto_FREE(buf);

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା psk__file_parse(ସଂରଚନା mosquitto__psk **psk_id, ଅଚଳ ଅକ୍ଷର *psk_file)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ସଂରଚନା mosquitto__psk *psk, *tmp = ଖାଲି;

	ଯଦି(!db.config || !psk_id){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	/* We haven't been asked to parse a psk file. */
	ଯଦି(!psk_file){
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}

	rc = pwfile__parse(psk_file, psk_id);
	ଯଦି(rc){
		ଫେରନ୍ତୁ rc;
	}

	HASH_ITER(hh, (*psk_id), psk, tmp){
		/* Check for hex only digits */
		ଯଦି(!psk->password){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Empty psk for identity \"%s\".", psk->username);
			ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
		}
		ଯଦି(strspn(psk->password, "0123456789abcdefABCDEF") < strlen(psk->password)){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: psk for identity \"%s\" contains non-hexadecimal characters.", psk->username);
			ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
		}
	}
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା psk__cleanup(ସଂରଚନା mosquitto__psk **ମୂଳ)
{
	ସଂରଚନା mosquitto__psk *psk, *tmp = ଖାଲି;

	ଯଦି(!ମୂଳ){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	HASH_ITER(hh, *ମୂଳ, psk, tmp){
		HASH_DEL(*ମୂଳ, psk);
		psk__free_item(psk);
	}

	*ମୂଳ = ଖାଲି;

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_psk_ଚାବି_get_default(ସଂରଚନା mosquitto *context, ଅଚଳ ଅକ୍ଷର *hint, ଅଚଳ ଅକ୍ଷର *identity, ଅକ୍ଷର *ଚାବି, ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_ଚାବି_len)
{
	ସଂରଚନା mosquitto__psk *psk;
	ସଂରଚନା mosquitto__psk *psk_id_ref = ଖାଲି;

	ଯଦି(!hint || !identity || !ଚାବି){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	ଯଦି(db.config->per_listener_settings){
		ଯଦି(!context->listener){
			ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
		}
		psk_id_ref = context->listener->security_options->psk_id;
	}ଅନ୍ୟଥା{
		psk_id_ref = db.config->security_options.psk_id;
	}
	ଯଦି(!psk_id_ref){
		ଫେରନ୍ତୁ MOSQ_ERR_PLUGIN_IGNORE;
	}

	HASH_FIND(hh, psk_id_ref, identity, strlen(identity), psk);
	ଯଦି(psk){
		strncpy(ଚାବି, psk->password, (size_t)max_ଚାବି_len);
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}

	ଫେରନ୍ତୁ MOSQ_ERR_AUTH;
}
