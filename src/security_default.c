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

#অন্তর্ভাব "config.h"

#অন্তর্ভাব <ctype.h>
#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <string.h>

#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "mosquitto/mqtt_protocol.h"
#অন্তর্ভাব "password_file.h"
#অন্তর্ভাব "send_mosq.h"
#অন্তর্ভাব "util_mosq.h"


পূর্ণাঙ্ক mosquitto_security_init_default(শূন্য)
{
	পূর্ণাঙ্ক rc;

	/* Configure plugin identifier */
	যদি(db.config->per_listener_settings){
		প্রতি(পূর্ণাঙ্ক i=0; i<db.config->listener_গুনতি; i++){
			db.config->listeners[i].security_options->pid = mosquitto_calloc(1, পরিমাণ(mosquitto_plugin_id_t));
			যদি(db.config->listeners[i].security_options->pid == রিক্ত){
				log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Out of memory.");
				প্রত্যাগম MOSQ_ERR_NOMEM;
			}
			db.config->listeners[i].security_options->pid->plugin_name = mosquitto_strdup("builtin-security");
			db.config->listeners[i].security_options->pid->listener = &db.config->listeners[i];
			config__plugin_add_secopt(db.config->listeners[i].security_options->pid, db.config->listeners[i].security_options);
		}
	}অন্যথা{
		db.config->security_options.pid = mosquitto_calloc(1, পরিমাণ(mosquitto_plugin_id_t));
		যদি(db.config->security_options.pid == রিক্ত){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Out of memory.");
			প্রত্যাগম MOSQ_ERR_NOMEM;
		}
		db.config->security_options.pid->plugin_name = mosquitto_strdup("builtin-security");
		config__plugin_add_secopt(db.config->security_options.pid, &db.config->security_options);
	}

	rc = broker_password_file__init();
	যদি(rc){
		প্রত্যাগম rc;
	}

	rc = broker_acl_file__init();
	যদি(rc){
		প্রত্যাগম rc;
	}

	rc = psk_file__init();
	যদি(rc){
		প্রত্যাগম rc;
	}

	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক mosquitto_security_cleanup_default(শূন্য)
{
	পূর্ণাঙ্ক rc = 0;

	broker_password_file__cleanup();
	broker_acl_file__cleanup();

	rc = psk_file__cleanup();
	যদি(rc != MOSQ_ERR_SUCCESS){
		প্রত্যাগম rc;
	}

	যদি(db.config->per_listener_settings){
		প্রতি(পূর্ণাঙ্ক i=0; i<db.config->listener_গুনতি; i++){
			যদি(db.config->listeners[i].security_options->pid){
				mosquitto_FREE(db.config->listeners[i].security_options->pid->plugin_name);
				mosquitto_FREE(db.config->listeners[i].security_options->pid->config.security_options);
				mosquitto_FREE(db.config->listeners[i].security_options->pid);
			}
		}
	}অন্যথা{
		যদি(db.config->security_options.pid){
			mosquitto_FREE(db.config->security_options.pid->plugin_name);
			mosquitto_FREE(db.config->security_options.pid->config.security_options);
			mosquitto_FREE(db.config->security_options.pid);
		}
	}
	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


#ifdef WITH_TLS


স্থির শূন্য security__disconnect_auth(সংরচনা mosquitto *context)
{
	যদি(context->protocol == mosq_p_mqtt5){
		send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, রিক্ত);
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
পূর্ণাঙ্ক mosquitto_security_apply_default(শূন্য)
{
	সংরচনা mosquitto *context, *ctxt_tmp = রিক্ত;
	bool allow_anonymous;
#ifdef WITH_TLS
	X509_NAME *name;
	X509_NAME_ENTRY *name_entry;
	ASN1_STRING *name_asn1 = রিক্ত;
	সংরচনা mosquitto__listener *listener;
	BIO *subject_bio;
	বর্ণ *data_start;
	size_t name_length;
	বর্ণ *subject;
#endif

#ifdef WITH_TLS
	প্রতি(পূর্ণাঙ্ক i=0; i<db.config->listener_গুনতি; i++){
		listener = &db.config->listeners[i];
		যদি(listener && listener->ssl_ctx && listener->certfile && listener->keyfile && listener->crlfile && listener->require_certificate){
			যদি(net__tls_server_ctx(listener)){
				প্রত্যাগম MOSQ_ERR_TLS;
			}

			যদি(net__tls_load_verify(listener)){
				প্রত্যাগম MOSQ_ERR_TLS;
			}
		}
	}
#endif

	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		যদি(context->bridge){
			অনুবর্ত;
		}

		যদি((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->per_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false)){
			allow_anonymous = true;
		}অন্যথা{
			allow_anonymous = false;
		}

		যদি(!allow_anonymous && !context->username){
			mosquitto__set_state(context, mosq_cs_disconnecting);
			do_disconnect(context, MOSQ_ERR_AUTH);
			অনুবর্ত;
		}

		/* Check for connected clients that are no longer authorised */
#ifdef WITH_TLS
		যদি(context->listener && context->listener->ssl_ctx && (context->listener->use_identity_as_username || context->listener->use_subject_as_username)){
			/* Client must have either a valid certificate, or valid PSK used as a username. */
			যদি(!context->ssl){
				যদি(context->protocol == mosq_p_mqtt5){
					send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, রিক্ত);
				}
				mosquitto__set_state(context, mosq_cs_disconnecting);
				do_disconnect(context, MOSQ_ERR_AUTH);
				অনুবর্ত;
			}
#ifdef FINAL_WITH_TLS_PSK
			যদি(context->listener->psk_hint){
				/* Client should have provided an identity to get this far. */
				যদি(!context->username){
					security__disconnect_auth(context);
					অনুবর্ত;
				}
			}অন্যথা
#endif /* FINAL_WITH_TLS_PSK */
			{
				/* Free existing credentials and then recover them. */
				mosquitto_FREE(context->username);
				mosquitto_FREE(context->password);

				X509 *client_cert = SSL_get_peer_certificate(context->ssl);
				যদি(!client_cert){
					security__disconnect_auth(context);
					অনুবর্ত;
				}
				name = X509_get_subject_name(client_cert);
				যদি(!name){
					X509_free(client_cert);
					security__disconnect_auth(context);
					অনুবর্ত;
				}
				যদি(context->listener->use_identity_as_username){   /* use_identity_as_username */
					পূর্ণাঙ্ক i = X509_NAME_get_index_by_NID(name, NID_commonName, -1);
					যদি(i == -1){
						X509_free(client_cert);
						security__disconnect_auth(context);
						অনুবর্ত;
					}
					name_entry = X509_NAME_get_entry(name, i);
					যদি(name_entry){
						name_asn1 = X509_NAME_ENTRY_get_data(name_entry);
						যদি(name_asn1 == রিক্ত){
							X509_free(client_cert);
							security__disconnect_auth(context);
							অনুবর্ত;
						}
						ধ্রুব বর্ণ *username = (ধ্রুব বর্ণ *)ASN1_STRING_get0_data(name_asn1);
						যদি(!username){
							X509_free(client_cert);
							client_cert = রিক্ত;
							security__disconnect_auth(context);
							অনুবর্ত;
						}
						context->username = mosquitto_strdup(username);
						যদি(!context->username){
							X509_free(client_cert);
							security__disconnect_auth(context);
							অনুবর্ত;
						}
						/* Make sure there isn't an embedded NUL character in the CN */
						যদি((size_t)ASN1_STRING_length(name_asn1) != strlen(context->username)){
							X509_free(client_cert);
							security__disconnect_auth(context);
							অনুবর্ত;
						}
					}
				}অন্যথা{   /* use_subject_as_username */
					subject_bio = BIO_new(BIO_s_mem());
					X509_NAME_print_ex(subject_bio, X509_get_subject_name(client_cert), 0, XN_FLAG_RFC2253);
					data_start = রিক্ত;
					name_length = (size_t)BIO_get_mem_data(subject_bio, &data_start);
					subject = mosquitto_malloc(পরিমাণ(বর্ণ)*name_length+1);
					যদি(!subject){
						BIO_free(subject_bio);
						X509_free(client_cert);
						security__disconnect_auth(context);
						অনুবর্ত;
					}
					memcpy(subject, data_start, name_length);
					subject[name_length] = '\0';
					BIO_free(subject_bio);
					context->username = subject;
				}
				যদি(!context->username){
					X509_free(client_cert);
					security__disconnect_auth(context);
					অনুবর্ত;
				}
				X509_free(client_cert);
			}
		}অন্যথা
#endif
		{
			/* Username/password check only if the identity/subject check not used */
			যদি(mosquitto_basic_auth(context) != MOSQ_ERR_SUCCESS){
				mosquitto__set_state(context, mosq_cs_disconnecting);
				do_disconnect(context, MOSQ_ERR_AUTH);
				অনুবর্ত;
			}
		}
	}

	প্রত্যাগম MOSQ_ERR_SUCCESS;
}
