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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <ctype.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/mqtt_protocol.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "password_file.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "send_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util_mosq.h"


ਪੂਰਨ mosquitto_security_init_default(ਖਾਲੀ)
{
	ਪੂਰਨ rc;

	/* Configure plugin identifier */
	ਜੇ(db.config->ਪ੍ਰਤੀ_listener_settings){
		ਲਈ(ਪੂਰਨ i=0; i<db.config->listener_ਗਿਣਤੀ; i++){
			db.config->listeners[i].security_options->pid = mosquitto_calloc(1, ਆਕਾਰ(mosquitto_plugin_id_t));
			ਜੇ(db.config->listeners[i].security_options->pid == ਨਿਰਾ){
				log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
				ਵਾਪਸ MOSQ_ERR_NOMEM;
			}
			db.config->listeners[i].security_options->pid->plugin_name = mosquitto_strdup("builtin-security");
			db.config->listeners[i].security_options->pid->listener = &db.config->listeners[i];
			config_plugin_ਜੋੜੋ_secopt(db.config->listeners[i].security_options->pid, db.config->listeners[i].security_options);
		}
	}ਨਹੀਂਤਾਂ{
		db.config->security_options.pid = mosquitto_calloc(1, ਆਕਾਰ(mosquitto_plugin_id_t));
		ਜੇ(db.config->security_options.pid == ਨਿਰਾ){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Out of memory.");
			ਵਾਪਸ MOSQ_ERR_NOMEM;
		}
		db.config->security_options.pid->plugin_name = mosquitto_strdup("builtin-security");
		config_plugin_ਜੋੜੋ_secopt(db.config->security_options.pid, &db.config->security_options);
	}

	rc = broker_password_file__init();
	ਜੇ(rc){
		ਵਾਪਸ rc;
	}

	rc = broker_acl_file__init();
	ਜੇ(rc){
		ਵਾਪਸ rc;
	}

	rc = psk_file__init();
	ਜੇ(rc){
		ਵਾਪਸ rc;
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ mosquitto_security_cleanup_default(ਖਾਲੀ)
{
	ਪੂਰਨ rc = 0;

	broker_password_file__cleanup();
	broker_acl_file__cleanup();

	rc = psk_file__cleanup();
	ਜੇ(rc != MOSQ_ERR_SUCCESS){
		ਵਾਪਸ rc;
	}

	ਜੇ(db.config->ਪ੍ਰਤੀ_listener_settings){
		ਲਈ(ਪੂਰਨ i=0; i<db.config->listener_ਗਿਣਤੀ; i++){
			ਜੇ(db.config->listeners[i].security_options->pid){
				mosquitto_FREE(db.config->listeners[i].security_options->pid->plugin_name);
				mosquitto_FREE(db.config->listeners[i].security_options->pid->config.security_options);
				mosquitto_FREE(db.config->listeners[i].security_options->pid);
			}
		}
	}ਨਹੀਂਤਾਂ{
		ਜੇ(db.config->security_options.pid){
			mosquitto_FREE(db.config->security_options.pid->plugin_name);
			mosquitto_FREE(db.config->security_options.pid->config.security_options);
			mosquitto_FREE(db.config->security_options.pid);
		}
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS


ਸਥਿਰ ਖਾਲੀ security__disconnect_auth(ਬਣਤਰ mosquitto *context)
{
	ਜੇ(context->protocol == mosq_p_mqtt5){
		send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, ਨਿਰਾ);
	}
	mosquitto_ਸੈੱਟ_state(context, mosq_cs_disconnecting);
	do_disconnect(context, MOSQ_ERR_AUTH);
}
#ਜੇ_ਅੰਤ


/* Apply security settings after a reload.
 * Includes:
 * - Disconnecting anonymous users if appropriate
 * - Disconnecting users with invalid passwords
 * - Reapplying ACLs
 */
ਪੂਰਨ mosquitto_security_apply_default(ਖਾਲੀ)
{
	ਬਣਤਰ mosquitto *context, *ctxt_tmp = ਨਿਰਾ;
	bool allow_anonymous;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
	X509_NAME *name;
	X509_NAME_ਇੰਦਰਾਜ *name_ਇੰਦਰਾਜ;
	ASN1_STRING *name_asn1 = ਨਿਰਾ;
	ਬਣਤਰ mosquitto__listener *listener;
	BIO *subject_bio;
	ਅੱਖਰ *ਅੰਕੜੇ_ਸ਼ੁਰੂ;
	size_t name_ਲੰਮਾਈ;
	ਅੱਖਰ *subject;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
	ਲਈ(ਪੂਰਨ i=0; i<db.config->listener_ਗਿਣਤੀ; i++){
		listener = &db.config->listeners[i];
		ਜੇ(listener && listener->ssl_ctx && listener->certfile && listener->keyfile && listener->crlfile && listener->require_certificate){
			ਜੇ(net__tls_server_ctx(listener)){
				ਵਾਪਸ MOSQ_ERR_TLS;
			}

			ਜੇ(net_tls_ਲੱਦੋ_verify(listener)){
				ਵਾਪਸ MOSQ_ERR_TLS;
			}
		}
	}
#ਜੇ_ਅੰਤ

	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		ਜੇ(context->bridge){
			ਜਾਰੀ;
		}

		ਜੇ((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->ਪ੍ਰਤੀ_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false)){
			allow_anonymous = true;
		}ਨਹੀਂਤਾਂ{
			allow_anonymous = false;
		}

		ਜੇ(!allow_anonymous && !context->username){
			mosquitto_ਸੈੱਟ_state(context, mosq_cs_disconnecting);
			do_disconnect(context, MOSQ_ERR_AUTH);
			ਜਾਰੀ;
		}

		/* Check for connected clients that are no longer authorised */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
		ਜੇ(context->listener && context->listener->ssl_ctx && (context->listener->use_ਪਛਾਣ_ਤੱਤ_as_username || context->listener->use_subject_as_username)){
			/* Client must have either a valid certificate, or valid PSK used as a username. */
			ਜੇ(!context->ssl){
				ਜੇ(context->protocol == mosq_p_mqtt5){
					send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, ਨਿਰਾ);
				}
				mosquitto_ਸੈੱਟ_state(context, mosq_cs_disconnecting);
				do_disconnect(context, MOSQ_ERR_AUTH);
				ਜਾਰੀ;
			}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FINAL_WITH_TLS_PSK
			ਜੇ(context->listener->psk_hint){
				/* Client should have provided an identity to get this far. */
				ਜੇ(!context->username){
					security__disconnect_auth(context);
					ਜਾਰੀ;
				}
			}ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ /* FINAL_WITH_TLS_PSK */
			{
				/* Free existing credentials and then recover them. */
				mosquitto_FREE(context->username);
				mosquitto_FREE(context->password);

				X509 *client_cert = SSL_get_peer_certificate(context->ssl);
				ਜੇ(!client_cert){
					security__disconnect_auth(context);
					ਜਾਰੀ;
				}
				name = X509_get_subject_name(client_cert);
				ਜੇ(!name){
					X509_free(client_cert);
					security__disconnect_auth(context);
					ਜਾਰੀ;
				}
				ਜੇ(context->listener->use_ਪਛਾਣ_ਤੱਤ_as_username){   /* use_identity_as_username */
					ਪੂਰਨ i = X509_NAME_get_ਸੂਚਕਾਂਕ_by_NID(name, NID_commonName, -1);
					ਜੇ(i == -1){
						X509_free(client_cert);
						security__disconnect_auth(context);
						ਜਾਰੀ;
					}
					name_ਇੰਦਰਾਜ = X509_NAME_get_ਇੰਦਰਾਜ(name, i);
					ਜੇ(name_ਇੰਦਰਾਜ){
						name_asn1 = X509_NAME_ਇੰਦਰਾਜ_get_ਅੰਕੜੇ(name_ਇੰਦਰਾਜ);
						ਜੇ(name_asn1 == ਨਿਰਾ){
							X509_free(client_cert);
							security__disconnect_auth(context);
							ਜਾਰੀ;
						}
						ਅਚਲ ਅੱਖਰ *username = (ਅਚਲ ਅੱਖਰ *)ASN1_STRING_get0_ਅੰਕੜੇ(name_asn1);
						ਜੇ(!username){
							X509_free(client_cert);
							client_cert = ਨਿਰਾ;
							security__disconnect_auth(context);
							ਜਾਰੀ;
						}
						context->username = mosquitto_strdup(username);
						ਜੇ(!context->username){
							X509_free(client_cert);
							security__disconnect_auth(context);
							ਜਾਰੀ;
						}
						/* Make sure there isn't an embedded NUL character in the CN */
						ਜੇ((size_t)ASN1_STRING_ਲੰਮਾਈ(name_asn1) != ਲੜੀ_ਲੰਬਾਈ(context->username)){
							X509_free(client_cert);
							security__disconnect_auth(context);
							ਜਾਰੀ;
						}
					}
				}ਨਹੀਂਤਾਂ{   /* use_subject_as_username */
					subject_bio = BIO_new(BIO_s_mem());
					X509_NAME_print_ex(subject_bio, X509_get_subject_name(client_cert), 0, XN_FLAG_RFC2253);
					ਅੰਕੜੇ_ਸ਼ੁਰੂ = ਨਿਰਾ;
					name_ਲੰਮਾਈ = (size_t)BIO_get_mem_ਅੰਕੜੇ(subject_bio, &ਅੰਕੜੇ_ਸ਼ੁਰੂ);
					subject = mosquitto_malloc(ਆਕਾਰ(ਅੱਖਰ)*name_ਲੰਮਾਈ+1);
					ਜੇ(!subject){
						BIO_free(subject_bio);
						X509_free(client_cert);
						security__disconnect_auth(context);
						ਜਾਰੀ;
					}
					ਸਮ੍ਰਿਤੀ_ਨਕਲ(subject, ਅੰਕੜੇ_ਸ਼ੁਰੂ, name_ਲੰਮਾਈ);
					subject[name_ਲੰਮਾਈ] = '\0';
					BIO_free(subject_bio);
					context->username = subject;
				}
				ਜੇ(!context->username){
					X509_free(client_cert);
					security__disconnect_auth(context);
					ਜਾਰੀ;
				}
				X509_free(client_cert);
			}
		}ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		{
			/* Username/password check only if the identity/subject check not used */
			ਜੇ(mosquitto_basic_auth(context) != MOSQ_ERR_SUCCESS){
				mosquitto_ਸੈੱਟ_state(context, mosq_cs_disconnecting);
				do_disconnect(context, MOSQ_ERR_AUTH);
				ਜਾਰੀ;
			}
		}
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}
