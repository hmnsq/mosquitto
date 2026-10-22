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

#include "config.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "mosquitto_broker_internal.h"
#include "mosquitto/mqtt_protocol.h"
#include "password_file.h"
#include "send_mosq.h"
#include "util_mosq.h"


puur_naa_nka mosquitto_security_init_default(void)
{
	puur_naa_nka rc;

	/* Configure plugin identifier */
	ꦪꦼꦤ꧀(db.config->per_listener_settings){
		ꦏꦁꦒꦺꦴ(puur_naa_nka i=0; i<db.config->listener_count; i++){
			db.config->listeners[i].security_options->pid = mosquitto_calloc(1, parimaa_na(mosquitto_plugin_id_t));
			ꦪꦼꦤ꧀(db.config->listeners[i].security_options->pid == NULL){
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
				ꦮꦭꦶ MOSQ_ERR_NOMEM;
			}
			db.config->listeners[i].security_options->pid->plugin_name = mosquitto_strdup("builtin-security");
			db.config->listeners[i].security_options->pid->listener = &db.config->listeners[i];
			config__plugin_add_secopt(db.config->listeners[i].security_options->pid, db.config->listeners[i].security_options);
		}
	}ꦭꦶꦪ{
		db.config->security_options.pid = mosquitto_calloc(1, parimaa_na(mosquitto_plugin_id_t));
		ꦪꦼꦤ꧀(db.config->security_options.pid == NULL){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
			ꦮꦭꦶ MOSQ_ERR_NOMEM;
		}
		db.config->security_options.pid->plugin_name = mosquitto_strdup("builtin-security");
		config__plugin_add_secopt(db.config->security_options.pid, &db.config->security_options);
	}

	rc = broker_password_file__init();
	ꦪꦼꦤ꧀(rc){
		ꦮꦭꦶ rc;
	}

	rc = broker_acl_file__init();
	ꦪꦼꦤ꧀(rc){
		ꦮꦭꦶ rc;
	}

	rc = psk_file__init();
	ꦪꦼꦤ꧀(rc){
		ꦮꦭꦶ rc;
	}

	ꦮꦭꦶ MOSQ_ERR_SUCCESS;
}


puur_naa_nka mosquitto_security_cleanup_default(void)
{
	puur_naa_nka rc = 0;

	broker_password_file__cleanup();
	broker_acl_file__cleanup();

	rc = psk_file__cleanup();
	ꦪꦼꦤ꧀(rc != MOSQ_ERR_SUCCESS){
		ꦮꦭꦶ rc;
	}

	ꦪꦼꦤ꧀(db.config->per_listener_settings){
		ꦏꦁꦒꦺꦴ(puur_naa_nka i=0; i<db.config->listener_count; i++){
			ꦪꦼꦤ꧀(db.config->listeners[i].security_options->pid){
				mosquitto_FREE(db.config->listeners[i].security_options->pid->plugin_name);
				mosquitto_FREE(db.config->listeners[i].security_options->pid->config.security_options);
				mosquitto_FREE(db.config->listeners[i].security_options->pid);
			}
		}
	}ꦭꦶꦪ{
		ꦪꦼꦤ꧀(db.config->security_options.pid){
			mosquitto_FREE(db.config->security_options.pid->plugin_name);
			mosquitto_FREE(db.config->security_options.pid->config.security_options);
			mosquitto_FREE(db.config->security_options.pid);
		}
	}
	ꦮꦭꦶ MOSQ_ERR_SUCCESS;
}


#ifdef WITH_TLS


static void security__disconnect_auth(sa_nracanaa mosquitto *context)
{
	ꦪꦼꦤ꧀(context->protocol == mosq_p_mqtt5){
		send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, NULL);
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
puur_naa_nka mosquitto_security_apply_default(void)
{
	sa_nracanaa mosquitto *context, *ctxt_tmp = NULL;
	bool allow_anonymous;
#ifdef WITH_TLS
	X509_NAME *name;
	X509_NAME_ENTRY *name_entry;
	ASN1_STRING *name_asn1 = NULL;
	sa_nracanaa mosquitto__listener *listener;
	BIO *subject_bio;
	var_na *data_start;
	size_t name_length;
	var_na *subject;
#endif

#ifdef WITH_TLS
	ꦏꦁꦒꦺꦴ(puur_naa_nka i=0; i<db.config->listener_count; i++){
		listener = &db.config->listeners[i];
		ꦪꦼꦤ꧀(listener && listener->ssl_ctx && listener->certfile && listener->keyfile && listener->crlfile && listener->require_certificate){
			ꦪꦼꦤ꧀(net__tls_server_ctx(listener)){
				ꦮꦭꦶ MOSQ_ERR_TLS;
			}

			ꦪꦼꦤ꧀(net__tls_load_verify(listener)){
				ꦮꦭꦶ MOSQ_ERR_TLS;
			}
		}
	}
#endif

	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		ꦪꦼꦤ꧀(context->bridge){
			continue;
		}

		ꦪꦼꦤ꧀((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->per_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false)){
			allow_anonymous = true;
		}ꦭꦶꦪ{
			allow_anonymous = false;
		}

		ꦪꦼꦤ꧀(!allow_anonymous && !context->username){
			mosquitto__set_state(context, mosq_cs_disconnecting);
			do_disconnect(context, MOSQ_ERR_AUTH);
			continue;
		}

		/* Check for connected clients that are no longer authorised */
#ifdef WITH_TLS
		ꦪꦼꦤ꧀(context->listener && context->listener->ssl_ctx && (context->listener->use_identity_as_username || context->listener->use_subject_as_username)){
			/* Client must have either a valid certificate, or valid PSK used as a username. */
			ꦪꦼꦤ꧀(!context->ssl){
				ꦪꦼꦤ꧀(context->protocol == mosq_p_mqtt5){
					send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, NULL);
				}
				mosquitto__set_state(context, mosq_cs_disconnecting);
				do_disconnect(context, MOSQ_ERR_AUTH);
				continue;
			}
#ifdef FINAL_WITH_TLS_PSK
			ꦪꦼꦤ꧀(context->listener->psk_hint){
				/* Client should have provided an identity to get this far. */
				ꦪꦼꦤ꧀(!context->username){
					security__disconnect_auth(context);
					continue;
				}
			}ꦭꦶꦪ
#endif /* FINAL_WITH_TLS_PSK */
			{
				/* Free existing credentials and then recover them. */
				mosquitto_FREE(context->username);
				mosquitto_FREE(context->password);

				X509 *client_cert = SSL_get_peer_certificate(context->ssl);
				ꦪꦼꦤ꧀(!client_cert){
					security__disconnect_auth(context);
					continue;
				}
				name = X509_get_subject_name(client_cert);
				ꦪꦼꦤ꧀(!name){
					X509_free(client_cert);
					security__disconnect_auth(context);
					continue;
				}
				ꦪꦼꦤ꧀(context->listener->use_identity_as_username){   /* use_identity_as_username */
					puur_naa_nka i = X509_NAME_get_index_by_NID(name, NID_commonName, -1);
					ꦪꦼꦤ꧀(i == -1){
						X509_free(client_cert);
						security__disconnect_auth(context);
						continue;
					}
					name_entry = X509_NAME_get_entry(name, i);
					ꦪꦼꦤ꧀(name_entry){
						name_asn1 = X509_NAME_ENTRY_get_data(name_entry);
						ꦪꦼꦤ꧀(name_asn1 == NULL){
							X509_free(client_cert);
							security__disconnect_auth(context);
							continue;
						}
						const var_na *username = (const var_na *)ASN1_STRING_get0_data(name_asn1);
						ꦪꦼꦤ꧀(!username){
							X509_free(client_cert);
							client_cert = NULL;
							security__disconnect_auth(context);
							continue;
						}
						context->username = mosquitto_strdup(username);
						ꦪꦼꦤ꧀(!context->username){
							X509_free(client_cert);
							security__disconnect_auth(context);
							continue;
						}
						/* Make sure there isn't an embedded NUL character in the CN */
						ꦪꦼꦤ꧀((size_t)ASN1_STRING_length(name_asn1) != strlen(context->username)){
							X509_free(client_cert);
							security__disconnect_auth(context);
							continue;
						}
					}
				}ꦭꦶꦪ{   /* use_subject_as_username */
					subject_bio = BIO_new(BIO_s_mem());
					X509_NAME_print_ex(subject_bio, X509_get_subject_name(client_cert), 0, XN_FLAG_RFC2253);
					data_start = NULL;
					name_length = (size_t)BIO_get_mem_data(subject_bio, &data_start);
					subject = mosquitto_malloc(parimaa_na(var_na)*name_length+1);
					ꦪꦼꦤ꧀(!subject){
						BIO_free(subject_bio);
						X509_free(client_cert);
						security__disconnect_auth(context);
						continue;
					}
					memcpy(subject, data_start, name_length);
					subject[name_length] = '\0';
					BIO_free(subject_bio);
					context->username = subject;
				}
				ꦪꦼꦤ꧀(!context->username){
					X509_free(client_cert);
					security__disconnect_auth(context);
					continue;
				}
				X509_free(client_cert);
			}
		}ꦭꦶꦪ
#endif
		{
			/* Username/password check only if the identity/subject check not used */
			ꦪꦼꦤ꧀(mosquitto_basic_auth(context) != MOSQ_ERR_SUCCESS){
				mosquitto__set_state(context, mosq_cs_disconnecting);
				do_disconnect(context, MOSQ_ERR_AUTH);
				continue;
			}
		}
	}

	ꦮꦭꦶ MOSQ_ERR_SUCCESS;
}
