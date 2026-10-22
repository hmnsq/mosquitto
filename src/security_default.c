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
#अन्तर्भाव "password_file.h"
#अन्तर्भाव "send_mosq.h"
#अन्तर्भाव "util_mosq.h"


पूर्णांक mosquitto_security_init_default(शून्य)
{
	पूर्णांक rc;

	/* Configure plugin identifier */
	जर(db.config->per_listener_settings){
		साठी(पूर्णांक i=0; i<db.config->listener_मोजणी; i++){
			db.config->listeners[i].security_options->pid = mosquitto_calloc(1, आकार(mosquitto_plugin_id_t));
			जर(db.config->listeners[i].security_options->pid == रिक्त){
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
				परत MOSQ_ERR_NOMEM;
			}
			db.config->listeners[i].security_options->pid->plugin_name = mosquitto_strdup("builtin-security");
			db.config->listeners[i].security_options->pid->listener = &db.config->listeners[i];
			config__plugin_add_secopt(db.config->listeners[i].security_options->pid, db.config->listeners[i].security_options);
		}
	}अन्यथा{
		db.config->security_options.pid = mosquitto_calloc(1, आकार(mosquitto_plugin_id_t));
		जर(db.config->security_options.pid == रिक्त){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
			परत MOSQ_ERR_NOMEM;
		}
		db.config->security_options.pid->plugin_name = mosquitto_strdup("builtin-security");
		config__plugin_add_secopt(db.config->security_options.pid, &db.config->security_options);
	}

	rc = broker_password_file__init();
	जर(rc){
		परत rc;
	}

	rc = broker_acl_file__init();
	जर(rc){
		परत rc;
	}

	rc = psk_file__init();
	जर(rc){
		परत rc;
	}

	परत MOSQ_ERR_SUCCESS;
}


पूर्णांक mosquitto_security_cleanup_default(शून्य)
{
	पूर्णांक rc = 0;

	broker_password_file__cleanup();
	broker_acl_file__cleanup();

	rc = psk_file__cleanup();
	जर(rc != MOSQ_ERR_SUCCESS){
		परत rc;
	}

	जर(db.config->per_listener_settings){
		साठी(पूर्णांक i=0; i<db.config->listener_मोजणी; i++){
			जर(db.config->listeners[i].security_options->pid){
				mosquitto_FREE(db.config->listeners[i].security_options->pid->plugin_name);
				mosquitto_FREE(db.config->listeners[i].security_options->pid->config.security_options);
				mosquitto_FREE(db.config->listeners[i].security_options->pid);
			}
		}
	}अन्यथा{
		जर(db.config->security_options.pid){
			mosquitto_FREE(db.config->security_options.pid->plugin_name);
			mosquitto_FREE(db.config->security_options.pid->config.security_options);
			mosquitto_FREE(db.config->security_options.pid);
		}
	}
	परत MOSQ_ERR_SUCCESS;
}


#ifdef WITH_TLS


स्थिर शून्य security__disconnect_auth(संरचना mosquitto *context)
{
	जर(context->protocol == mosq_p_mqtt5){
		send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, रिक्त);
	}
	mosquitto__set_state(context, mosq_cs_disconnecting);
	do_disconnect(context, MOSQ_ERR_AUTH);
}
#endif


/* Apply security settings after a reload.
 * Includes:
 * - Disconnecting anonymous users if appropriate
 * - Disconnecting users with invalid passwords
 * - Reapplying ACLs
 */
पूर्णांक mosquitto_security_apply_default(शून्य)
{
	संरचना mosquitto *context, *ctxt_tmp = रिक्त;
	bool allow_anonymous;
#ifdef WITH_TLS
	X509_NAME *name;
	X509_NAME_ENTRY *name_entry;
	ASN1_STRING *name_asn1 = रिक्त;
	संरचना mosquitto__listener *listener;
	BIO *subject_bio;
	अक्षर *data_start;
	माप_प्रकार name_length;
	अक्षर *subject;
#endif

#ifdef WITH_TLS
	साठी(पूर्णांक i=0; i<db.config->listener_मोजणी; i++){
		listener = &db.config->listeners[i];
		जर(listener && listener->ssl_ctx && listener->certfile && listener->keyfile && listener->crlfile && listener->require_certificate){
			जर(net__tls_server_ctx(listener)){
				परत MOSQ_ERR_TLS;
			}

			जर(net__tls_load_verify(listener)){
				परत MOSQ_ERR_TLS;
			}
		}
	}
#endif

	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		जर(context->bridge){
			सुरूठेवा;
		}

		जर((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->per_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false)){
			allow_anonymous = true;
		}अन्यथा{
			allow_anonymous = false;
		}

		जर(!allow_anonymous && !context->username){
			mosquitto__set_state(context, mosq_cs_disconnecting);
			do_disconnect(context, MOSQ_ERR_AUTH);
			सुरूठेवा;
		}

		/* Check for connected clients that are no longer authorised */
#ifdef WITH_TLS
		जर(context->listener && context->listener->ssl_ctx && (context->listener->use_identity_as_username || context->listener->use_subject_as_username)){
			/* Client must have either a valid certificate, or valid PSK used as a username. */
			जर(!context->ssl){
				जर(context->protocol == mosq_p_mqtt5){
					send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, रिक्त);
				}
				mosquitto__set_state(context, mosq_cs_disconnecting);
				do_disconnect(context, MOSQ_ERR_AUTH);
				सुरूठेवा;
			}
#ifdef FINAL_WITH_TLS_PSK
			जर(context->listener->psk_hint){
				/* Client should have provided an identity to get this far. */
				जर(!context->username){
					security__disconnect_auth(context);
					सुरूठेवा;
				}
			}अन्यथा
#endif /* FINAL_WITH_TLS_PSK */
			{
				/* Free existing credentials and then recover them. */
				mosquitto_FREE(context->username);
				mosquitto_FREE(context->password);

				X509 *client_cert = SSL_get_peer_certificate(context->ssl);
				जर(!client_cert){
					security__disconnect_auth(context);
					सुरूठेवा;
				}
				name = X509_get_subject_name(client_cert);
				जर(!name){
					X509_free(client_cert);
					security__disconnect_auth(context);
					सुरूठेवा;
				}
				जर(context->listener->use_identity_as_username){   /* use_identity_as_username */
					पूर्णांक i = X509_NAME_get_index_by_NID(name, NID_commonName, -1);
					जर(i == -1){
						X509_free(client_cert);
						security__disconnect_auth(context);
						सुरूठेवा;
					}
					name_entry = X509_NAME_get_entry(name, i);
					जर(name_entry){
						name_asn1 = X509_NAME_ENTRY_get_data(name_entry);
						जर(name_asn1 == रिक्त){
							X509_free(client_cert);
							security__disconnect_auth(context);
							सुरूठेवा;
						}
						स्थिरांक अक्षर *username = (स्थिरांक अक्षर *)ASN1_STRING_get0_data(name_asn1);
						जर(!username){
							X509_free(client_cert);
							client_cert = रिक्त;
							security__disconnect_auth(context);
							सुरूठेवा;
						}
						context->username = mosquitto_strdup(username);
						जर(!context->username){
							X509_free(client_cert);
							security__disconnect_auth(context);
							सुरूठेवा;
						}
						/* Make sure there isn't an embedded NUL character in the CN */
						जर((माप_प्रकार)ASN1_STRING_length(name_asn1) != म_माप(context->username)){
							X509_free(client_cert);
							security__disconnect_auth(context);
							सुरूठेवा;
						}
					}
				}अन्यथा{   /* use_subject_as_username */
					subject_bio = BIO_new(BIO_s_mem());
					X509_NAME_print_ex(subject_bio, X509_get_subject_name(client_cert), 0, XN_FLAG_RFC2253);
					data_start = रिक्त;
					name_length = (माप_प्रकार)BIO_get_mem_data(subject_bio, &data_start);
					subject = mosquitto_malloc(आकार(अक्षर)*name_length+1);
					जर(!subject){
						BIO_free(subject_bio);
						X509_free(client_cert);
						security__disconnect_auth(context);
						सुरूठेवा;
					}
					स_नकल(subject, data_start, name_length);
					subject[name_length] = '\0';
					BIO_free(subject_bio);
					context->username = subject;
				}
				जर(!context->username){
					X509_free(client_cert);
					security__disconnect_auth(context);
					सुरूठेवा;
				}
				X509_free(client_cert);
			}
		}अन्यथा
#endif
		{
			/* Username/password check only if the identity/subject check not used */
			जर(mosquitto_basic_auth(context) != MOSQ_ERR_SUCCESS){
				mosquitto__set_state(context, mosq_cs_disconnecting);
				do_disconnect(context, MOSQ_ERR_AUTH);
				सुरूठेवा;
			}
		}
	}

	परत MOSQ_ERR_SUCCESS;
}
