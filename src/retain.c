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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/mqtt_protocol.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"

#ಅನ್ತರ್ಭಾವ "utlist.h"

ಸ್ಥಿರ time_t ಮುಂದಿನ_expire_check = 0;

ಸ್ಥಿರ ರಚನೆ mosquitto__retainhier *retain_ಕೂಡಿಸು_hier_ನಮೂದು(ರಚನೆ mosquitto__retainhier *ಪೋಷಕ, ರಚನೆ mosquitto__retainhier **sibling, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *topic, uint16_t len)
{
	ರಚನೆ mosquitto__retainhier *ಮಗು;

	ಪ್ರತಿಜ್ಞಾ(sibling);

	ಮಗು = mosquitto_calloc(1, ಗಾತ್ರ(ರಚನೆ mosquitto__retainhier) + len + 1);
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


ಪೂರ್ಣಾಂಕ retain__init(ಶೂನ್ಯ)
{
	ರಚನೆ mosquitto__retainhier *retainhier;

	retainhier = retain_ಕೂಡಿಸು_hier_ನಮೂದು(ಖಾಲಿ, &db.retains, "", 0);
	ಆದರೆ(!retainhier){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}

	retainhier = retain_ಕೂಡಿಸು_hier_ನಮೂದು(ಖಾಲಿ, &db.retains, "$SYS", (uint16_t)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ("$SYS"));
	ಆದರೆ(!retainhier){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ಪೂರ್ಣಾಂಕ mosquitto_persist_retain_ಸಂದೇಶ_ಗಣ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *topic, uint64_t ತಳ_ಸಂದೇಶ_id)
{
	ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ;
	ಪೂರ್ಣಾಂಕ rc = MOSQ_ERR_UNKNOWN;
	ಅಕ್ಷರ **ವಿಭಜಿಸು_topics = ಖಾಲಿ;
	ಅಕ್ಷರ *local_topic = ಖಾಲಿ;

	ಆದರೆ(topic == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	HASH_ಕಂಡುಹಿಡಿ(hh, db.ಸಂದೇಶ_store, &ತಳ_ಸಂದೇಶ_id, ಗಾತ್ರ(ತಳ_ಸಂದೇಶ_id), ತಳ_ಸಂದೇಶ);
	ಆದರೆ(ತಳ_ಸಂದೇಶ){
		ಆದರೆ(sub__topic_tokenise(topic, &local_topic, &ವಿಭಜಿಸು_topics, ಖಾಲಿ)){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
		}

		rc = retain__store(topic, ತಳ_ಸಂದೇಶ, ವಿಭಜಿಸು_topics, false);
		mosquitto_free(ವಿಭಜಿಸು_topics);
		mosquitto_free(local_topic);
	}

	ಹಿಂತಿರುಗಿ rc;
}


BROKER_EXPORT ಪೂರ್ಣಾಂಕ mosquitto_persist_retain_ಸಂದೇಶ_delete(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *topic)
{
	ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ ತಳ_ಸಂದೇಶ;
	ಪೂರ್ಣಾಂಕ rc = MOSQ_ERR_UNKNOWN;
	ಅಕ್ಷರ **ವಿಭಜಿಸು_topics = ಖಾಲಿ;
	ಅಕ್ಷರ *local_topic = ಖಾಲಿ;

	ಆದರೆ(topic == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	memset(&ತಳ_ಸಂದೇಶ, 0, ಗಾತ್ರ(ತಳ_ಸಂದೇಶ));
	ತಳ_ಸಂದೇಶ.ref_ಎಣಿಕೆ = 10; /* Ensure this isn't freed */

	ಆದರೆ(sub__topic_tokenise(topic, &local_topic, &ವಿಭಜಿಸು_topics, ಖಾಲಿ)){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}

	/* With stored->payloadlen == 0, this means the message will be removed */
	rc = retain__store(topic, &ತಳ_ಸಂದೇಶ, ವಿಭಜಿಸು_topics, false);
	mosquitto_FREE(ವಿಭಜಿಸು_topics);
	mosquitto_FREE(local_topic);

	ಹಿಂತಿರುಗಿ rc;
}


ಶೂನ್ಯ retain_clean_ಖಾಲಿಗಳು_hierarchy(ರಚನೆ mosquitto__retainhier *retainhier)
{
	ಆಗ(retainhier){
		ಆದರೆ(retainhier->children || retainhier->retained || retainhier->ಪೋಷಕ == ಖಾಲಿ){
			/* Entry is being used */
			ಹಿಂತಿರುಗಿ;
		}ಇಲ್ಲದಿದ್ದರೆ{
			HASH_DELETE(hh, retainhier->ಪೋಷಕ->children, retainhier);

			ರಚನೆ mosquitto__retainhier *ಪೋಷಕ = retainhier->ಪೋಷಕ;
			mosquitto_FREE(retainhier);
			retainhier = ಪೋಷಕ;
		}
	}
}


ಪೂರ್ಣಾಂಕ retain__store(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *topic, ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *ತಳ_ಸಂದೇಶ, ಅಕ್ಷರ **ವಿಭಜಿಸು_topics, bool persist)
{
	ರಚನೆ mosquitto__retainhier *retainhier;
	ರಚನೆ mosquitto__retainhier *branch;
	size_t slen;

	ಪ್ರತಿಜ್ಞಾ(ತಳ_ಸಂದೇಶ);
	ಪ್ರತಿಜ್ಞಾ(ವಿಭಜಿಸು_topics);

	HASH_ಕಂಡುಹಿಡಿ(hh, db.retains, ವಿಭಜಿಸು_topics[0], ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ವಿಭಜಿಸು_topics[0]), retainhier);
	ಆದರೆ(retainhier == ಖಾಲಿ){
		retainhier = retain_ಕೂಡಿಸು_hier_ನಮೂದು(ಖಾಲಿ, &db.retains, ವಿಭಜಿಸು_topics[0], (uint16_t)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ವಿಭಜಿಸು_topics[0]));
		ಆದರೆ(!retainhier){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
		}
	}

	ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; ವಿಭಜಿಸು_topics[i] != ಖಾಲಿ; i++){
		slen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ವಿಭಜಿಸು_topics[i]);
		HASH_ಕಂಡುಹಿಡಿ(hh, retainhier->children, ವಿಭಜಿಸು_topics[i], slen, branch);
		ಆದರೆ(branch == ಖಾಲಿ){
			branch = retain_ಕೂಡಿಸು_hier_ನಮೂದು(retainhier, &retainhier->children, ವಿಭಜಿಸು_topics[i], (uint16_t)slen);
			ಆದರೆ(branch == ಖಾಲಿ){
				ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
			}
		}
		retainhier = branch;
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_PERSISTENCE
	ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(topic, "$SYS", 4)){
		/* Retained messages count as a persistence change, but only if
		 * they aren't for $SYS. */
		db.persistence_changes++;
	}
#ಇಲ್ಲದಿದ್ದರೆ
	ಬಳಸದ(topic);
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ(retainhier->retained){
		ಆದರೆ(retainhier->retained == ತಳ_ಸಂದೇಶ){
			/* This may occur if multiple persistence providers are used */
			ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
		}

		ಆದರೆ(persist && retainhier->retained->ದತ್ತಾಂಶ.topic[0] != '$' && ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen == 0){
			/* Only delete if another retained message isn't replacing this one */
			plugin_persist_handle_retain_ಸಂದೇಶ_delete(retainhier->retained);
		}
		db_ಸಂದೇಶ_store_ref_dec(&retainhier->retained);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_SYS_ವೃಕ್ಷ
		db.retained_ಎಣಿಕೆ--;
#ಆದರೆ_ಅಂತ್ಯ
		ಆದರೆ(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen == 0){
			retainhier->retained = ಖಾಲಿ;
			retain_clean_ಖಾಲಿಗಳು_hierarchy(retainhier);
		}
	}
	ಆದರೆ(ತಳ_ಸಂದೇಶ->ದತ್ತಾಂಶ.payloadlen){
		retainhier->retained = ತಳ_ಸಂದೇಶ;
		db_ಸಂದೇಶ_store_ref_inc(retainhier->retained);
		ಆದರೆ(persist && retainhier->retained->ದತ್ತಾಂಶ.topic[0] != '$'){
			plugin_persist_handle_ತಳ_ಸಂದೇಶ_ಕೂಡಿಸು(retainhier->retained);
			plugin_persist_handle_retain_ಸಂದೇಶ_ಗಣ(retainhier->retained);
		}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_SYS_ವೃಕ್ಷ
		db.retained_ಎಣಿಕೆ++;
#ಆದರೆ_ಅಂತ್ಯ
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ bool retain_delete_expired_ಸಂದೇಶ(ರಚನೆ mosquitto__retainhier *branch)
{
	ಆದರೆ(branch->retained && branch->retained->ದತ್ತಾಂಶ.expiry_ಸಮಯ > 0 && db.now_real_s >= branch->retained->ದತ್ತಾಂಶ.expiry_ಸಮಯ){
		plugin_persist_handle_retain_ಸಂದೇಶ_delete(branch->retained);
		db_ಸಂದೇಶ_store_ref_dec(&branch->retained);
		branch->retained = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_SYS_ವೃಕ್ಷ
		db.retained_ಎಣಿಕೆ--;
#ಆದರೆ_ಅಂತ್ಯ
		ಹಿಂತಿರುಗಿ true;
	}
	ಹಿಂತಿರುಗಿ false;
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ retain__process(ರಚನೆ mosquitto__retainhier *branch, ರಚನೆ mosquitto *context, ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto_subscription *sub)
{
	ಪೂರ್ಣಾಂಕ rc = 0;
	uint8_t qos, sub_qos;
	uint16_t mid;
	ರಚನೆ mosquitto_ತಳ_ಸಂದೇಶ *retained;

	ಆದರೆ(retain_delete_expired_ಸಂದೇಶ(branch)){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}

	retained = branch->retained;

	rc = mosquitto_acl_check(context, retained->ದತ್ತಾಂಶ.topic, retained->ದತ್ತಾಂಶ.payloadlen, retained->ದತ್ತಾಂಶ.payload,
			retained->ದತ್ತಾಂಶ.qos, retained->ದತ್ತಾಂಶ.retain, retained->ದತ್ತಾಂಶ.properties, MOSQ_ACL_READ);
	ಆದರೆ(rc == MOSQ_ERR_ACL_DENIED){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc != MOSQ_ERR_SUCCESS){
		ಹಿಂತಿರುಗಿ rc;
	}

	/* Check for original source access */
	ಆದರೆ(db.config->check_retain_ಮೂಲ && retained->origin != mosq_mo_broker && retained->ದತ್ತಾಂಶ.ಮೂಲ_id){
		ರಚನೆ mosquitto retain_ctxt;
		memset(&retain_ctxt, 0, ಗಾತ್ರ(ರಚನೆ mosquitto));

		retain_ctxt.id = retained->ದತ್ತಾಂಶ.ಮೂಲ_id;
		retain_ctxt.username = retained->ದತ್ತಾಂಶ.ಮೂಲ_username;
		retain_ctxt.listener = retained->ಮೂಲ_listener;

		rc = mosquitto_acl_check(&retain_ctxt, retained->ದತ್ತಾಂಶ.topic, retained->ದತ್ತಾಂಶ.payloadlen, retained->ದತ್ತಾಂಶ.payload,
				retained->ದತ್ತಾಂಶ.qos, retained->ದತ್ತಾಂಶ.retain, retained->ದತ್ತಾಂಶ.properties, MOSQ_ACL_WRITE);
		ಆದರೆ(rc == MOSQ_ERR_ACL_DENIED){
			ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc != MOSQ_ERR_SUCCESS){
			ಹಿಂತಿರುಗಿ rc;
		}
	}

	sub_qos = sub->options & 0x03;
	ಆದರೆ(db.config->upgrade_outgoing_qos){
		qos = sub_qos;
	}ಇಲ್ಲದಿದ್ದರೆ{
		qos = retained->ದತ್ತಾಂಶ.qos;
		ಆದರೆ(qos > sub_qos){
			qos = sub_qos;
		}
	}
	ಆದರೆ(qos > 0){
		mid = mosquitto__mid_generate(context);
	}ಇಲ್ಲದಿದ್ದರೆ{
		mid = 0;
	}
	ಹಿಂತಿರುಗಿ db_ಸಂದೇಶ_ಸೇರಿಸು_outgoing(context, 0, mid, qos, true, retained, sub->identifier, false, true);
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ retain_ಹುಡುಕು(ರಚನೆ mosquitto__retainhier *retainhier, ಅಕ್ಷರ **ವಿಭಜಿಸು_topics, ರಚನೆ mosquitto *context, ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto_subscription *sub, ಪೂರ್ಣಾಂಕ level)
{
	ರಚನೆ mosquitto__retainhier *branch, *branch_tmp;
	ಪೂರ್ಣಾಂಕ flag = 0;

	ಆದರೆ(!strcmp(ವಿಭಜಿಸು_topics[0], "#") && ವಿಭಜಿಸು_topics[1] == ಖಾಲಿ){
		HASH_ITER(hh, retainhier->children, branch, branch_tmp){
			/* Set flag to indicate that we should check for retained messages
			 * on "foo" when we are subscribing to e.g. "foo/#" and then exit
			 * this function and return to an earlier retain__search().
			 */
			flag = -1;
			ಆದರೆ(branch->retained){
				retain__process(branch, context, sub);
			}
			ಆದರೆ(branch->children){
				retain_ಹುಡುಕು(branch, ವಿಭಜಿಸು_topics, context, sub, level+1);
			}
		}
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಆದರೆ(!strcmp(ವಿಭಜಿಸು_topics[0], "+")){
			HASH_ITER(hh, retainhier->children, branch, branch_tmp){
				ಆದರೆ(ವಿಭಜಿಸು_topics[1] != ಖಾಲಿ){
					ಆದರೆ(retain_ಹುಡುಕು(branch, &(ವಿಭಜಿಸು_topics[1]), context, sub, level+1) == -1
							|| (ವಿಭಜಿಸು_topics[1] != ಖಾಲಿ && !strcmp(ವಿಭಜಿಸು_topics[1], "#") && level>0)){

						ಆದರೆ(branch->retained){
							retain__process(branch, context, sub);
						}
					}
				}ಇಲ್ಲದಿದ್ದರೆ{
					ಆದರೆ(branch->retained){
						retain__process(branch, context, sub);
					}
				}
			}
		}ಇಲ್ಲದಿದ್ದರೆ{
			HASH_ಕಂಡುಹಿಡಿ(hh, retainhier->children, ವಿಭಜಿಸು_topics[0], ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ವಿಭಜಿಸು_topics[0]), branch);
			ಆದರೆ(branch){
				ಆದರೆ(ವಿಭಜಿಸು_topics[1] != ಖಾಲಿ){
					ಆದರೆ(retain_ಹುಡುಕು(branch, &(ವಿಭಜಿಸು_topics[1]), context, sub, level+1) == -1
							|| (ವಿಭಜಿಸು_topics[1] != ಖಾಲಿ && !strcmp(ವಿಭಜಿಸು_topics[1], "#") && level>0)){

						ಆದರೆ(branch->retained){
							retain__process(branch, context, sub);
						}
					}
				}ಇಲ್ಲದಿದ್ದರೆ{
					ಆದರೆ(branch->retained){
						retain__process(branch, context, sub);
					}
				}
			}
		}
	}
	ಹಿಂತಿರುಗಿ flag;
}


ಪೂರ್ಣಾಂಕ retain_ಸರತಿ(ರಚನೆ mosquitto *context, ಸ್ಥಿರಾಂಕ ರಚನೆ mosquitto_subscription *sub)
{
	ರಚನೆ mosquitto__retainhier *retainhier;
	ಅಕ್ಷರ *local_sub;
	ಅಕ್ಷರ **ವಿಭಜಿಸು_topics;
	ಪೂರ್ಣಾಂಕ rc;

	ಪ್ರತಿಜ್ಞಾ(context);
	ಪ್ರತಿಜ್ಞಾ(sub);

	ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(sub->topic_filter, "$share/", ಅಕ್ಷರಸರಣಿ_ಉದ್ದ("$share/"))){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &ವಿಭಜಿಸು_topics, ಖಾಲಿ);
	ಆದರೆ(rc){
		ಹಿಂತಿರುಗಿ rc;
	}

	HASH_ಕಂಡುಹಿಡಿ(hh, db.retains, ವಿಭಜಿಸು_topics[0], ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ವಿಭಜಿಸು_topics[0]), retainhier);

	ಆದರೆ(retainhier){
		retain_ಹುಡುಕು(retainhier, ವಿಭಜಿಸು_topics, context, sub, 0);
	}
	mosquitto_FREE(local_sub);
	mosquitto_FREE(ವಿಭಜಿಸು_topics);

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಶೂನ್ಯ retain__expire(ರಚನೆ mosquitto__retainhier **retainhier)
{
	ರಚನೆ mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		retain__expire(&peer->children);

		ರಚನೆ mosquitto__retainhier *ಪೋಷಕ = peer->ಪೋಷಕ;
		ಆದರೆ(retain_delete_expired_ಸಂದೇಶ(peer)){
			retain_clean_ಖಾಲಿಗಳು_hierarchy(ಪೋಷಕ);
		}
	}
}


ಶೂನ್ಯ retain__clean(ರಚನೆ mosquitto__retainhier **retainhier)
{
	ರಚನೆ mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		ಆದರೆ(peer->retained){
			db_ಸಂದೇಶ_store_ref_dec(&peer->retained);
		}
		retain__clean(&peer->children);

		HASH_DELETE(hh, *retainhier, peer);
		mosquitto_FREE(peer);
	}
}


ಶೂನ್ಯ retain__expiry_check(ಶೂನ್ಯ)
{
	ಆದರೆ(db.config->retain_expiry_interval > 0 && db.now_s > ಮುಂದಿನ_expire_check){
		retain__expire(&db.retains);
		ಮುಂದಿನ_expire_check = db.now_s + db.config->retain_expiry_interval;
	}
}
