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

#شامل "config.h"

#شامل <ctype.h>
#شامل <stdio.h>
#شامل <string.h>

#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto/mqtt_protocol.h"
#شامل "password_file.h"
#شامل "send_mosq.h"
#شامل "util_mosq.h"


صحیح mosquitto_security_init_default(تهی)
{
	صحیح rc;

	/* Configure plugin identifier */
	اگر(db.config->per_listener_settings){
		برای(صحیح i=0; i<db.config->listener_شمار; i++){
			db.config->listeners[i].security_options->pid = mosquitto_calloc(1, اندازه(mosquitto_plugin_id_t));
			اگر(db.config->listeners[i].security_options->pid == پوچ){
				log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
				بازگشت MOSQ_ERR_NOMEM;
			}
			db.config->listeners[i].security_options->pid->plugin_name = mosquitto_strdup("builtin-security");
			db.config->listeners[i].security_options->pid->listener = &db.config->listeners[i];
			config__plugin_add_secopt(db.config->listeners[i].security_options->pid, db.config->listeners[i].security_options);
		}
	}وگرنه{
		db.config->security_options.pid = mosquitto_calloc(1, اندازه(mosquitto_plugin_id_t));
		اگر(db.config->security_options.pid == پوچ){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error: Out of memory.");
			بازگشت MOSQ_ERR_NOMEM;
		}
		db.config->security_options.pid->plugin_name = mosquitto_strdup("builtin-security");
		config__plugin_add_secopt(db.config->security_options.pid, &db.config->security_options);
	}

	rc = broker_password_file__init();
	اگر(rc){
		بازگشت rc;
	}

	rc = broker_acl_file__init();
	اگر(rc){
		بازگشت rc;
	}

	rc = psk_file__init();
	اگر(rc){
		بازگشت rc;
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح mosquitto_security_cleanup_default(تهی)
{
	صحیح rc = 0;

	broker_password_file__cleanup();
	broker_acl_file__cleanup();

	rc = psk_file__cleanup();
	اگر(rc != MOSQ_ERR_SUCCESS){
		بازگشت rc;
	}

	اگر(db.config->per_listener_settings){
		برای(صحیح i=0; i<db.config->listener_شمار; i++){
			اگر(db.config->listeners[i].security_options->pid){
				mosquitto_FREE(db.config->listeners[i].security_options->pid->plugin_name);
				mosquitto_FREE(db.config->listeners[i].security_options->pid->config.security_options);
				mosquitto_FREE(db.config->listeners[i].security_options->pid);
			}
		}
	}وگرنه{
		اگر(db.config->security_options.pid){
			mosquitto_FREE(db.config->security_options.pid->plugin_name);
			mosquitto_FREE(db.config->security_options.pid->config.security_options);
			mosquitto_FREE(db.config->security_options.pid);
		}
	}
	بازگشت MOSQ_ERR_SUCCESS;
}


#ifdef WITH_TLS


ایستا تهی security__disconnect_auth(ساختار mosquitto *context)
{
	اگر(context->protocol == mosq_p_mqtt5){
		send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, پوچ);
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
صحیح mosquitto_security_apply_default(تهی)
{
	ساختار mosquitto *context, *ctxt_tmp = پوچ;
	bool allow_anonymous;
#ifdef WITH_TLS
	X509_NAME *name;
	X509_NAME_ENTRY *name_entry;
	ASN1_STRING *name_asn1 = پوچ;
	ساختار mosquitto__listener *listener;
	BIO *subject_bio;
	نویسه *data_start;
	size_t name_length;
	نویسه *subject;
#endif

#ifdef WITH_TLS
	برای(صحیح i=0; i<db.config->listener_شمار; i++){
		listener = &db.config->listeners[i];
		اگر(listener && listener->ssl_ctx && listener->certfile && listener->keyfile && listener->crlfile && listener->require_certificate){
			اگر(net__tls_server_ctx(listener)){
				بازگشت MOSQ_ERR_TLS;
			}

			اگر(net__tls_load_verify(listener)){
				بازگشت MOSQ_ERR_TLS;
			}
		}
	}
#endif

	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		اگر(context->bridge){
			ادامه;
		}

		اگر((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->per_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false)){
			allow_anonymous = true;
		}وگرنه{
			allow_anonymous = false;
		}

		اگر(!allow_anonymous && !context->username){
			mosquitto__set_state(context, mosq_cs_disconnecting);
			do_disconnect(context, MOSQ_ERR_AUTH);
			ادامه;
		}

		/* Check for connected clients that are no longer authorised */
#ifdef WITH_TLS
		اگر(context->listener && context->listener->ssl_ctx && (context->listener->use_identity_as_username || context->listener->use_subject_as_username)){
			/* Client must have either a valid certificate, or valid PSK used as a username. */
			اگر(!context->ssl){
				اگر(context->protocol == mosq_p_mqtt5){
					send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, پوچ);
				}
				mosquitto__set_state(context, mosq_cs_disconnecting);
				do_disconnect(context, MOSQ_ERR_AUTH);
				ادامه;
			}
#ifdef FINAL_WITH_TLS_PSK
			اگر(context->listener->psk_hint){
				/* Client should have provided an identity to get this far. */
				اگر(!context->username){
					security__disconnect_auth(context);
					ادامه;
				}
			}وگرنه
#endif /* FINAL_WITH_TLS_PSK */
			{
				/* Free existing credentials and then recover them. */
				mosquitto_FREE(context->username);
				mosquitto_FREE(context->password);

				X509 *client_cert = SSL_get_peer_certificate(context->ssl);
				اگر(!client_cert){
					security__disconnect_auth(context);
					ادامه;
				}
				name = X509_get_subject_name(client_cert);
				اگر(!name){
					X509_free(client_cert);
					security__disconnect_auth(context);
					ادامه;
				}
				اگر(context->listener->use_identity_as_username){   /* use_identity_as_username */
					صحیح i = X509_NAME_get_index_by_NID(name, NID_commonName, -1);
					اگر(i == -1){
						X509_free(client_cert);
						security__disconnect_auth(context);
						ادامه;
					}
					name_entry = X509_NAME_get_entry(name, i);
					اگر(name_entry){
						name_asn1 = X509_NAME_ENTRY_get_data(name_entry);
						اگر(name_asn1 == پوچ){
							X509_free(client_cert);
							security__disconnect_auth(context);
							ادامه;
						}
						ثابت نویسه *username = (ثابت نویسه *)ASN1_STRING_get0_data(name_asn1);
						اگر(!username){
							X509_free(client_cert);
							client_cert = پوچ;
							security__disconnect_auth(context);
							ادامه;
						}
						context->username = mosquitto_strdup(username);
						اگر(!context->username){
							X509_free(client_cert);
							security__disconnect_auth(context);
							ادامه;
						}
						/* Make sure there isn't an embedded NUL character in the CN */
						اگر((size_t)ASN1_STRING_length(name_asn1) != strlen(context->username)){
							X509_free(client_cert);
							security__disconnect_auth(context);
							ادامه;
						}
					}
				}وگرنه{   /* use_subject_as_username */
					subject_bio = BIO_new(BIO_s_mem());
					X509_NAME_print_ex(subject_bio, X509_get_subject_name(client_cert), 0, XN_FLAG_RFC2253);
					data_start = پوچ;
					name_length = (size_t)BIO_get_mem_data(subject_bio, &data_start);
					subject = mosquitto_malloc(اندازه(نویسه)*name_length+1);
					اگر(!subject){
						BIO_free(subject_bio);
						X509_free(client_cert);
						security__disconnect_auth(context);
						ادامه;
					}
					memcpy(subject, data_start, name_length);
					subject[name_length] = '\0';
					BIO_free(subject_bio);
					context->username = subject;
				}
				اگر(!context->username){
					X509_free(client_cert);
					security__disconnect_auth(context);
					ادامه;
				}
				X509_free(client_cert);
			}
		}وگرنه
#endif
		{
			/* Username/password check only if the identity/subject check not used */
			اگر(mosquitto_basic_auth(context) != MOSQ_ERR_SUCCESS){
				mosquitto__set_state(context, mosq_cs_disconnecting);
				do_disconnect(context, MOSQ_ERR_AUTH);
				ادامه;
			}
		}
	}

	بازگشت MOSQ_ERR_SUCCESS;
}
