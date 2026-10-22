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

#അന്തര്ഭാവ "config.h"

#അന്തര്ഭാവ <ctype.h>
#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <string.h>

#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "mosquitto/mqtt_protocol.h"
#അന്തര്ഭാവ "password_file.h"
#അന്തര്ഭാവ "send_mosq.h"
#അന്തര്ഭാവ "util_mosq.h"


പൂർണ്ണസംഖ്യ mosquitto_security_init_default(ശൂന്യം)
{
	പൂർണ്ണസംഖ്യ rc;

	/* Configure plugin identifier */
	എങ്കിൽ(db.config->per_listener_settings){
		വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<db.config->listener_എണ്ണം; i++){
			db.config->listeners[i].security_options->pid = mosquitto_calloc(1, വലുപ്പം(mosquitto_plugin_id_t));
			എങ്കിൽ(db.config->listeners[i].security_options->pid == ശൂന്യ){
				log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Out of memory.");
				തിരികെ MOSQ_ERR_NOMEM;
			}
			db.config->listeners[i].security_options->pid->plugin_name = mosquitto_strdup("builtin-security");
			db.config->listeners[i].security_options->pid->listener = &db.config->listeners[i];
			config__plugin_add_secopt(db.config->listeners[i].security_options->pid, db.config->listeners[i].security_options);
		}
	}അല്ലെങ്കിൽ{
		db.config->security_options.pid = mosquitto_calloc(1, വലുപ്പം(mosquitto_plugin_id_t));
		എങ്കിൽ(db.config->security_options.pid == ശൂന്യ){
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Out of memory.");
			തിരികെ MOSQ_ERR_NOMEM;
		}
		db.config->security_options.pid->plugin_name = mosquitto_strdup("builtin-security");
		config__plugin_add_secopt(db.config->security_options.pid, &db.config->security_options);
	}

	rc = broker_password_file__init();
	എങ്കിൽ(rc){
		തിരികെ rc;
	}

	rc = broker_acl_file__init();
	എങ്കിൽ(rc){
		തിരികെ rc;
	}

	rc = psk_file__init();
	എങ്കിൽ(rc){
		തിരികെ rc;
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ mosquitto_security_cleanup_default(ശൂന്യം)
{
	പൂർണ്ണസംഖ്യ rc = 0;

	broker_password_file__cleanup();
	broker_acl_file__cleanup();

	rc = psk_file__cleanup();
	എങ്കിൽ(rc != MOSQ_ERR_SUCCESS){
		തിരികെ rc;
	}

	എങ്കിൽ(db.config->per_listener_settings){
		വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<db.config->listener_എണ്ണം; i++){
			എങ്കിൽ(db.config->listeners[i].security_options->pid){
				mosquitto_FREE(db.config->listeners[i].security_options->pid->plugin_name);
				mosquitto_FREE(db.config->listeners[i].security_options->pid->config.security_options);
				mosquitto_FREE(db.config->listeners[i].security_options->pid);
			}
		}
	}അല്ലെങ്കിൽ{
		എങ്കിൽ(db.config->security_options.pid){
			mosquitto_FREE(db.config->security_options.pid->plugin_name);
			mosquitto_FREE(db.config->security_options.pid->config.security_options);
			mosquitto_FREE(db.config->security_options.pid);
		}
	}
	തിരികെ MOSQ_ERR_SUCCESS;
}


#ifdef WITH_TLS


സ്ഥിതം ശൂന്യം security__disconnect_auth(ഘടന mosquitto *context)
{
	എങ്കിൽ(context->protocol == mosq_p_mqtt5){
		send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, ശൂന്യ);
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
പൂർണ്ണസംഖ്യ mosquitto_security_apply_default(ശൂന്യം)
{
	ഘടന mosquitto *context, *ctxt_tmp = ശൂന്യ;
	bool allow_anonymous;
#ifdef WITH_TLS
	X509_NAME *name;
	X509_NAME_ENTRY *name_entry;
	ASN1_STRING *name_asn1 = ശൂന്യ;
	ഘടന mosquitto__listener *listener;
	BIO *subject_bio;
	അക്ഷരം *data_start;
	size_t name_length;
	അക്ഷരം *subject;
#endif

#ifdef WITH_TLS
	വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<db.config->listener_എണ്ണം; i++){
		listener = &db.config->listeners[i];
		എങ്കിൽ(listener && listener->ssl_ctx && listener->certfile && listener->keyfile && listener->crlfile && listener->require_certificate){
			എങ്കിൽ(net__tls_server_ctx(listener)){
				തിരികെ MOSQ_ERR_TLS;
			}

			എങ്കിൽ(net__tls_load_verify(listener)){
				തിരികെ MOSQ_ERR_TLS;
			}
		}
	}
#endif

	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		എങ്കിൽ(context->bridge){
			തുടരുക;
		}

		എങ്കിൽ((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->per_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false)){
			allow_anonymous = true;
		}അല്ലെങ്കിൽ{
			allow_anonymous = false;
		}

		എങ്കിൽ(!allow_anonymous && !context->username){
			mosquitto__set_state(context, mosq_cs_disconnecting);
			do_disconnect(context, MOSQ_ERR_AUTH);
			തുടരുക;
		}

		/* Check for connected clients that are no longer authorised */
#ifdef WITH_TLS
		എങ്കിൽ(context->listener && context->listener->ssl_ctx && (context->listener->use_identity_as_username || context->listener->use_subject_as_username)){
			/* Client must have either a valid certificate, or valid PSK used as a username. */
			എങ്കിൽ(!context->ssl){
				എങ്കിൽ(context->protocol == mosq_p_mqtt5){
					send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, ശൂന്യ);
				}
				mosquitto__set_state(context, mosq_cs_disconnecting);
				do_disconnect(context, MOSQ_ERR_AUTH);
				തുടരുക;
			}
#ifdef FINAL_WITH_TLS_PSK
			എങ്കിൽ(context->listener->psk_hint){
				/* Client should have provided an identity to get this far. */
				എങ്കിൽ(!context->username){
					security__disconnect_auth(context);
					തുടരുക;
				}
			}അല്ലെങ്കിൽ
#endif /* FINAL_WITH_TLS_PSK */
			{
				/* Free existing credentials and then recover them. */
				mosquitto_FREE(context->username);
				mosquitto_FREE(context->password);

				X509 *client_cert = SSL_get_peer_certificate(context->ssl);
				എങ്കിൽ(!client_cert){
					security__disconnect_auth(context);
					തുടരുക;
				}
				name = X509_get_subject_name(client_cert);
				എങ്കിൽ(!name){
					X509_free(client_cert);
					security__disconnect_auth(context);
					തുടരുക;
				}
				എങ്കിൽ(context->listener->use_identity_as_username){   /* use_identity_as_username */
					പൂർണ്ണസംഖ്യ i = X509_NAME_get_index_by_NID(name, NID_commonName, -1);
					എങ്കിൽ(i == -1){
						X509_free(client_cert);
						security__disconnect_auth(context);
						തുടരുക;
					}
					name_entry = X509_NAME_get_entry(name, i);
					എങ്കിൽ(name_entry){
						name_asn1 = X509_NAME_ENTRY_get_data(name_entry);
						എങ്കിൽ(name_asn1 == ശൂന്യ){
							X509_free(client_cert);
							security__disconnect_auth(context);
							തുടരുക;
						}
						സ്ഥിരം അക്ഷരം *username = (സ്ഥിരം അക്ഷരം *)ASN1_STRING_get0_data(name_asn1);
						എങ്കിൽ(!username){
							X509_free(client_cert);
							client_cert = ശൂന്യ;
							security__disconnect_auth(context);
							തുടരുക;
						}
						context->username = mosquitto_strdup(username);
						എങ്കിൽ(!context->username){
							X509_free(client_cert);
							security__disconnect_auth(context);
							തുടരുക;
						}
						/* Make sure there isn't an embedded NUL character in the CN */
						എങ്കിൽ((size_t)ASN1_STRING_length(name_asn1) != strlen(context->username)){
							X509_free(client_cert);
							security__disconnect_auth(context);
							തുടരുക;
						}
					}
				}അല്ലെങ്കിൽ{   /* use_subject_as_username */
					subject_bio = BIO_new(BIO_s_mem());
					X509_NAME_print_ex(subject_bio, X509_get_subject_name(client_cert), 0, XN_FLAG_RFC2253);
					data_start = ശൂന്യ;
					name_length = (size_t)BIO_get_mem_data(subject_bio, &data_start);
					subject = mosquitto_malloc(വലുപ്പം(അക്ഷരം)*name_length+1);
					എങ്കിൽ(!subject){
						BIO_free(subject_bio);
						X509_free(client_cert);
						security__disconnect_auth(context);
						തുടരുക;
					}
					memcpy(subject, data_start, name_length);
					subject[name_length] = '\0';
					BIO_free(subject_bio);
					context->username = subject;
				}
				എങ്കിൽ(!context->username){
					X509_free(client_cert);
					security__disconnect_auth(context);
					തുടരുക;
				}
				X509_free(client_cert);
			}
		}അല്ലെങ്കിൽ
#endif
		{
			/* Username/password check only if the identity/subject check not used */
			എങ്കിൽ(mosquitto_basic_auth(context) != MOSQ_ERR_SUCCESS){
				mosquitto__set_state(context, mosq_cs_disconnecting);
				do_disconnect(context, MOSQ_ERR_AUTH);
				തുടരുക;
			}
		}
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}
