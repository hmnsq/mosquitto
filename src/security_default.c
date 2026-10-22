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


صحیح mosquitto_security_init_default(خالی)
{
	صحیح rc;

	/* Configure plugin identifier */
	اگر(db.config->فی_listener_settings){
		برائے(صحیح i=0; i<db.config->listener_گنتی; i++){
			db.config->listeners[i].security_options->pid = mosquitto_calloc(1, حجم(mosquitto_plugin_id_t));
			اگر(db.config->listeners[i].security_options->pid == تہی){
				log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
				واپس MOSQ_ERR_NOMEM;
			}
			db.config->listeners[i].security_options->pid->plugin_name = mosquitto_strdup("builtin-security");
			db.config->listeners[i].security_options->pid->listener = &db.config->listeners[i];
			config_plugin_جوڑو_secopt(db.config->listeners[i].security_options->pid, db.config->listeners[i].security_options);
		}
	}ورنہ{
		db.config->security_options.pid = mosquitto_calloc(1, حجم(mosquitto_plugin_id_t));
		اگر(db.config->security_options.pid == تہی){
			log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
			واپس MOSQ_ERR_NOMEM;
		}
		db.config->security_options.pid->plugin_name = mosquitto_strdup("builtin-security");
		config_plugin_جوڑو_secopt(db.config->security_options.pid, &db.config->security_options);
	}

	rc = broker_password_file__init();
	اگر(rc){
		واپس rc;
	}

	rc = broker_acl_file__init();
	اگر(rc){
		واپس rc;
	}

	rc = psk_file__init();
	اگر(rc){
		واپس rc;
	}

	واپس MOSQ_ERR_SUCCESS;
}


صحیح mosquitto_security_cleanup_default(خالی)
{
	صحیح rc = 0;

	broker_password_file__cleanup();
	broker_acl_file__cleanup();

	rc = psk_file__cleanup();
	اگر(rc != MOSQ_ERR_SUCCESS){
		واپس rc;
	}

	اگر(db.config->فی_listener_settings){
		برائے(صحیح i=0; i<db.config->listener_گنتی; i++){
			اگر(db.config->listeners[i].security_options->pid){
				mosquitto_FREE(db.config->listeners[i].security_options->pid->plugin_name);
				mosquitto_FREE(db.config->listeners[i].security_options->pid->config.security_options);
				mosquitto_FREE(db.config->listeners[i].security_options->pid);
			}
		}
	}ورنہ{
		اگر(db.config->security_options.pid){
			mosquitto_FREE(db.config->security_options.pid->plugin_name);
			mosquitto_FREE(db.config->security_options.pid->config.security_options);
			mosquitto_FREE(db.config->security_options.pid);
		}
	}
	واپس MOSQ_ERR_SUCCESS;
}


#اگر_تعریف_شدہ WITH_TLS


ساکن خالی security__disconnect_auth(ساخت mosquitto *context)
{
	اگر(context->protocol == mosq_p_mqtt5){
		send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, تہی);
	}
	mosquitto_سیٹ_state(context, mosq_cs_disconnecting);
	do_disconnect(context, MOSQ_ERR_AUTH);
}
#اگر_ختم


/* Apply security settings after a reload.
 * Includes:
 * - Disconnecting anonymous users if appropriate
 * - Disconnecting users with invalid passwords
 * - Reapplying ACLs
 */
صحیح mosquitto_security_apply_default(خالی)
{
	ساخت mosquitto *context, *ctxt_tmp = تہی;
	bool allow_anonymous;
#اگر_تعریف_شدہ WITH_TLS
	X509_NAME *name;
	X509_NAME_اندراج *name_اندراج;
	ASN1_STRING *name_asn1 = تہی;
	ساخت mosquitto__listener *listener;
	BIO *subject_bio;
	حرف *ڈیٹا_آغاز;
	size_t name_لمبائی;
	حرف *subject;
#اگر_ختم

#اگر_تعریف_شدہ WITH_TLS
	برائے(صحیح i=0; i<db.config->listener_گنتی; i++){
		listener = &db.config->listeners[i];
		اگر(listener && listener->ssl_ctx && listener->certfile && listener->keyfile && listener->crlfile && listener->require_certificate){
			اگر(net__tls_server_ctx(listener)){
				واپس MOSQ_ERR_TLS;
			}

			اگر(net_tls_بھرو_verify(listener)){
				واپس MOSQ_ERR_TLS;
			}
		}
	}
#اگر_ختم

	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		اگر(context->bridge){
			جاری;
		}

		اگر((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->فی_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false)){
			allow_anonymous = true;
		}ورنہ{
			allow_anonymous = false;
		}

		اگر(!allow_anonymous && !context->username){
			mosquitto_سیٹ_state(context, mosq_cs_disconnecting);
			do_disconnect(context, MOSQ_ERR_AUTH);
			جاری;
		}

		/* Check for connected clients that are no longer authorised */
#اگر_تعریف_شدہ WITH_TLS
		اگر(context->listener && context->listener->ssl_ctx && (context->listener->use_شناختی_عنصر_as_username || context->listener->use_subject_as_username)){
			/* Client must have either a valid certificate, or valid PSK used as a username. */
			اگر(!context->ssl){
				اگر(context->protocol == mosq_p_mqtt5){
					send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, تہی);
				}
				mosquitto_سیٹ_state(context, mosq_cs_disconnecting);
				do_disconnect(context, MOSQ_ERR_AUTH);
				جاری;
			}
#اگر_تعریف_شدہ FINAL_WITH_TLS_PSK
			اگر(context->listener->psk_hint){
				/* Client should have provided an identity to get this far. */
				اگر(!context->username){
					security__disconnect_auth(context);
					جاری;
				}
			}ورنہ
#اگر_ختم /* FINAL_WITH_TLS_PSK */
			{
				/* Free existing credentials and then recover them. */
				mosquitto_FREE(context->username);
				mosquitto_FREE(context->password);

				X509 *client_cert = SSL_get_peer_certificate(context->ssl);
				اگر(!client_cert){
					security__disconnect_auth(context);
					جاری;
				}
				name = X509_get_subject_name(client_cert);
				اگر(!name){
					X509_free(client_cert);
					security__disconnect_auth(context);
					جاری;
				}
				اگر(context->listener->use_شناختی_عنصر_as_username){   /* use_identity_as_username */
					صحیح i = X509_NAME_get_اشاریہ_by_NID(name, NID_commonName, -1);
					اگر(i == -1){
						X509_free(client_cert);
						security__disconnect_auth(context);
						جاری;
					}
					name_اندراج = X509_NAME_get_اندراج(name, i);
					اگر(name_اندراج){
						name_asn1 = X509_NAME_اندراج_get_ڈیٹا(name_اندراج);
						اگر(name_asn1 == تہی){
							X509_free(client_cert);
							security__disconnect_auth(context);
							جاری;
						}
						ثابت حرف *username = (ثابت حرف *)ASN1_STRING_get0_ڈیٹا(name_asn1);
						اگر(!username){
							X509_free(client_cert);
							client_cert = تہی;
							security__disconnect_auth(context);
							جاری;
						}
						context->username = mosquitto_strdup(username);
						اگر(!context->username){
							X509_free(client_cert);
							security__disconnect_auth(context);
							جاری;
						}
						/* Make sure there isn't an embedded NUL character in the CN */
						اگر((size_t)ASN1_STRING_لمبائی(name_asn1) != لڑی_لمبائی(context->username)){
							X509_free(client_cert);
							security__disconnect_auth(context);
							جاری;
						}
					}
				}ورنہ{   /* use_subject_as_username */
					subject_bio = BIO_new(BIO_s_mem());
					X509_NAME_print_ex(subject_bio, X509_get_subject_name(client_cert), 0, XN_FLAG_RFC2253);
					ڈیٹا_آغاز = تہی;
					name_لمبائی = (size_t)BIO_get_mem_ڈیٹا(subject_bio, &ڈیٹا_آغاز);
					subject = mosquitto_malloc(حجم(حرف)*name_لمبائی+1);
					اگر(!subject){
						BIO_free(subject_bio);
						X509_free(client_cert);
						security__disconnect_auth(context);
						جاری;
					}
					حافظہ_نقل(subject, ڈیٹا_آغاز, name_لمبائی);
					subject[name_لمبائی] = '\0';
					BIO_free(subject_bio);
					context->username = subject;
				}
				اگر(!context->username){
					X509_free(client_cert);
					security__disconnect_auth(context);
					جاری;
				}
				X509_free(client_cert);
			}
		}ورنہ
#اگر_ختم
		{
			/* Username/password check only if the identity/subject check not used */
			اگر(mosquitto_basic_auth(context) != MOSQ_ERR_SUCCESS){
				mosquitto_سیٹ_state(context, mosq_cs_disconnecting);
				do_disconnect(context, MOSQ_ERR_AUTH);
				جاری;
			}
		}
	}

	واپس MOSQ_ERR_SUCCESS;
}
