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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/mqtt_protocol.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"

#ಅನ್ತರ್ಭಾವ "utlist.h"

ಸ್ಥಿರ ರಚನೆ mosquitto__subhier *sub_ಕೂಡಿಸು_hier_ನಮೂದು(ರಚನೆ mosquitto__subhier *ಪೋಷಕ, ರಚನೆ mosquitto__subhier **sibling, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *topic, uint16_t len);

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ hashv_plus = 0;
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ hashv_hash = 0;


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ subs__send(ರಚನೆ mosquitto__subleaf *ಎಲೆ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *topic, uint8_t qos, ಪೂರ್ಣಾಂಕ retain, ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *stored)
{
	bool client_retain;
	uint16_t mid;
	uint8_t client_qos, ಸಂದೇಶ_qos;
	ಪೂರ್ಣಾಂಕ rc2;

	/* Check for ACL topic access. */
	rc2 = mosquitto_acl_check(ಎಲೆ->context, topic, stored->ದತ್ತಾಂಶ.payloadlen, stored->ದತ್ತಾಂಶ.payload, stored->ದತ್ತಾಂಶ.qos, stored->ದತ್ತಾಂಶ.retain, stored->ದತ್ತಾಂಶ.properties, MOSQ_ACL_READ);
	ಆದರೆ(rc2 == MOSQ_ERR_ACL_DENIED){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc2 == MOSQ_ERR_SUCCESS){
		client_qos = MQTT_SUB_OPT_GET_QOS(ಎಲೆ->subscription_options);

		ಆದರೆ(db.config->upgrade_outgoing_qos){
			ಸಂದೇಶ_qos = client_qos;
		}ಇಲ್ಲದಿದ್ದರೆ{
			ಆದರೆ(qos > client_qos){
				ಸಂದೇಶ_qos = client_qos;
			}ಇಲ್ಲದಿದ್ದರೆ{
				ಸಂದೇಶ_qos = qos;
			}
		}
		ಆದರೆ(ಸಂದೇಶ_qos){
			mid = mosquitto__mid_generate(ಎಲೆ->context);
		}ಇಲ್ಲದಿದ್ದರೆ{
			mid = 0;
		}
		ಆದರೆ(MQTT_SUB_OPT_GET_RETAIN_AS_PUBLISHED(ಎಲೆ->subscription_options)){
			client_retain = retain;
		}ಇಲ್ಲದಿದ್ದರೆ{
			client_retain = false;
		}
		ಆದರೆ(db_ಸಂದೇಶ_ಸೇರಿಸು_outgoing(ಎಲೆ->context, 0, mid, ಸಂದೇಶ_qos, client_retain, stored, ಎಲೆ->identifier, true, true) == 1){
			ಹಿಂತಿರುಗಿ 1;
		}
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಹಿಂತಿರುಗಿ 1; /* Application error */
	}
	ಹಿಂತಿರುಗಿ 0;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ subs__shared_process(ರಚನೆ mosquitto__subhier *hier, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *topic, uint8_t qos, ಪೂರ್ಣಾಂಕ retain, ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *stored)
{
	ಪೂರ್ಣಾಂಕ rc = 0, rc2;
	ರಚನೆ mosquitto__subshared *shared, *shared_tmp;
	ರಚನೆ mosquitto__subleaf *ಎಲೆ;

	HASH_ITER(hh, hier->shared, shared, shared_tmp){
		ಎಲೆ = shared->subs;
		rc2 = subs__send(ಎಲೆ, topic, qos, retain, stored);
		/* Remove current from the top, add back to the bottom */
		DL_DELETE(shared->subs, ಎಲೆ);
		DL_APPEND(shared->subs, ಎಲೆ);

		ಆದರೆ(rc2){
			rc = 1;
		}
	}

	ಹಿಂತಿರುಗಿ rc;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ subs__process(ರಚನೆ mosquitto__subhier *hier, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ_id, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *topic, uint8_t qos, ಪೂರ್ಣಾಂಕ retain, ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *stored)
{
	ಪೂರ್ಣಾಂಕ rc = 0;
	ಪೂರ್ಣಾಂಕ rc2;
	ರಚನೆ mosquitto__subleaf *ಎಲೆ;

	rc = subs__shared_process(hier, topic, qos, retain, stored);

	ಎಲೆ = hier->subs;
	ಆಗ(ಮೂಲ_id && ಎಲೆ){
		ಆದರೆ(!ಎಲೆ->context->id || (MQTT_SUB_OPT_GET_NO_LOCAL(ಎಲೆ->subscription_options) && !strcmp(ಎಲೆ->context->id, ಮೂಲ_id))){
			ಎಲೆ = ಎಲೆ->ಮುಂದಿನ;
			ಮುಂದುವರಿ;
		}
		rc2 = subs__send(ಎಲೆ, topic, qos, retain, stored);
		ಆದರೆ(rc2){
			rc = 1;
		}
		ಎಲೆ = ಎಲೆ->ಮುಂದಿನ;
	}
	ಆದರೆ(hier->subs || hier->shared){
		ಹಿಂತಿರುಗಿ rc;
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sub_ಕೂಡಿಸು_ಎಲೆ(ರಚನೆ mosquitto *context, ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto_subscription *sub, ರಚನೆ mosquitto__subleaf **ತಲೆ, ರಚನೆ mosquitto__subleaf **newleaf)
{
	ರಚನೆ mosquitto__subleaf *ಎಲೆ;

	*newleaf = ಖಾಲಿ;
	ಎಲೆ = *ತಲೆ;

	ಆಗ(ಎಲೆ){
		ಆದರೆ(ಎಲೆ->context && ಎಲೆ->context->id && !strcmp(ಎಲೆ->context->id, context->id)){
			/* Client making a second subscription to same topic. Only
			 * need to update QoS. Return MOSQ_ERR_SUB_EXISTS to
			 * indicate this to the calling function. */
			ಎಲೆ->identifier = sub->identifier;
			ಎಲೆ->subscription_options = sub->options;
			ಹಿಂತಿರುಗಿ MOSQ_ERR_SUB_EXISTS;
		}
		ಎಲೆ = ಎಲೆ->ಮುಂದಿನ;
	}
	ಎಲೆ = mosquitto_calloc(1, ಗಾತ್ರ(ರಚನೆ mosquitto__subleaf) + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sub->topic_filter) + 1);
	ಆದರೆ(!ಎಲೆ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}
	ಎಲೆ->context = context;
	ಎಲೆ->identifier = sub->identifier;
	ಎಲೆ->subscription_options = sub->options;
	strcpy(ಎಲೆ->topic_filter, sub->topic_filter);

	DL_APPEND(*ತಲೆ, ಎಲೆ);
	*newleaf = ಎಲೆ;

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಶೂನ್ಯ sub_remove_shared_ಎಲೆ(ರಚನೆ mosquitto__subhier *subhier, ರಚನೆ mosquitto__subshared *shared, ರಚನೆ mosquitto__subleaf *ಎಲೆ)
{
	DL_DELETE(shared->subs, ಎಲೆ);
	ಆದರೆ(shared->subs == ಖಾಲಿ){
		HASH_DELETE(hh, subhier->shared, shared);
		mosquitto_FREE(shared);
	}
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sub_ಕೂಡಿಸು_shared(ರಚನೆ mosquitto *context, ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto_subscription *sub, ರಚನೆ mosquitto__subhier *subhier, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sharename)
{
	ರಚನೆ mosquitto__subleaf *newleaf;
	ರಚನೆ mosquitto__subshared *shared = ಖಾಲಿ;
	ರಚನೆ mosquitto__subleaf **subs;
	size_t slen;
	ಪೂರ್ಣಾಂಕ rc;
	ಚಿಹ್ನೆರಹಿತ hashv;

	slen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sharename);

	HASH_ಮೌಲ್ಯ(sharename, slen, hashv);

	HASH_ಕಂಡುಹಿಡಿ_BYHASHVALUE(hh, subhier->shared, sharename, slen, hashv, shared);
	ಆದರೆ(shared == ಖಾಲಿ){
		shared = mosquitto_calloc(1, ಗಾತ್ರ(ರಚನೆ mosquitto__subshared) + slen + 1);
		ಆದರೆ(!shared){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
		}
		strncpy(shared->name, sharename, slen+1);

		HASH_ಕೂಡಿಸು_BYHASHVALUE(hh, subhier->shared, name, slen, hashv, shared);
	}

	rc = sub_ಕೂಡಿಸು_ಎಲೆ(context, sub, &shared->subs, &newleaf);
	ಆದರೆ(rc > 0){
		ಆದರೆ(shared->subs == ಖಾಲಿ){
			HASH_DELETE(hh, subhier->shared, shared);
			mosquitto_FREE(shared);
		}
		ಹಿಂತಿರುಗಿ rc;
	}

	ಆದರೆ(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = shared;

		bool assigned = false;
		ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<context->subs_ಸಾಮರ್ಥ್ಯ; i++){
			ಆದರೆ(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_ಎಣಿಕೆ++;
				assigned = true;
				ನಿಲ್ಲಿಸು;
			}
		}
		ಆದರೆ(assigned == false){
			subs = mosquitto_realloc(context->subs, ಗಾತ್ರ(ರಚನೆ mosquitto__subleaf *)*(size_t)(context->subs_ಸಾಮರ್ಥ್ಯ + 1));
			ಆದರೆ(!subs){
				sub_remove_shared_ಎಲೆ(subhier, shared, newleaf);
				mosquitto_FREE(newleaf);
				ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
			}
			context->subs = subs;
			context->subs_ಸಾಮರ್ಥ್ಯ++;
			context->subs_ಎಣಿಕೆ++;
			context->subs[context->subs_ಸಾಮರ್ಥ್ಯ-1] = newleaf;
		}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_SYS_ವೃಕ್ಷ
		db.shared_subscription_ಎಣಿಕೆ++;
#ಆದರೆ_ಅಂತ್ಯ
	}

	ಆದರೆ(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		ಹಿಂತಿರುಗಿ rc;
	}ಇಲ್ಲದಿದ್ದರೆ{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sub_ಕೂಡಿಸು_ಸಾಮಾನ್ಯ(ರಚನೆ mosquitto *context, ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto_subscription *sub, ರಚನೆ mosquitto__subhier *subhier)
{
	ರಚನೆ mosquitto__subleaf *newleaf = ಖಾಲಿ;
	ರಚನೆ mosquitto__subleaf **subs;
	ಪೂರ್ಣಾಂಕ rc;

	rc = sub_ಕೂಡಿಸು_ಎಲೆ(context, sub, &subhier->subs, &newleaf);
	ಆದರೆ(rc > 0){
		ಹಿಂತಿರುಗಿ rc;
	}

	ಆದರೆ(rc != MOSQ_ERR_SUB_EXISTS){
		newleaf->hier = subhier;
		newleaf->shared = ಖಾಲಿ;

		bool assigned = false;
		ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<context->subs_ಸಾಮರ್ಥ್ಯ; i++){
			ಆದರೆ(!context->subs[i]){
				context->subs[i] = newleaf;
				context->subs_ಎಣಿಕೆ++;
				assigned = true;
				ನಿಲ್ಲಿಸು;
			}
		}
		ಆದರೆ(assigned == false){
			subs = mosquitto_realloc(context->subs, ಗಾತ್ರ(ರಚನೆ mosquitto__subleaf *)*(size_t)(context->subs_ಸಾಮರ್ಥ್ಯ + 1));
			ಆದರೆ(!subs){
				DL_DELETE(subhier->subs, newleaf);
				mosquitto_FREE(newleaf);
				ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
			}
			context->subs = subs;
			context->subs_ಸಾಮರ್ಥ್ಯ++;
			context->subs_ಎಣಿಕೆ++;
			context->subs[context->subs_ಸಾಮರ್ಥ್ಯ-1] = newleaf;
		}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_SYS_ವೃಕ್ಷ
		db.subscription_ಎಣಿಕೆ++;
#ಆದರೆ_ಅಂತ್ಯ
	}

	ಆದರೆ(context->protocol == mosq_p_mqtt31 || context->protocol == mosq_p_mqtt5){
		ಹಿಂತಿರುಗಿ rc;
	}ಇಲ್ಲದಿದ್ದರೆ{
		/* mqttv311/mqttv5 requires retained messages are resent on
		 * resubscribe. */
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sub_ಕೂಡಿಸು_context(ರಚನೆ mosquitto *context, ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto_subscription *sub, ರಚನೆ mosquitto__subhier *subhier, ಅಕ್ಷರ *ಸ್ಥಿರಾಂಕ *ಸ್ಥಿರಾಂಕ topics, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sharename)
{
	ರಚನೆ mosquitto__subhier *branch;
	ಪೂರ್ಣಾಂಕ topic_ಸೂಚ್ಯಂಕ = 0;
	size_t topiclen;

	/* Find leaf node */
	ಆಗ(topics && topics[topic_ಸೂಚ್ಯಂಕ] != ಖಾಲಿ){
		topiclen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(topics[topic_ಸೂಚ್ಯಂಕ]);
		ಆದರೆ(topiclen > UINT16_MAX){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
		}
		HASH_ಕಂಡುಹಿಡಿ(hh, subhier->children, topics[topic_ಸೂಚ್ಯಂಕ], topiclen, branch);
		ಆದರೆ(!branch){
			/* Not found */
			branch = sub_ಕೂಡಿಸು_hier_ನಮೂದು(subhier, &subhier->children, topics[topic_ಸೂಚ್ಯಂಕ], (uint16_t)topiclen);
			ಆದರೆ(!branch){
				ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
			}
		}
		subhier = branch;
		topic_ಸೂಚ್ಯಂಕ++;
	}

	/* Add add our context */
	ಆದರೆ(context && context->id){
		ಆದರೆ(sharename){
			ಹಿಂತಿರುಗಿ sub_ಕೂಡಿಸು_shared(context, sub, subhier, sharename);
		}ಇಲ್ಲದಿದ್ದರೆ{
			ಹಿಂತಿರುಗಿ sub_ಕೂಡಿಸು_ಸಾಮಾನ್ಯ(context, sub, subhier);
		}
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sub_remove_ಸಾಮಾನ್ಯ(ರಚನೆ mosquitto *context, ರಚನೆ mosquitto__subhier *subhier, uint8_t *reason)
{
	ರಚನೆ mosquitto__subleaf *ಎಲೆ;

	ಎಲೆ = subhier->subs;
	ಆಗ(ಎಲೆ){
		ಆದರೆ(ಎಲೆ->context==context){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_SYS_ವೃಕ್ಷ
			db.subscription_ಎಣಿಕೆ--;
#ಆದರೆ_ಅಂತ್ಯ
			DL_DELETE(subhier->subs, ಎಲೆ);

			/* Remove the reference to the sub that the client is keeping.
			 * It would be nice to be able to use the reference directly,
			 * but that would involve keeping a copy of the topic string in
			 * each subleaf. Might be worth considering though. */
			ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<context->subs_ಸಾಮರ್ಥ್ಯ; i++){
				ಆದರೆ(context->subs[i] && context->subs[i]->hier == subhier){
					context->subs_ಎಣಿಕೆ--;
					mosquitto_free(context->subs[i]);
					context->subs[i] = ಖಾಲಿ;
					ನಿಲ್ಲಿಸು;
				}
			}
			*reason = 0;
			ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
		}
		ಎಲೆ = ಎಲೆ->ಮುಂದಿನ;
	}
	ಹಿಂತಿರುಗಿ MOSQ_ERR_NO_SUBSCRIBERS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sub__remove_shared(ರಚನೆ mosquitto *context, ರಚನೆ mosquitto__subhier *subhier, uint8_t *reason, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sharename)
{
	ರಚನೆ mosquitto__subshared *shared;

	HASH_ಕಂಡುಹಿಡಿ(hh, subhier->shared, sharename, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sharename), shared);
	ಆದರೆ(shared){
		ರಚನೆ mosquitto__subleaf *ಎಲೆ = shared->subs;
		ಆಗ(ಎಲೆ){
			ಆದರೆ(ಎಲೆ->context==context){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_SYS_ವೃಕ್ಷ
				db.shared_subscription_ಎಣಿಕೆ--;
#ಆದರೆ_ಅಂತ್ಯ
				DL_DELETE(shared->subs, ಎಲೆ);

				/* Remove the reference to the sub that the client is keeping.
				* It would be nice to be able to use the reference directly,
				* but that would involve keeping a copy of the topic string in
				* each subleaf. Might be worth considering though. */
				ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<context->subs_ಸಾಮರ್ಥ್ಯ; i++){
					ಆದರೆ(context->subs[i]
							&& context->subs[i]->hier == subhier
							&& context->subs[i]->shared == shared){

						mosquitto_free(context->subs[i]);
						context->subs[i] = ಖಾಲಿ;
						context->subs_ಎಣಿಕೆ--;
						ನಿಲ್ಲಿಸು;
					}
				}

				ಆದರೆ(shared->subs == ಖಾಲಿ){
					HASH_DELETE(hh, subhier->shared, shared);
					mosquitto_FREE(shared);
				}

				*reason = 0;
				ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
			}
			ಎಲೆ = ಎಲೆ->ಮುಂದಿನ;
		}
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NO_SUBSCRIBERS;
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sub__remove_recurse(ರಚನೆ mosquitto *context, ರಚನೆ mosquitto__subhier *subhier, ಅಕ್ಷರ **topics, uint8_t *reason, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sharename)
{
	ರಚನೆ mosquitto__subhier *branch;

	ಆದರೆ(topics == ಖಾಲಿ || topics[0] == ಖಾಲಿ){
		ಆದರೆ(sharename){
			ಹಿಂತಿರುಗಿ sub__remove_shared(context, subhier, reason, sharename);
		}ಇಲ್ಲದಿದ್ದರೆ{
			ಹಿಂತಿರುಗಿ sub_remove_ಸಾಮಾನ್ಯ(context, subhier, reason);
		}
	}

	HASH_ಕಂಡುಹಿಡಿ(hh, subhier->children, topics[0], ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(topics[0]), branch);
	ಆದರೆ(branch){
		sub__remove_recurse(context, branch, &(topics[1]), reason, sharename);
		ಆದರೆ(!branch->children && !branch->subs && !branch->shared){
			HASH_DELETE(hh, subhier->children, branch);
			mosquitto_FREE(branch);
		}
	}
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sub_ಹುಡುಕು(ರಚನೆ mosquitto__subhier *subhier, ಅಕ್ಷರ **ವಿಭಜಿಸು_topics, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ_id, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *topic, uint8_t qos, ಪೂರ್ಣಾಂಕ retain, ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *stored)
{
	/* FIXME - need to take into account source_id if the client is a bridge */
	ರಚನೆ mosquitto__subhier *branch;
	ಪೂರ್ಣಾಂಕ rc;
	bool have_subscribers = false;

	ಆದರೆ(ವಿಭಜಿಸು_topics && ವಿಭಜಿಸು_topics[0]){
		/* Check for literal match */
		HASH_ಕಂಡುಹಿಡಿ(hh, subhier->children, ವಿಭಜಿಸು_topics[0], ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ವಿಭಜಿಸು_topics[0]), branch);

		ಆದರೆ(branch){
			rc = sub_ಹುಡುಕು(branch, &(ವಿಭಜಿಸು_topics[1]), ಮೂಲ_id, topic, qos, retain, stored);
			ಆದರೆ(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				ಹಿಂತಿರುಗಿ rc;
			}
			ಆದರೆ(ವಿಭಜಿಸು_topics[1] == ಖಾಲಿ){ /* End of list */
				rc = subs__process(branch, ಮೂಲ_id, topic, qos, retain, stored);
				ಆದರೆ(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					ಹಿಂತಿರುಗಿ rc;
				}
			}
		}

		/* Check for + match */
		HASH_ಕಂಡುಹಿಡಿ_BYHASHVALUE(hh, subhier->children, "+", 1, hashv_plus, branch);

		ಆದರೆ(branch){
			rc = sub_ಹುಡುಕು(branch, &(ವಿಭಜಿಸು_topics[1]), ಮೂಲ_id, topic, qos, retain, stored);
			ಆದರೆ(rc == MOSQ_ERR_SUCCESS){
				have_subscribers = true;
			}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc != MOSQ_ERR_NO_SUBSCRIBERS){
				ಹಿಂತಿರುಗಿ rc;
			}
			ಆದರೆ(ವಿಭಜಿಸು_topics[1] == ಖಾಲಿ){ /* End of list */
				rc = subs__process(branch, ಮೂಲ_id, topic, qos, retain, stored);
				ಆದರೆ(rc == MOSQ_ERR_SUCCESS){
					have_subscribers = true;
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc != MOSQ_ERR_NO_SUBSCRIBERS){
					ಹಿಂತಿರುಗಿ rc;
				}
			}
		}
	}

	/* Check for # match */
	HASH_ಕಂಡುಹಿಡಿ_BYHASHVALUE(hh, subhier->children, "#", 1, hashv_hash, branch);
	ಆದರೆ(branch && !branch->children){
		/* The topic matches due to a # wildcard - process the
		 * subscriptions but *don't* return. Although this branch has ended
		 * there may still be other subscriptions to deal with.
		 */
		rc = subs__process(branch, ಮೂಲ_id, topic, qos, retain, stored);
		ಆದರೆ(rc == MOSQ_ERR_SUCCESS){
			have_subscribers = true;
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc != MOSQ_ERR_NO_SUBSCRIBERS){
			ಹಿಂತಿರುಗಿ rc;
		}
	}

	ಆದರೆ(have_subscribers){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NO_SUBSCRIBERS;
	}
}


ಸ್ಥಿರ ರಚನೆ mosquitto__subhier *sub_ಕೂಡಿಸು_hier_ನಮೂದು(ರಚನೆ mosquitto__subhier *ಪೋಷಕ, ರಚನೆ mosquitto__subhier **sibling, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *topic, uint16_t len)
{
	ರಚನೆ mosquitto__subhier *ಮಗು;

	ಪ್ರತಿಜ್ಞಾ(sibling);

	ಮಗು = mosquitto_calloc(1, ಗಾತ್ರ(ರಚನೆ mosquitto__subhier) + len + 1);
	ಆದರೆ(!ಮಗು){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Out of memory.");
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	ಮಗು->ಪೋಷಕ = ಪೋಷಕ;
	ಮಗು->topic_len = len;
	ಆದರೆ(len > 0){
		strncpy(ಮಗು->topic, topic, (size_t)(len+1));
	}

	HASH_ಕೂಡಿಸು(hh, *sibling, topic, ಮಗು->topic_len, ಮಗು);

	ಹಿಂತಿರುಗಿ ಮಗು;
}


ಪೂರ್ಣಾಂಕ sub_ಕೂಡಿಸು(ರಚನೆ mosquitto *context, ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto_subscription *sub)
{
	ಪೂರ್ಣಾಂಕ rc = 0;
	ರಚನೆ mosquitto__subhier *subhier;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sharename = ಖಾಲಿ;
	ಅಕ್ಷರ *local_sub;
	ಅಕ್ಷರ **topics;
	size_t topiclen;

	ಪ್ರತಿಜ್ಞಾ(sub);
	ಪ್ರತಿಜ್ಞಾ(sub->topic_filter);

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &topics, &sharename);
	ಆದರೆ(rc){
		ಹಿಂತಿರುಗಿ rc;
	}

	topiclen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(topics[0]);
	ಆದರೆ(topiclen > UINT16_MAX){
		mosquitto_FREE(local_sub);
		mosquitto_FREE(topics);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	ಆದರೆ(sharename){
		HASH_ಕಂಡುಹಿಡಿ(hh, db.shared_subs, topics[0], topiclen, subhier);
		ಆದರೆ(!subhier){
			subhier = sub_ಕೂಡಿಸು_hier_ನಮೂದು(ಖಾಲಿ, &db.shared_subs, topics[0], (uint16_t)topiclen);
			ಆದರೆ(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Out of memory.");
				ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
			}
		}
	}ಇಲ್ಲದಿದ್ದರೆ{
		HASH_ಕಂಡುಹಿಡಿ(hh, db.ಸಾಮಾನ್ಯ_subs, topics[0], topiclen, subhier);
		ಆದರೆ(!subhier){
			subhier = sub_ಕೂಡಿಸು_hier_ನಮೂದು(ಖಾಲಿ, &db.ಸಾಮಾನ್ಯ_subs, topics[0], (uint16_t)topiclen);
			ಆದರೆ(!subhier){
				mosquitto_FREE(local_sub);
				mosquitto_FREE(topics);
				log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Out of memory.");
				ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
			}
		}
	}
	rc = sub_ಕೂಡಿಸು_context(context, sub, subhier, topics, sharename);

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	ಹಿಂತಿರುಗಿ rc;
}


ಪೂರ್ಣಾಂಕ sub__remove(ರಚನೆ mosquitto *context, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sub, uint8_t *reason)
{
	ಪೂರ್ಣಾಂಕ rc = 0;
	ರಚನೆ mosquitto__subhier *subhier;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sharename = ಖಾಲಿ;
	ಅಕ್ಷರ *local_sub = ಖಾಲಿ;
	ಅಕ್ಷರ **topics = ಖಾಲಿ;

	ಪ್ರತಿಜ್ಞಾ(sub);

	rc = sub__topic_tokenise(sub, &local_sub, &topics, &sharename);
	ಆದರೆ(rc){
		ಹಿಂತಿರುಗಿ rc;
	}

	ಆದರೆ(sharename){
		HASH_ಕಂಡುಹಿಡಿ(hh, db.shared_subs, topics[0], ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(topics[0]), subhier);
	}ಇಲ್ಲದಿದ್ದರೆ{
		HASH_ಕಂಡುಹಿಡಿ(hh, db.ಸಾಮಾನ್ಯ_subs, topics[0], ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(topics[0]), subhier);
	}
	ಆದರೆ(subhier){
		*reason = MQTT_RC_NO_SUBSCRIPTION_EXISTED;
		rc = sub__remove_recurse(context, subhier, topics, reason, sharename);
	}

	mosquitto_FREE(local_sub);
	mosquitto_FREE(topics);

	ಹಿಂತಿರುಗಿ rc;
}


ಪೂರ್ಣಾಂಕ sub_messages_ಸರತಿ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ_id, uint8_t qos, ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ **stored)
{
	ಪೂರ್ಣಾಂಕ rc = MOSQ_ERR_SUCCESS, rc2;
	ಪೂರ್ಣಾಂಕ rc_ಸಾಮಾನ್ಯ = MOSQ_ERR_NO_SUBSCRIBERS, rc_shared = MOSQ_ERR_NO_SUBSCRIBERS;
	ರಚನೆ mosquitto__subhier *subhier;
	ಅಕ್ಷರ **ವಿಭಜಿಸು_topics = ಖಾಲಿ;
	ಅಕ್ಷರ *local_topic = ಖಾಲಿ;
	ಚಿಹ್ನೆರಹಿತ hashv;
	size_t topiclen;
	bool retain = (*stored)->ದತ್ತಾಂಶ.retain;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *topic = (*stored)->ದತ್ತಾಂಶ.topic;

	ಪ್ರತಿಜ್ಞಾ(topic);

	ಆದರೆ(sub__topic_tokenise(topic, &local_topic, &ವಿಭಜಿಸು_topics, ಖಾಲಿ)){
		ಹಿಂತಿರುಗಿ 1;
	}

	/* Protect this message until we have sent it to all
	clients - this is required because websockets client calls
	db__message_write(), which could remove the message if ref_count==0.
	*/
	db_ಸಂದೇಶ_store_ref_inc(*stored);

	topiclen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ವಿಭಜಿಸು_topics[0]);
	HASH_ಮೌಲ್ಯ(ವಿಭಜಿಸು_topics[0], topiclen, hashv);
	HASH_ಕಂಡುಹಿಡಿ_BYHASHVALUE(hh, db.ಸಾಮಾನ್ಯ_subs, ವಿಭಜಿಸು_topics[0], topiclen, hashv, subhier);
	ಆದರೆ(subhier){
		rc_ಸಾಮಾನ್ಯ = sub_ಹುಡುಕು(subhier, ವಿಭಜಿಸು_topics, ಮೂಲ_id, topic, qos, retain, *stored);
		ಆದರೆ(rc_ಸಾಮಾನ್ಯ > 0){
			rc = rc_ಸಾಮಾನ್ಯ;
			ಹೋಗು ಕೊನೆ;
		}
	}

	HASH_ಕಂಡುಹಿಡಿ_BYHASHVALUE(hh, db.shared_subs, ವಿಭಜಿಸು_topics[0], topiclen, hashv, subhier);
	ಆದರೆ(subhier){
		rc_shared = sub_ಹುಡುಕು(subhier, ವಿಭಜಿಸು_topics, ಮೂಲ_id, topic, qos, retain, *stored);
		ಆದರೆ(rc_shared > 0){
			rc = rc_shared;
			ಹೋಗು ಕೊನೆ;
		}
	}

	ಆದರೆ(rc_ಸಾಮಾನ್ಯ == MOSQ_ERR_NO_SUBSCRIBERS && rc_shared == MOSQ_ERR_NO_SUBSCRIBERS){
		rc = MOSQ_ERR_NO_SUBSCRIBERS;
	}

	ಆದರೆ(retain){
		rc2 = retain__store(topic, *stored, ವಿಭಜಿಸು_topics, true);
		ಆದರೆ(rc2){
			rc = rc2;
		}
	}

ಕೊನೆ:
	mosquitto_FREE(ವಿಭಜಿಸು_topics);
	mosquitto_FREE(local_topic);
	/* Remove our reference and free if needed. */
	db_ಸಂದೇಶ_store_ref_dec(stored);

	ಹಿಂತಿರುಗಿ rc;
}


/* Remove a subhier element, and return its parent if that needs freeing as well. */
ಸ್ಥಿರ ರಚನೆ mosquitto__subhier *tmp_remove_subs(ರಚನೆ mosquitto__subhier *sub)
{
	ರಚನೆ mosquitto__subhier *ಪೋಷಕ;

	ಆದರೆ(!sub || !sub->ಪೋಷಕ){
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	ಆದರೆ(sub->children || sub->subs){
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	ಪೋಷಕ = sub->ಪೋಷಕ;
	HASH_DELETE(hh, ಪೋಷಕ->children, sub);
	mosquitto_FREE(sub);

	ಆದರೆ(ಪೋಷಕ->subs == ಖಾಲಿ
			&& ಪೋಷಕ->children == ಖಾಲಿ
			&& ಪೋಷಕ->shared == ಖಾಲಿ
			&& ಪೋಷಕ->ಪೋಷಕ){

		ಹಿಂತಿರುಗಿ ಪೋಷಕ;
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
}


/* Remove all subscriptions for a client.
 */
ಪೂರ್ಣಾಂಕ sub__clean_session(ರಚನೆ mosquitto *context)
{
	ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<context->subs_ಸಾಮರ್ಥ್ಯ; i++){
		ಆದರೆ(context->subs[i] == ಖಾಲಿ || context->subs[i]->hier == ಖಾಲಿ){
			ಮುಂದುವರಿ;
		}

		ರಚನೆ mosquitto__subhier *hier = context->subs[i]->hier;
		ರಚನೆ mosquitto__subleaf *ಎಲೆ;

		plugin_persist__handle_subscription_delete(context, context->subs[i]->topic_filter);
		ಆದರೆ(context->subs[i]->shared){
			ಎಲೆ = context->subs[i]->shared->subs;
			ಆಗ(ಎಲೆ){
				ಆದರೆ(ಎಲೆ->context==context){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_SYS_ವೃಕ್ಷ
					db.shared_subscription_ಎಣಿಕೆ--;
#ಆದರೆ_ಅಂತ್ಯ
					sub_remove_shared_ಎಲೆ(context->subs[i]->hier, context->subs[i]->shared, ಎಲೆ);
					ನಿಲ್ಲಿಸು;
				}
				ಎಲೆ = ಎಲೆ->ಮುಂದಿನ;
			}
		}ಇಲ್ಲದಿದ್ದರೆ{
			ಎಲೆ = hier->subs;
			ಆಗ(ಎಲೆ){
				ಆದರೆ(ಎಲೆ->context==context){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_SYS_ವೃಕ್ಷ
					db.subscription_ಎಣಿಕೆ--;
#ಆದರೆ_ಅಂತ್ಯ
					DL_DELETE(hier->subs, ಎಲೆ);
					ನಿಲ್ಲಿಸು;
				}
				ಎಲೆ = ಎಲೆ->ಮುಂದಿನ;
			}
		}
		mosquitto_FREE(context->subs[i]);

		ಆದರೆ(hier->subs == ಖಾಲಿ
				&& hier->children == ಖಾಲಿ
				&& hier->shared == ಖಾಲಿ
				&& hier->ಪೋಷಕ){

			ಮಾಡು{
				hier = tmp_remove_subs(hier);
			}ಆಗ(hier);
		}
	}
	mosquitto_FREE(context->subs);
	context->subs_ಸಾಮರ್ಥ್ಯ = 0;
	context->subs_ಎಣಿಕೆ = 0;

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಶೂನ್ಯ sub_ವೃಕ್ಷ_print(ರಚನೆ mosquitto__subhier *ಬೇರು, ಪೂರ್ಣಾಂಕ level)
{
	ಪೂರ್ಣಾಂಕ i;
	ರಚನೆ mosquitto__subhier *branch, *branch_tmp;
	ರಚನೆ mosquitto__subleaf *ಎಲೆ;

	HASH_ITER(hh, ಬೇರು, branch, branch_tmp){
		ಆದರೆ(level > -1){
			ಗಾಗಿ(i=0; i<(level+2)*2; i++){
				ಬರೆ(" ");
			}
			ಬರೆ("%s", branch->topic);
			ಎಲೆ = branch->subs;
			ಆಗ(ಎಲೆ){
				ಆದರೆ(ಎಲೆ->context){
					ಬರೆ(" (%s, %d)", ಎಲೆ->context->id, MQTT_SUB_OPT_GET_QOS(ಎಲೆ->subscription_options));
				}ಇಲ್ಲದಿದ್ದರೆ{
					ಬರೆ(" (%s, %d)", "", MQTT_SUB_OPT_GET_QOS(ಎಲೆ->subscription_options));
				}
				ಎಲೆ = ಎಲೆ->ಮುಂದಿನ;
			}
			ಬರೆ("\n");
		}

		sub_ವೃಕ್ಷ_print(branch->children, level+1);
	}
}


ಪೂರ್ಣಾಂಕ sub__init(ಶೂನ್ಯ)
{
	HASH_ಮೌಲ್ಯ("+", 1, hashv_plus);
	HASH_ಮೌಲ್ಯ("#", 1, hashv_hash);

	ಆದರೆ(sub_ಕೂಡಿಸು_hier_ನಮೂದು(ಖಾಲಿ, &db.shared_subs, "", 0) == ಖಾಲಿ
			|| sub_ಕೂಡಿಸು_hier_ನಮೂದು(ಖಾಲಿ, &db.ಸಾಮಾನ್ಯ_subs, "", 0) == ಖಾಲಿ
			|| sub_ಕೂಡಿಸು_hier_ನಮೂದು(ಖಾಲಿ, &db.ಸಾಮಾನ್ಯ_subs, "$SYS", (uint16_t)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ("$SYS")) == ಖಾಲಿ
			){

		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}
}
