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

#અન્તર્ભાવ "config.h"

#અન્તર્ભાવ <ctype.h>
#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <string.h>

#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "mosquitto/mqtt_protocol.h"
#અન્તર્ભાવ "password_file.h"
#અન્તર્ભાવ "send_mosq.h"
#અન્તર્ભાવ "util_mosq.h"


પૂર્ણાંક mosquitto_security_init_default(શૂન્ય)
{
	પૂર્ણાંક rc;

	/* Configure plugin identifier */
	જો(db.config->per_listener_settings){
		માટે(પૂર્ણાંક i=0; i<db.config->listener_ગણતરી; i++){
			db.config->listeners[i].security_options->pid = mosquitto_calloc(1, કદ(mosquitto_plugin_id_t));
			જો(db.config->listeners[i].security_options->pid == ખાલી){
				log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
				પાછા MOSQ_ERR_NOMEM;
			}
			db.config->listeners[i].security_options->pid->plugin_name = mosquitto_strdup("builtin-security");
			db.config->listeners[i].security_options->pid->listener = &db.config->listeners[i];
			config__plugin_add_secopt(db.config->listeners[i].security_options->pid, db.config->listeners[i].security_options);
		}
	}નહીંતર{
		db.config->security_options.pid = mosquitto_calloc(1, કદ(mosquitto_plugin_id_t));
		જો(db.config->security_options.pid == ખાલી){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Out of memory.");
			પાછા MOSQ_ERR_NOMEM;
		}
		db.config->security_options.pid->plugin_name = mosquitto_strdup("builtin-security");
		config__plugin_add_secopt(db.config->security_options.pid, &db.config->security_options);
	}

	rc = broker_password_file__init();
	જો(rc){
		પાછા rc;
	}

	rc = broker_acl_file__init();
	જો(rc){
		પાછા rc;
	}

	rc = psk_file__init();
	જો(rc){
		પાછા rc;
	}

	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક mosquitto_security_cleanup_default(શૂન્ય)
{
	પૂર્ણાંક rc = 0;

	broker_password_file__cleanup();
	broker_acl_file__cleanup();

	rc = psk_file__cleanup();
	જો(rc != MOSQ_ERR_SUCCESS){
		પાછા rc;
	}

	જો(db.config->per_listener_settings){
		માટે(પૂર્ણાંક i=0; i<db.config->listener_ગણતરી; i++){
			જો(db.config->listeners[i].security_options->pid){
				mosquitto_FREE(db.config->listeners[i].security_options->pid->plugin_name);
				mosquitto_FREE(db.config->listeners[i].security_options->pid->config.security_options);
				mosquitto_FREE(db.config->listeners[i].security_options->pid);
			}
		}
	}નહીંતર{
		જો(db.config->security_options.pid){
			mosquitto_FREE(db.config->security_options.pid->plugin_name);
			mosquitto_FREE(db.config->security_options.pid->config.security_options);
			mosquitto_FREE(db.config->security_options.pid);
		}
	}
	પાછા MOSQ_ERR_SUCCESS;
}


#ifdef WITH_TLS


સ્થિર શૂન્ય security__disconnect_auth(સંરચના mosquitto *context)
{
	જો(context->protocol == mosq_p_mqtt5){
		send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, ખાલી);
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
પૂર્ણાંક mosquitto_security_apply_default(શૂન્ય)
{
	સંરચના mosquitto *context, *ctxt_tmp = ખાલી;
	bool allow_anonymous;
#ifdef WITH_TLS
	X509_NAME *name;
	X509_NAME_ENTRY *name_entry;
	ASN1_STRING *name_asn1 = ખાલી;
	સંરચના mosquitto__listener *listener;
	BIO *subject_bio;
	અક્ષર *data_start;
	size_t name_length;
	અક્ષર *subject;
#endif

#ifdef WITH_TLS
	માટે(પૂર્ણાંક i=0; i<db.config->listener_ગણતરી; i++){
		listener = &db.config->listeners[i];
		જો(listener && listener->ssl_ctx && listener->certfile && listener->keyfile && listener->crlfile && listener->require_certificate){
			જો(net__tls_server_ctx(listener)){
				પાછા MOSQ_ERR_TLS;
			}

			જો(net__tls_load_verify(listener)){
				પાછા MOSQ_ERR_TLS;
			}
		}
	}
#endif

	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		જો(context->bridge){
			ચાલુ;
		}

		જો((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->per_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false)){
			allow_anonymous = true;
		}નહીંતર{
			allow_anonymous = false;
		}

		જો(!allow_anonymous && !context->username){
			mosquitto__set_state(context, mosq_cs_disconnecting);
			do_disconnect(context, MOSQ_ERR_AUTH);
			ચાલુ;
		}

		/* Check for connected clients that are no longer authorised */
#ifdef WITH_TLS
		જો(context->listener && context->listener->ssl_ctx && (context->listener->use_identity_as_username || context->listener->use_subject_as_username)){
			/* Client must have either a valid certificate, or valid PSK used as a username. */
			જો(!context->ssl){
				જો(context->protocol == mosq_p_mqtt5){
					send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, ખાલી);
				}
				mosquitto__set_state(context, mosq_cs_disconnecting);
				do_disconnect(context, MOSQ_ERR_AUTH);
				ચાલુ;
			}
#ifdef FINAL_WITH_TLS_PSK
			જો(context->listener->psk_hint){
				/* Client should have provided an identity to get this far. */
				જો(!context->username){
					security__disconnect_auth(context);
					ચાલુ;
				}
			}નહીંતર
#endif /* FINAL_WITH_TLS_PSK */
			{
				/* Free existing credentials and then recover them. */
				mosquitto_FREE(context->username);
				mosquitto_FREE(context->password);

				X509 *client_cert = SSL_get_peer_certificate(context->ssl);
				જો(!client_cert){
					security__disconnect_auth(context);
					ચાલુ;
				}
				name = X509_get_subject_name(client_cert);
				જો(!name){
					X509_free(client_cert);
					security__disconnect_auth(context);
					ચાલુ;
				}
				જો(context->listener->use_identity_as_username){   /* use_identity_as_username */
					પૂર્ણાંક i = X509_NAME_get_index_by_NID(name, NID_commonName, -1);
					જો(i == -1){
						X509_free(client_cert);
						security__disconnect_auth(context);
						ચાલુ;
					}
					name_entry = X509_NAME_get_entry(name, i);
					જો(name_entry){
						name_asn1 = X509_NAME_ENTRY_get_data(name_entry);
						જો(name_asn1 == ખાલી){
							X509_free(client_cert);
							security__disconnect_auth(context);
							ચાલુ;
						}
						અચલ અક્ષર *username = (અચલ અક્ષર *)ASN1_STRING_get0_data(name_asn1);
						જો(!username){
							X509_free(client_cert);
							client_cert = ખાલી;
							security__disconnect_auth(context);
							ચાલુ;
						}
						context->username = mosquitto_strdup(username);
						જો(!context->username){
							X509_free(client_cert);
							security__disconnect_auth(context);
							ચાલુ;
						}
						/* Make sure there isn't an embedded NUL character in the CN */
						જો((size_t)ASN1_STRING_length(name_asn1) != strlen(context->username)){
							X509_free(client_cert);
							security__disconnect_auth(context);
							ચાલુ;
						}
					}
				}નહીંતર{   /* use_subject_as_username */
					subject_bio = BIO_new(BIO_s_mem());
					X509_NAME_print_ex(subject_bio, X509_get_subject_name(client_cert), 0, XN_FLAG_RFC2253);
					data_start = ખાલી;
					name_length = (size_t)BIO_get_mem_data(subject_bio, &data_start);
					subject = mosquitto_malloc(કદ(અક્ષર)*name_length+1);
					જો(!subject){
						BIO_free(subject_bio);
						X509_free(client_cert);
						security__disconnect_auth(context);
						ચાલુ;
					}
					memcpy(subject, data_start, name_length);
					subject[name_length] = '\0';
					BIO_free(subject_bio);
					context->username = subject;
				}
				જો(!context->username){
					X509_free(client_cert);
					security__disconnect_auth(context);
					ચાલુ;
				}
				X509_free(client_cert);
			}
		}નહીંતર
#endif
		{
			/* Username/password check only if the identity/subject check not used */
			જો(mosquitto_basic_auth(context) != MOSQ_ERR_SUCCESS){
				mosquitto__set_state(context, mosq_cs_disconnecting);
				do_disconnect(context, MOSQ_ERR_AUTH);
				ચાલુ;
			}
		}
	}

	પાછા MOSQ_ERR_SUCCESS;
}
