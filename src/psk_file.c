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

#ضمن "config.h"

#ضمن <ctype.h>
#ضمن <stdio.h>
#ضمن <string.h>

#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/mqtt_protocol.h"
#ضمن "send_mosq.h"
#ضمن "util_mosq.h"

ساكن صحيح psk__cleanup(بنية mosquitto__psk **psk);
ساكن صحيح psk__file_parse(بنية mosquitto__psk **psk_id, ثابت حرف *psk_file);


ساكن فراغ psk_free_شي(بنية mosquitto__psk *psk)
{
	mosquitto_FREE(psk->username);
	mosquitto_FREE(psk->password);
	mosquitto_FREE(psk);
}


صحيح psk_file__init(فراغ)
{
	صحيح rc;
	حرف *pskf = عدم;

	/* Load psk data if required. */
	إذا(db.config->في_listener_settings){
		لكل(صحيح i=0; i<db.config->listener_ڳڻپ; i++){
			pskf = db.config->listeners[i].security_options->psk_file;
			إذا(pskf){
				rc = psk__file_parse(&db.config->listeners[i].security_options->psk_id, pskf);
				إذا(rc){
					log__printf(عدم, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
					أرجع rc;
				}
			}
		}
	}وإلا{
		pskf = db.config->security_options.psk_file;
		إذا(pskf){
			rc = psk__file_parse(&db.config->security_options.psk_id, pskf);
			إذا(rc){
				log__printf(عدم, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
				أرجع rc;
			}
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح psk_file__cleanup(فراغ)
{
	صحيح rc;

	rc = psk__cleanup(&db.config->security_options.psk_id);
	إذا(rc != MOSQ_ERR_SUCCESS){
		أرجع rc;
	}

	لكل(صحيح i=0; i<db.config->listener_ڳڻپ; i++){
		إذا(db.config->listeners[i].security_options->psk_id){
			rc = psk__cleanup(&db.config->listeners[i].security_options->psk_id);
			إذا(rc != MOSQ_ERR_SUCCESS){
				أرجع rc;
			}
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح pwfile__parse(ثابت حرف *file, بنية mosquitto__psk **پاڙ)
{
	FILE *pwfile;
	بنية mosquitto__psk *psk;
	حرف *username, *password;
	حرف *saveptr = عدم;
	حرف *buf;
	صحيح buflen = 256;

	buf = mosquitto_malloc((size_t)buflen);
	إذا(buf == عدم){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		أرجع MOSQ_ERR_NOMEM;
	}

	pwfile = mosquitto_fopen(file, "rt", true);
	إذا(!pwfile){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to open pwfile \"%s\".", file);
		mosquitto_FREE(buf);
		أرجع MOSQ_ERR_UNKNOWN;
	}

	طالما(!feof(pwfile)){
		إذا(mosquitto_fgets(&buf, &buflen, pwfile)){
			إذا(buf[0] == '#'){
				استمر;
			}
			إذا(!strchr(buf, ':')){
				استمر;
			}

			username = strtok_r(buf, ":", &saveptr);
			إذا(username){
				username = mosquitto_trimblanks(username);
				إذا(لڙي_ڊيگهه(username) > 65535){
					log__printf(عدم, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', username too long.", file);
					استمر;
				}
				إذا(لڙي_ڊيگهه(username) <= 0){
					log__printf(عدم, MOSQ_LOG_NOTICE, "Warning: Empty username in password file '%s', ingoring.", file);
					استمر;
				}

				HASH_ڳوليو(hh, *پاڙ, username, لڙي_ڊيگهه(username), psk);
				إذا(psk){
					log__printf(عدم, MOSQ_LOG_NOTICE, "Error: Duplicate user '%s' in password file '%s', ignoring.", username, file);
					استمر;
				}

				psk = mosquitto_calloc(1, حجم(بنية mosquitto__psk));
				إذا(!psk){
					fclose(pwfile);
					mosquitto_FREE(buf);
					أرجع MOSQ_ERR_NOMEM;
				}

				psk->username = mosquitto_strdup(username);
				إذا(!psk->username){
					psk_free_شي(psk);
					mosquitto_FREE(buf);
					fclose(pwfile);
					أرجع MOSQ_ERR_NOMEM;
				}
				password = strtok_r(عدم, ":", &saveptr);
				إذا(password){
					password = mosquitto_trimblanks(password);

					إذا(لڙي_ڊيگهه(password) > 65535){
						log__printf(عدم, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', password too long.", file);
						psk_free_شي(psk);
						استمر;
					}

					psk->password = mosquitto_strdup(password);
					إذا(!psk->password){
						log__printf(عدم, MOSQ_LOG_NOTICE, "Warning: Unable to decode line in password file '%s'.", file);
						psk_free_شي(psk);
						استمر;
					}

					HASH_جوڙيو_KEYPTR(hh, *پاڙ, psk->username, لڙي_ڊيگهه(psk->username), psk);
				}وإلا{
					log__printf(عدم, MOSQ_LOG_NOTICE, "Warning: Invalid line in psk file '%s': %s", file, buf);
					psk_free_شي(psk);
				}
			}
		}
	}
	fclose(pwfile);
	mosquitto_FREE(buf);

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح psk__file_parse(بنية mosquitto__psk **psk_id, ثابت حرف *psk_file)
{
	صحيح rc;
	بنية mosquitto__psk *psk, *tmp = عدم;

	إذا(!db.config || !psk_id){
		أرجع MOSQ_ERR_INVAL;
	}

	/* We haven't been asked to parse a psk file. */
	إذا(!psk_file){
		أرجع MOSQ_ERR_SUCCESS;
	}

	rc = pwfile__parse(psk_file, psk_id);
	إذا(rc){
		أرجع rc;
	}

	HASH_ITER(hh, (*psk_id), psk, tmp){
		/* Check for hex only digits */
		إذا(!psk->password){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Empty psk for identity \"%s\".", psk->username);
			أرجع MOSQ_ERR_INVAL;
		}
		إذا(strspn(psk->password, "0123456789abcdefABCDEF") < لڙي_ڊيگهه(psk->password)){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: psk for identity \"%s\" contains non-hexadecimal characters.", psk->username);
			أرجع MOSQ_ERR_INVAL;
		}
	}
	أرجع MOSQ_ERR_SUCCESS;
}


ساكن صحيح psk__cleanup(بنية mosquitto__psk **پاڙ)
{
	بنية mosquitto__psk *psk, *tmp = عدم;

	إذا(!پاڙ){
		أرجع MOSQ_ERR_INVAL;
	}

	HASH_ITER(hh, *پاڙ, psk, tmp){
		HASH_DEL(*پاڙ, psk);
		psk_free_شي(psk);
	}

	*پاڙ = عدم;

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح mosquitto_psk_ڪنجي_get_default(بنية mosquitto *context, ثابت حرف *hint, ثابت حرف *سڃاڻپ_عنصر, حرف *ڪنجي, صحيح max_ڪنجي_len)
{
	بنية mosquitto__psk *psk;
	بنية mosquitto__psk *psk_id_ref = عدم;

	إذا(!hint || !سڃاڻپ_عنصر || !ڪنجي){
		أرجع MOSQ_ERR_INVAL;
	}

	إذا(db.config->في_listener_settings){
		إذا(!context->listener){
			أرجع MOSQ_ERR_INVAL;
		}
		psk_id_ref = context->listener->security_options->psk_id;
	}وإلا{
		psk_id_ref = db.config->security_options.psk_id;
	}
	إذا(!psk_id_ref){
		أرجع MOSQ_ERR_PLUGIN_IGNORE;
	}

	HASH_ڳوليو(hh, psk_id_ref, سڃاڻپ_عنصر, لڙي_ڊيگهه(سڃاڻپ_عنصر), psk);
	إذا(psk){
		strncpy(ڪنجي, psk->password, (size_t)max_ڪنجي_len);
		أرجع MOSQ_ERR_SUCCESS;
	}

	أرجع MOSQ_ERR_AUTH;
}
