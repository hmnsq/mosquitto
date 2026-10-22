/*
Copyright (c) 2010-2021 Roger Light <roger@atchoo.org>

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

/* A note on matching topic subscriptions.
 *
 * Topics can be up to 32767 characters in length. The / character is used as a
 * hierarchy delimiter. Messages are published to a particular topic.
 * Clients may subscribe to particular topics directly, but may also use
 * wildcards in subscriptions.  The + and # characters are used as wildcards.
 * The # wildcard can be used at the end of a subscription only, and is a
 * wildcard for the level of hierarchy at which it is placed and all subsequent
 * levels.
 * The + wildcard may be used at any point within the subscription and is a
 * wildcard for only the level of hierarchy at which it is placed.
 * Neither wildcard may be used as part of a substring.
 * Valid:
 * 	a/b/+
 * 	a/+/c
 * 	a/#
 * 	a/b/#
 * 	#
 * 	+/b/c
 * 	+/+/+
 * Invalid:
 *	a/#/c
 *	a+/b/c
 * Valid but non-matching:
 *	a/b
 *	a/+
 *	+/b
 *	b/c/a
 *	a/b/d
 */

#అన్తర్భావ "config.h"

#అన్తర్భావ <assert.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <string.h>

#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "mosquitto/mqtt_protocol.h"
#అన్తర్భావ "util_mosq.h"

#అన్తర్భావ "utlist.h"

స్థిర నిర్మాణం mosquitto__subhier *sub_కూడు_hier_నమోదు(నిర్మాణం mosquitto__subhier *మాతృ, నిర్మాణం mosquitto__subhier **sibling, స్థిరరాశి అక్షరం *topic, uint16_t len);

స్థిర గుర్తులేని పూర్ణాంకం hashv_plus = 0;
స్థిర గుర్తులేని పూర్ణాంకం hashv_hash = 0;


స్థిర పూర్ణాంకం subs__send(నిర్మాణం mosquitto__subleaf *ఆకు, స్థిరరాశి అక్షరం *topic, uint8_t qos, పూర్ణాంకం retain, నిర్మాణం mosquitto_పునాది_సందేశం *stored)
{
	bool client_retain;
	uint16_t mid;
	uint8_t client_qos, సందేశం_qos;
	పూర్ణాంకం rc2;

	/* Check for ACL topic access. */
	rc2 = mosquitto_acl_check(ఆకు->context, topic, stored->దత్తాంశం.payloadlen, stored->దత్తాంశం.payload, stored->దత్తాంశం.qos, stored->దత్తాంశం.retain, stored->దత్తాంశం.properties, MOSQ_ACL_READ);
	ఒకవేళ(rc2 == MOSQ_ERR_ACL_DENIED){
		తిరిగి MOSQ_ERR_SUCCESS;
	}లేకపోతే ఒకవేళ(rc2 == MOSQ_ERR_SUCCESS){
		client_qos = MQTT_SUB_OPT_GET_QOS(ఆకు->subscription_options);

		ఒకవేళ(db.config->upgrade_outgoing_qos){
			సందేశం_qos = client_qos;
		}లేకపోతే{
			ఒకవేళ(qos > client_qos){
				సందేశం_qos = client_qos;
			}లేకపోతే{
				సందేశం_qos = qos;
			}
		}
		ఒకవేళ(సందేశం_qos){
			mid = mosquitto__mid_generate(ఆకు->context);
		}లేకపోతే{
			mid = 0;
		}
		ఒకవేళ(MQTT_SUB_OPT_GET_RETAIN_AS_PUBLISHED(ఆకు->subscription_options)){
			client_retain = retain;
		}లేకపోతే{
			client_retain = false;
		}
		ఒకవేళ(db_సందేశం_చొప్పించు_outgoing(ఆకు->context, 0, mid, సందేశం_qos, client_retain, stored, ఆకు->identifier, true, true) == 1){
			తిరిగి 1;
		}
	}లేకపోతే{
		తిరిగి 1; /* Application error */
	}
	తిరిగి 0;
}


స్థిర పూర్ణాంకం subs__shared_process(నిర్మాణం mosquitto__subhier *hier, స్థిరరాశి అక్షరం *topic, uint8_t qos, పూర్ణాంకం retain, నిర్మాణం mosquitto_పునాది_సందేశం *stored)
{
	పూర్ణాంకం rc = 0, rc2;
	నిర్మాణం mosquitto__subshared *shared, *shared_tmp;
	నిర్మాణం mosquitto__subleaf *ఆకు;

	HASH_ITER(hh, hier->shared, shared, shared_tmp){
		ఆకు = shared->subs;
		rc2 = subs__send(ఆకు, topic, qos, retain, stored);
		/* Remove current from the top, add back to the bottom */
		DL_DELETE(shared->subs, ఆకు);
		DL_APPEND(shared->subs, ఆకు);

		ఒకవేళ(rc2){
			rc = 1;
		}
	}

	తిరిగి rc;
}


స్థిర పూర్ణాంకం subs__process(నిర్మాణం mosquitto__subhier *hier, స్థిరరాశి అక్షరం *ఆధారం_id, స్థిరరాశి అక్షరం *topic, uint8_t qos, పూర్ణాంకం retain, నిర్మాణం mosquitto_పునాది_సందేశం *stored)
{
	పూర్ణాంకం rc = 0;
	పూర్ణాంకం rc2;
	నిర్మాణం mosquitto__subleaf *ఆకు;

	rc = subs__shared_process(hier, topic, qos, retain, stored);

	ఆకు = hier->subs;
	అయితే(ఆధారం_id && ఆకు){
		ఒకవేళ(!ఆకు->context->id || (MQTT_SUB_OPT_GET_NO_LOCAL(ఆకు->subscription_options) && !strcmp(ఆకు->context->id, ఆధారం_id))){
			ఆకు = ఆకు->తదుపరి;
			కొనసాగు;
		}
		rc2 = subs__send(ఆకు, topic, qos, retain, stored);
		ఒకవేళ(rc2){
			rc = 1;
		}
		ఆకు = ఆకు->తదుపరి;
	}
	ఒకవేళ(hier->subs || hier->shared){
		తిరిగి rc;
	}లేకపోతే{
		తిరిగి MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


స్థిర పూర్ణాంకం sub_కూడు_ఆకు(నిర్మాణం mosquitto *context, స్థిరరాశి నిర్మాణం mosquitto_subscription *sub, నిర్మాణం mosquitto__subleaf **తల, నిర్మాణం mosquitto__subleaf **newleaf)
{
	నిర్మాణం mosquitto__subleaf *ఆకు;

	*newleaf = ఖాళీ;
	ఆకు = *తల;

	అయితే(ఆకు){
		ఒకవేళ(ఆకు->context && ఆకు->context->id && !strcmp(ఆకు->context->id, context->id)){
			/* Client making a second subscription to same topic. Only
			 * need to update QoS. Return MOSQ_ERR_SUB_EXISTS to
			 * indicate this to the calling function. */
			ఆకు->identifier = sub->identifier;
			ఆకు->subscription_options = sub->options;
			తిరిగి MOSQ_ERR_SUB_EXISTS;
		}
		ఆకు = ఆకు->తదుపరి;
	}
	ఆకు = mosquitto_calloc(1, పరిమాణం(నిర్మాణం mosquitto__subleaf) + అక్షరశ్రేణి_పొడవు(sub->topic_filter) + 1);
	ఒకవేళ(!ఆకు){
		తిరిగి MOSQ_ERR_NOMEM;
	}
	ఆకు->context = context;
	ఆకు->identifier = sub->identifier;
	ఆకు->subscription_options = sub->options;
	strcpy(ఆకు->topic_filter, sub->topic_filter);

	DL_APPEND(*తల, ఆకు);
	*newleaf = ఆకు;

	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర శూన్య sub_remove_shared_ఆకు(నిర్మాణం mosquitto__subhier *subhier, నిర్మాణం mosquitto__subshared *shared, నిర్మాణం mosquitto__subleaf *ఆకు)
{
	DL_DELETE(shared->subs, ఆకు);
	ఒకవేళ(shared->subs == ఖాళీ){
		HASH_DELETE(hh, subhier->shared, shared);
		mosquitto_FREE(shared);
	}
}


స్థిర పూర్ణాంకం sub_కూడు_shared(నిర్మాణం mosquitto *context, స్థిరరాశి నిర్మాణం mosquitto_subscription *sub, నిర్మాణం mosquitto__subhier *subhier, స్థిరరాశి అక్షరం *sharename)
{
	నిర్మాణం mosquitto__subleaf *newleaf;
	నిర్మాణం mosquitto__subshared *shared = ఖాళీ;
	నిర్మాణం mosquitto__subleaf **subs;
	size_t slen;
	పూర్ణాంకం rc;
	గుర్తులేని hashv;

	slen = అక్షరశ్రేణి_పొడవు(sharename);

	HASH_విలువ(sharename, slen, hashv);

	HASH_కనుగొను_BYHASHVALUE(hh, subhier->shared, sharename, slen, hashv, shared);
	ఒకవేళ(shared == ఖాళీ){
		shared = mosquitto_calloc(1, పరిమాణం(నిర్మాణం mosquitto__subshared) + slen + 1);
		ఒకవేళ(!shared){
			తిరిగి MOSQ_ERR_NOMEM;
		}
		strncpy(shared->name, sharename, slen+1);

		HASH_కూడు_BYHASHVALUE(hh, subhier->shared, name, slen, hashv, shared);
	}

	rc = sub_కూడు_ఆకు(context, sub, &shared->subs, &newleaf);
	ఒకవేళ(rc > 0){
		ఒకవేళ(shared->subs == ఖాళీ){
			HASH_DELETE(hh, subhier->shared, shared);
			mosquitto_FREE(shared);
		}
		తిరిగి rc;
	}

	ఒకవేళ(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = shared;

		bool assigned = false;
		కొరకు(పూర్ణాంకం i=0; i<context->subs_సామర్థ్యం; i++){
			ఒకవేళ(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_లెక్క++;
				assigned = true;
				ఆపు;
			}
		}
		ఒకవేళ(assigned == false){
			subs = mosquitto_realloc(context->subs, పరిమాణం(నిర్మాణం mosquitto__subleaf *)*(size_t)(context->subs_సామర్థ్యం + 1));
			ఒకవేళ(!subs){
				sub_remove_shared_ఆకు(subhier, shared, newleaf);
				mosquitto_FREE(newleaf);
				తిరిగి MOSQ_ERR_NOMEM;
			}
			context->subs = subs;
			context->subs_సామర్థ్యం++;
			context->subs_లెక్క++;
			context->subs[context->subs_సామర్థ్యం-1] = newleaf;
		}
#నిర్వచితమైతే WITH_SYS_వృక్షం
		db.shared_subscription_లెక్క++;
#అయితే_ముగింపు
	}

	ఒకవేళ(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		తిరిగి rc;
	}లేకపోతే{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		తిరిగి MOSQ_ERR_SUCCESS;
	}
}


స్థిర పూర్ణాంకం sub_కూడు_సాధారణ(నిర్మాణం mosquitto *context, స్థిరరాశి నిర్మాణం mosquitto_subscription *sub, నిర్మాణం mosquitto__subhier *subhier)
{
	నిర్మాణం mosquitto__subleaf *newleaf = ఖాళీ;
	నిర్మాణం mosquitto__subleaf **subs;
	పూర్ణాంకం rc;

	rc = sub_కూడు_ఆకు(context, sub, &subhier->subs, &newleaf);
	ఒకవేళ(rc > 0){
		తిరిగి rc;
	}

	ఒకవేళ(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = ఖాళీ;

		bool assigned = false;
		కొరకు(పూర్ణాంకం i=0; i<context->subs_సామర్థ్యం; i++){
			ఒకవేళ(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_లెక్క++;
				assigned = true;
				ఆపు;
			}
		}
		ఒకవేళ(assigned == false){
			subs = mosquitto_realloc(context->subs, పరిమాణం(నిర్మాణం mosquitto__subleaf *)*(size_t)(context->subs_సామర్థ్యం + 1));
			ఒకవేళ(!subs){
				DL_DELETE(subhier->subs, newleaf);
				mosquitto_FREE(newleaf);
				తిరిగి MOSQ_ERR_NOMEM;
			}
			context->subs = subs;
			context->subs_సామర్థ్యం++;
			context->subs_లెక్క++;
			context->subs[context->subs_సామర్థ్యం-1] = newleaf;
		}
#నిర్వచితమైతే WITH_SYS_వృక్షం
		db.subscription_లెక్క++;
#అయితే_ముగింపు
	}

	ఒకవేళ(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		తిరిగి rc;
	}లేకపోతే{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		తిరిగి MOSQ_ERR_SUCCESS;
	}
}


స్థిర పూర్ణాంకం sub_కూడు_context(నిర్మాణం mosquitto *context, స్థిరరాశి నిర్మాణం mosquitto_subscription *sub, నిర్మాణం mosquitto__subhier *subhier, అక్షరం *స్థిరరాశి *స్థిరరాశి topics, స్థిరరాశి అక్షరం *sharename)
{
	నిర్మాణం mosquitto__subhier *branch;
	పూర్ణాంకం topic_సూచీ = 0;
	size_t topiclen;

	/* Find leaf node */
	అయితే(topics && topics[topic_సూచీ] != ఖాళీ){
		topiclen = అక్షరశ్రేణి_పొడవు(topics[topic_సూచీ]);
		ఒకవేళ(topiclen > UINT16_MAX){
			తిరిగి MOSQ_ERR_INVAL;
		}
		HASH_కనుగొను(hh, subhier->children, topics[topic_సూచీ], topiclen, branch);
		ఒకవేళ(!branch){
			/* Not found */
			branch = sub_కూడు_hier_నమోదు(subhier, &subhier->children, topics[topic_సూచీ], (uint16_t)topiclen);
			ఒకవేళ(!branch){
				తిరిగి MOSQ_ERR_NOMEM;
			}
		}
		subhier = branch;
		topic_సూచీ++;
	}

	/* Add add our context */
	ఒకవేళ(context && context->id){
		ఒకవేళ(sharename){
			తిరిగి sub_కూడు_shared(context, sub, subhier, sharename);
		}లేకపోతే{
			తిరిగి sub_కూడు_సాధారణ(context, sub, subhier);
		}
	}లేకపోతే{
		తిరిగి MOSQ_ERR_SUCCESS;
	}
}


స్థిర పూర్ణాంకం sub_remove_సాధారణ(నిర్మాణం mosquitto *context, నిర్మాణం mosquitto__subhier *subhier, uint8_t *reason)
{
	నిర్మాణం mosquitto__subleaf *ఆకు;

	ఆకు = subhier->subs;
	అయితే(ఆకు){
		ఒకవేళ(ఆకు->context==context){
#నిర్వచితమైతే WITH_SYS_వృక్షం
			db.subscription_లెక్క--;
#అయితే_ముగింపు
			DL_DELETE(subhier->subs, ఆకు);

			/* Remove the reference to the sub that the client is keeping.
			 * It would be nice to be able to use the reference directly,
			 * but that would involve keeping a copy of the topic string in
			 * each subleaf. Might be worth considering though. */
			కొరకు(పూర్ణాంకం i=0; i<context->subs_సామర్థ్యం; i++){
				ఒకవేళ(context->subs[i] && context->subs[i]->hier == subhier){
					context->subs_లెక్క--;
					mosquitto_free(context->subs[i]);
					context->subs[i] = ఖాళీ;
					ఆపు;
				}
			}
			*reason = 0;
			తిరిగి MOSQ_ERR_SUCCESS;
		}
		ఆకు = ఆకు->తదుపరి;
	}
	తిరిగి MOSQ_ERR_NO_SUBSCRIBERS;
}


స్థిర పూర్ణాంకం sub__remove_shared(నిర్మాణం mosquitto *context, నిర్మాణం mosquitto__subhier *subhier, uint8_t *reason, స్థిరరాశి అక్షరం *sharename)
{
	నిర్మాణం mosquitto__subshared *shared;

	HASH_కనుగొను(hh, subhier->shared, sharename, అక్షరశ్రేణి_పొడవు(sharename), shared);
	ఒకవేళ(shared){
		నిర్మాణం mosquitto__subleaf *ఆకు = shared->subs;
		అయితే(ఆకు){
			ఒకవేళ(ఆకు->context==context){
#నిర్వచితమైతే WITH_SYS_వృక్షం
				db.shared_subscription_లెక్క--;
#అయితే_ముగింపు
				DL_DELETE(shared->subs, ఆకు);

				/* Remove the reference to the sub that the client is keeping.
				* It would be nice to be able to use the reference directly,
				* but that would involve keeping a copy of the topic string in
				* each subleaf. Might be worth considering though. */
				కొరకు(పూర్ణాంకం i=0; i<context->subs_సామర్థ్యం; i++){
					ఒకవేళ(context->subs[i]
							&& context->subs[i]->hier == subhier
							&& context->subs[i]->shared == shared){

						mosquitto_free(context->subs[i]);
						context->subs[i] = ఖాళీ;
						context->subs_లెక్క--;
						ఆపు;
					}
				}

				ఒకవేళ(shared->subs == ఖాళీ){
					HASH_DELETE(hh, subhier->shared, shared);
					mosquitto_FREE(shared);
				}

				*reason = 0;
				తిరిగి MOSQ_ERR_SUCCESS;
			}
			ఆకు = ఆకు->తదుపరి;
		}
		తిరిగి MOSQ_ERR_NO_SUBSCRIBERS;
	}లేకపోతే{
		తిరిగి MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


స్థిర పూర్ణాంకం sub__remove_recurse(నిర్మాణం mosquitto *context, నిర్మాణం mosquitto__subhier *subhier, అక్షరం **topics, uint8_t *reason, స్థిరరాశి అక్షరం *sharename)
{
	నిర్మాణం mosquitto__subhier *branch;

	ఒకవేళ(topics == ఖాళీ || topics[0] == ఖాళీ){
		ఒకవేళ(sharename){
			తిరిగి sub__remove_shared(context, subhier, reason, sharename);
		}లేకపోతే{
			తిరిగి sub_remove_సాధారణ(context, subhier, reason);
		}
	}

	HASH_కనుగొను(hh, subhier->children, topics[0], అక్షరశ్రేణి_పొడవు(topics[0]), branch);
	ఒకవేళ(branch){
		sub__remove_recurse(context, branch, &(topics[1]), reason, sharename);
		ఒకవేళ(!branch->children && !branch->subs && !branch->shared){
			HASH_DELETE(hh, subhier->children, branch);
			mosquitto_FREE(branch);
		}
	}
	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర పూర్ణాంకం sub_శోధన(నిర్మాణం mosquitto__subhier *subhier, అక్షరం **విభజించు_topics, స్థిరరాశి అక్షరం *ఆధారం_id, స్థిరరాశి అక్షరం *topic, uint8_t qos, పూర్ణాంకం retain, నిర్మాణం mosquitto_పునాది_సందేశం *stored)
{
	/* FIXME - need to take into account source_id if the client is a bridge */
	నిర్మాణం mosquitto__subhier *branch;
	పూర్ణాంకం rc;
	bool have_subscribers = false;

	ఒకవేళ(విభజించు_topics && విభజించు_topics[0]){
		/* Check for literal match */
		HASH_కనుగొను(hh, subhier->children, విభజించు_topics[0], అక్షరశ్రేణి_పొడవు(విభజించు_topics[0]), branch);

		ఒకవేళ(branch){
			rc = sub_శోధన(branch, &(విభజించు_topics[1]), ఆధారం_id, topic, qos, retain, stored);
			ఒకవేళ(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}లేకపోతే ఒకవేళ(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				తిరిగి rc;
			}
			ఒకవేళ(విభజించు_topics[1] == ఖాళీ){ /* End of list */
				rc = subs__process(branch, ఆధారం_id, topic, qos, retain, stored);
				ఒకవేళ(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}లేకపోతే ఒకవేళ(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					తిరిగి rc;
				}
			}
		}

		/* Check for + match */
		HASH_కనుగొను_BYHASHVALUE(hh, subhier->children, "+", 1, hashv_plus, branch);

		ఒకవేళ(branch){
			rc = sub_శోధన(branch, &(విభజించు_topics[1]), ఆధారం_id, topic, qos, retain, stored);
			ఒకవేళ(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}లేకపోతే ఒకవేళ(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				తిరిగి rc;
			}
			ఒకవేళ(విభజించు_topics[1] == ఖాళీ){ /* End of list */
				rc = subs__process(branch, ఆధారం_id, topic, qos, retain, stored);
				ఒకవేళ(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}లేకపోతే ఒకవేళ(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					తిరిగి rc;
				}
			}
		}
	}

	/* Check for # match */
	HASH_కనుగొను_BYHASHVALUE(hh, subhier->children, "#", 1, hashv_hash, branch);
	ఒకవేళ(branch && !branch->children){
		/* The topic matches due to a # wildcard - process the
		 * subscriptions but *don't* return. Although this branch has ended
		 * there may still be other subscriptions to deal with.
		 */
		rc = subs__process(branch, ఆధారం_id, topic, qos, retain, stored);
		ఒకవేళ(rc == MOSQ_ERR_SUCCESS){
			have_subscribers = true;
		}లేకపోతే ఒకవేళ(rc != MOSQ_ERR_NO_SUBSCRIBERS){
			తిరిగి rc;
		}
	}

	ఒకవేళ(have_subscribers){
		తిరిగి MOSQ_ERR_SUCCESS;
	}లేకపోతే{
		తిరిగి MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


స్థిర నిర్మాణం mosquitto__subhier *sub_కూడు_hier_నమోదు(నిర్మాణం mosquitto__subhier *మాతృ, నిర్మాణం mosquitto__subhier **sibling, స్థిరరాశి అక్షరం *topic, uint16_t len)
{
	నిర్మాణం mosquitto__subhier *బిడ్డ;

	ప్రతిజ్ఞా(sibling);

	బిడ్డ = mosquitto_calloc(1, పరిమాణం(నిర్మాణం mosquitto__subhier) + len + 1);
	ఒకవేళ(!బిడ్డ){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
		తిరిగి ఖాళీ;
	}
	బిడ్డ->మాతృ = మాతృ;
	బిడ్డ->topic_len = len;
	ఒకవేళ(len > 0){
		strncpy(బిడ్డ->topic, topic, (size_t)(len+1));
	}

	HASH_కూడు(hh, *sibling, topic, బిడ్డ->topic_len, బిడ్డ);

	తిరిగి బిడ్డ;
}


పూర్ణాంకం sub_కూడు(నిర్మాణం mosquitto *context, స్థిరరాశి నిర్మాణం mosquitto_subscription *sub)
{
	పూర్ణాంకం rc = 0;
	నిర్మాణం mosquitto__subhier *subhier;
	స్థిరరాశి అక్షరం *sharename = ఖాళీ;
	అక్షరం *local_sub;
	అక్షరం **topics;
	size_t topiclen;

	ప్రతిజ్ఞా(sub);
	ప్రతిజ్ఞా(sub->topic_filter);

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &topics, &sharename);
	ఒకవేళ(rc){
		తిరిగి rc;
	}

	topiclen = అక్షరశ్రేణి_పొడవు(topics[0]);
	ఒకవేళ(topiclen > UINT16_MAX){
		mosquitto_FREE(local_sub);
		mosquitto_FREE(topics);
		తిరిగి MOSQ_ERR_INVAL;
	}

	ఒకవేళ(sharename){
		HASH_కనుగొను(hh, db.shared_subs, topics[0], topiclen, subhier);
		ఒకవేళ(!subhier){
			subhier = sub_కూడు_hier_నమోదు(ఖాళీ, &db.shared_subs, topics[0], (uint16_t)topiclen);
			ఒకవేళ(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
				తిరిగి MOSQ_ERR_NOMEM;
			}
		}
	}లేకపోతే{
		HASH_కనుగొను(hh, db.సాధారణ_subs, topics[0], topiclen, subhier);
		ఒకవేళ(!subhier){
			subhier = sub_కూడు_hier_నమోదు(ఖాళీ, &db.సాధారణ_subs, topics[0], (uint16_t)topiclen);
			ఒకవేళ(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
				తిరిగి MOSQ_ERR_NOMEM;
			}
		}
	}
	rc = sub_కూడు_context(context, sub, subhier, topics, sharename);

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	తిరిగి rc;
}


పూర్ణాంకం sub__remove(నిర్మాణం mosquitto *context, స్థిరరాశి అక్షరం *sub, uint8_t *reason)
{
	పూర్ణాంకం rc = 0;
	నిర్మాణం mosquitto__subhier *subhier;
	స్థిరరాశి అక్షరం *sharename = ఖాళీ;
	అక్షరం *local_sub = ఖాళీ;
	అక్షరం **topics = ఖాళీ;

	ప్రతిజ్ఞా(sub);

	rc = sub__topic_tokenise(sub, &local_sub, &topics, &sharename);
	ఒకవేళ(rc){
		తిరిగి rc;
	}

	ఒకవేళ(sharename){
		HASH_కనుగొను(hh, db.shared_subs, topics[0], అక్షరశ్రేణి_పొడవు(topics[0]), subhier);
	}లేకపోతే{
		HASH_కనుగొను(hh, db.సాధారణ_subs, topics[0], అక్షరశ్రేణి_పొడవు(topics[0]), subhier);
	}
	ఒకవేళ(subhier){
		*reason = MQTT_RC_NO_SUBSCRIPTION_EXISTED;
		rc = sub__remove_recurse(context, subhier, topics, reason, sharename);
	}

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	తిరిగి rc;
}


పూర్ణాంకం sub_messages_వరుస(స్థిరరాశి అక్షరం *ఆధారం_id, uint8_t qos, నిర్మాణం mosquitto_పునాది_సందేశం **stored)
{
	పూర్ణాంకం rc = MOSQ_ERR_SUCCESS, rc2;
	పూర్ణాంకం rc_సాధారణ = MOSQ_ERR_NO_SUBSCRIBERS, rc_shared = MOSQ_ERR_NO_SUBSCRIBERS;
	నిర్మాణం mosquitto__subhier *subhier;
	అక్షరం **విభజించు_topics = ఖాళీ;
	అక్షరం *local_topic = ఖాళీ;
	గుర్తులేని hashv;
	size_t topiclen;
	bool retain = (*stored)->దత్తాంశం.retain;
	స్థిరరాశి అక్షరం *topic = (*stored)->దత్తాంశం.topic;

	ప్రతిజ్ఞా(topic);

	ఒకవేళ(sub__topic_tokenise(topic, &local_topic, &విభజించు_topics, ఖాళీ)){
		తిరిగి 1;
	}

	/* Protect this message until we have sent it to all
	clients - this is required because websockets client calls
	db__message_write(), which could remove the message if ref_count==0.
	*/
	db_సందేశం_store_ref_inc(*stored);

	topiclen = అక్షరశ్రేణి_పొడవు(విభజించు_topics[0]);
	HASH_విలువ(విభజించు_topics[0], topiclen, hashv);
	HASH_కనుగొను_BYHASHVALUE(hh, db.సాధారణ_subs, విభజించు_topics[0], topiclen, hashv, subhier);
	ఒకవేళ(subhier){
		rc_సాధారణ = sub_శోధన(subhier, విభజించు_topics, ఆధారం_id, topic, qos, retain, *stored);
		ఒకవేళ(rc_సాధారణ > 0){
			rc = rc_సాధారణ;
			వెళ్ళు చివర;
		}
	}

	HASH_కనుగొను_BYHASHVALUE(hh, db.shared_subs, విభజించు_topics[0], topiclen, hashv, subhier);
	ఒకవేళ(subhier){
		rc_shared = sub_శోధన(subhier, విభజించు_topics, ఆధారం_id, topic, qos, retain, *stored);
		ఒకవేళ(rc_shared > 0){
			rc = rc_shared;
			వెళ్ళు చివర;
		}
	}

	ఒకవేళ(rc_సాధారణ == MOSQ_ERR_NO_SUBSCRIBERS && rc_shared == MOSQ_ERR_NO_SUBSCRIBERS){
		rc = MOSQ_ERR_NO_SUBSCRIBERS;
	}

	ఒకవేళ(retain){
		rc2 = retain__store(topic, *stored, విభజించు_topics, true);
		ఒకవేళ(rc2){
			rc = rc2;
		}
	}

చివర:
	mosquitto_FREE(విభజించు_topics);
	mosquitto_FREE(local_topic);
	/* Remove our reference and free if needed. */
	db_సందేశం_store_ref_dec(stored);

	తిరిగి rc;
}


/* Remove a subhier element, and return its parent if that needs freeing as well. */
స్థిర నిర్మాణం mosquitto__subhier *tmp_remove_subs(నిర్మాణం mosquitto__subhier *sub)
{
	నిర్మాణం mosquitto__subhier *మాతృ;

	ఒకవేళ(!sub || !sub->మాతృ){
		తిరిగి ఖాళీ;
	}

	ఒకవేళ(sub->children || sub->subs){
		తిరిగి ఖాళీ;
	}

	మాతృ = sub->మాతృ;
	HASH_DELETE(hh, మాతృ->children, sub);
	mosquitto_FREE(sub);

	ఒకవేళ(మాతృ->subs == ఖాళీ
			&& మాతృ->children == ఖాళీ
			&& మాతృ->shared == ఖాళీ
			&& మాతృ->మాతృ){

		తిరిగి మాతృ;
	}లేకపోతే{
		తిరిగి ఖాళీ;
	}
}


/* Remove all subscriptions for a client.
 */
పూర్ణాంకం sub__clean_session(నిర్మాణం mosquitto *context)
{
	కొరకు(పూర్ణాంకం i=0; i<context->subs_సామర్థ్యం; i++){
		ఒకవేళ(context->subs[i] == ఖాళీ || context->subs[i]->hier == ఖాళీ){
			కొనసాగు;
		}

		నిర్మాణం mosquitto__subhier *hier = context->subs[i]->hier;
		నిర్మాణం mosquitto__subleaf *ఆకు;

		plugin_persist__handle_subscription_delete(context, context->subs[i]->topic_filter);
		ఒకవేళ(context->subs[i]->shared){
			ఆకు = context->subs[i]->shared->subs;
			అయితే(ఆకు){
				ఒకవేళ(ఆకు->context==context){
#నిర్వచితమైతే WITH_SYS_వృక్షం
					db.shared_subscription_లెక్క--;
#అయితే_ముగింపు
					sub_remove_shared_ఆకు(context->subs[i]->hier, context->subs[i]->shared, ఆకు);
					ఆపు;
				}
				ఆకు = ఆకు->తదుపరి;
			}
		}లేకపోతే{
			ఆకు = hier->subs;
			అయితే(ఆకు){
				ఒకవేళ(ఆకు->context==context){
#నిర్వచితమైతే WITH_SYS_వృక్షం
					db.subscription_లెక్క--;
#అయితే_ముగింపు
					DL_DELETE(hier->subs, ఆకు);
					ఆపు;
				}
				ఆకు = ఆకు->తదుపరి;
			}
		}
		mosquitto_FREE(context->subs[i]);

		ఒకవేళ(hier->subs == ఖాళీ
				&& hier->children == ఖాళీ
				&& hier->shared == ఖాళీ
				&& hier->మాతృ){

			చేయి{
				hier = tmp_remove_subs(hier);
			}అయితే(hier);
		}
	}
	mosquitto_FREE(context->subs);
	context->subs_సామర్థ్యం = 0;
	context->subs_లెక్క = 0;

	తిరిగి MOSQ_ERR_SUCCESS;
}


శూన్య sub_వృక్షం_print(నిర్మాణం mosquitto__subhier *మూలం, పూర్ణాంకం level)
{
	పూర్ణాంకం i;
	నిర్మాణం mosquitto__subhier *branch, *branch_tmp;
	నిర్మాణం mosquitto__subleaf *ఆకు;

	HASH_ITER(hh, మూలం, branch, branch_tmp){
		ఒకవేళ(level > -1){
			కొరకు(i=0; i<(level+2)*2; i++){
				రాయి(" ");
			}
			రాయి("%s", branch->topic);
			ఆకు = branch->subs;
			అయితే(ఆకు){
				ఒకవేళ(ఆకు->context){
					రాయి(" (%s, %d)", ఆకు->context->id, MQTT_SUB_OPT_GET_QOS(ఆకు->subscription_options));
				}లేకపోతే{
					రాయి(" (%s, %d)", "", MQTT_SUB_OPT_GET_QOS(ఆకు->subscription_options));
				}
				ఆకు = ఆకు->తదుపరి;
			}
			రాయి("\n");
		}

		sub_వృక్షం_print(branch->children, level+1);
	}
}


పూర్ణాంకం sub__init(శూన్య)
{
	HASH_విలువ("+", 1, hashv_plus);
	HASH_విలువ("#", 1, hashv_hash);

	ఒకవేళ(sub_కూడు_hier_నమోదు(ఖాళీ, &db.shared_subs, "", 0) == ఖాళీ
			|| sub_కూడు_hier_నమోదు(ఖాళీ, &db.సాధారణ_subs, "", 0) == ఖాళీ
			|| sub_కూడు_hier_నమోదు(ఖాళీ, &db.సాధారణ_subs, "$SYS", (uint16_t)అక్షరశ్రేణి_పొడవు("$SYS")) == ఖాళీ
			){

		తిరిగి MOSQ_ERR_NOMEM;
	}లేకపోతే{
		తిరిగి MOSQ_ERR_SUCCESS;
	}
}
