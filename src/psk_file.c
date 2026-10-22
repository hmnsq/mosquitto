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

#അന്തര്ഭാവ "config.h"

#അന്തര്ഭാവ <ctype.h>
#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <string.h>

#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "mosquitto/mqtt_protocol.h"
#അന്തര്ഭാവ "send_mosq.h"
#അന്തര്ഭാവ "util_mosq.h"

സ്ഥിതം പൂർണ്ണസംഖ്യ psk__cleanup(ഘടന mosquitto__psk **psk);
സ്ഥിതം പൂർണ്ണസംഖ്യ psk__file_parse(ഘടന mosquitto__psk **psk_id, സ്ഥിരം അക്ഷരം *psk_file);


സ്ഥിതം ശൂന്യം psk__free_item(ഘടന mosquitto__psk *psk)
{
	mosquitto_FREE(psk->username);
	mosquitto_FREE(psk->password);
	mosquitto_FREE(psk);
}


പൂർണ്ണസംഖ്യ psk_file__init(ശൂന്യം)
{
	പൂർണ്ണസംഖ്യ rc;
	അക്ഷരം *pskf = ശൂന്യ;

	/* Load psk data if required. */
	എങ്കിൽ(db.config->per_listener_settings){
		വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<db.config->listener_എണ്ണം; i++){
			pskf = db.config->listeners[i].security_options->psk_file;
			എങ്കിൽ(pskf){
				rc = psk__file_parse(&db.config->listeners[i].security_options->psk_id, pskf);
				എങ്കിൽ(rc){
					log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
					തിരികെ rc;
				}
			}
		}
	}അല്ലെങ്കിൽ{
		pskf = db.config->security_options.psk_file;
		എങ്കിൽ(pskf){
			rc = psk__file_parse(&db.config->security_options.psk_id, pskf);
			എങ്കിൽ(rc){
				log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
				തിരികെ rc;
			}
		}
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ psk_file__cleanup(ശൂന്യം)
{
	പൂർണ്ണസംഖ്യ rc;

	rc = psk__cleanup(&db.config->security_options.psk_id);
	എങ്കിൽ(rc != MOSQ_ERR_SUCCESS){
		തിരികെ rc;
	}

	വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<db.config->listener_എണ്ണം; i++){
		എങ്കിൽ(db.config->listeners[i].security_options->psk_id){
			rc = psk__cleanup(&db.config->listeners[i].security_options->psk_id);
			എങ്കിൽ(rc != MOSQ_ERR_SUCCESS){
				തിരികെ rc;
			}
		}
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ pwfile__parse(സ്ഥിരം അക്ഷരം *file, ഘടന mosquitto__psk **വേര്)
{
	FILE *pwfile;
	ഘടന mosquitto__psk *psk;
	അക്ഷരം *username, *password;
	അക്ഷരം *saveptr = ശൂന്യ;
	അക്ഷരം *buf;
	പൂർണ്ണസംഖ്യ buflen = 256;

	buf = mosquitto_malloc((size_t)buflen);
	എങ്കിൽ(buf == ശൂന്യ){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Out of memory.");
		തിരികെ MOSQ_ERR_NOMEM;
	}

	pwfile = mosquitto_fopen(file, "rt", true);
	എങ്കിൽ(!pwfile){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Unable to open pwfile \"%s\".", file);
		mosquitto_FREE(buf);
		തിരികെ MOSQ_ERR_UNKNOWN;
	}

	ആകുമ്പോൾ(!feof(pwfile)){
		എങ്കിൽ(mosquitto_fgets(&buf, &buflen, pwfile)){
			എങ്കിൽ(buf[0] == '#'){
				തുടരുക;
			}
			എങ്കിൽ(!strchr(buf, ':')){
				തുടരുക;
			}

			username = strtok_r(buf, ":", &saveptr);
			എങ്കിൽ(username){
				username = mosquitto_trimblanks(username);
				എങ്കിൽ(strlen(username) > 65535){
					log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', username too long.", file);
					തുടരുക;
				}
				എങ്കിൽ(strlen(username) <= 0){
					log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Warning: Empty username in password file '%s', ingoring.", file);
					തുടരുക;
				}

				HASH_FIND(hh, *വേര്, username, strlen(username), psk);
				എങ്കിൽ(psk){
					log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Error: Duplicate user '%s' in password file '%s', ignoring.", username, file);
					തുടരുക;
				}

				psk = mosquitto_calloc(1, വലുപ്പം(ഘടന mosquitto__psk));
				എങ്കിൽ(!psk){
					fclose(pwfile);
					mosquitto_FREE(buf);
					തിരികെ MOSQ_ERR_NOMEM;
				}

				psk->username = mosquitto_strdup(username);
				എങ്കിൽ(!psk->username){
					psk__free_item(psk);
					mosquitto_FREE(buf);
					fclose(pwfile);
					തിരികെ MOSQ_ERR_NOMEM;
				}
				password = strtok_r(ശൂന്യ, ":", &saveptr);
				എങ്കിൽ(password){
					password = mosquitto_trimblanks(password);

					എങ്കിൽ(strlen(password) > 65535){
						log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', password too long.", file);
						psk__free_item(psk);
						തുടരുക;
					}

					psk->password = mosquitto_strdup(password);
					എങ്കിൽ(!psk->password){
						log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Warning: Unable to decode line in password file '%s'.", file);
						psk__free_item(psk);
						തുടരുക;
					}

					HASH_ADD_KEYPTR(hh, *വേര്, psk->username, strlen(psk->username), psk);
				}അല്ലെങ്കിൽ{
					log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "Warning: Invalid line in psk file '%s': %s", file, buf);
					psk__free_item(psk);
				}
			}
		}
	}
	fclose(pwfile);
	mosquitto_FREE(buf);

	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ psk__file_parse(ഘടന mosquitto__psk **psk_id, സ്ഥിരം അക്ഷരം *psk_file)
{
	പൂർണ്ണസംഖ്യ rc;
	ഘടന mosquitto__psk *psk, *tmp = ശൂന്യ;

	എങ്കിൽ(!db.config || !psk_id){
		തിരികെ MOSQ_ERR_INVAL;
	}

	/* We haven't been asked to parse a psk file. */
	എങ്കിൽ(!psk_file){
		തിരികെ MOSQ_ERR_SUCCESS;
	}

	rc = pwfile__parse(psk_file, psk_id);
	എങ്കിൽ(rc){
		തിരികെ rc;
	}

	HASH_ITER(hh, (*psk_id), psk, tmp){
		/* Check for hex only digits */
		എങ്കിൽ(!psk->password){
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Empty psk for identity \"%s\".", psk->username);
			തിരികെ MOSQ_ERR_INVAL;
		}
		എങ്കിൽ(strspn(psk->password, "0123456789abcdefABCDEF") < strlen(psk->password)){
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: psk for identity \"%s\" contains non-hexadecimal characters.", psk->username);
			തിരികെ MOSQ_ERR_INVAL;
		}
	}
	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ psk__cleanup(ഘടന mosquitto__psk **വേര്)
{
	ഘടന mosquitto__psk *psk, *tmp = ശൂന്യ;

	എങ്കിൽ(!വേര്){
		തിരികെ MOSQ_ERR_INVAL;
	}

	HASH_ITER(hh, *വേര്, psk, tmp){
		HASH_DEL(*വേര്, psk);
		psk__free_item(psk);
	}

	*വേര് = ശൂന്യ;

	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ mosquitto_psk_താക്കോൽ_get_default(ഘടന mosquitto *context, സ്ഥിരം അക്ഷരം *hint, സ്ഥിരം അക്ഷരം *identity, അക്ഷരം *താക്കോൽ, പൂർണ്ണസംഖ്യ max_താക്കോൽ_len)
{
	ഘടന mosquitto__psk *psk;
	ഘടന mosquitto__psk *psk_id_ref = ശൂന്യ;

	എങ്കിൽ(!hint || !identity || !താക്കോൽ){
		തിരികെ MOSQ_ERR_INVAL;
	}

	എങ്കിൽ(db.config->per_listener_settings){
		എങ്കിൽ(!context->listener){
			തിരികെ MOSQ_ERR_INVAL;
		}
		psk_id_ref = context->listener->security_options->psk_id;
	}അല്ലെങ്കിൽ{
		psk_id_ref = db.config->security_options.psk_id;
	}
	എങ്കിൽ(!psk_id_ref){
		തിരികെ MOSQ_ERR_PLUGIN_IGNORE;
	}

	HASH_FIND(hh, psk_id_ref, identity, strlen(identity), psk);
	എങ്കിൽ(psk){
		strncpy(താക്കോൽ, psk->password, (size_t)max_താക്കോൽ_len);
		തിരികെ MOSQ_ERR_SUCCESS;
	}

	തിരികെ MOSQ_ERR_AUTH;
}
