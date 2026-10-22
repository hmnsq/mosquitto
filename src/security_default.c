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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ଅନ୍ତର୍ଭାଵ <ctype.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto/mqtt_protocol.h"
#ଅନ୍ତର୍ଭାଵ "password_file.h"
#ଅନ୍ତର୍ଭାଵ "send_mosq.h"
#ଅନ୍ତର୍ଭାଵ "util_mosq.h"


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_security_init_default(ଶୂନ୍ୟ)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	/* Configure plugin identifier */
	ଯଦି(db.config->per_listener_settings){
		ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<db.config->listener_ଗଣନା; i++){
			db.config->listeners[i].security_options->pid = mosquitto_calloc(1, ଆକାର(mosquitto_plugin_id_t));
			ଯଦି(db.config->listeners[i].security_options->pid == ଖାଲି){
				log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
				ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
			}
			db.config->listeners[i].security_options->pid->plugin_name = mosquitto_strdup("builtin-security");
			db.config->listeners[i].security_options->pid->listener = &db.config->listeners[i];
			config__plugin_add_secopt(db.config->listeners[i].security_options->pid, db.config->listeners[i].security_options);
		}
	}ଅନ୍ୟଥା{
		db.config->security_options.pid = mosquitto_calloc(1, ଆକାର(mosquitto_plugin_id_t));
		ଯଦି(db.config->security_options.pid == ଖାଲି){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
			ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
		}
		db.config->security_options.pid->plugin_name = mosquitto_strdup("builtin-security");
		config__plugin_add_secopt(db.config->security_options.pid, &db.config->security_options);
	}

	rc = broker_password_file__init();
	ଯଦି(rc){
		ଫେରନ୍ତୁ rc;
	}

	rc = broker_acl_file__init();
	ଯଦି(rc){
		ଫେରନ୍ତୁ rc;
	}

	rc = psk_file__init();
	ଯଦି(rc){
		ଫେରନ୍ତୁ rc;
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_security_cleanup_default(ଶୂନ୍ୟ)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = 0;

	broker_password_file__cleanup();
	broker_acl_file__cleanup();

	rc = psk_file__cleanup();
	ଯଦି(rc != MOSQ_ERR_SUCCESS){
		ଫେରନ୍ତୁ rc;
	}

	ଯଦି(db.config->per_listener_settings){
		ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<db.config->listener_ଗଣନା; i++){
			ଯଦି(db.config->listeners[i].security_options->pid){
				mosquitto_FREE(db.config->listeners[i].security_options->pid->plugin_name);
				mosquitto_FREE(db.config->listeners[i].security_options->pid->config.security_options);
				mosquitto_FREE(db.config->listeners[i].security_options->pid);
			}
		}
	}ଅନ୍ୟଥା{
		ଯଦି(db.config->security_options.pid){
			mosquitto_FREE(db.config->security_options.pid->plugin_name);
			mosquitto_FREE(db.config->security_options.pid->config.security_options);
			mosquitto_FREE(db.config->security_options.pid);
		}
	}
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


#ifdef WITH_TLS


ସ୍ଥିର ଶୂନ୍ୟ security__disconnect_auth(ସଂରଚନା mosquitto *context)
{
	ଯଦି(context->protocol == mosq_p_mqtt5){
		send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, ଖାଲି);
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
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_security_apply_default(ଶୂନ୍ୟ)
{
	ସଂରଚନା mosquitto *context, *ctxt_tmp = ଖାଲି;
	bool allow_anonymous;
#ifdef WITH_TLS
	X509_NAME *name;
	X509_NAME_ENTRY *name_entry;
	ASN1_STRING *name_asn1 = ଖାଲି;
	ସଂରଚନା mosquitto__listener *listener;
	BIO *subject_bio;
	ଅକ୍ଷର *data_start;
	size_t name_length;
	ଅକ୍ଷର *subject;
#endif

#ifdef WITH_TLS
	ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<db.config->listener_ଗଣନା; i++){
		listener = &db.config->listeners[i];
		ଯଦି(listener && listener->ssl_ctx && listener->certfile && listener->keyfile && listener->crlfile && listener->require_certificate){
			ଯଦି(net__tls_server_ctx(listener)){
				ଫେରନ୍ତୁ MOSQ_ERR_TLS;
			}

			ଯଦି(net__tls_load_verify(listener)){
				ଫେରନ୍ତୁ MOSQ_ERR_TLS;
			}
		}
	}
#endif

	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		ଯଦି(context->bridge){
			ଜାରି;
		}

		ଯଦି((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->per_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false)){
			allow_anonymous = true;
		}ଅନ୍ୟଥା{
			allow_anonymous = false;
		}

		ଯଦି(!allow_anonymous && !context->username){
			mosquitto__set_state(context, mosq_cs_disconnecting);
			do_disconnect(context, MOSQ_ERR_AUTH);
			ଜାରି;
		}

		/* Check for connected clients that are no longer authorised */
#ifdef WITH_TLS
		ଯଦି(context->listener && context->listener->ssl_ctx && (context->listener->use_identity_as_username || context->listener->use_subject_as_username)){
			/* Client must have either a valid certificate, or valid PSK used as a username. */
			ଯଦି(!context->ssl){
				ଯଦି(context->protocol == mosq_p_mqtt5){
					send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, ଖାଲି);
				}
				mosquitto__set_state(context, mosq_cs_disconnecting);
				do_disconnect(context, MOSQ_ERR_AUTH);
				ଜାରି;
			}
#ifdef FINAL_WITH_TLS_PSK
			ଯଦି(context->listener->psk_hint){
				/* Client should have provided an identity to get this far. */
				ଯଦି(!context->username){
					security__disconnect_auth(context);
					ଜାରି;
				}
			}ଅନ୍ୟଥା
#endif /* FINAL_WITH_TLS_PSK */
			{
				/* Free existing credentials and then recover them. */
				mosquitto_FREE(context->username);
				mosquitto_FREE(context->password);

				X509 *client_cert = SSL_get_peer_certificate(context->ssl);
				ଯଦି(!client_cert){
					security__disconnect_auth(context);
					ଜାରି;
				}
				name = X509_get_subject_name(client_cert);
				ଯଦି(!name){
					X509_free(client_cert);
					security__disconnect_auth(context);
					ଜାରି;
				}
				ଯଦି(context->listener->use_identity_as_username){   /* use_identity_as_username */
					ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = X509_NAME_get_index_by_NID(name, NID_commonName, -1);
					ଯଦି(i == -1){
						X509_free(client_cert);
						security__disconnect_auth(context);
						ଜାରି;
					}
					name_entry = X509_NAME_get_entry(name, i);
					ଯଦି(name_entry){
						name_asn1 = X509_NAME_ENTRY_get_data(name_entry);
						ଯଦି(name_asn1 == ଖାଲି){
							X509_free(client_cert);
							security__disconnect_auth(context);
							ଜାରି;
						}
						ଅଚଳ ଅକ୍ଷର *username = (ଅଚଳ ଅକ୍ଷର *)ASN1_STRING_get0_data(name_asn1);
						ଯଦି(!username){
							X509_free(client_cert);
							client_cert = ଖାଲି;
							security__disconnect_auth(context);
							ଜାରି;
						}
						context->username = mosquitto_strdup(username);
						ଯଦି(!context->username){
							X509_free(client_cert);
							security__disconnect_auth(context);
							ଜାରି;
						}
						/* Make sure there isn't an embedded NUL character in the CN */
						ଯଦି((size_t)ASN1_STRING_length(name_asn1) != strlen(context->username)){
							X509_free(client_cert);
							security__disconnect_auth(context);
							ଜାରି;
						}
					}
				}ଅନ୍ୟଥା{   /* use_subject_as_username */
					subject_bio = BIO_new(BIO_s_mem());
					X509_NAME_print_ex(subject_bio, X509_get_subject_name(client_cert), 0, XN_FLAG_RFC2253);
					data_start = ଖାଲି;
					name_length = (size_t)BIO_get_mem_data(subject_bio, &data_start);
					subject = mosquitto_malloc(ଆକାର(ଅକ୍ଷର)*name_length+1);
					ଯଦି(!subject){
						BIO_free(subject_bio);
						X509_free(client_cert);
						security__disconnect_auth(context);
						ଜାରି;
					}
					memcpy(subject, data_start, name_length);
					subject[name_length] = '\0';
					BIO_free(subject_bio);
					context->username = subject;
				}
				ଯଦି(!context->username){
					X509_free(client_cert);
					security__disconnect_auth(context);
					ଜାରି;
				}
				X509_free(client_cert);
			}
		}ଅନ୍ୟଥା
#endif
		{
			/* Username/password check only if the identity/subject check not used */
			ଯଦି(mosquitto_basic_auth(context) != MOSQ_ERR_SUCCESS){
				mosquitto__set_state(context, mosq_cs_disconnecting);
				do_disconnect(context, MOSQ_ERR_AUTH);
				ଜାରି;
			}
		}
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}
