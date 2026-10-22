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

#अन्तर्भाव "config.h"

#अन्तर्भाव <ctype.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto/mqtt_protocol.h"
#अन्तर्भाव "send_mosq.h"
#अन्तर्भाव "util_mosq.h"

स्थिर पूर्णाङ्क psk__cleanup(संरचना mosquitto__psk **psk);
स्थिर पूर्णाङ्क psk__file_parse(संरचना mosquitto__psk **psk_id, ध्रुव वर्ण *psk_file);


स्थिर शून्य psk__free_item(संरचना mosquitto__psk *psk)
{
	mosquitto_FREE(psk->username);
	mosquitto_FREE(psk->password);
	mosquitto_FREE(psk);
}


पूर्णाङ्क psk_file__init(शून्य)
{
	पूर्णाङ्क rc;
	वर्ण *pskf = रिक्त;

	/* Load psk data if required. */
	यदि(db.config->per_listener_settings){
		प्रति(पूर्णाङ्क i=0; i<db.config->listener_गिनती; i++){
			pskf = db.config->listeners[i].security_options->psk_file;
			यदि(pskf){
				rc = psk__file_parse(&db.config->listeners[i].security_options->psk_id, pskf);
				यदि(rc){
					log__printf(रिक्त, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
					प्रत्यागम rc;
				}
			}
		}
	}अन्यथा{
		pskf = db.config->security_options.psk_file;
		यदि(pskf){
			rc = psk__file_parse(&db.config->security_options.psk_id, pskf);
			यदि(rc){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
				प्रत्यागम rc;
			}
		}
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क psk_file__cleanup(शून्य)
{
	पूर्णाङ्क rc;

	rc = psk__cleanup(&db.config->security_options.psk_id);
	यदि(rc != MOSQ_ERR_SUCCESS){
		प्रत्यागम rc;
	}

	प्रति(पूर्णाङ्क i=0; i<db.config->listener_गिनती; i++){
		यदि(db.config->listeners[i].security_options->psk_id){
			rc = psk__cleanup(&db.config->listeners[i].security_options->psk_id);
			यदि(rc != MOSQ_ERR_SUCCESS){
				प्रत्यागम rc;
			}
		}
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क pwfile__parse(ध्रुव वर्ण *file, संरचना mosquitto__psk **मूल)
{
	FILE *pwfile;
	संरचना mosquitto__psk *psk;
	वर्ण *username, *password;
	वर्ण *saveptr = रिक्त;
	वर्ण *buf;
	पूर्णाङ्क buflen = 256;

	buf = mosquitto_malloc((size_t)buflen);
	यदि(buf == रिक्त){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	pwfile = mosquitto_fopen(file, "rt", true);
	यदि(!pwfile){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to open pwfile \"%s\".", file);
		mosquitto_FREE(buf);
		प्रत्यागम MOSQ_ERR_UNKNOWN;
	}

	यावत्(!feof(pwfile)){
		यदि(mosquitto_fgets(&buf, &buflen, pwfile)){
			यदि(buf[0] == '#'){
				अनुवर्त;
			}
			यदि(!strchr(buf, ':')){
				अनुवर्त;
			}

			username = strtok_r(buf, ":", &saveptr);
			यदि(username){
				username = mosquitto_trimblanks(username);
				यदि(strlen(username) > 65535){
					log__printf(रिक्त, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', username too long.", file);
					अनुवर्त;
				}
				यदि(strlen(username) <= 0){
					log__printf(रिक्त, MOSQ_LOG_NOTICE, "Warning: Empty username in password file '%s', ingoring.", file);
					अनुवर्त;
				}

				HASH_FIND(hh, *मूल, username, strlen(username), psk);
				यदि(psk){
					log__printf(रिक्त, MOSQ_LOG_NOTICE, "Error: Duplicate user '%s' in password file '%s', ignoring.", username, file);
					अनुवर्त;
				}

				psk = mosquitto_calloc(1, परिमाण(संरचना mosquitto__psk));
				यदि(!psk){
					fclose(pwfile);
					mosquitto_FREE(buf);
					प्रत्यागम MOSQ_ERR_NOMEM;
				}

				psk->username = mosquitto_strdup(username);
				यदि(!psk->username){
					psk__free_item(psk);
					mosquitto_FREE(buf);
					fclose(pwfile);
					प्रत्यागम MOSQ_ERR_NOMEM;
				}
				password = strtok_r(रिक्त, ":", &saveptr);
				यदि(password){
					password = mosquitto_trimblanks(password);

					यदि(strlen(password) > 65535){
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', password too long.", file);
						psk__free_item(psk);
						अनुवर्त;
					}

					psk->password = mosquitto_strdup(password);
					यदि(!psk->password){
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Warning: Unable to decode line in password file '%s'.", file);
						psk__free_item(psk);
						अनुवर्त;
					}

					HASH_ADD_KEYPTR(hh, *मूल, psk->username, strlen(psk->username), psk);
				}अन्यथा{
					log__printf(रिक्त, MOSQ_LOG_NOTICE, "Warning: Invalid line in psk file '%s': %s", file, buf);
					psk__free_item(psk);
				}
			}
		}
	}
	fclose(pwfile);
	mosquitto_FREE(buf);

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क psk__file_parse(संरचना mosquitto__psk **psk_id, ध्रुव वर्ण *psk_file)
{
	पूर्णाङ्क rc;
	संरचना mosquitto__psk *psk, *tmp = रिक्त;

	यदि(!db.config || !psk_id){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	/* We haven't been asked to parse a psk file. */
	यदि(!psk_file){
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}

	rc = pwfile__parse(psk_file, psk_id);
	यदि(rc){
		प्रत्यागम rc;
	}

	HASH_ITER(hh, (*psk_id), psk, tmp){
		/* Check for hex only digits */
		यदि(!psk->password){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Empty psk for identity \"%s\".", psk->username);
			प्रत्यागम MOSQ_ERR_INVAL;
		}
		यदि(strspn(psk->password, "0123456789abcdefABCDEF") < strlen(psk->password)){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: psk for identity \"%s\" contains non-hexadecimal characters.", psk->username);
			प्रत्यागम MOSQ_ERR_INVAL;
		}
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क psk__cleanup(संरचना mosquitto__psk **मूल)
{
	संरचना mosquitto__psk *psk, *tmp = रिक्त;

	यदि(!मूल){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	HASH_ITER(hh, *मूल, psk, tmp){
		HASH_DEL(*मूल, psk);
		psk__free_item(psk);
	}

	*मूल = रिक्त;

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क mosquitto_psk_कुंजी_get_default(संरचना mosquitto *context, ध्रुव वर्ण *hint, ध्रुव वर्ण *identity, वर्ण *कुंजी, पूर्णाङ्क max_कुंजी_len)
{
	संरचना mosquitto__psk *psk;
	संरचना mosquitto__psk *psk_id_ref = रिक्त;

	यदि(!hint || !identity || !कुंजी){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	यदि(db.config->per_listener_settings){
		यदि(!context->listener){
			प्रत्यागम MOSQ_ERR_INVAL;
		}
		psk_id_ref = context->listener->security_options->psk_id;
	}अन्यथा{
		psk_id_ref = db.config->security_options.psk_id;
	}
	यदि(!psk_id_ref){
		प्रत्यागम MOSQ_ERR_PLUGIN_IGNORE;
	}

	HASH_FIND(hh, psk_id_ref, identity, strlen(identity), psk);
	यदि(psk){
		strncpy(कुंजी, psk->password, (size_t)max_कुंजी_len);
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}

	प्रत्यागम MOSQ_ERR_AUTH;
}
