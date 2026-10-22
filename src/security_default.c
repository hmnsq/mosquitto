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

#ضمن "config.h"

#ضمن <ctype.h>
#ضمن <stdio.h>
#ضمن <string.h>

#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/mqtt_protocol.h"
#ضمن "password_file.h"
#ضمن "send_mosq.h"
#ضمن "util_mosq.h"


صحيح mosquitto_security_init_default(فراغ)
{
	صحيح rc;

	/* Configure plugin identifier */
	إذا(db.config->per_listener_settings){
		لكل(صحيح i=0; i<db.config->listener_سانى; i++){
			db.config->listeners[i].security_options->pid = mosquitto_calloc(1, حجم(mosquitto_plugin_id_t));
			إذا(db.config->listeners[i].security_options->pid == عدم){
				log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
				أرجع MOSQ_ERR_NOMEM;
			}
			db.config->listeners[i].security_options->pid->plugin_name = mosquitto_strdup("builtin-security");
			db.config->listeners[i].security_options->pid->listener = &db.config->listeners[i];
			config__plugin_add_secopt(db.config->listeners[i].security_options->pid, db.config->listeners[i].security_options);
		}
	}وإلا{
		db.config->security_options.pid = mosquitto_calloc(1, حجم(mosquitto_plugin_id_t));
		إذا(db.config->security_options.pid == عدم){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
			أرجع MOSQ_ERR_NOMEM;
		}
		db.config->security_options.pid->plugin_name = mosquitto_strdup("builtin-security");
		config__plugin_add_secopt(db.config->security_options.pid, &db.config->security_options);
	}

	rc = broker_password_file__init();
	إذا(rc){
		أرجع rc;
	}

	rc = broker_acl_file__init();
	إذا(rc){
		أرجع rc;
	}

	rc = psk_file__init();
	إذا(rc){
		أرجع rc;
	}

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح mosquitto_security_cleanup_default(فراغ)
{
	صحيح rc = 0;

	broker_password_file__cleanup();
	broker_acl_file__cleanup();

	rc = psk_file__cleanup();
	إذا(rc != MOSQ_ERR_SUCCESS){
		أرجع rc;
	}

	إذا(db.config->per_listener_settings){
		لكل(صحيح i=0; i<db.config->listener_سانى; i++){
			إذا(db.config->listeners[i].security_options->pid){
				mosquitto_FREE(db.config->listeners[i].security_options->pid->plugin_name);
				mosquitto_FREE(db.config->listeners[i].security_options->pid->config.security_options);
				mosquitto_FREE(db.config->listeners[i].security_options->pid);
			}
		}
	}وإلا{
		إذا(db.config->security_options.pid){
			mosquitto_FREE(db.config->security_options.pid->plugin_name);
			mosquitto_FREE(db.config->security_options.pid->config.security_options);
			mosquitto_FREE(db.config->security_options.pid);
		}
	}
	أرجع MOSQ_ERR_SUCCESS;
}


#ifdef WITH_TLS


ساكن فراغ security__disconnect_auth(بنية mosquitto *context)
{
	إذا(context->protocol == mosq_p_mqtt5){
		send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, عدم);
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
صحيح mosquitto_security_apply_default(فراغ)
{
	بنية mosquitto *context, *ctxt_tmp = عدم;
	bool allow_anonymous;
#ifdef WITH_TLS
	X509_NAME *name;
	X509_NAME_ENTRY *name_entry;
	ASN1_STRING *name_asn1 = عدم;
	بنية mosquitto__listener *listener;
	BIO *subject_bio;
	حرف *data_start;
	size_t name_length;
	حرف *subject;
#endif

#ifdef WITH_TLS
	لكل(صحيح i=0; i<db.config->listener_سانى; i++){
		listener = &db.config->listeners[i];
		إذا(listener && listener->ssl_ctx && listener->certfile && listener->keyfile && listener->crlfile && listener->require_certificate){
			إذا(net__tls_server_ctx(listener)){
				أرجع MOSQ_ERR_TLS;
			}

			إذا(net__tls_load_verify(listener)){
				أرجع MOSQ_ERR_TLS;
			}
		}
	}
#endif

	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		إذا(context->bridge){
			استمر;
		}

		إذا((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->per_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false)){
			allow_anonymous = true;
		}وإلا{
			allow_anonymous = false;
		}

		إذا(!allow_anonymous && !context->username){
			mosquitto__set_state(context, mosq_cs_disconnecting);
			do_disconnect(context, MOSQ_ERR_AUTH);
			استمر;
		}

		/* Check for connected clients that are no longer authorised */
#ifdef WITH_TLS
		إذا(context->listener && context->listener->ssl_ctx && (context->listener->use_identity_as_username || context->listener->use_subject_as_username)){
			/* Client must have either a valid certificate, or valid PSK used as a username. */
			إذا(!context->ssl){
				إذا(context->protocol == mosq_p_mqtt5){
					send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, عدم);
				}
				mosquitto__set_state(context, mosq_cs_disconnecting);
				do_disconnect(context, MOSQ_ERR_AUTH);
				استمر;
			}
#ifdef FINAL_WITH_TLS_PSK
			إذا(context->listener->psk_hint){
				/* Client should have provided an identity to get this far. */
				إذا(!context->username){
					security__disconnect_auth(context);
					استمر;
				}
			}وإلا
#endif /* FINAL_WITH_TLS_PSK */
			{
				/* Free existing credentials and then recover them. */
				mosquitto_FREE(context->username);
				mosquitto_FREE(context->password);

				X509 *client_cert = SSL_get_peer_certificate(context->ssl);
				إذا(!client_cert){
					security__disconnect_auth(context);
					استمر;
				}
				name = X509_get_subject_name(client_cert);
				إذا(!name){
					X509_free(client_cert);
					security__disconnect_auth(context);
					استمر;
				}
				إذا(context->listener->use_identity_as_username){   /* use_identity_as_username */
					صحيح i = X509_NAME_get_index_by_NID(name, NID_commonName, -1);
					إذا(i == -1){
						X509_free(client_cert);
						security__disconnect_auth(context);
						استمر;
					}
					name_entry = X509_NAME_get_entry(name, i);
					إذا(name_entry){
						name_asn1 = X509_NAME_ENTRY_get_data(name_entry);
						إذا(name_asn1 == عدم){
							X509_free(client_cert);
							security__disconnect_auth(context);
							استمر;
						}
						ثابت حرف *username = (ثابت حرف *)ASN1_STRING_get0_data(name_asn1);
						إذا(!username){
							X509_free(client_cert);
							client_cert = عدم;
							security__disconnect_auth(context);
							استمر;
						}
						context->username = mosquitto_strdup(username);
						إذا(!context->username){
							X509_free(client_cert);
							security__disconnect_auth(context);
							استمر;
						}
						/* Make sure there isn't an embedded NUL character in the CN */
						إذا((size_t)ASN1_STRING_length(name_asn1) != strlen(context->username)){
							X509_free(client_cert);
							security__disconnect_auth(context);
							استمر;
						}
					}
				}وإلا{   /* use_subject_as_username */
					subject_bio = BIO_new(BIO_s_mem());
					X509_NAME_print_ex(subject_bio, X509_get_subject_name(client_cert), 0, XN_FLAG_RFC2253);
					data_start = عدم;
					name_length = (size_t)BIO_get_mem_data(subject_bio, &data_start);
					subject = mosquitto_malloc(حجم(حرف)*name_length+1);
					إذا(!subject){
						BIO_free(subject_bio);
						X509_free(client_cert);
						security__disconnect_auth(context);
						استمر;
					}
					memcpy(subject, data_start, name_length);
					subject[name_length] = '\0';
					BIO_free(subject_bio);
					context->username = subject;
				}
				إذا(!context->username){
					X509_free(client_cert);
					security__disconnect_auth(context);
					استمر;
				}
				X509_free(client_cert);
			}
		}وإلا
#endif
		{
			/* Username/password check only if the identity/subject check not used */
			إذا(mosquitto_basic_auth(context) != MOSQ_ERR_SUCCESS){
				mosquitto__set_state(context, mosq_cs_disconnecting);
				do_disconnect(context, MOSQ_ERR_AUTH);
				استمر;
			}
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}
