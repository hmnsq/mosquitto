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

स्थिर पूर्णांक psk__cleanup(संरचना mosquitto__psk **psk);
स्थिर पूर्णांक psk__file_parse(संरचना mosquitto__psk **psk_id, स्थिरांक अक्षर *psk_file);


स्थिर शून्य psk__free_item(संरचना mosquitto__psk *psk)
{
	mosquitto_FREE(psk->username);
	mosquitto_FREE(psk->password);
	mosquitto_FREE(psk);
}


पूर्णांक psk_file__init(शून्य)
{
	पूर्णांक rc;
	अक्षर *pskf = रिक्त;

	/* Load psk data if required. */
	जर(db.config->per_listener_settings){
		साठी(पूर्णांक i=0; i<db.config->listener_मोजणी; i++){
			pskf = db.config->listeners[i].security_options->psk_file;
			जर(pskf){
				rc = psk__file_parse(&db.config->listeners[i].security_options->psk_id, pskf);
				जर(rc){
					log__printf(रिक्त, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
					परत rc;
				}
			}
		}
	}अन्यथा{
		pskf = db.config->security_options.psk_file;
		जर(pskf){
			rc = psk__file_parse(&db.config->security_options.psk_id, pskf);
			जर(rc){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
				परत rc;
			}
		}
	}

	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक psk_file__cleanup(शून्य)
{
	पूर्णांक rc;

	rc = psk__cleanup(&db.config->security_options.psk_id);
	जर(rc != MOSQ_ERR_SUCCESS){
		परत rc;
	}

	साठी(पूर्णांक i=0; i<db.config->listener_मोजणी; i++){
		जर(db.config->listeners[i].security_options->psk_id){
			rc = psk__cleanup(&db.config->listeners[i].security_options->psk_id);
			जर(rc != MOSQ_ERR_SUCCESS){
				परत rc;
			}
		}
	}

	परत MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णांक pwfile__parse(स्थिरांक अक्षर *file, संरचना mosquitto__psk **मूळ)
{
	खाता *pwfile;
	संरचना mosquitto__psk *psk;
	अक्षर *username, *password;
	अक्षर *saveptr = रिक्त;
	अक्षर *buf;
	पूर्णांक buflen = 256;

	buf = mosquitto_malloc((माप_प्रकार)buflen);
	जर(buf == रिक्त){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		परत MOSQ_ERR_NOMEM;
	}

	pwfile = mosquitto_fopen(file, "rt", true);
	जर(!pwfile){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to open pwfile \"%s\".", file);
		mosquitto_FREE(buf);
		परत MOSQ_ERR_UNKNOWN;
	}

	पर्यंत(!ख_पूर्ण(pwfile)){
		जर(mosquitto_fgets(&buf, &buflen, pwfile)){
			जर(buf[0] == '#'){
				सुरूठेवा;
			}
			जर(!म_अक्षर(buf, ':')){
				सुरूठेवा;
			}

			username = strtok_r(buf, ":", &saveptr);
			जर(username){
				username = mosquitto_trimblanks(username);
				जर(म_माप(username) > 65535){
					log__printf(रिक्त, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', username too long.", file);
					सुरूठेवा;
				}
				जर(म_माप(username) <= 0){
					log__printf(रिक्त, MOSQ_LOG_NOTICE, "Warning: Empty username in password file '%s', ingoring.", file);
					सुरूठेवा;
				}

				HASH_FIND(hh, *मूळ, username, म_माप(username), psk);
				जर(psk){
					log__printf(रिक्त, MOSQ_LOG_NOTICE, "Error: Duplicate user '%s' in password file '%s', ignoring.", username, file);
					सुरूठेवा;
				}

				psk = mosquitto_calloc(1, आकार(संरचना mosquitto__psk));
				जर(!psk){
					ख_बंद(pwfile);
					mosquitto_FREE(buf);
					परत MOSQ_ERR_NOMEM;
				}

				psk->username = mosquitto_strdup(username);
				जर(!psk->username){
					psk__free_item(psk);
					mosquitto_FREE(buf);
					ख_बंद(pwfile);
					परत MOSQ_ERR_NOMEM;
				}
				password = strtok_r(रिक्त, ":", &saveptr);
				जर(password){
					password = mosquitto_trimblanks(password);

					जर(म_माप(password) > 65535){
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', password too long.", file);
						psk__free_item(psk);
						सुरूठेवा;
					}

					psk->password = mosquitto_strdup(password);
					जर(!psk->password){
						log__printf(रिक्त, MOSQ_LOG_NOTICE, "Warning: Unable to decode line in password file '%s'.", file);
						psk__free_item(psk);
						सुरूठेवा;
					}

					HASH_ADD_KEYPTR(hh, *मूळ, psk->username, म_माप(psk->username), psk);
				}अन्यथा{
					log__printf(रिक्त, MOSQ_LOG_NOTICE, "Warning: Invalid line in psk file '%s': %s", file, buf);
					psk__free_item(psk);
				}
			}
		}
	}
	ख_बंद(pwfile);
	mosquitto_FREE(buf);

	परत MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णांक psk__file_parse(संरचना mosquitto__psk **psk_id, स्थिरांक अक्षर *psk_file)
{
	पूर्णांक rc;
	संरचना mosquitto__psk *psk, *tmp = रिक्त;

	जर(!db.config || !psk_id){
		परत MOSQ_ERR_INVAL;
	}

	/* We haven't been asked to parse a psk file. */
	जर(!psk_file){
		परत MOSQ_ERR_SUCCESS;
	}

	rc = pwfile__parse(psk_file, psk_id);
	जर(rc){
		परत rc;
	}

	HASH_ITER(hh, (*psk_id), psk, tmp){
		/* Check for hex only digits */
		जर(!psk->password){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Empty psk for identity \"%s\".", psk->username);
			परत MOSQ_ERR_INVAL;
		}
		जर(म_अखोज(psk->password, "0123456789abcdefABCDEF") < म_माप(psk->password)){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: psk for identity \"%s\" contains non-hexadecimal characters.", psk->username);
			परत MOSQ_ERR_INVAL;
		}
	}
	परत MOSQ_ERR_SUCCESS;
}


स्थिर पूर्णांक psk__cleanup(संरचना mosquitto__psk **मूळ)
{
	संरचना mosquitto__psk *psk, *tmp = रिक्त;

	जर(!मूळ){
		परत MOSQ_ERR_INVAL;
	}

	HASH_ITER(hh, *मूळ, psk, tmp){
		HASH_DEL(*मूळ, psk);
		psk__free_item(psk);
	}

	*मूळ = रिक्त;

	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक mosquitto_psk_किल्ली_get_default(संरचना mosquitto *context, स्थिरांक अक्षर *hint, स्थिरांक अक्षर *identity, अक्षर *किल्ली, पूर्णांक max_किल्ली_len)
{
	संरचना mosquitto__psk *psk;
	संरचना mosquitto__psk *psk_id_ref = रिक्त;

	जर(!hint || !identity || !किल्ली){
		परत MOSQ_ERR_INVAL;
	}

	जर(db.config->per_listener_settings){
		जर(!context->listener){
			परत MOSQ_ERR_INVAL;
		}
		psk_id_ref = context->listener->security_options->psk_id;
	}अन्यथा{
		psk_id_ref = db.config->security_options.psk_id;
	}
	जर(!psk_id_ref){
		परत MOSQ_ERR_PLUGIN_IGNORE;
	}

	HASH_FIND(hh, psk_id_ref, identity, म_माप(identity), psk);
	जर(psk){
		म_नकलन(किल्ली, psk->password, (माप_प्रकार)max_किल्ली_len);
		परत MOSQ_ERR_SUCCESS;
	}

	परत MOSQ_ERR_AUTH;
}
