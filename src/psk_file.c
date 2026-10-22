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
स्थिर पूर्णाङ्क psk__file_parse(संरचना mosquitto__psk **psk_id, अचर वर्ण *psk_file);


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
		लागि(पूर्णाङ्क i=0; i<db.config->listener_गन्ती; i++){
			pskf = db.config->listeners[i].security_options->psk_file;
			यदि(pskf){
				rc = psk__file_parse(&db.config->listeners[i].security_options->psk_id, pskf);
				यदि(rc){
					log__printf(रिक्त, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
					फर्काऊ rc;
				}
			}
		}
	}अन्यथा{
		pskf = db.config->security_options.psk_file;
		यदि(pskf){
			rc = psk__file_parse(&db.config->security_options.psk_id, pskf);
			यदि(rc){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
				फर्काऊ rc;
			}
		}
	}

	फर्काऊ MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क psk_file__cleanup(शून्य)
{
	पूर्णाङ्क rc;

	rc = psk__cleanup(&db.config->security_options.psk_id);
	यदि(rc != MOSQ_ERR_SUCCESS){
		फर्काऊ rc;
	}

	लागि(पूर्णाङ्क i=0; i<db.config->listener_गन्ती; i++){
		यदि(db.config->listeners[i].security_options->psk_id){
			rc = psk__cleanup(&db.config->listeners[i].security_options->psk_id);
			यदि(rc != MOSQ_ERR_SUCCESS){
				फर्काऊ rc;
			}
		}
	}

	फर्काऊ MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क pwfile__parse(अचर वर्ण *file, संरचना mosquitto__psk **जरा)
{
	खाता *pwfile;
	संरचना mosquitto__psk *psk;
	वर्ण *username, *password;
	वर्ण *saveptr = रिक्त;
	वर्ण *buf;
	पूर्णाङ्क buflen = 256;

	buf = mosquitto_malloc((माप_प्रकार)buflen);
	यदि(buf == रिक्त){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		फर्काऊ MOSQ_ERR_NOMEM;
	}

	pwfile = mosquitto_fopen(file, "rt", true);
	यदि(!pwfile){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to open pwfile \"%s\".", file);
		mosquitto_FREE(buf);
		फर्काऊ MOSQ_ERR_UNKNOWN;
	}

	जबसम्म(!ख_पूर्ण(pwfile)){
		यदि(mosquitto_fgets(&buf, &buflen, pwfile)){
			यदि(buf[0] == '#'){
				जारी;
			}
			यदि(!म_अक्षर(buf, ':')){
				जारी;
			}

			username = strtok_r(buf, ":", &saveptr);
			यदि(username){
				username = mosquitto_trimblanks(username);
				यदि(म_माप(username) > 65535){
					log__printf(रिक्त, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', username too long.", file);
					जारी;
				}
				यदि(म_माप(username) <= 0){
					log__printf(रिक्त, MOSQ_LOG_NOTICE, "Warning: Empty username in password file '%s', ingoring.", file);
					जारी;
				}

				HASH_FIND(hh, *जरा, username, म_माप(username), psk);
				यदि(psk){
					log__printf(रिक्त, MOSQ_LOG_NOTICE, "Error: Duplicate user '%s' in password file '%s', ignoring.", username, file);
					जारी;
				}

				psk = mosquitto_calloc(1, परिमाण(संरचना mosquitto__psk));
				यदि(!psk){
					ख_बंद(pwfile);
					mosquitto_FREE(buf);
					फर्काऊ MOSQ_ERR_NOMEM;
				}

				psk->username = mosquitto_strdup(username);
				यदि(!psk->username){
					psk__free_item(psk);
					mosquitto_FREE(buf);
					ख_बंद(pwfile);
					फर्काऊ MOSQ_ERR_NOMEM;
				}
				password = strtok_r(रिक्त, ":", &saveptr);
				यदि(password){
					password = mosquitto_trimblanks(password);

					यदि(म_माप(password) > 65535){
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', password too long.", file);
						psk__free_item(psk);
						जारी;
					}

					psk->password = mosquitto_strdup(password);
					यदि(!psk->password){
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Warning: Unable to decode line in password file '%s'.", file);
						psk__free_item(psk);
						जारी;
					}

					HASH_ADD_KEYPTR(hh, *जरा, psk->username, म_माप(psk->username), psk);
				}अन्यथा{
					log__printf(रिक्त, MOSQ_LOG_NOTICE, "Warning: Invalid line in psk file '%s': %s", file, buf);
					psk__free_item(psk);
				}
			}
		}
	}
	ख_बंद(pwfile);
	mosquitto_FREE(buf);

	फर्काऊ MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क psk__file_parse(संरचना mosquitto__psk **psk_id, अचर वर्ण *psk_file)
{
	पूर्णाङ्क rc;
	संरचना mosquitto__psk *psk, *tmp = रिक्त;

	यदि(!db.config || !psk_id){
		फर्काऊ MOSQ_ERR_INVAL;
	}

	/* We haven't been asked to parse a psk file. */
	यदि(!psk_file){
		फर्काऊ MOSQ_ERR_SUCCESS;
	}

	rc = pwfile__parse(psk_file, psk_id);
	यदि(rc){
		फर्काऊ rc;
	}

	HASH_ITER(hh, (*psk_id), psk, tmp){
		/* Check for hex only digits */
		यदि(!psk->password){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Empty psk for identity \"%s\".", psk->username);
			फर्काऊ MOSQ_ERR_INVAL;
		}
		यदि(म_अखोज(psk->password, "0123456789abcdefABCDEF") < म_माप(psk->password)){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: psk for identity \"%s\" contains non-hexadecimal characters.", psk->username);
			फर्काऊ MOSQ_ERR_INVAL;
		}
	}
	फर्काऊ MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णाङ्क psk__cleanup(संरचना mosquitto__psk **जरा)
{
	संरचना mosquitto__psk *psk, *tmp = रिक्त;

	यदि(!जरा){
		फर्काऊ MOSQ_ERR_INVAL;
	}

	HASH_ITER(hh, *जरा, psk, tmp){
		HASH_DEL(*जरा, psk);
		psk__free_item(psk);
	}

	*जरा = रिक्त;

	फर्काऊ MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क mosquitto_psk_साँचो_get_default(संरचना mosquitto *context, अचर वर्ण *hint, अचर वर्ण *identity, वर्ण *साँचो, पूर्णाङ्क max_साँचो_len)
{
	संरचना mosquitto__psk *psk;
	संरचना mosquitto__psk *psk_id_ref = रिक्त;

	यदि(!hint || !identity || !साँचो){
		फर्काऊ MOSQ_ERR_INVAL;
	}

	यदि(db.config->per_listener_settings){
		यदि(!context->listener){
			फर्काऊ MOSQ_ERR_INVAL;
		}
		psk_id_ref = context->listener->security_options->psk_id;
	}अन्यथा{
		psk_id_ref = db.config->security_options.psk_id;
	}
	यदि(!psk_id_ref){
		फर्काऊ MOSQ_ERR_PLUGIN_IGNORE;
	}

	HASH_FIND(hh, psk_id_ref, identity, म_माप(identity), psk);
	यदि(psk){
		म_नकलन(साँचो, psk->password, (माप_प्रकार)max_साँचो_len);
		फर्काऊ MOSQ_ERR_SUCCESS;
	}

	फर्काऊ MOSQ_ERR_AUTH;
}
