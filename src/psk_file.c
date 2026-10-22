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

#অন্তর্ভাৱ "config.h"

#অন্তর্ভাৱ <ctype.h>
#অন্তর্ভাৱ <stdio.h>
#অন্তর্ভাৱ <string.h>

#অন্তর্ভাৱ "mosquitto_broker_internal.h"
#অন্তর্ভাৱ "mosquitto/mqtt_protocol.h"
#অন্তর্ভাৱ "send_mosq.h"
#অন্তর্ভাৱ "util_mosq.h"

স্থির পূর্ণাঙ্ক psk__cleanup(সংরচনা mosquitto__psk **psk);
স্থির পূর্ণাঙ্ক psk__file_parse(সংরচনা mosquitto__psk **psk_id, ধ্রুৱ ৱর্ণ *psk_file);


স্থির শূন্য psk__free_item(সংরচনা mosquitto__psk *psk)
{
	mosquitto_FREE(psk->username);
	mosquitto_FREE(psk->password);
	mosquitto_FREE(psk);
}


পূর্ণাঙ্ক psk_file__init(শূন্য)
{
	পূর্ণাঙ্ক rc;
	ৱর্ণ *pskf = রিক্ত;

	/* Load psk data if required. */
	যদি(db.config->per_listener_settings){
		বাবে(পূর্ণাঙ্ক i=0; i<db.config->listener_গুনতি; i++){
			pskf = db.config->listeners[i].security_options->psk_file;
			যদি(pskf){
				rc = psk__file_parse(&db.config->listeners[i].security_options->psk_id, pskf);
				যদি(rc){
					log__printf(রিক্ত, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
					ঘূৰাই rc;
				}
			}
		}
	}অন্যথা{
		pskf = db.config->security_options.psk_file;
		যদি(pskf){
			rc = psk__file_parse(&db.config->security_options.psk_id, pskf);
			যদি(rc){
				log__printf(রিক্ত, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
				ঘূৰাই rc;
			}
		}
	}

	ঘূৰাই MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক psk_file__cleanup(শূন্য)
{
	পূর্ণাঙ্ক rc;

	rc = psk__cleanup(&db.config->security_options.psk_id);
	যদি(rc != MOSQ_ERR_SUCCESS){
		ঘূৰাই rc;
	}

	বাবে(পূর্ণাঙ্ক i=0; i<db.config->listener_গুনতি; i++){
		যদি(db.config->listeners[i].security_options->psk_id){
			rc = psk__cleanup(&db.config->listeners[i].security_options->psk_id);
			যদি(rc != MOSQ_ERR_SUCCESS){
				ঘূৰাই rc;
			}
		}
	}

	ঘূৰাই MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণাঙ্ক pwfile__parse(ধ্রুৱ ৱর্ণ *file, সংরচনা mosquitto__psk **মূল)
{
	FILE *pwfile;
	সংরচনা mosquitto__psk *psk;
	ৱর্ণ *username, *password;
	ৱর্ণ *saveptr = রিক্ত;
	ৱর্ণ *buf;
	পূর্ণাঙ্ক buflen = 256;

	buf = mosquitto_malloc((size_t)buflen);
	যদি(buf == রিক্ত){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Out of memory.");
		ঘূৰাই MOSQ_ERR_NOMEM;
	}

	pwfile = mosquitto_fopen(file, "rt", true);
	যদি(!pwfile){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Unable to open pwfile \"%s\".", file);
		mosquitto_FREE(buf);
		ঘূৰাই MOSQ_ERR_UNKNOWN;
	}

	যেতিয়ালৈ(!feof(pwfile)){
		যদি(mosquitto_fgets(&buf, &buflen, pwfile)){
			যদি(buf[0] == '#'){
				অনুৱর্ত;
			}
			যদি(!strchr(buf, ':')){
				অনুৱর্ত;
			}

			username = strtok_r(buf, ":", &saveptr);
			যদি(username){
				username = mosquitto_trimblanks(username);
				যদি(strlen(username) > 65535){
					log__printf(রিক্ত, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', username too long.", file);
					অনুৱর্ত;
				}
				যদি(strlen(username) <= 0){
					log__printf(রিক্ত, MOSQ_LOG_NOTICE, "Warning: Empty username in password file '%s', ingoring.", file);
					অনুৱর্ত;
				}

				HASH_FIND(hh, *মূল, username, strlen(username), psk);
				যদি(psk){
					log__printf(রিক্ত, MOSQ_LOG_NOTICE, "Error: Duplicate user '%s' in password file '%s', ignoring.", username, file);
					অনুৱর্ত;
				}

				psk = mosquitto_calloc(1, পরিমাণ(সংরচনা mosquitto__psk));
				যদি(!psk){
					fclose(pwfile);
					mosquitto_FREE(buf);
					ঘূৰাই MOSQ_ERR_NOMEM;
				}

				psk->username = mosquitto_strdup(username);
				যদি(!psk->username){
					psk__free_item(psk);
					mosquitto_FREE(buf);
					fclose(pwfile);
					ঘূৰাই MOSQ_ERR_NOMEM;
				}
				password = strtok_r(রিক্ত, ":", &saveptr);
				যদি(password){
					password = mosquitto_trimblanks(password);

					যদি(strlen(password) > 65535){
						log__printf(রিক্ত, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', password too long.", file);
						psk__free_item(psk);
						অনুৱর্ত;
					}

					psk->password = mosquitto_strdup(password);
					যদি(!psk->password){
						log__printf(রিক্ত, MOSQ_LOG_NOTICE, "Warning: Unable to decode line in password file '%s'.", file);
						psk__free_item(psk);
						অনুৱর্ত;
					}

					HASH_ADD_KEYPTR(hh, *মূল, psk->username, strlen(psk->username), psk);
				}অন্যথা{
					log__printf(রিক্ত, MOSQ_LOG_NOTICE, "Warning: Invalid line in psk file '%s': %s", file, buf);
					psk__free_item(psk);
				}
			}
		}
	}
	fclose(pwfile);
	mosquitto_FREE(buf);

	ঘূৰাই MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণাঙ্ক psk__file_parse(সংরচনা mosquitto__psk **psk_id, ধ্রুৱ ৱর্ণ *psk_file)
{
	পূর্ণাঙ্ক rc;
	সংরচনা mosquitto__psk *psk, *tmp = রিক্ত;

	যদি(!db.config || !psk_id){
		ঘূৰাই MOSQ_ERR_INVAL;
	}

	/* We haven't been asked to parse a psk file. */
	যদি(!psk_file){
		ঘূৰাই MOSQ_ERR_SUCCESS;
	}

	rc = pwfile__parse(psk_file, psk_id);
	যদি(rc){
		ঘূৰাই rc;
	}

	HASH_ITER(hh, (*psk_id), psk, tmp){
		/* Check for hex only digits */
		যদি(!psk->password){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Empty psk for identity \"%s\".", psk->username);
			ঘূৰাই MOSQ_ERR_INVAL;
		}
		যদি(strspn(psk->password, "0123456789abcdefABCDEF") < strlen(psk->password)){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: psk for identity \"%s\" contains non-hexadecimal characters.", psk->username);
			ঘূৰাই MOSQ_ERR_INVAL;
		}
	}
	ঘূৰাই MOSQ_ERR_SUCCESS;
}


স্থির পূর্ণাঙ্ক psk__cleanup(সংরচনা mosquitto__psk **মূল)
{
	সংরচনা mosquitto__psk *psk, *tmp = রিক্ত;

	যদি(!মূল){
		ঘূৰাই MOSQ_ERR_INVAL;
	}

	HASH_ITER(hh, *মূল, psk, tmp){
		HASH_DEL(*মূল, psk);
		psk__free_item(psk);
	}

	*মূল = রিক্ত;

	ঘূৰাই MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক mosquitto_psk_চাবি_get_default(সংরচনা mosquitto *context, ধ্রুৱ ৱর্ণ *hint, ধ্রুৱ ৱর্ণ *identity, ৱর্ণ *চাবি, পূর্ণাঙ্ক max_চাবি_len)
{
	সংরচনা mosquitto__psk *psk;
	সংরচনা mosquitto__psk *psk_id_ref = রিক্ত;

	যদি(!hint || !identity || !চাবি){
		ঘূৰাই MOSQ_ERR_INVAL;
	}

	যদি(db.config->per_listener_settings){
		যদি(!context->listener){
			ঘূৰাই MOSQ_ERR_INVAL;
		}
		psk_id_ref = context->listener->security_options->psk_id;
	}অন্যথা{
		psk_id_ref = db.config->security_options.psk_id;
	}
	যদি(!psk_id_ref){
		ঘূৰাই MOSQ_ERR_PLUGIN_IGNORE;
	}

	HASH_FIND(hh, psk_id_ref, identity, strlen(identity), psk);
	যদি(psk){
		strncpy(চাবি, psk->password, (size_t)max_চাবি_len);
		ঘূৰাই MOSQ_ERR_SUCCESS;
	}

	ঘূৰাই MOSQ_ERR_AUTH;
}
