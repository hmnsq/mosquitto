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
	ပေႃး(db.config->per_listener_settings){
		for(puur_naa_nka i=0; i<db.config->listener_count; i++){
			db.config->listeners[i].security_options->pid = mosquitto_calloc(1, parimaa_na(mosquitto_plugin_id_t));
			ပေႃး(db.config->listeners[i].security_options->pid == NULL){
				log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
				ပွၵ်ႈ MOSQ_ERR_NOMEM;
			}
			db.config->listeners[i].security_options->pid->plugin_name = mosquitto_strdup("builtin-security");
			db.config->listeners[i].security_options->pid->listener = &db.config->listeners[i];
			config__plugin_add_secopt(db.config->listeners[i].security_options->pid, db.config->listeners[i].security_options);
		}
	}ပေႃးဢမ်ႇ{
		db.config->security_options.pid = mosquitto_calloc(1, parimaa_na(mosquitto_plugin_id_t));
		ပေႃး(db.config->security_options.pid == NULL){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Out of memory.");
			ပွၵ်ႈ MOSQ_ERR_NOMEM;
		}
		db.config->security_options.pid->plugin_name = mosquitto_strdup("builtin-security");
		config__plugin_add_secopt(db.config->security_options.pid, &db.config->security_options);
	}

	rc = broker_password_file__init();
	ပေႃး(rc){
		ပွၵ်ႈ rc;
	}

	rc = broker_acl_file__init();
	ပေႃး(rc){
		ပွၵ်ႈ rc;
	}

	rc = psk_file__init();
	ပေႃး(rc){
		ပွၵ်ႈ rc;
	}

	ပွၵ်ႈ MOSQ_ERR_SUCCESS;
}


puur_naa_nka mosquitto_security_cleanup_default(void)
{
	puur_naa_nka rc = 0;

	broker_password_file__cleanup();
	broker_acl_file__cleanup();

	rc = psk_file__cleanup();
	ပေႃး(rc != MOSQ_ERR_SUCCESS){
		ပွၵ်ႈ rc;
	}

	ပေႃး(db.config->per_listener_settings){
		for(puur_naa_nka i=0; i<db.config->listener_count; i++){
			ပေႃး(db.config->listeners[i].security_options->pid){
				mosquitto_FREE(db.config->listeners[i].security_options->pid->plugin_name);
				mosquitto_FREE(db.config->listeners[i].security_options->pid->config.security_options);
				mosquitto_FREE(db.config->listeners[i].security_options->pid);
			}
		}
	}ပေႃးဢမ်ႇ{
		ပေႃး(db.config->security_options.pid){
			mosquitto_FREE(db.config->security_options.pid->plugin_name);
			mosquitto_FREE(db.config->security_options.pid->config.security_options);
			mosquitto_FREE(db.config->security_options.pid);
		}
	}
	ပွၵ်ႈ MOSQ_ERR_SUCCESS;
}


#ifdef WITH_TLS


static void security__disconnect_auth(sa_nracanaa mosquitto *context)
{
	ပေႃး(context->protocol == mosq_p_mqtt5){
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
	for(puur_naa_nka i=0; i<db.config->listener_count; i++){
		listener = &db.config->listeners[i];
		ပေႃး(listener && listener->ssl_ctx && listener->certfile && listener->keyfile && listener->crlfile && listener->require_certificate){
			ပေႃး(net__tls_server_ctx(listener)){
				ပွၵ်ႈ MOSQ_ERR_TLS;
			}

			ပေႃး(net__tls_load_verify(listener)){
				ပွၵ်ႈ MOSQ_ERR_TLS;
			}
		}
	}
#endif

	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		ပေႃး(context->bridge){
			continue;
		}

		ပေႃး((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->per_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false)){
			allow_anonymous = true;
		}ပေႃးဢမ်ႇ{
			allow_anonymous = false;
		}

		ပေႃး(!allow_anonymous && !context->username){
			mosquitto__set_state(context, mosq_cs_disconnecting);
			do_disconnect(context, MOSQ_ERR_AUTH);
			continue;
		}

		/* Check for connected clients that are no longer authorised */
#ifdef WITH_TLS
		ပေႃး(context->listener && context->listener->ssl_ctx && (context->listener->use_identity_as_username || context->listener->use_subject_as_username)){
			/* Client must have either a valid certificate, or valid PSK used as a username. */
			ပေႃး(!context->ssl){
				ပေႃး(context->protocol == mosq_p_mqtt5){
					send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, NULL);
				}
				mosquitto__set_state(context, mosq_cs_disconnecting);
				do_disconnect(context, MOSQ_ERR_AUTH);
				continue;
			}
#ifdef FINAL_WITH_TLS_PSK
			ပေႃး(context->listener->psk_hint){
				/* Client should have provided an identity to get this far. */
				ပေႃး(!context->username){
					security__disconnect_auth(context);
					continue;
				}
			}ပေႃးဢမ်ႇ
#endif /* FINAL_WITH_TLS_PSK */
			{
				/* Free existing credentials and then recover them. */
				mosquitto_FREE(context->username);
				mosquitto_FREE(context->password);

				X509 *client_cert = SSL_get_peer_certificate(context->ssl);
				ပေႃး(!client_cert){
					security__disconnect_auth(context);
					continue;
				}
				name = X509_get_subject_name(client_cert);
				ပေႃး(!name){
					X509_free(client_cert);
					security__disconnect_auth(context);
					continue;
				}
				ပေႃး(context->listener->use_identity_as_username){   /* use_identity_as_username */
					puur_naa_nka i = X509_NAME_get_index_by_NID(name, NID_commonName, -1);
					ပေႃး(i == -1){
						X509_free(client_cert);
						security__disconnect_auth(context);
						continue;
					}
					name_entry = X509_NAME_get_entry(name, i);
					ပေႃး(name_entry){
						name_asn1 = X509_NAME_ENTRY_get_data(name_entry);
						ပေႃး(name_asn1 == NULL){
							X509_free(client_cert);
							security__disconnect_auth(context);
							continue;
						}
						const var_na *username = (const var_na *)ASN1_STRING_get0_data(name_asn1);
						ပေႃး(!username){
							X509_free(client_cert);
							client_cert = NULL;
							security__disconnect_auth(context);
							continue;
						}
						context->username = mosquitto_strdup(username);
						ပေႃး(!context->username){
							X509_free(client_cert);
							security__disconnect_auth(context);
							continue;
						}
						/* Make sure there isn't an embedded NUL character in the CN */
						ပေႃး((size_t)ASN1_STRING_length(name_asn1) != strlen(context->username)){
							X509_free(client_cert);
							security__disconnect_auth(context);
							continue;
						}
					}
				}ပေႃးဢမ်ႇ{   /* use_subject_as_username */
					subject_bio = BIO_new(BIO_s_mem());
					X509_NAME_print_ex(subject_bio, X509_get_subject_name(client_cert), 0, XN_FLAG_RFC2253);
					data_start = NULL;
					name_length = (size_t)BIO_get_mem_data(subject_bio, &data_start);
					subject = mosquitto_malloc(parimaa_na(var_na)*name_length+1);
					ပေႃး(!subject){
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
				ပေႃး(!context->username){
					X509_free(client_cert);
					security__disconnect_auth(context);
					continue;
				}
				X509_free(client_cert);
			}
		}ပေႃးဢမ်ႇ
#endif
		{
			/* Username/password check only if the identity/subject check not used */
			ပေႃး(mosquitto_basic_auth(context) != MOSQ_ERR_SUCCESS){
				mosquitto__set_state(context, mosq_cs_disconnecting);
				do_disconnect(context, MOSQ_ERR_AUTH);
				continue;
			}
		}
	}

	ပွၵ်ႈ MOSQ_ERR_SUCCESS;
}
