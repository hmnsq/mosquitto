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

ساکن صحیح psk__cleanup(ساخت mosquitto__psk **psk);
ساکن صحیح psk__file_parse(ساخت mosquitto__psk **psk_id, ثابت حرف *psk_file);


ساکن خالی psk_free_شے(ساخت mosquitto__psk *psk)
{
	mosquitto_FREE(psk->username);
	mosquitto_FREE(psk->password);
	mosquitto_FREE(psk);
}


صحیح psk_file__init(خالی)
{
	صحیح rc;
	حرف *pskf = تہی;

	/* Load psk data if required. */
	اگر(db.config->فی_listener_settings){
		برائے(صحیح i=0; i<db.config->listener_گنتی; i++){
			pskf = db.config->listeners[i].security_options->psk_file;
			اگر(pskf){
				rc = psk__file_parse(&db.config->listeners[i].security_options->psk_id, pskf);
				اگر(rc){
					log__printf(تہی, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
					واپس rc;
				}
			}
		}
	}ورنہ{
		pskf = db.config->security_options.psk_file;
		اگر(pskf){
			rc = psk__file_parse(&db.config->security_options.psk_id, pskf);
			اگر(rc){
				log__printf(تہی, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
				واپس rc;
			}
		}
	}

	واپس MOSQ_ERR_SUCCESS;
}


صحیح psk_file__cleanup(خالی)
{
	صحیح rc;

	rc = psk__cleanup(&db.config->security_options.psk_id);
	اگر(rc != MOSQ_ERR_SUCCESS){
		واپس rc;
	}

	برائے(صحیح i=0; i<db.config->listener_گنتی; i++){
		اگر(db.config->listeners[i].security_options->psk_id){
			rc = psk__cleanup(&db.config->listeners[i].security_options->psk_id);
			اگر(rc != MOSQ_ERR_SUCCESS){
				واپس rc;
			}
		}
	}

	واپس MOSQ_ERR_SUCCESS;
}


ساکن صحیح pwfile__parse(ثابت حرف *file, ساخت mosquitto__psk **جڑ)
{
	FILE *pwfile;
	ساخت mosquitto__psk *psk;
	حرف *username, *password;
	حرف *saveptr = تہی;
	حرف *buf;
	صحیح buflen = 256;

	buf = mosquitto_malloc((size_t)buflen);
	اگر(buf == تہی){
		log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
		واپس MOSQ_ERR_NOMEM;
	}

	pwfile = mosquitto_fopen(file, "rt", true);
	اگر(!pwfile){
		log__printf(تہی, MOSQ_LOG_ERR, "Error: Unable to open pwfile \"%s\".", file);
		mosquitto_FREE(buf);
		واپس MOSQ_ERR_UNKNOWN;
	}

	جب(!feof(pwfile)){
		اگر(mosquitto_fgets(&buf, &buflen, pwfile)){
			اگر(buf[0] == '#'){
				جاری;
			}
			اگر(!strchr(buf, ':')){
				جاری;
			}

			username = strtok_r(buf, ":", &saveptr);
			اگر(username){
				username = mosquitto_trimblanks(username);
				اگر(لڑی_لمبائی(username) > 65535){
					log__printf(تہی, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', username too long.", file);
					جاری;
				}
				اگر(لڑی_لمبائی(username) <= 0){
					log__printf(تہی, MOSQ_LOG_NOTICE, "Warning: Empty username in password file '%s', ingoring.", file);
					جاری;
				}

				HASH_ڈھونڈو(hh, *جڑ, username, لڑی_لمبائی(username), psk);
				اگر(psk){
					log__printf(تہی, MOSQ_LOG_NOTICE, "Error: Duplicate user '%s' in password file '%s', ignoring.", username, file);
					جاری;
				}

				psk = mosquitto_calloc(1, حجم(ساخت mosquitto__psk));
				اگر(!psk){
					fclose(pwfile);
					mosquitto_FREE(buf);
					واپس MOSQ_ERR_NOMEM;
				}

				psk->username = mosquitto_strdup(username);
				اگر(!psk->username){
					psk_free_شے(psk);
					mosquitto_FREE(buf);
					fclose(pwfile);
					واپس MOSQ_ERR_NOMEM;
				}
				password = strtok_r(تہی, ":", &saveptr);
				اگر(password){
					password = mosquitto_trimblanks(password);

					اگر(لڑی_لمبائی(password) > 65535){
						log__printf(تہی, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', password too long.", file);
						psk_free_شے(psk);
						جاری;
					}

					psk->password = mosquitto_strdup(password);
					اگر(!psk->password){
						log__printf(تہی, MOSQ_LOG_NOTICE, "Warning: Unable to decode line in password file '%s'.", file);
						psk_free_شے(psk);
						جاری;
					}

					HASH_جوڑو_KEYPTR(hh, *جڑ, psk->username, لڑی_لمبائی(psk->username), psk);
				}ورنہ{
					log__printf(تہی, MOSQ_LOG_NOTICE, "Warning: Invalid line in psk file '%s': %s", file, buf);
					psk_free_شے(psk);
				}
			}
		}
	}
	fclose(pwfile);
	mosquitto_FREE(buf);

	واپس MOSQ_ERR_SUCCESS;
}


ساکن صحیح psk__file_parse(ساخت mosquitto__psk **psk_id, ثابت حرف *psk_file)
{
	صحیح rc;
	ساخت mosquitto__psk *psk, *tmp = تہی;

	اگر(!db.config || !psk_id){
		واپس MOSQ_ERR_INVAL;
	}

	/* We haven't been asked to parse a psk file. */
	اگر(!psk_file){
		واپس MOSQ_ERR_SUCCESS;
	}

	rc = pwfile__parse(psk_file, psk_id);
	اگر(rc){
		واپس rc;
	}

	HASH_ITER(hh, (*psk_id), psk, tmp){
		/* Check for hex only digits */
		اگر(!psk->password){
			log__printf(تہی, MOSQ_LOG_ERR, "Error: Empty psk for identity \"%s\".", psk->username);
			واپس MOSQ_ERR_INVAL;
		}
		اگر(strspn(psk->password, "0123456789abcdefABCDEF") < لڑی_لمبائی(psk->password)){
			log__printf(تہی, MOSQ_LOG_ERR, "Error: psk for identity \"%s\" contains non-hexadecimal characters.", psk->username);
			واپس MOSQ_ERR_INVAL;
		}
	}
	واپس MOSQ_ERR_SUCCESS;
}


ساکن صحیح psk__cleanup(ساخت mosquitto__psk **جڑ)
{
	ساخت mosquitto__psk *psk, *tmp = تہی;

	اگر(!جڑ){
		واپس MOSQ_ERR_INVAL;
	}

	HASH_ITER(hh, *جڑ, psk, tmp){
		HASH_DEL(*جڑ, psk);
		psk_free_شے(psk);
	}

	*جڑ = تہی;

	واپس MOSQ_ERR_SUCCESS;
}


صحیح mosquitto_psk_کلید_get_default(ساخت mosquitto *context, ثابت حرف *hint, ثابت حرف *شناختی_عنصر, حرف *کلید, صحیح max_کلید_len)
{
	ساخت mosquitto__psk *psk;
	ساخت mosquitto__psk *psk_id_ref = تہی;

	اگر(!hint || !شناختی_عنصر || !کلید){
		واپس MOSQ_ERR_INVAL;
	}

	اگر(db.config->فی_listener_settings){
		اگر(!context->listener){
			واپس MOSQ_ERR_INVAL;
		}
		psk_id_ref = context->listener->security_options->psk_id;
	}ورنہ{
		psk_id_ref = db.config->security_options.psk_id;
	}
	اگر(!psk_id_ref){
		واپس MOSQ_ERR_PLUGIN_IGNORE;
	}

	HASH_ڈھونڈو(hh, psk_id_ref, شناختی_عنصر, لڑی_لمبائی(شناختی_عنصر), psk);
	اگر(psk){
		strncpy(کلید, psk->password, (size_t)max_کلید_len);
		واپس MOSQ_ERR_SUCCESS;
	}

	واپس MOSQ_ERR_AUTH;
}
