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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ <ctype.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/mqtt_protocol.h"
#ಅನ್ತರ್ಭಾವ "password_file.h"
#ಅನ್ತರ್ಭಾವ "send_mosq.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"


ಪೂರ್ಣಾಙ್ಕ mosquitto_security_init_default(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಙ್ಕ rc;

	/* Configure plugin identifier */
	ಯದಿ(db.config->per_listener_settings){
		ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<db.config->listener_ಎಣಿಕೆ; i++){
			db.config->listeners[i].security_options->pid = mosquitto_calloc(1, ಪರಿಮಾಣ(mosquitto_plugin_id_t));
			ಯದಿ(db.config->listeners[i].security_options->pid == ರಿಕ್ತ){
				log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
			}
			db.config->listeners[i].security_options->pid->plugin_name = mosquitto_strdup("builtin-security");
			db.config->listeners[i].security_options->pid->listener = &db.config->listeners[i];
			config__plugin_add_secopt(db.config->listeners[i].security_options->pid, db.config->listeners[i].security_options);
		}
	}ಅನ್ಯಥಾ{
		db.config->security_options.pid = mosquitto_calloc(1, ಪರಿಮಾಣ(mosquitto_plugin_id_t));
		ಯದಿ(db.config->security_options.pid == ರಿಕ್ತ){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Out of memory.");
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
		}
		db.config->security_options.pid->plugin_name = mosquitto_strdup("builtin-security");
		config__plugin_add_secopt(db.config->security_options.pid, &db.config->security_options);
	}

	rc = broker_password_file__init();
	ಯದಿ(rc){
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	rc = broker_acl_file__init();
	ಯದಿ(rc){
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	rc = psk_file__init();
	ಯದಿ(rc){
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ mosquitto_security_cleanup_default(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಙ್ಕ rc = 0;

	broker_password_file__cleanup();
	broker_acl_file__cleanup();

	rc = psk_file__cleanup();
	ಯದಿ(rc != MOSQ_ERR_SUCCESS){
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	ಯದಿ(db.config->per_listener_settings){
		ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<db.config->listener_ಎಣಿಕೆ; i++){
			ಯದಿ(db.config->listeners[i].security_options->pid){
				mosquitto_FREE(db.config->listeners[i].security_options->pid->plugin_name);
				mosquitto_FREE(db.config->listeners[i].security_options->pid->config.security_options);
				mosquitto_FREE(db.config->listeners[i].security_options->pid);
			}
		}
	}ಅನ್ಯಥಾ{
		ಯದಿ(db.config->security_options.pid){
			mosquitto_FREE(db.config->security_options.pid->plugin_name);
			mosquitto_FREE(db.config->security_options.pid->config.security_options);
			mosquitto_FREE(db.config->security_options.pid);
		}
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


#ifdef WITH_TLS


ಸ್ಥಿರ ಶೂನ್ಯ security__disconnect_auth(ಸಂರಚನಾ mosquitto *context)
{
	ಯದಿ(context->protocol == mosq_p_mqtt5){
		send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, ರಿಕ್ತ);
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
ಪೂರ್ಣಾಙ್ಕ mosquitto_security_apply_default(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ mosquitto *context, *ctxt_tmp = ರಿಕ್ತ;
	bool allow_anonymous;
#ifdef WITH_TLS
	X509_NAME *name;
	X509_NAME_ENTRY *name_entry;
	ASN1_STRING *name_asn1 = ರಿಕ್ತ;
	ಸಂರಚನಾ mosquitto__listener *listener;
	BIO *subject_bio;
	ವರ್ಣ *data_start;
	size_t name_length;
	ವರ್ಣ *subject;
#endif

#ifdef WITH_TLS
	ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<db.config->listener_ಎಣಿಕೆ; i++){
		listener = &db.config->listeners[i];
		ಯದಿ(listener && listener->ssl_ctx && listener->certfile && listener->keyfile && listener->crlfile && listener->require_certificate){
			ಯದಿ(net__tls_server_ctx(listener)){
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_TLS;
			}

			ಯದಿ(net__tls_load_verify(listener)){
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_TLS;
			}
		}
	}
#endif

	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		ಯದಿ(context->bridge){
			ಅನುವರ್ತ;
		}

		ಯದಿ((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->per_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false)){
			allow_anonymous = true;
		}ಅನ್ಯಥಾ{
			allow_anonymous = false;
		}

		ಯದಿ(!allow_anonymous && !context->username){
			mosquitto__set_state(context, mosq_cs_disconnecting);
			do_disconnect(context, MOSQ_ERR_AUTH);
			ಅನುವರ್ತ;
		}

		/* Check for connected clients that are no longer authorised */
#ifdef WITH_TLS
		ಯದಿ(context->listener && context->listener->ssl_ctx && (context->listener->use_identity_as_username || context->listener->use_subject_as_username)){
			/* Client must have either a valid certificate, or valid PSK used as a username. */
			ಯದಿ(!context->ssl){
				ಯದಿ(context->protocol == mosq_p_mqtt5){
					send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, ರಿಕ್ತ);
				}
				mosquitto__set_state(context, mosq_cs_disconnecting);
				do_disconnect(context, MOSQ_ERR_AUTH);
				ಅನುವರ್ತ;
			}
#ifdef FINAL_WITH_TLS_PSK
			ಯದಿ(context->listener->psk_hint){
				/* Client should have provided an identity to get this far. */
				ಯದಿ(!context->username){
					security__disconnect_auth(context);
					ಅನುವರ್ತ;
				}
			}ಅನ್ಯಥಾ
#endif /* FINAL_WITH_TLS_PSK */
			{
				/* Free existing credentials and then recover them. */
				mosquitto_FREE(context->username);
				mosquitto_FREE(context->password);

				X509 *client_cert = SSL_get_peer_certificate(context->ssl);
				ಯದಿ(!client_cert){
					security__disconnect_auth(context);
					ಅನುವರ್ತ;
				}
				name = X509_get_subject_name(client_cert);
				ಯದಿ(!name){
					X509_free(client_cert);
					security__disconnect_auth(context);
					ಅನುವರ್ತ;
				}
				ಯದಿ(context->listener->use_identity_as_username){   /* use_identity_as_username */
					ಪೂರ್ಣಾಙ್ಕ i = X509_NAME_get_index_by_NID(name, NID_commonName, -1);
					ಯದಿ(i == -1){
						X509_free(client_cert);
						security__disconnect_auth(context);
						ಅನುವರ್ತ;
					}
					name_entry = X509_NAME_get_entry(name, i);
					ಯದಿ(name_entry){
						name_asn1 = X509_NAME_ENTRY_get_data(name_entry);
						ಯದಿ(name_asn1 == ರಿಕ್ತ){
							X509_free(client_cert);
							security__disconnect_auth(context);
							ಅನುವರ್ತ;
						}
						ಧ್ರುವ ವರ್ಣ *username = (ಧ್ರುವ ವರ್ಣ *)ASN1_STRING_get0_data(name_asn1);
						ಯದಿ(!username){
							X509_free(client_cert);
							client_cert = ರಿಕ್ತ;
							security__disconnect_auth(context);
							ಅನುವರ್ತ;
						}
						context->username = mosquitto_strdup(username);
						ಯದಿ(!context->username){
							X509_free(client_cert);
							security__disconnect_auth(context);
							ಅನುವರ್ತ;
						}
						/* Make sure there isn't an embedded NUL character in the CN */
						ಯದಿ((size_t)ASN1_STRING_length(name_asn1) != strlen(context->username)){
							X509_free(client_cert);
							security__disconnect_auth(context);
							ಅನುವರ್ತ;
						}
					}
				}ಅನ್ಯಥಾ{   /* use_subject_as_username */
					subject_bio = BIO_new(BIO_s_mem());
					X509_NAME_print_ex(subject_bio, X509_get_subject_name(client_cert), 0, XN_FLAG_RFC2253);
					data_start = ರಿಕ್ತ;
					name_length = (size_t)BIO_get_mem_data(subject_bio, &data_start);
					subject = mosquitto_malloc(ಪರಿಮಾಣ(ವರ್ಣ)*name_length+1);
					ಯದಿ(!subject){
						BIO_free(subject_bio);
						X509_free(client_cert);
						security__disconnect_auth(context);
						ಅನುವರ್ತ;
					}
					memcpy(subject, data_start, name_length);
					subject[name_length] = '\0';
					BIO_free(subject_bio);
					context->username = subject;
				}
				ಯದಿ(!context->username){
					X509_free(client_cert);
					security__disconnect_auth(context);
					ಅನುವರ್ತ;
				}
				X509_free(client_cert);
			}
		}ಅನ್ಯಥಾ
#endif
		{
			/* Username/password check only if the identity/subject check not used */
			ಯದಿ(mosquitto_basic_auth(context) != MOSQ_ERR_SUCCESS){
				mosquitto__set_state(context, mosq_cs_disconnecting);
				do_disconnect(context, MOSQ_ERR_AUTH);
				ಅನುವರ್ತ;
			}
		}
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}
