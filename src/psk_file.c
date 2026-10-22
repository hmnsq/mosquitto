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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <ctype.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/mqtt_protocol.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "send_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util_mosq.h"

ਸਥਿਰ ਪੂਰਨ psk__cleanup(ਬਣਤਰ mosquitto__psk **psk);
ਸਥਿਰ ਪੂਰਨ psk__file_parse(ਬਣਤਰ mosquitto__psk **psk_id, ਅਚਲ ਅੱਖਰ *psk_file);


ਸਥਿਰ ਖਾਲੀ psk_free_ਵਸਤੂ(ਬਣਤਰ mosquitto__psk *psk)
{
	mosquitto_FREE(psk->username);
	mosquitto_FREE(psk->password);
	mosquitto_FREE(psk);
}


ਪੂਰਨ psk_file__init(ਖਾਲੀ)
{
	ਪੂਰਨ rc;
	ਅੱਖਰ *pskf = ਨਿਰਾ;

	/* Load psk data if required. */
	ਜੇ(db.config->ਪ੍ਰਤੀ_listener_settings){
		ਲਈ(ਪੂਰਨ i=0; i<db.config->listener_ਗਿਣਤੀ; i++){
			pskf = db.config->listeners[i].security_options->psk_file;
			ਜੇ(pskf){
				rc = psk__file_parse(&db.config->listeners[i].security_options->psk_id, pskf);
				ਜੇ(rc){
					log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
					ਵਾਪਸ rc;
				}
			}
		}
	}ਨਹੀਂਤਾਂ{
		pskf = db.config->security_options.psk_file;
		ਜੇ(pskf){
			rc = psk__file_parse(&db.config->security_options.psk_id, pskf);
			ਜੇ(rc){
				log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
				ਵਾਪਸ rc;
			}
		}
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ psk_file__cleanup(ਖਾਲੀ)
{
	ਪੂਰਨ rc;

	rc = psk__cleanup(&db.config->security_options.psk_id);
	ਜੇ(rc != MOSQ_ERR_SUCCESS){
		ਵਾਪਸ rc;
	}

	ਲਈ(ਪੂਰਨ i=0; i<db.config->listener_ਗਿਣਤੀ; i++){
		ਜੇ(db.config->listeners[i].security_options->psk_id){
			rc = psk__cleanup(&db.config->listeners[i].security_options->psk_id);
			ਜੇ(rc != MOSQ_ERR_SUCCESS){
				ਵਾਪਸ rc;
			}
		}
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ pwfile__parse(ਅਚਲ ਅੱਖਰ *file, ਬਣਤਰ mosquitto__psk **ਜੜ੍ਹ)
{
	FILE *pwfile;
	ਬਣਤਰ mosquitto__psk *psk;
	ਅੱਖਰ *username, *password;
	ਅੱਖਰ *saveptr = ਨਿਰਾ;
	ਅੱਖਰ *buf;
	ਪੂਰਨ buflen = 256;

	buf = mosquitto_malloc((size_t)buflen);
	ਜੇ(buf == ਨਿਰਾ){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}

	pwfile = mosquitto_fopen(file, "rt", true);
	ਜੇ(!pwfile){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Unable to open pwfile \"%s\".", file);
		mosquitto_FREE(buf);
		ਵਾਪਸ MOSQ_ERR_UNKNOWN;
	}

	ਜਦੋਂ(!feof(pwfile)){
		ਜੇ(mosquitto_fgets(&buf, &buflen, pwfile)){
			ਜੇ(buf[0] == '#'){
				ਜਾਰੀ;
			}
			ਜੇ(!strchr(buf, ':')){
				ਜਾਰੀ;
			}

			username = strtok_r(buf, ":", &saveptr);
			ਜੇ(username){
				username = mosquitto_trimblanks(username);
				ਜੇ(ਲੜੀ_ਲੰਬਾਈ(username) > 65535){
					log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', username too long.", file);
					ਜਾਰੀ;
				}
				ਜੇ(ਲੜੀ_ਲੰਬਾਈ(username) <= 0){
					log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Warning: Empty username in password file '%s', ingoring.", file);
					ਜਾਰੀ;
				}

				HASH_ਲੱਭੋ(hh, *ਜੜ੍ਹ, username, ਲੜੀ_ਲੰਬਾਈ(username), psk);
				ਜੇ(psk){
					log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Error: Duplicate user '%s' in password file '%s', ignoring.", username, file);
					ਜਾਰੀ;
				}

				psk = mosquitto_calloc(1, ਆਕਾਰ(ਬਣਤਰ mosquitto__psk));
				ਜੇ(!psk){
					fclose(pwfile);
					mosquitto_FREE(buf);
					ਵਾਪਸ MOSQ_ERR_NOMEM;
				}

				psk->username = mosquitto_strdup(username);
				ਜੇ(!psk->username){
					psk_free_ਵਸਤੂ(psk);
					mosquitto_FREE(buf);
					fclose(pwfile);
					ਵਾਪਸ MOSQ_ERR_NOMEM;
				}
				password = strtok_r(ਨਿਰਾ, ":", &saveptr);
				ਜੇ(password){
					password = mosquitto_trimblanks(password);

					ਜੇ(ਲੜੀ_ਲੰਬਾਈ(password) > 65535){
						log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', password too long.", file);
						psk_free_ਵਸਤੂ(psk);
						ਜਾਰੀ;
					}

					psk->password = mosquitto_strdup(password);
					ਜੇ(!psk->password){
						log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Warning: Unable to decode line in password file '%s'.", file);
						psk_free_ਵਸਤੂ(psk);
						ਜਾਰੀ;
					}

					HASH_ਜੋੜੋ_KEYPTR(hh, *ਜੜ੍ਹ, psk->username, ਲੜੀ_ਲੰਬਾਈ(psk->username), psk);
				}ਨਹੀਂਤਾਂ{
					log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "Warning: Invalid line in psk file '%s': %s", file, buf);
					psk_free_ਵਸਤੂ(psk);
				}
			}
		}
	}
	fclose(pwfile);
	mosquitto_FREE(buf);

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ psk__file_parse(ਬਣਤਰ mosquitto__psk **psk_id, ਅਚਲ ਅੱਖਰ *psk_file)
{
	ਪੂਰਨ rc;
	ਬਣਤਰ mosquitto__psk *psk, *tmp = ਨਿਰਾ;

	ਜੇ(!db.config || !psk_id){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	/* We haven't been asked to parse a psk file. */
	ਜੇ(!psk_file){
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}

	rc = pwfile__parse(psk_file, psk_id);
	ਜੇ(rc){
		ਵਾਪਸ rc;
	}

	HASH_ITER(hh, (*psk_id), psk, tmp){
		/* Check for hex only digits */
		ਜੇ(!psk->password){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Empty psk for identity \"%s\".", psk->username);
			ਵਾਪਸ MOSQ_ERR_INVAL;
		}
		ਜੇ(strspn(psk->password, "0123456789abcdefABCDEF") < ਲੜੀ_ਲੰਬਾਈ(psk->password)){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: psk for identity \"%s\" contains non-hexadecimal characters.", psk->username);
			ਵਾਪਸ MOSQ_ERR_INVAL;
		}
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਪੂਰਨ psk__cleanup(ਬਣਤਰ mosquitto__psk **ਜੜ੍ਹ)
{
	ਬਣਤਰ mosquitto__psk *psk, *tmp = ਨਿਰਾ;

	ਜੇ(!ਜੜ੍ਹ){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	HASH_ITER(hh, *ਜੜ੍ਹ, psk, tmp){
		HASH_DEL(*ਜੜ੍ਹ, psk);
		psk_free_ਵਸਤੂ(psk);
	}

	*ਜੜ੍ਹ = ਨਿਰਾ;

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ mosquitto_psk_ਕੁੰਜੀ_get_default(ਬਣਤਰ mosquitto *context, ਅਚਲ ਅੱਖਰ *hint, ਅਚਲ ਅੱਖਰ *ਪਛਾਣ_ਤੱਤ, ਅੱਖਰ *ਕੁੰਜੀ, ਪੂਰਨ max_ਕੁੰਜੀ_len)
{
	ਬਣਤਰ mosquitto__psk *psk;
	ਬਣਤਰ mosquitto__psk *psk_id_ref = ਨਿਰਾ;

	ਜੇ(!hint || !ਪਛਾਣ_ਤੱਤ || !ਕੁੰਜੀ){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	ਜੇ(db.config->ਪ੍ਰਤੀ_listener_settings){
		ਜੇ(!context->listener){
			ਵਾਪਸ MOSQ_ERR_INVAL;
		}
		psk_id_ref = context->listener->security_options->psk_id;
	}ਨਹੀਂਤਾਂ{
		psk_id_ref = db.config->security_options.psk_id;
	}
	ਜੇ(!psk_id_ref){
		ਵਾਪਸ MOSQ_ERR_PLUGIN_IGNORE;
	}

	HASH_ਲੱਭੋ(hh, psk_id_ref, ਪਛਾਣ_ਤੱਤ, ਲੜੀ_ਲੰਬਾਈ(ਪਛਾਣ_ਤੱਤ), psk);
	ਜੇ(psk){
		strncpy(ਕੁੰਜੀ, psk->password, (size_t)max_ਕੁੰਜੀ_len);
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}

	ਵਾਪਸ MOSQ_ERR_AUTH;
}
