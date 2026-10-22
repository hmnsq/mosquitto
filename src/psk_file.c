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

#అన్తర్భావ "config.h"

#అన్తర్భావ <ctype.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <string.h>

#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "mosquitto/mqtt_protocol.h"
#అన్తర్భావ "send_mosq.h"
#అన్తర్భావ "util_mosq.h"

స్థిర పూర్ణాంకం psk__cleanup(నిర్మాణం mosquitto__psk **psk);
స్థిర పూర్ణాంకం psk__file_parse(నిర్మాణం mosquitto__psk **psk_id, స్థిరరాశి అక్షరం *psk_file);


స్థిర శూన్య psk_free_అంశం(నిర్మాణం mosquitto__psk *psk)
{
	mosquitto_FREE(psk->username);
	mosquitto_FREE(psk->password);
	mosquitto_FREE(psk);
}


పూర్ణాంకం psk_file__init(శూన్య)
{
	పూర్ణాంకం rc;
	అక్షరం *pskf = ఖాళీ;

	/* Load psk data if required. */
	ఒకవేళ(db.config->చొప్పున_listener_settings){
		కొరకు(పూర్ణాంకం i=0; i<db.config->listener_లెక్క; i++){
			pskf = db.config->listeners[i].security_options->psk_file;
			ఒకవేళ(pskf){
				rc = psk__file_parse(&db.config->listeners[i].security_options->psk_id, pskf);
				ఒకవేళ(rc){
					log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
					తిరిగి rc;
				}
			}
		}
	}లేకపోతే{
		pskf = db.config->security_options.psk_file;
		ఒకవేళ(pskf){
			rc = psk__file_parse(&db.config->security_options.psk_id, pskf);
			ఒకవేళ(rc){
				log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error opening psk file \"%s\".", pskf);
				తిరిగి rc;
			}
		}
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం psk_file__cleanup(శూన్య)
{
	పూర్ణాంకం rc;

	rc = psk__cleanup(&db.config->security_options.psk_id);
	ఒకవేళ(rc != MOSQ_ERR_SUCCESS){
		తిరిగి rc;
	}

	కొరకు(పూర్ణాంకం i=0; i<db.config->listener_లెక్క; i++){
		ఒకవేళ(db.config->listeners[i].security_options->psk_id){
			rc = psk__cleanup(&db.config->listeners[i].security_options->psk_id);
			ఒకవేళ(rc != MOSQ_ERR_SUCCESS){
				తిరిగి rc;
			}
		}
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర పూర్ణాంకం pwfile__parse(స్థిరరాశి అక్షరం *file, నిర్మాణం mosquitto__psk **మూలం)
{
	FILE *pwfile;
	నిర్మాణం mosquitto__psk *psk;
	అక్షరం *username, *password;
	అక్షరం *saveptr = ఖాళీ;
	అక్షరం *buf;
	పూర్ణాంకం buflen = 256;

	buf = mosquitto_malloc((size_t)buflen);
	ఒకవేళ(buf == ఖాళీ){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
		తిరిగి MOSQ_ERR_NOMEM;
	}

	pwfile = mosquitto_fopen(file, "rt", true);
	ఒకవేళ(!pwfile){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Unable to open pwfile \"%s\".", file);
		mosquitto_FREE(buf);
		తిరిగి MOSQ_ERR_UNKNOWN;
	}

	అయితే(!feof(pwfile)){
		ఒకవేళ(mosquitto_fgets(&buf, &buflen, pwfile)){
			ఒకవేళ(buf[0] == '#'){
				కొనసాగు;
			}
			ఒకవేళ(!strchr(buf, ':')){
				కొనసాగు;
			}

			username = strtok_r(buf, ":", &saveptr);
			ఒకవేళ(username){
				username = mosquitto_trimblanks(username);
				ఒకవేళ(అక్షరశ్రేణి_పొడవు(username) > 65535){
					log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', username too long.", file);
					కొనసాగు;
				}
				ఒకవేళ(అక్షరశ్రేణి_పొడవు(username) <= 0){
					log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Warning: Empty username in password file '%s', ingoring.", file);
					కొనసాగు;
				}

				HASH_కనుగొను(hh, *మూలం, username, అక్షరశ్రేణి_పొడవు(username), psk);
				ఒకవేళ(psk){
					log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Error: Duplicate user '%s' in password file '%s', ignoring.", username, file);
					కొనసాగు;
				}

				psk = mosquitto_calloc(1, పరిమాణం(నిర్మాణం mosquitto__psk));
				ఒకవేళ(!psk){
					fclose(pwfile);
					mosquitto_FREE(buf);
					తిరిగి MOSQ_ERR_NOMEM;
				}

				psk->username = mosquitto_strdup(username);
				ఒకవేళ(!psk->username){
					psk_free_అంశం(psk);
					mosquitto_FREE(buf);
					fclose(pwfile);
					తిరిగి MOSQ_ERR_NOMEM;
				}
				password = strtok_r(ఖాళీ, ":", &saveptr);
				ఒకవేళ(password){
					password = mosquitto_trimblanks(password);

					ఒకవేళ(అక్షరశ్రేణి_పొడవు(password) > 65535){
						log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Warning: Invalid line in password file '%s', password too long.", file);
						psk_free_అంశం(psk);
						కొనసాగు;
					}

					psk->password = mosquitto_strdup(password);
					ఒకవేళ(!psk->password){
						log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Warning: Unable to decode line in password file '%s'.", file);
						psk_free_అంశం(psk);
						కొనసాగు;
					}

					HASH_కూడు_KEYPTR(hh, *మూలం, psk->username, అక్షరశ్రేణి_పొడవు(psk->username), psk);
				}లేకపోతే{
					log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "Warning: Invalid line in psk file '%s': %s", file, buf);
					psk_free_అంశం(psk);
				}
			}
		}
	}
	fclose(pwfile);
	mosquitto_FREE(buf);

	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర పూర్ణాంకం psk__file_parse(నిర్మాణం mosquitto__psk **psk_id, స్థిరరాశి అక్షరం *psk_file)
{
	పూర్ణాంకం rc;
	నిర్మాణం mosquitto__psk *psk, *tmp = ఖాళీ;

	ఒకవేళ(!db.config || !psk_id){
		తిరిగి MOSQ_ERR_INVAL;
	}

	/* We haven't been asked to parse a psk file. */
	ఒకవేళ(!psk_file){
		తిరిగి MOSQ_ERR_SUCCESS;
	}

	rc = pwfile__parse(psk_file, psk_id);
	ఒకవేళ(rc){
		తిరిగి rc;
	}

	HASH_ITER(hh, (*psk_id), psk, tmp){
		/* Check for hex only digits */
		ఒకవేళ(!psk->password){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Empty psk for identity \"%s\".", psk->username);
			తిరిగి MOSQ_ERR_INVAL;
		}
		ఒకవేళ(strspn(psk->password, "0123456789abcdefABCDEF") < అక్షరశ్రేణి_పొడవు(psk->password)){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: psk for identity \"%s\" contains non-hexadecimal characters.", psk->username);
			తిరిగి MOSQ_ERR_INVAL;
		}
	}
	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర పూర్ణాంకం psk__cleanup(నిర్మాణం mosquitto__psk **మూలం)
{
	నిర్మాణం mosquitto__psk *psk, *tmp = ఖాళీ;

	ఒకవేళ(!మూలం){
		తిరిగి MOSQ_ERR_INVAL;
	}

	HASH_ITER(hh, *మూలం, psk, tmp){
		HASH_DEL(*మూలం, psk);
		psk_free_అంశం(psk);
	}

	*మూలం = ఖాళీ;

	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం mosquitto_psk_కీలకం_get_default(నిర్మాణం mosquitto *context, స్థిరరాశి అక్షరం *hint, స్థిరరాశి అక్షరం *తటస్థం, అక్షరం *కీలకం, పూర్ణాంకం max_కీలకం_len)
{
	నిర్మాణం mosquitto__psk *psk;
	నిర్మాణం mosquitto__psk *psk_id_ref = ఖాళీ;

	ఒకవేళ(!hint || !తటస్థం || !కీలకం){
		తిరిగి MOSQ_ERR_INVAL;
	}

	ఒకవేళ(db.config->చొప్పున_listener_settings){
		ఒకవేళ(!context->listener){
			తిరిగి MOSQ_ERR_INVAL;
		}
		psk_id_ref = context->listener->security_options->psk_id;
	}లేకపోతే{
		psk_id_ref = db.config->security_options.psk_id;
	}
	ఒకవేళ(!psk_id_ref){
		తిరిగి MOSQ_ERR_PLUGIN_IGNORE;
	}

	HASH_కనుగొను(hh, psk_id_ref, తటస్థం, అక్షరశ్రేణి_పొడవు(తటస్థం), psk);
	ఒకవేళ(psk){
		strncpy(కీలకం, psk->password, (size_t)max_కీలకం_len);
		తిరిగి MOSQ_ERR_SUCCESS;
	}

	తిరిగి MOSQ_ERR_AUTH;
}
