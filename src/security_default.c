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

#அந்தர்பாவ "config.h"

#அந்தர்பாவ <ctype.h>
#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <string.h>

#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "mosquitto/mqtt_protocol.h"
#அந்தர்பாவ "password_file.h"
#அந்தர்பாவ "send_mosq.h"
#அந்தர்பாவ "util_mosq.h"


முழு mosquitto_security_init_default(வெறுமை)
{
	முழு rc;

	/* Configure plugin identifier */
	எனில்(db.config->per_listener_settings){
		க்கு(முழு i=0; i<db.config->listener_மொத்தம்; i++){
			db.config->listeners[i].security_options->pid = mosquitto_calloc(1, அளவு(mosquitto_plugin_id_t));
			எனில்(db.config->listeners[i].security_options->pid == வெற்று){
				log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
				திரும்பு MOSQ_ERR_NOMEM;
			}
			db.config->listeners[i].security_options->pid->plugin_name = mosquitto_strdup("builtin-security");
			db.config->listeners[i].security_options->pid->listener = &db.config->listeners[i];
			config__plugin_add_secopt(db.config->listeners[i].security_options->pid, db.config->listeners[i].security_options);
		}
	}இல்லையெனில்{
		db.config->security_options.pid = mosquitto_calloc(1, அளவு(mosquitto_plugin_id_t));
		எனில்(db.config->security_options.pid == வெற்று){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Out of memory.");
			திரும்பு MOSQ_ERR_NOMEM;
		}
		db.config->security_options.pid->plugin_name = mosquitto_strdup("builtin-security");
		config__plugin_add_secopt(db.config->security_options.pid, &db.config->security_options);
	}

	rc = broker_password_file__init();
	எனில்(rc){
		திரும்பு rc;
	}

	rc = broker_acl_file__init();
	எனில்(rc){
		திரும்பு rc;
	}

	rc = psk_file__init();
	எனில்(rc){
		திரும்பு rc;
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு mosquitto_security_cleanup_default(வெறுமை)
{
	முழு rc = 0;

	broker_password_file__cleanup();
	broker_acl_file__cleanup();

	rc = psk_file__cleanup();
	எனில்(rc != MOSQ_ERR_SUCCESS){
		திரும்பு rc;
	}

	எனில்(db.config->per_listener_settings){
		க்கு(முழு i=0; i<db.config->listener_மொத்தம்; i++){
			எனில்(db.config->listeners[i].security_options->pid){
				mosquitto_FREE(db.config->listeners[i].security_options->pid->plugin_name);
				mosquitto_FREE(db.config->listeners[i].security_options->pid->config.security_options);
				mosquitto_FREE(db.config->listeners[i].security_options->pid);
			}
		}
	}இல்லையெனில்{
		எனில்(db.config->security_options.pid){
			mosquitto_FREE(db.config->security_options.pid->plugin_name);
			mosquitto_FREE(db.config->security_options.pid->config.security_options);
			mosquitto_FREE(db.config->security_options.pid);
		}
	}
	திரும்பு MOSQ_ERR_SUCCESS;
}


#ifdef WITH_TLS


நிலையான வெறுமை security__disconnect_auth(கட்டமைப்பு mosquitto *context)
{
	எனில்(context->protocol == mosq_p_mqtt5){
		send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, வெற்று);
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
முழு mosquitto_security_apply_default(வெறுமை)
{
	கட்டமைப்பு mosquitto *context, *ctxt_tmp = வெற்று;
	bool allow_anonymous;
#ifdef WITH_TLS
	X509_NAME *name;
	X509_NAME_ENTRY *name_entry;
	ASN1_STRING *name_asn1 = வெற்று;
	கட்டமைப்பு mosquitto__listener *listener;
	BIO *subject_bio;
	எழுத்து *data_start;
	size_t name_length;
	எழுத்து *subject;
#endif

#ifdef WITH_TLS
	க்கு(முழு i=0; i<db.config->listener_மொத்தம்; i++){
		listener = &db.config->listeners[i];
		எனில்(listener && listener->ssl_ctx && listener->certfile && listener->keyfile && listener->crlfile && listener->require_certificate){
			எனில்(net__tls_server_ctx(listener)){
				திரும்பு MOSQ_ERR_TLS;
			}

			எனில்(net__tls_load_verify(listener)){
				திரும்பு MOSQ_ERR_TLS;
			}
		}
	}
#endif

	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		எனில்(context->bridge){
			தொடர்;
		}

		எனில்((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->per_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false)){
			allow_anonymous = true;
		}இல்லையெனில்{
			allow_anonymous = false;
		}

		எனில்(!allow_anonymous && !context->username){
			mosquitto__set_state(context, mosq_cs_disconnecting);
			do_disconnect(context, MOSQ_ERR_AUTH);
			தொடர்;
		}

		/* Check for connected clients that are no longer authorised */
#ifdef WITH_TLS
		எனில்(context->listener && context->listener->ssl_ctx && (context->listener->use_identity_as_username || context->listener->use_subject_as_username)){
			/* Client must have either a valid certificate, or valid PSK used as a username. */
			எனில்(!context->ssl){
				எனில்(context->protocol == mosq_p_mqtt5){
					send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, வெற்று);
				}
				mosquitto__set_state(context, mosq_cs_disconnecting);
				do_disconnect(context, MOSQ_ERR_AUTH);
				தொடர்;
			}
#ifdef FINAL_WITH_TLS_PSK
			எனில்(context->listener->psk_hint){
				/* Client should have provided an identity to get this far. */
				எனில்(!context->username){
					security__disconnect_auth(context);
					தொடர்;
				}
			}இல்லையெனில்
#endif /* FINAL_WITH_TLS_PSK */
			{
				/* Free existing credentials and then recover them. */
				mosquitto_FREE(context->username);
				mosquitto_FREE(context->password);

				X509 *client_cert = SSL_get_peer_certificate(context->ssl);
				எனில்(!client_cert){
					security__disconnect_auth(context);
					தொடர்;
				}
				name = X509_get_subject_name(client_cert);
				எனில்(!name){
					X509_free(client_cert);
					security__disconnect_auth(context);
					தொடர்;
				}
				எனில்(context->listener->use_identity_as_username){   /* use_identity_as_username */
					முழு i = X509_NAME_get_index_by_NID(name, NID_commonName, -1);
					எனில்(i == -1){
						X509_free(client_cert);
						security__disconnect_auth(context);
						தொடர்;
					}
					name_entry = X509_NAME_get_entry(name, i);
					எனில்(name_entry){
						name_asn1 = X509_NAME_ENTRY_get_data(name_entry);
						எனில்(name_asn1 == வெற்று){
							X509_free(client_cert);
							security__disconnect_auth(context);
							தொடர்;
						}
						மாறிலி எழுத்து *username = (மாறிலி எழுத்து *)ASN1_STRING_get0_data(name_asn1);
						எனில்(!username){
							X509_free(client_cert);
							client_cert = வெற்று;
							security__disconnect_auth(context);
							தொடர்;
						}
						context->username = mosquitto_strdup(username);
						எனில்(!context->username){
							X509_free(client_cert);
							security__disconnect_auth(context);
							தொடர்;
						}
						/* Make sure there isn't an embedded NUL character in the CN */
						எனில்((size_t)ASN1_STRING_length(name_asn1) != strlen(context->username)){
							X509_free(client_cert);
							security__disconnect_auth(context);
							தொடர்;
						}
					}
				}இல்லையெனில்{   /* use_subject_as_username */
					subject_bio = BIO_new(BIO_s_mem());
					X509_NAME_print_ex(subject_bio, X509_get_subject_name(client_cert), 0, XN_FLAG_RFC2253);
					data_start = வெற்று;
					name_length = (size_t)BIO_get_mem_data(subject_bio, &data_start);
					subject = mosquitto_malloc(அளவு(எழுத்து)*name_length+1);
					எனில்(!subject){
						BIO_free(subject_bio);
						X509_free(client_cert);
						security__disconnect_auth(context);
						தொடர்;
					}
					memcpy(subject, data_start, name_length);
					subject[name_length] = '\0';
					BIO_free(subject_bio);
					context->username = subject;
				}
				எனில்(!context->username){
					X509_free(client_cert);
					security__disconnect_auth(context);
					தொடர்;
				}
				X509_free(client_cert);
			}
		}இல்லையெனில்
#endif
		{
			/* Username/password check only if the identity/subject check not used */
			எனில்(mosquitto_basic_auth(context) != MOSQ_ERR_SUCCESS){
				mosquitto__set_state(context, mosq_cs_disconnecting);
				do_disconnect(context, MOSQ_ERR_AUTH);
				தொடர்;
			}
		}
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}
