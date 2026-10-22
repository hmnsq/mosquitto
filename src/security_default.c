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

#అన్తర్భావ "config.h"

#అన్తర్భావ <ctype.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <string.h>

#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "mosquitto/mqtt_protocol.h"
#అన్తర్భావ "password_file.h"
#అన్తర్భావ "send_mosq.h"
#అన్తర్భావ "util_mosq.h"


పూర్ణాంకం mosquitto_security_init_default(శూన్య)
{
	పూర్ణాంకం rc;

	/* Configure plugin identifier */
	ఒకవేళ(db.config->చొప్పున_listener_settings){
		కొరకు(పూర్ణాంకం i=0; i<db.config->listener_లెక్క; i++){
			db.config->listeners[i].security_options->pid = mosquitto_calloc(1, పరిమాణం(mosquitto_plugin_id_t));
			ఒకవేళ(db.config->listeners[i].security_options->pid == ఖాళీ){
				log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
				తిరిగి MOSQ_ERR_NOMEM;
			}
			db.config->listeners[i].security_options->pid->plugin_name = mosquitto_strdup("builtin-security");
			db.config->listeners[i].security_options->pid->listener = &db.config->listeners[i];
			config_plugin_కూడు_secopt(db.config->listeners[i].security_options->pid, db.config->listeners[i].security_options);
		}
	}లేకపోతే{
		db.config->security_options.pid = mosquitto_calloc(1, పరిమాణం(mosquitto_plugin_id_t));
		ఒకవేళ(db.config->security_options.pid == ఖాళీ){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
			తిరిగి MOSQ_ERR_NOMEM;
		}
		db.config->security_options.pid->plugin_name = mosquitto_strdup("builtin-security");
		config_plugin_కూడు_secopt(db.config->security_options.pid, &db.config->security_options);
	}

	rc = broker_password_file__init();
	ఒకవేళ(rc){
		తిరిగి rc;
	}

	rc = broker_acl_file__init();
	ఒకవేళ(rc){
		తిరిగి rc;
	}

	rc = psk_file__init();
	ఒకవేళ(rc){
		తిరిగి rc;
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం mosquitto_security_cleanup_default(శూన్య)
{
	పూర్ణాంకం rc = 0;

	broker_password_file__cleanup();
	broker_acl_file__cleanup();

	rc = psk_file__cleanup();
	ఒకవేళ(rc != MOSQ_ERR_SUCCESS){
		తిరిగి rc;
	}

	ఒకవేళ(db.config->చొప్పున_listener_settings){
		కొరకు(పూర్ణాంకం i=0; i<db.config->listener_లెక్క; i++){
			ఒకవేళ(db.config->listeners[i].security_options->pid){
				mosquitto_FREE(db.config->listeners[i].security_options->pid->plugin_name);
				mosquitto_FREE(db.config->listeners[i].security_options->pid->config.security_options);
				mosquitto_FREE(db.config->listeners[i].security_options->pid);
			}
		}
	}లేకపోతే{
		ఒకవేళ(db.config->security_options.pid){
			mosquitto_FREE(db.config->security_options.pid->plugin_name);
			mosquitto_FREE(db.config->security_options.pid->config.security_options);
			mosquitto_FREE(db.config->security_options.pid);
		}
	}
	తిరిగి MOSQ_ERR_SUCCESS;
}


#నిర్వచితమైతే WITH_TLS


స్థిర శూన్య security__disconnect_auth(నిర్మాణం mosquitto *context)
{
	ఒకవేళ(context->protocol == mosq_p_mqtt5){
		send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, ఖాళీ);
	}
	mosquitto_సమితి_state(context, mosq_cs_disconnecting);
	do_disconnect(context, MOSQ_ERR_AUTH);
}
#అయితే_ముగింపు


/* Apply security settings after a reload.
 * Includes:
 * - Disconnecting anonymous users if appropriate
 * - Disconnecting users with invalid passwords
 * - Reapplying ACLs
 */
పూర్ణాంకం mosquitto_security_apply_default(శూన్య)
{
	నిర్మాణం mosquitto *context, *ctxt_tmp = ఖాళీ;
	bool allow_anonymous;
#నిర్వచితమైతే WITH_TLS
	X509_NAME *name;
	X509_NAME_నమోదు *name_నమోదు;
	ASN1_STRING *name_asn1 = ఖాళీ;
	నిర్మాణం mosquitto__listener *listener;
	BIO *subject_bio;
	అక్షరం *దత్తాంశం_ప్రారంభం;
	size_t name_నిడివి;
	అక్షరం *subject;
#అయితే_ముగింపు

#నిర్వచితమైతే WITH_TLS
	కొరకు(పూర్ణాంకం i=0; i<db.config->listener_లెక్క; i++){
		listener = &db.config->listeners[i];
		ఒకవేళ(listener && listener->ssl_ctx && listener->certfile && listener->keyfile && listener->crlfile && listener->require_certificate){
			ఒకవేళ(net__tls_server_ctx(listener)){
				తిరిగి MOSQ_ERR_TLS;
			}

			ఒకవేళ(net_tls_ఎక్కించు_verify(listener)){
				తిరిగి MOSQ_ERR_TLS;
			}
		}
	}
#అయితే_ముగింపు

	HASH_ITER(hh_id, db.contexts_by_id, context, ctxt_tmp){
		ఒకవేళ(context->bridge){
			కొనసాగు;
		}

		ఒకవేళ((context->listener && context->listener->security_options->allow_anonymous == true)
				|| (!db.config->చొప్పున_listener_settings && db.config->security_options.allow_anonymous == true
				&& context->listener && context->listener->security_options->allow_anonymous != false)){
			allow_anonymous = true;
		}లేకపోతే{
			allow_anonymous = false;
		}

		ఒకవేళ(!allow_anonymous && !context->username){
			mosquitto_సమితి_state(context, mosq_cs_disconnecting);
			do_disconnect(context, MOSQ_ERR_AUTH);
			కొనసాగు;
		}

		/* Check for connected clients that are no longer authorised */
#నిర్వచితమైతే WITH_TLS
		ఒకవేళ(context->listener && context->listener->ssl_ctx && (context->listener->use_తటస్థం_as_username || context->listener->use_subject_as_username)){
			/* Client must have either a valid certificate, or valid PSK used as a username. */
			ఒకవేళ(!context->ssl){
				ఒకవేళ(context->protocol == mosq_p_mqtt5){
					send__disconnect(context, MQTT_RC_ADMINISTRATIVE_ACTION, ఖాళీ);
				}
				mosquitto_సమితి_state(context, mosq_cs_disconnecting);
				do_disconnect(context, MOSQ_ERR_AUTH);
				కొనసాగు;
			}
#నిర్వచితమైతే FINAL_WITH_TLS_PSK
			ఒకవేళ(context->listener->psk_hint){
				/* Client should have provided an identity to get this far. */
				ఒకవేళ(!context->username){
					security__disconnect_auth(context);
					కొనసాగు;
				}
			}లేకపోతే
#అయితే_ముగింపు /* FINAL_WITH_TLS_PSK */
			{
				/* Free existing credentials and then recover them. */
				mosquitto_FREE(context->username);
				mosquitto_FREE(context->password);

				X509 *client_cert = SSL_get_peer_certificate(context->ssl);
				ఒకవేళ(!client_cert){
					security__disconnect_auth(context);
					కొనసాగు;
				}
				name = X509_get_subject_name(client_cert);
				ఒకవేళ(!name){
					X509_free(client_cert);
					security__disconnect_auth(context);
					కొనసాగు;
				}
				ఒకవేళ(context->listener->use_తటస్థం_as_username){   /* use_identity_as_username */
					పూర్ణాంకం i = X509_NAME_get_సూచీ_by_NID(name, NID_commonName, -1);
					ఒకవేళ(i == -1){
						X509_free(client_cert);
						security__disconnect_auth(context);
						కొనసాగు;
					}
					name_నమోదు = X509_NAME_get_నమోదు(name, i);
					ఒకవేళ(name_నమోదు){
						name_asn1 = X509_NAME_నమోదు_get_దత్తాంశం(name_నమోదు);
						ఒకవేళ(name_asn1 == ఖాళీ){
							X509_free(client_cert);
							security__disconnect_auth(context);
							కొనసాగు;
						}
						స్థిరరాశి అక్షరం *username = (స్థిరరాశి అక్షరం *)ASN1_STRING_get0_దత్తాంశం(name_asn1);
						ఒకవేళ(!username){
							X509_free(client_cert);
							client_cert = ఖాళీ;
							security__disconnect_auth(context);
							కొనసాగు;
						}
						context->username = mosquitto_strdup(username);
						ఒకవేళ(!context->username){
							X509_free(client_cert);
							security__disconnect_auth(context);
							కొనసాగు;
						}
						/* Make sure there isn't an embedded NUL character in the CN */
						ఒకవేళ((size_t)ASN1_STRING_నిడివి(name_asn1) != అక్షరశ్రేణి_పొడవు(context->username)){
							X509_free(client_cert);
							security__disconnect_auth(context);
							కొనసాగు;
						}
					}
				}లేకపోతే{   /* use_subject_as_username */
					subject_bio = BIO_new(BIO_s_mem());
					X509_NAME_print_ex(subject_bio, X509_get_subject_name(client_cert), 0, XN_FLAG_RFC2253);
					దత్తాంశం_ప్రారంభం = ఖాళీ;
					name_నిడివి = (size_t)BIO_get_mem_దత్తాంశం(subject_bio, &దత్తాంశం_ప్రారంభం);
					subject = mosquitto_malloc(పరిమాణం(అక్షరం)*name_నిడివి+1);
					ఒకవేళ(!subject){
						BIO_free(subject_bio);
						X509_free(client_cert);
						security__disconnect_auth(context);
						కొనసాగు;
					}
					జ్ఞాపకం_నకలు(subject, దత్తాంశం_ప్రారంభం, name_నిడివి);
					subject[name_నిడివి] = '\0';
					BIO_free(subject_bio);
					context->username = subject;
				}
				ఒకవేళ(!context->username){
					X509_free(client_cert);
					security__disconnect_auth(context);
					కొనసాగు;
				}
				X509_free(client_cert);
			}
		}లేకపోతే
#అయితే_ముగింపు
		{
			/* Username/password check only if the identity/subject check not used */
			ఒకవేళ(mosquitto_basic_auth(context) != MOSQ_ERR_SUCCESS){
				mosquitto_సమితి_state(context, mosq_cs_disconnecting);
				do_disconnect(context, MOSQ_ERR_AUTH);
				కొనసాగు;
			}
		}
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}
