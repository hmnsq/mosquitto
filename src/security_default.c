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
#समावेश "password_file.h"
#समावेश "send_mosq.h"
#समावेश "util_mosq.h"


पूर्णांक mosquitto_security_init_default(व्योम)
{
	पूर्णांक rc;

	/* Configure plugin identifier */
	अगर(db.config->per_listener_settings){
		क्रम(पूर्णांक i=0; i<db.config->listener_गिनती; i++){
			db.config->listeners[i].security_options->pid = mosquitto_calloc(1, माप(mosquitto_plugin_id_t));
			अगर(db.config->listeners[i].security_options->pid == शून्य){
				log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
				वापस MOSQ_ERR_NOMEM;
			}
			db.config->listeners[i].security_options->pid->plugin_name = mosquitto_strdup("builtin-security");
			db.config->listeners[i].security_options->pid->listener = &db.config->listeners[i];
			config__plugin_add_secopt(db.config->listeners[i].security_options->pid, db.config->listeners[i].security_options);
		}
	}अन्यथा{
		db.config->security_options.pid = mosquitto_calloc(1, माप(mosquitto_plugin_id_t));
		अगर(db.config->security_options.pid == शून्य){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
			वापस MOSQ_ERR_NOMEM;
		}
		db.config->security_options.pid->plugin_name = mosquitto_strdup("builtin-security");
		config__plugin_add_secopt(db.config->security_options.pid, &db.config->security_options);
	}

	rc = broker_password_file__init();
	अगर(rc){
		वापस rc;
	}

	rc = broker_acl_file__init();
	अगर(rc){
		वापस rc;
	}

	rc = psk_file__init();
	अगर(rc){
		वापस rc;
	}

	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक mosquitto_security_cleanup_default(व्योम)
{
	पूर्णांक rc = 0;

	broker_password_file__cleanup();
	broker_acl_file__cleanup();

	rc = psk_file__cleanup();
	अगर(rc != MOSQ_ERR_SUCCESS){
		वापस rc;
	}

	अगर(db.config->per_listener_settings){
		क्रम(पूर्णांक i=0; i<db.config->listener_गिनती; i++){
			अगर(db.config->listeners[i].security_options->pid){
				mosquitto_FREE(db.config->listeners[i].security_options->pid->plugin_name);
				mosquitto_FREE(db.config->listeners[i].security_options->pid->config.security_options);
				mosquitto_FREE(db.config->listeners[i].security_options->pid);
			}
		}
	}अन्यथा{
		अगर(db.config->security_options.pid){
			mosquitto_FREE(db.config->security_options.pid->plugin_name);
			mosquitto_FREE(db.config->security_options.pid->config.security_options);
			mosquitto_FREE(db.config->security_options.pid);
		}
	}
	वापस MOSQ_ERR_SUCCESS;
}


#अगर_घोषित WITH_TLS


अटल व्योम security__disconnect_auth(काष्ठा mosquitto *context)
{
	अगर(context->protocol == mosq_p_mqtt5){
		send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, शून्य);
	}
	mosquitto__set_state(context, mosq_cs_disconnecting);
	do_disconnect(context, MOSQ_ERR_AUTH);
}
#पूर्ण_अगर


/* Apply security settings after a reload.
 * Includes:
 * - Disconnecting anonymous users if appropriate
 * - Disconnecting users with invalid passwords
 * - Reapplying ACLs
 */
पूर्णांक mosquitto_security_apply_default(व्योम)
{
	काष्ठा mosquitto *context, *ctxt_tmp = शून्य;
	bool allow_anonymous;
#अगर_घोषित WITH_TLS
	X509_NAME *name;
	X509_NAME_ENTRY *name_entry;
	ASN1_STRING *name_asn1 = शून्य;
	काष्ठा mosquitto__listener *listener;
	BIO *subject_bio;
	अक्षर *तथ्य_start;
	माप_प्रकार name_लंबाई;
	अक्षर *subject;
#पूर्ण_अगर

#अगर_घोषित WITH_TLS
	क्रम(पूर्णांक i=0; i<db.config->listener_गिनती; i++){
		listener = &db.config->listeners[i];
		अगर(listener && listener->ssl_ctx && listener->certfile && listener->keyfile && listener->crlfile && listener->require_certificate){
			अगर(net__tls_server_ctx(listener)){
				वापस MOSQ_ERR_TLS;
			}

			अगर(net__tls_load_verify(listener)){
				वापस MOSQ_ERR_TLS;
			}
		}
	}
#पूर्ण_अगर

	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		अगर(context->bridge){
			जारी;
		}

		अगर((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->per_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false)){
			allow_anonymous = true;
		}अन्यथा{
			allow_anonymous = false;
		}

		अगर(!allow_anonymous && !context->username){
			mosquitto__set_state(context, mosq_cs_disconnecting);
			do_disconnect(context, MOSQ_ERR_AUTH);
			जारी;
		}

		/* Check for connected clients that are no longer authorised */
#अगर_घोषित WITH_TLS
		अगर(context->listener && context->listener->ssl_ctx && (context->listener->use_identity_as_username || context->listener->use_subject_as_username)){
			/* Client must have either a valid certificate, or valid PSK used as a username. */
			अगर(!context->ssl){
				अगर(context->protocol == mosq_p_mqtt5){
					send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, शून्य);
				}
				mosquitto__set_state(context, mosq_cs_disconnecting);
				do_disconnect(context, MOSQ_ERR_AUTH);
				जारी;
			}
#अगर_घोषित FINAL_WITH_TLS_PSK
			अगर(context->listener->psk_hint){
				/* Client should have provided an identity to get this far. */
				अगर(!context->username){
					security__disconnect_auth(context);
					जारी;
				}
			}अन्यथा
#पूर्ण_अगर /* FINAL_WITH_TLS_PSK */
			{
				/* Free existing credentials and then recover them. */
				mosquitto_FREE(context->username);
				mosquitto_FREE(context->password);

				X509 *client_cert = SSL_get_peer_certificate(context->ssl);
				अगर(!client_cert){
					security__disconnect_auth(context);
					जारी;
				}
				name = X509_get_subject_name(client_cert);
				अगर(!name){
					X509_free(client_cert);
					security__disconnect_auth(context);
					जारी;
				}
				अगर(context->listener->use_identity_as_username){   /* use_identity_as_username */
					पूर्णांक i = X509_NAME_get_सूचकांक_by_NID(name, NID_commonName, -1);
					अगर(i == -1){
						X509_free(client_cert);
						security__disconnect_auth(context);
						जारी;
					}
					name_entry = X509_NAME_get_entry(name, i);
					अगर(name_entry){
						name_asn1 = X509_NAME_ENTRY_get_तथ्य(name_entry);
						अगर(name_asn1 == शून्य){
							X509_free(client_cert);
							security__disconnect_auth(context);
							जारी;
						}
						स्थिर अक्षर *username = (स्थिर अक्षर *)ASN1_STRING_get0_तथ्य(name_asn1);
						अगर(!username){
							X509_free(client_cert);
							client_cert = शून्य;
							security__disconnect_auth(context);
							जारी;
						}
						context->username = mosquitto_strdup(username);
						अगर(!context->username){
							X509_free(client_cert);
							security__disconnect_auth(context);
							जारी;
						}
						/* Make sure there isn't an embedded NUL character in the CN */
						अगर((माप_प्रकार)ASN1_STRING_लंबाई(name_asn1) != म_माप(context->username)){
							X509_free(client_cert);
							security__disconnect_auth(context);
							जारी;
						}
					}
				}अन्यथा{   /* use_subject_as_username */
					subject_bio = BIO_new(BIO_s_mem());
					X509_NAME_print_ex(subject_bio, X509_get_subject_name(client_cert), 0, XN_FLAG_RFC2253);
					तथ्य_start = शून्य;
					name_लंबाई = (माप_प्रकार)BIO_get_mem_तथ्य(subject_bio, &तथ्य_start);
					subject = mosquitto_malloc(माप(अक्षर)*name_लंबाई+1);
					अगर(!subject){
						BIO_free(subject_bio);
						X509_free(client_cert);
						security__disconnect_auth(context);
						जारी;
					}
					स_नकल(subject, तथ्य_start, name_लंबाई);
					subject[name_लंबाई] = '\0';
					BIO_free(subject_bio);
					context->username = subject;
				}
				अगर(!context->username){
					X509_free(client_cert);
					security__disconnect_auth(context);
					जारी;
				}
				X509_free(client_cert);
			}
		}अन्यथा
#पूर्ण_अगर
		{
			/* Username/password check only if the identity/subject check not used */
			अगर(mosquitto_basic_auth(context) != MOSQ_ERR_SUCCESS){
				mosquitto__set_state(context, mosq_cs_disconnecting);
				do_disconnect(context, MOSQ_ERR_AUTH);
				जारी;
			}
		}
	}

	वापस MOSQ_ERR_SUCCESS;
}
