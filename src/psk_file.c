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

#समावेश "config.h"

#समावेश <ctype.h>
#समावेश <stdio.h>
#समावेश <string.h>

#समावेश "mosquitto_broker_internal.h"
#समावेश "mosquitto/mqtt_protocol.h"
#समावेश "send_mosq.h"
#समावेश "util_mosq.h"

अटल पूर्णांक psk__cleanup(काष्ठा mosquitto__psk **psk);
अटल पूर्णांक psk__file_parse(काष्ठा mosquitto__psk **psk_id, स्थिर अक्षर *psk_file);


अटल व्योम psk__free_item(काष्ठा mosquitto__psk *psk)
{
	mosquitto_FREE(psk->username);
	mosquitto_FREE(psk->password);
	mosquitto_FREE(psk);
}


पूर्णांक psk_file__init(व्योम)
{
	पूर्णांक rc;
	अक्षर *pskf = शून्य;

	/* Load psk data if required. */
	अगर(db.config->per_listener_settings){
		क्रम(पूर्णांक i=0; i<db.config->listener_गिनती; i++){
			pskf = db.config->listeners[i].security_options->psk_file;
			अगर(pskf){
				rc = psk__file_parse(&db.config->listeners[i].security_options->psk_id, pskf);
				अगर(rc){
					log__printf(शून्य, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
					वापस rc;
				}
			}
		}
	}अन्यथा{
		pskf = db.config->security_options.psk_file;
		अगर(pskf){
			rc = psk__file_parse(&db.config->security_options.psk_id, pskf);
			अगर(rc){
				log__printf(शून्य, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
				वापस rc;
			}
		}
	}

	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक psk_file__cleanup(व्योम)
{
	पूर्णांक rc;

	rc = psk__cleanup(&db.config->security_options.psk_id);
	अगर(rc != MOSQ_ERR_SUCCESS){
		वापस rc;
	}

	क्रम(पूर्णांक i=0; i<db.config->listener_गिनती; i++){
		अगर(db.config->listeners[i].security_options->psk_id){
			rc = psk__cleanup(&db.config->listeners[i].security_options->psk_id);
			अगर(rc != MOSQ_ERR_SUCCESS){
				वापस rc;
			}
		}
	}

	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक pwfile__parse(स्थिर अक्षर *file, काष्ठा mosquitto__psk **मूल)
{
	खाता *pwfile;
	काष्ठा mosquitto__psk *psk;
	अक्षर *username, *password;
	अक्षर *saveptr = शून्य;
	अक्षर *buf;
	पूर्णांक buflen = 256;

	buf = mosquitto_malloc((माप_प्रकार)buflen);
	अगर(buf == शून्य){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
		वापस MOSQ_ERR_NOMEM;
	}

	pwfile = mosquitto_fopen(file, "rt", true);
	अगर(!pwfile){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: Unable to open pwfile \"%s\".", file);
		mosquitto_FREE(buf);
		वापस MOSQ_ERR_UNKNOWN;
	}

	जबतक(!ख_पूर्ण(pwfile)){
		अगर(mosquitto_fgets(&buf, &buflen, pwfile)){
			अगर(buf[0] == '#'){
				जारी;
			}
			अगर(!म_अक्षर(buf, ':')){
				जारी;
			}

			username = strtok_r(buf, ":", &saveptr);
			अगर(username){
				username = mosquitto_trimblanks(username);
				अगर(म_माप(username) > 65535){
					log__printf(शून्य, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', username too long.", file);
					जारी;
				}
				अगर(म_माप(username) <= 0){
					log__printf(शून्य, MOSQ_LOG_NOTICE, "Warning: Empty username in password file '%s', ingoring.", file);
					जारी;
				}

				HASH_FIND(hh, *मूल, username, म_माप(username), psk);
				अगर(psk){
					log__printf(शून्य, MOSQ_LOG_NOTICE, "Error: Duplicate user '%s' in password file '%s', ignoring.", username, file);
					जारी;
				}

				psk = mosquitto_calloc(1, माप(काष्ठा mosquitto__psk));
				अगर(!psk){
					ख_बंद(pwfile);
					mosquitto_FREE(buf);
					वापस MOSQ_ERR_NOMEM;
				}

				psk->username = mosquitto_strdup(username);
				अगर(!psk->username){
					psk__free_item(psk);
					mosquitto_FREE(buf);
					ख_बंद(pwfile);
					वापस MOSQ_ERR_NOMEM;
				}
				password = strtok_r(शून्य, ":", &saveptr);
				अगर(password){
					password = mosquitto_trimblanks(password);

					अगर(म_माप(password) > 65535){
						log__printf(शून्य, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', password too long.", file);
						psk__free_item(psk);
						जारी;
					}

					psk->password = mosquitto_strdup(password);
					अगर(!psk->password){
						log__printf(शून्य, MOSQ_LOG_NOTICE, "Warning: Unable to decode line in password file '%s'.", file);
						psk__free_item(psk);
						जारी;
					}

					HASH_ADD_KEYPTR(hh, *मूल, psk->username, म_माप(psk->username), psk);
				}अन्यथा{
					log__printf(शून्य, MOSQ_LOG_NOTICE, "Warning: Invalid line in psk file '%s': %s", file, buf);
					psk__free_item(psk);
				}
			}
		}
	}
	ख_बंद(pwfile);
	mosquitto_FREE(buf);

	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक psk__file_parse(काष्ठा mosquitto__psk **psk_id, स्थिर अक्षर *psk_file)
{
	पूर्णांक rc;
	काष्ठा mosquitto__psk *psk, *tmp = शून्य;

	अगर(!db.config || !psk_id){
		वापस MOSQ_ERR_INVAL;
	}

	/* We haven't been asked to parse a psk file. */
	अगर(!psk_file){
		वापस MOSQ_ERR_SUCCESS;
	}

	rc = pwfile__parse(psk_file, psk_id);
	अगर(rc){
		वापस rc;
	}

	HASH_ITER(hh, (*psk_id), psk, tmp){
		/* Check for hex only digits */
		अगर(!psk->password){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error: Empty psk for identity \"%s\".", psk->username);
			वापस MOSQ_ERR_INVAL;
		}
		अगर(म_अखोज(psk->password, "0123456789abcdefABCDEF") < म_माप(psk->password)){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error: psk for identity \"%s\" contains non-hexadecimal characters.", psk->username);
			वापस MOSQ_ERR_INVAL;
		}
	}
	वापस MOSQ_ERR_SUCCESS;
}


अटल पूर्णांक psk__cleanup(काष्ठा mosquitto__psk **मूल)
{
	काष्ठा mosquitto__psk *psk, *tmp = शून्य;

	अगर(!मूल){
		वापस MOSQ_ERR_INVAL;
	}

	HASH_ITER(hh, *मूल, psk, tmp){
		HASH_DEL(*मूल, psk);
		psk__free_item(psk);
	}

	*मूल = शून्य;

	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक mosquitto_psk_कुंजी_get_default(काष्ठा mosquitto *context, स्थिर अक्षर *hint, स्थिर अक्षर *identity, अक्षर *कुंजी, पूर्णांक max_कुंजी_len)
{
	काष्ठा mosquitto__psk *psk;
	काष्ठा mosquitto__psk *psk_id_ref = शून्य;

	अगर(!hint || !identity || !कुंजी){
		वापस MOSQ_ERR_INVAL;
	}

	अगर(db.config->per_listener_settings){
		अगर(!context->listener){
			वापस MOSQ_ERR_INVAL;
		}
		psk_id_ref = context->listener->security_options->psk_id;
	}अन्यथा{
		psk_id_ref = db.config->security_options.psk_id;
	}
	अगर(!psk_id_ref){
		वापस MOSQ_ERR_PLUGIN_IGNORE;
	}

	HASH_FIND(hh, psk_id_ref, identity, म_माप(identity), psk);
	अगर(psk){
		म_नकलन(कुंजी, psk->password, (माप_प्रकार)max_कुंजी_len);
		वापस MOSQ_ERR_SUCCESS;
	}

	वापस MOSQ_ERR_AUTH;
}
