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


ಪೂರ್ಣಾಂಕ mosquitto_security_init_default(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಂಕ rc;

	/* Configure plugin identifier */
	ಆದರೆ(db.config->ಪ್ರತಿ_listener_settings){
		ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<db.config->listener_ಎಣಿಕೆ; i++){
			db.config->listeners[i].security_options->pid = mosquitto_calloc(1, ಗಾತ್ರ(mosquitto_plugin_id_t));
			ಆದರೆ(db.config->listeners[i].security_options->pid == ಖಾಲಿ){
				log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Out of memory.");
				ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
			}
			db.config->listeners[i].security_options->pid->plugin_name = mosquitto_strdup("builtin-security");
			db.config->listeners[i].security_options->pid->listener = &db.config->listeners[i];
			config_plugin_ಕೂಡಿಸು_secopt(db.config->listeners[i].security_options->pid, db.config->listeners[i].security_options);
		}
	}ಇಲ್ಲದಿದ್ದರೆ{
		db.config->security_options.pid = mosquitto_calloc(1, ಗಾತ್ರ(mosquitto_plugin_id_t));
		ಆದರೆ(db.config->security_options.pid == ಖಾಲಿ){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Out of memory.");
			ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
		}
		db.config->security_options.pid->plugin_name = mosquitto_strdup("builtin-security");
		config_plugin_ಕೂಡಿಸು_secopt(db.config->security_options.pid, &db.config->security_options);
	}

	rc = broker_password_file__init();
	ಆದರೆ(rc){
		ಹಿಂತಿರುಗಿ rc;
	}

	rc = broker_acl_file__init();
	ಆದರೆ(rc){
		ಹಿಂತಿರುಗಿ rc;
	}

	rc = psk_file__init();
	ಆದರೆ(rc){
		ಹಿಂತಿರುಗಿ rc;
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ mosquitto_security_cleanup_default(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಂಕ rc = 0;

	broker_password_file__cleanup();
	broker_acl_file__cleanup();

	rc = psk_file__cleanup();
	ಆದರೆ(rc != MOSQ_ERR_SUCCESS){
		ಹಿಂತಿರುಗಿ rc;
	}

	ಆದರೆ(db.config->ಪ್ರತಿ_listener_settings){
		ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<db.config->listener_ಎಣಿಕೆ; i++){
			ಆದರೆ(db.config->listeners[i].security_options->pid){
				mosquitto_FREE(db.config->listeners[i].security_options->pid->plugin_name);
				mosquitto_FREE(db.config->listeners[i].security_options->pid->config.security_options);
				mosquitto_FREE(db.config->listeners[i].security_options->pid);
			}
		}
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಆದರೆ(db.config->security_options.pid){
			mosquitto_FREE(db.config->security_options.pid->plugin_name);
			mosquitto_FREE(db.config->security_options.pid->config.security_options);
			mosquitto_FREE(db.config->security_options.pid);
		}
	}
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS


ಸ್ಥಿರ ಶೂನ್ಯ security__disconnect_auth(ರಚನೆ mosquitto *context)
{
	ಆದರೆ(context->protocol == mosq_p_mqtt5){
		send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, ಖಾಲಿ);
	}
	mosquitto_ಗಣ_state(context, mosq_cs_disconnecting);
	do_disconnect(context, MOSQ_ERR_AUTH);
}
#ಆದರೆ_ಅಂತ್ಯ


/* Apply security settings after a reload.
 * Includes:
 * - Disconnecting anonymous users if appropriate
 * - Disconnecting users with invalid passwords
 * - Reapplying ACLs
 */
ಪೂರ್ಣಾಂಕ mosquitto_security_apply_default(ಶೂನ್ಯ)
{
	ರಚನೆ mosquitto *context, *ctxt_tmp = ಖಾಲಿ;
	bool allow_anonymous;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
	X509_NAME *name;
	X509_NAME_ನಮೂದು *name_ನಮೂದು;
	ASN1_STRING *name_asn1 = ಖಾಲಿ;
	ರಚನೆ mosquitto__listener *listener;
	BIO *subject_bio;
	ಅಕ್ಷರ *ದತ್ತಾಂಶ_ಆರಂಭ;
	size_t name_ಉದ್ದಳತೆ;
	ಅಕ್ಷರ *subject;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
	ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<db.config->listener_ಎಣಿಕೆ; i++){
		listener = &db.config->listeners[i];
		ಆದರೆ(listener && listener->ssl_ctx && listener->certfile && listener->keyfile && listener->crlfile && listener->require_certificate){
			ಆದರೆ(net__tls_server_ctx(listener)){
				ಹಿಂತಿರುಗಿ MOSQ_ERR_TLS;
			}

			ಆದರೆ(net_tls_ತುಂಬು_verify(listener)){
				ಹಿಂತಿರುಗಿ MOSQ_ERR_TLS;
			}
		}
	}
#ಆದರೆ_ಅಂತ್ಯ

	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		ಆದರೆ(context->bridge){
			ಮುಂದುವರಿ;
		}

		ಆದರೆ((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->ಪ್ರತಿ_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false)){
			allow_anonymous = true;
		}ಇಲ್ಲದಿದ್ದರೆ{
			allow_anonymous = false;
		}

		ಆದರೆ(!allow_anonymous && !context->username){
			mosquitto_ಗಣ_state(context, mosq_cs_disconnecting);
			do_disconnect(context, MOSQ_ERR_AUTH);
			ಮುಂದುವರಿ;
		}

		/* Check for connected clients that are no longer authorised */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
		ಆದರೆ(context->listener && context->listener->ssl_ctx && (context->listener->use_ತಟಸ್ಥ_as_username || context->listener->use_subject_as_username)){
			/* Client must have either a valid certificate, or valid PSK used as a username. */
			ಆದರೆ(!context->ssl){
				ಆದರೆ(context->protocol == mosq_p_mqtt5){
					send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, ಖಾಲಿ);
				}
				mosquitto_ಗಣ_state(context, mosq_cs_disconnecting);
				do_disconnect(context, MOSQ_ERR_AUTH);
				ಮುಂದುವರಿ;
			}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FINAL_WITH_TLS_PSK
			ಆದರೆ(context->listener->psk_hint){
				/* Client should have provided an identity to get this far. */
				ಆದರೆ(!context->username){
					security__disconnect_auth(context);
					ಮುಂದುವರಿ;
				}
			}ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ /* FINAL_WITH_TLS_PSK */
			{
				/* Free existing credentials and then recover them. */
				mosquitto_FREE(context->username);
				mosquitto_FREE(context->password);

				X509 *client_cert = SSL_get_peer_certificate(context->ssl);
				ಆದರೆ(!client_cert){
					security__disconnect_auth(context);
					ಮುಂದುವರಿ;
				}
				name = X509_get_subject_name(client_cert);
				ಆದರೆ(!name){
					X509_free(client_cert);
					security__disconnect_auth(context);
					ಮುಂದುವರಿ;
				}
				ಆದರೆ(context->listener->use_ತಟಸ್ಥ_as_username){   /* use_identity_as_username */
					ಪೂರ್ಣಾಂಕ i = X509_NAME_get_ಸೂಚ್ಯಂಕ_by_NID(name, NID_commonName, -1);
					ಆದರೆ(i == -1){
						X509_free(client_cert);
						security__disconnect_auth(context);
						ಮುಂದುವರಿ;
					}
					name_ನಮೂದು = X509_NAME_get_ನಮೂದು(name, i);
					ಆದರೆ(name_ನಮೂದು){
						name_asn1 = X509_NAME_ನಮೂದು_get_ದತ್ತಾಂಶ(name_ನಮೂದು);
						ಆದರೆ(name_asn1 == ಖಾಲಿ){
							X509_free(client_cert);
							security__disconnect_auth(context);
							ಮುಂದುವರಿ;
						}
						ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *username = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ASN1_STRING_get0_ದತ್ತಾಂಶ(name_asn1);
						ಆದರೆ(!username){
							X509_free(client_cert);
							client_cert = ಖಾಲಿ;
							security__disconnect_auth(context);
							ಮುಂದುವರಿ;
						}
						context->username = mosquitto_strdup(username);
						ಆದರೆ(!context->username){
							X509_free(client_cert);
							security__disconnect_auth(context);
							ಮುಂದುವರಿ;
						}
						/* Make sure there isn't an embedded NUL character in the CN */
						ಆದರೆ((size_t)ASN1_STRING_ಉದ್ದಳತೆ(name_asn1) != ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(context->username)){
							X509_free(client_cert);
							security__disconnect_auth(context);
							ಮುಂದುವರಿ;
						}
					}
				}ಇಲ್ಲದಿದ್ದರೆ{   /* use_subject_as_username */
					subject_bio = BIO_new(BIO_s_mem());
					X509_NAME_print_ex(subject_bio, X509_get_subject_name(client_cert), 0, XN_FLAG_RFC2253);
					ದತ್ತಾಂಶ_ಆರಂಭ = ಖಾಲಿ;
					name_ಉದ್ದಳತೆ = (size_t)BIO_get_mem_ದತ್ತಾಂಶ(subject_bio, &ದತ್ತಾಂಶ_ಆರಂಭ);
					subject = mosquitto_malloc(ಗಾತ್ರ(ಅಕ್ಷರ)*name_ಉದ್ದಳತೆ+1);
					ಆದರೆ(!subject){
						BIO_free(subject_bio);
						X509_free(client_cert);
						security__disconnect_auth(context);
						ಮುಂದುವರಿ;
					}
					ಸ್ಮರಣೆ_ನಕಲು(subject, ದತ್ತಾಂಶ_ಆರಂಭ, name_ಉದ್ದಳತೆ);
					subject[name_ಉದ್ದಳತೆ] = '\0';
					BIO_free(subject_bio);
					context->username = subject;
				}
				ಆದರೆ(!context->username){
					X509_free(client_cert);
					security__disconnect_auth(context);
					ಮುಂದುವರಿ;
				}
				X509_free(client_cert);
			}
		}ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		{
			/* Username/password check only if the identity/subject check not used */
			ಆದರೆ(mosquitto_basic_auth(context) != MOSQ_ERR_SUCCESS){
				mosquitto_ಗಣ_state(context, mosq_cs_disconnecting);
				do_disconnect(context, MOSQ_ERR_AUTH);
				ಮುಂದುವರಿ;
			}
		}
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}
