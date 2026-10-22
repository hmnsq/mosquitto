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

#شامل "config.h"

#شامل <ctype.h>
#شامل <stdio.h>
#شامل <string.h>

#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto/mqtt_protocol.h"
#شامل "send_mosq.h"
#شامل "util_mosq.h"

ایستا صحیح psk__cleanup(ساختار mosquitto__psk **psk);
ایستا صحیح psk__file_parse(ساختار mosquitto__psk **psk_id, ثابت نویسه *psk_file);


ایستا تهی psk__free_item(ساختار mosquitto__psk *psk)
{
	mosquitto_FREE(psk->username);
	mosquitto_FREE(psk->password);
	mosquitto_FREE(psk);
}


صحیح psk_file__init(تهی)
{
	صحیح rc;
	نویسه *pskf = پوچ;

	/* Load psk data if required. */
	اگر(db.config->per_listener_settings){
		برای(صحیح i=0; i<db.config->listener_شمار; i++){
			pskf = db.config->listeners[i].security_options->psk_file;
			اگر(pskf){
				rc = psk__file_parse(&db.config->listeners[i].security_options->psk_id, pskf);
				اگر(rc){
					log__printf(پوچ, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
					بازگشت rc;
				}
			}
		}
	}وگرنه{
		pskf = db.config->security_options.psk_file;
		اگر(pskf){
			rc = psk__file_parse(&db.config->security_options.psk_id, pskf);
			اگر(rc){
				log__printf(پوچ, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
				بازگشت rc;
			}
		}
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح psk_file__cleanup(تهی)
{
	صحیح rc;

	rc = psk__cleanup(&db.config->security_options.psk_id);
	اگر(rc != MOSQ_ERR_SUCCESS){
		بازگشت rc;
	}

	برای(صحیح i=0; i<db.config->listener_شمار; i++){
		اگر(db.config->listeners[i].security_options->psk_id){
			rc = psk__cleanup(&db.config->listeners[i].security_options->psk_id);
			اگر(rc != MOSQ_ERR_SUCCESS){
				بازگشت rc;
			}
		}
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا صحیح pwfile__parse(ثابت نویسه *file, ساختار mosquitto__psk **ریشه)
{
	FILE *pwfile;
	ساختار mosquitto__psk *psk;
	نویسه *username, *password;
	نویسه *saveptr = پوچ;
	نویسه *buf;
	صحیح buflen = 256;

	buf = mosquitto_malloc((size_t)buflen);
	اگر(buf == پوچ){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
		بازگشت MOSQ_ERR_NOMEM;
	}

	pwfile = mosquitto_fopen(file, "rt", true);
	اگر(!pwfile){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: Unable to open pwfile \"%s\".", file);
		mosquitto_FREE(buf);
		بازگشت MOSQ_ERR_UNKNOWN;
	}

	وقتی(!feof(pwfile)){
		اگر(mosquitto_fgets(&buf, &buflen, pwfile)){
			اگر(buf[0] == '#'){
				ادامه;
			}
			اگر(!strchr(buf, ':')){
				ادامه;
			}

			username = strtok_r(buf, ":", &saveptr);
			اگر(username){
				username = mosquitto_trimblanks(username);
				اگر(strlen(username) > 65535){
					log__printf(پوچ, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', username too long.", file);
					ادامه;
				}
				اگر(strlen(username) <= 0){
					log__printf(پوچ, MOSQ_LOG_NOTICE, "Warning: Empty username in password file '%s', ingoring.", file);
					ادامه;
				}

				HASH_FIND(hh, *ریشه, username, strlen(username), psk);
				اگر(psk){
					log__printf(پوچ, MOSQ_LOG_NOTICE, "Error: Duplicate user '%s' in password file '%s', ignoring.", username, file);
					ادامه;
				}

				psk = mosquitto_calloc(1, اندازه(ساختار mosquitto__psk));
				اگر(!psk){
					fclose(pwfile);
					mosquitto_FREE(buf);
					بازگشت MOSQ_ERR_NOMEM;
				}

				psk->username = mosquitto_strdup(username);
				اگر(!psk->username){
					psk__free_item(psk);
					mosquitto_FREE(buf);
					fclose(pwfile);
					بازگشت MOSQ_ERR_NOMEM;
				}
				password = strtok_r(پوچ, ":", &saveptr);
				اگر(password){
					password = mosquitto_trimblanks(password);

					اگر(strlen(password) > 65535){
						log__printf(پوچ, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', password too long.", file);
						psk__free_item(psk);
						ادامه;
					}

					psk->password = mosquitto_strdup(password);
					اگر(!psk->password){
						log__printf(پوچ, MOSQ_LOG_NOTICE, "Warning: Unable to decode line in password file '%s'.", file);
						psk__free_item(psk);
						ادامه;
					}

					HASH_ADD_KEYPTR(hh, *ریشه, psk->username, strlen(psk->username), psk);
				}وگرنه{
					log__printf(پوچ, MOSQ_LOG_NOTICE, "Warning: Invalid line in psk file '%s': %s", file, buf);
					psk__free_item(psk);
				}
			}
		}
	}
	fclose(pwfile);
	mosquitto_FREE(buf);

	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا صحیح psk__file_parse(ساختار mosquitto__psk **psk_id, ثابت نویسه *psk_file)
{
	صحیح rc;
	ساختار mosquitto__psk *psk, *tmp = پوچ;

	اگر(!db.config || !psk_id){
		بازگشت MOSQ_ERR_INVAL;
	}

	/* We haven't been asked to parse a psk file. */
	اگر(!psk_file){
		بازگشت MOSQ_ERR_SUCCESS;
	}

	rc = pwfile__parse(psk_file, psk_id);
	اگر(rc){
		بازگشت rc;
	}

	HASH_ITER(hh, (*psk_id), psk, tmp){
		/* Check for hex only digits */
		اگر(!psk->password){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error: Empty psk for identity \"%s\".", psk->username);
			بازگشت MOSQ_ERR_INVAL;
		}
		اگر(strspn(psk->password, "0123456789abcdefABCDEF") < strlen(psk->password)){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error: psk for identity \"%s\" contains non-hexadecimal characters.", psk->username);
			بازگشت MOSQ_ERR_INVAL;
		}
	}
	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا صحیح psk__cleanup(ساختار mosquitto__psk **ریشه)
{
	ساختار mosquitto__psk *psk, *tmp = پوچ;

	اگر(!ریشه){
		بازگشت MOSQ_ERR_INVAL;
	}

	HASH_ITER(hh, *ریشه, psk, tmp){
		HASH_DEL(*ریشه, psk);
		psk__free_item(psk);
	}

	*ریشه = پوچ;

	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح mosquitto_psk_کلید_get_default(ساختار mosquitto *context, ثابت نویسه *hint, ثابت نویسه *identity, نویسه *کلید, صحیح max_کلید_len)
{
	ساختار mosquitto__psk *psk;
	ساختار mosquitto__psk *psk_id_ref = پوچ;

	اگر(!hint || !identity || !کلید){
		بازگشت MOSQ_ERR_INVAL;
	}

	اگر(db.config->per_listener_settings){
		اگر(!context->listener){
			بازگشت MOSQ_ERR_INVAL;
		}
		psk_id_ref = context->listener->security_options->psk_id;
	}وگرنه{
		psk_id_ref = db.config->security_options.psk_id;
	}
	اگر(!psk_id_ref){
		بازگشت MOSQ_ERR_PLUGIN_IGNORE;
	}

	HASH_FIND(hh, psk_id_ref, identity, strlen(identity), psk);
	اگر(psk){
		strncpy(کلید, psk->password, (size_t)max_کلید_len);
		بازگشت MOSQ_ERR_SUCCESS;
	}

	بازگشت MOSQ_ERR_AUTH;
}
