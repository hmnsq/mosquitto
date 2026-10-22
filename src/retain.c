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

#अन्तर्भाव "config.h"

#अन्तर्भाव <assert.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto/mqtt_protocol.h"
#अन्तर्भाव "util_mosq.h"

#अन्तर्भाव "utlist.h"

स्थिर time_t अग्र_expire_check = 0;

स्थिर संरचना mosquitto__retainhier *retain_सङ्कलय_hier_प्रविष्टिः(संरचना mosquitto__retainhier *parent, संरचना mosquitto__retainhier **sibling, ध्रुव वर्ण *topic, uint16_t len)
{
	संरचना mosquitto__retainhier *child;

	प्रतिज्ञा(sibling);

	child = mosquitto_calloc(1, परिमाण(संरचना mosquitto__retainhier) + len + 1);
	यदि(!child){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		प्रत्यागम रिक्त;
	}
	child->parent = parent;
	child->topic_len = len;
	यदि(len > 0){
		strncpy(child->topic, topic, (size_t)(len+1));
	}

	HASH_सङ्कलय(hh, *sibling, topic, child->topic_len, child);

	प्रत्यागम child;
}


पूर्णाङ्क retain__init(शून्य)
{
	संरचना mosquitto__retainhier *retainhier;

	retainhier = retain_सङ्कलय_hier_प्रविष्टिः(रिक्त, &db.retains, "", 0);
	यदि(!retainhier){
		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	retainhier = retain_सङ्कलय_hier_प्रविष्टिः(रिक्त, &db.retains, "$SYS", (uint16_t)वाक्यदैर्घ्यम्("$SYS"));
	यदि(!retainhier){
		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_persist_retain_सन्देशः_समुच्चयः(ध्रुव वर्ण *topic, uint64_t आधारः_सन्देशः_id)
{
	संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः;
	पूर्णाङ्क rc = MOSQ_ERR_UNKNOWN;
	वर्ण **विभज_topics = रिक्त;
	वर्ण *local_topic = रिक्त;

	यदि(topic == रिक्त){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh, db.सन्देशः_store, &आधारः_सन्देशः_id, परिमाण(आधारः_सन्देशः_id), आधारः_सन्देशः);
	यदि(आधारः_सन्देशः){
		यदि(sub__topic_tokenise(topic, &local_topic, &विभज_topics, रिक्त)){
			प्रत्यागम MOSQ_ERR_NOMEM;
		}

		rc = retain__store(topic, आधारः_सन्देशः, विभज_topics, false);
		mosquitto_free(विभज_topics);
		mosquitto_free(local_topic);
	}

	प्रत्यागम rc;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_persist_retain_सन्देशः_delete(ध्रुव वर्ण *topic)
{
	संरचना mosquitto_आधारः_सन्देशः आधारः_सन्देशः;
	पूर्णाङ्क rc = MOSQ_ERR_UNKNOWN;
	वर्ण **विभज_topics = रिक्त;
	वर्ण *local_topic = रिक्त;

	यदि(topic == रिक्त){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	स्मृतिं_पूरय(&आधारः_सन्देशः, 0, परिमाण(आधारः_सन्देशः));
	आधारः_सन्देशः.ref_गणनम् = 10; /* Ensure this isn't freed */

	यदि(sub__topic_tokenise(topic, &local_topic, &विभज_topics, रिक्त)){
		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	/* With stored->payloadlen == 0, this means the message will be removed */
	rc = retain__store(topic, &आधारः_सन्देशः, विभज_topics, false);
	mosquitto_FREE(विभज_topics);
	mosquitto_FREE(local_topic);

	प्रत्यागम rc;
}


शून्य retain_clean_रिक्तानि_hierarchy(संरचना mosquitto__retainhier *retainhier)
{
	यावत्(retainhier){
		यदि(retainhier->children || retainhier->retained || retainhier->parent == रिक्त){
			/* Entry is being used */
			प्रत्यागम;
		}अन्यथा{
			HASH_DELETE(hh, retainhier->parent->children, retainhier);

			संरचना mosquitto__retainhier *parent = retainhier->parent;
			mosquitto_FREE(retainhier);
			retainhier = parent;
		}
	}
}


पूर्णाङ्क retain__store(ध्रुव वर्ण *topic, संरचना mosquitto_आधारः_सन्देशः *आधारः_सन्देशः, वर्ण **विभज_topics, bool persist)
{
	संरचना mosquitto__retainhier *retainhier;
	संरचना mosquitto__retainhier *branch;
	size_t slen;

	प्रतिज्ञा(आधारः_सन्देशः);
	प्रतिज्ञा(विभज_topics);

	HASH_FIND(hh, db.retains, विभज_topics[0], वाक्यदैर्घ्यम्(विभज_topics[0]), retainhier);
	यदि(retainhier == रिक्त){
		retainhier = retain_सङ्कलय_hier_प्रविष्टिः(रिक्त, &db.retains, विभज_topics[0], (uint16_t)वाक्यदैर्घ्यम्(विभज_topics[0]));
		यदि(!retainhier){
			प्रत्यागम MOSQ_ERR_NOMEM;
		}
	}

	प्रति(पूर्णाङ्क i=0; विभज_topics[i] != रिक्त; i++){
		slen = वाक्यदैर्घ्यम्(विभज_topics[i]);
		HASH_FIND(hh, retainhier->children, विभज_topics[i], slen, branch);
		यदि(branch == रिक्त){
			branch = retain_सङ्कलय_hier_प्रविष्टिः(retainhier, &retainhier->children, विभज_topics[i], (uint16_t)slen);
			यदि(branch == रिक्त){
				प्रत्यागम MOSQ_ERR_NOMEM;
			}
		}
		retainhier = branch;
	}

#यदि_परिभाषितम् WITH_PERSISTENCE
	यदि(वाक्ये_तुलय(topic, "$SYS", 4)){
		/* Retained messages count as a persistence change, but only if
		 * they aren't for $SYS. */
		db.persistence_changes++;
	}
#अन्यथा
	अप्रयुक्तम्(topic);
#यदि_अन्तः

	यदि(retainhier->retained){
		यदि(retainhier->retained == आधारः_सन्देशः){
			/* This may occur if multiple persistence providers are used */
			प्रत्यागम MOSQ_ERR_SUCCESS;
		}

		यदि(persist && retainhier->retained->दत्त.topic[0] != '$' && आधारः_सन्देशः->दत्त.payloadlen == 0){
			/* Only delete if another retained message isn't replacing this one */
			plugin_persist_handle_retain_सन्देशः_delete(retainhier->retained);
		}
		db_सन्देशः_store_ref_dec(&retainhier->retained);
#यदि_परिभाषितम् WITH_SYS_वृक्षः
		db.retained_गणनम्--;
#यदि_अन्तः
		यदि(आधारः_सन्देशः->दत्त.payloadlen == 0){
			retainhier->retained = रिक्त;
			retain_clean_रिक्तानि_hierarchy(retainhier);
		}
	}
	यदि(आधारः_सन्देशः->दत्त.payloadlen){
		retainhier->retained = आधारः_सन्देशः;
		db_सन्देशः_store_ref_inc(retainhier->retained);
		यदि(persist && retainhier->retained->दत्त.topic[0] != '$'){
			plugin_persist_handle_आधारः_सन्देशः_सङ्कलय(retainhier->retained);
			plugin_persist_handle_retain_सन्देशः_समुच्चयः(retainhier->retained);
		}
#यदि_परिभाषितम् WITH_SYS_वृक्षः
		db.retained_गणनम्++;
#यदि_अन्तः
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर bool retain_delete_expired_सन्देशः(संरचना mosquitto__retainhier *branch)
{
	यदि(branch->retained && branch->retained->दत्त.expiry_time > 0 && db.now_real_s >= branch->retained->दत्त.expiry_time){
		plugin_persist_handle_retain_सन्देशः_delete(branch->retained);
		db_सन्देशः_store_ref_dec(&branch->retained);
		branch->retained = रिक्त;
#यदि_परिभाषितम् WITH_SYS_वृक्षः
		db.retained_गणनम्--;
#यदि_अन्तः
		प्रत्यागम true;
	}
	प्रत्यागम false;
}


स्थिर पूर्णाङ्क retain__process(संरचना mosquitto__retainhier *branch, संरचना mosquitto *context, ध्रुव संरचना mosquitto_subscription *sub)
{
	पूर्णाङ्क rc = 0;
	uint8_t qos, sub_qos;
	uint16_t mid;
	संरचना mosquitto_आधारः_सन्देशः *retained;

	यदि(retain_delete_expired_सन्देशः(branch)){
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}

	retained = branch->retained;

	rc = mosquitto_acl_check(context, retained->दत्त.topic, retained->दत्त.payloadlen, retained->दत्त.payload,
			retained->दत्त.qos, retained->दत्त.retain, retained->दत्त.properties, MOSQ_ACL_READ);
	यदि(rc == MOSQ_ERR_ACL_DENIED){
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}अन्यथा यदि(rc != MOSQ_ERR_SUCCESS){
		प्रत्यागम rc;
	}

	/* Check for original source access */
	यदि(db.config->check_retain_source && retained->origin != mosq_mo_broker && retained->दत्त.source_id){
		संरचना mosquitto retain_ctxt;
		स्मृतिं_पूरय(&retain_ctxt, 0, परिमाण(संरचना mosquitto));

		retain_ctxt.id = retained->दत्त.source_id;
		retain_ctxt.username = retained->दत्त.source_username;
		retain_ctxt.listener = retained->source_listener;

		rc = mosquitto_acl_check(&retain_ctxt, retained->दत्त.topic, retained->दत्त.payloadlen, retained->दत्त.payload,
				retained->दत्त.qos, retained->दत्त.retain, retained->दत्त.properties, MOSQ_ACL_WRITE);
		यदि(rc == MOSQ_ERR_ACL_DENIED){
			प्रत्यागम MOSQ_ERR_SUCCESS;
		}अन्यथा यदि(rc != MOSQ_ERR_SUCCESS){
			प्रत्यागम rc;
		}
	}

	sub_qos = sub->options & 0x03;
	यदि(db.config->upgrade_outgoing_qos){
		qos = sub_qos;
	}अन्यथा{
		qos = retained->दत्त.qos;
		यदि(qos > sub_qos){
			qos = sub_qos;
		}
	}
	यदि(qos > 0){
		mid = mosquitto__mid_generate(context);
	}अन्यथा{
		mid = 0;
	}
	प्रत्यागम db_सन्देशः_निवेशय_outgoing(context, 0, mid, qos, true, retained, sub->identifier, false, true);
}


स्थिर पूर्णाङ्क retain_अन्वेषणम्(संरचना mosquitto__retainhier *retainhier, वर्ण **विभज_topics, संरचना mosquitto *context, ध्रुव संरचना mosquitto_subscription *sub, पूर्णाङ्क level)
{
	संरचना mosquitto__retainhier *branch, *branch_tmp;
	पूर्णाङ्क flag = 0;

	यदि(!वाक्यं_तुलय(विभज_topics[0], "#") && विभज_topics[1] == रिक्त){
		HASH_ITER(hh, retainhier->children, branch, branch_tmp){
			/* Set flag to indicate that we should check for retained messages
			 * on "foo" when we are subscribing to e.g. "foo/#" and then exit
			 * this function and return to an earlier retain__search().
			 */
			flag = -1;
			यदि(branch->retained){
				retain__process(branch, context, sub);
			}
			यदि(branch->children){
				retain_अन्वेषणम्(branch, विभज_topics, context, sub, level+1);
			}
		}
	}अन्यथा{
		यदि(!वाक्यं_तुलय(विभज_topics[0], "+")){
			HASH_ITER(hh, retainhier->children, branch, branch_tmp){
				यदि(विभज_topics[1] != रिक्त){
					यदि(retain_अन्वेषणम्(branch, &(विभज_topics[1]), context, sub, level+1) == -1
							|| (विभज_topics[1] != रिक्त && !वाक्यं_तुलय(विभज_topics[1], "#") && level>0)){

						यदि(branch->retained){
							retain__process(branch, context, sub);
						}
					}
				}अन्यथा{
					यदि(branch->retained){
						retain__process(branch, context, sub);
					}
				}
			}
		}अन्यथा{
			HASH_FIND(hh, retainhier->children, विभज_topics[0], वाक्यदैर्घ्यम्(विभज_topics[0]), branch);
			यदि(branch){
				यदि(विभज_topics[1] != रिक्त){
					यदि(retain_अन्वेषणम्(branch, &(विभज_topics[1]), context, sub, level+1) == -1
							|| (विभज_topics[1] != रिक्त && !वाक्यं_तुलय(विभज_topics[1], "#") && level>0)){

						यदि(branch->retained){
							retain__process(branch, context, sub);
						}
					}
				}अन्यथा{
					यदि(branch->retained){
						retain__process(branch, context, sub);
					}
				}
			}
		}
	}
	प्रत्यागम flag;
}


पूर्णाङ्क retain__queue(संरचना mosquitto *context, ध्रुव संरचना mosquitto_subscription *sub)
{
	संरचना mosquitto__retainhier *retainhier;
	वर्ण *local_sub;
	वर्ण **विभज_topics;
	पूर्णाङ्क rc;

	प्रतिज्ञा(context);
	प्रतिज्ञा(sub);

	यदि(!वाक्ये_तुलय(sub->topic_filter, "$share/", वाक्यदैर्घ्यम्("$share/"))){
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &विभज_topics, रिक्त);
	यदि(rc){
		प्रत्यागम rc;
	}

	HASH_FIND(hh, db.retains, विभज_topics[0], वाक्यदैर्घ्यम्(विभज_topics[0]), retainhier);

	यदि(retainhier){
		retain_अन्वेषणम्(retainhier, विभज_topics, context, sub, 0);
	}
	mosquitto_FREE(local_sub);
	mosquitto_FREE(विभज_topics);

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


शून्य retain__expire(संरचना mosquitto__retainhier **retainhier)
{
	संरचना mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		retain__expire(&peer->children);

		संरचना mosquitto__retainhier *parent = peer->parent;
		यदि(retain_delete_expired_सन्देशः(peer)){
			retain_clean_रिक्तानि_hierarchy(parent);
		}
	}
}


शून्य retain__clean(संरचना mosquitto__retainhier **retainhier)
{
	संरचना mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		यदि(peer->retained){
			db_सन्देशः_store_ref_dec(&peer->retained);
		}
		retain__clean(&peer->children);

		HASH_DELETE(hh, *retainhier, peer);
		mosquitto_FREE(peer);
	}
}


शून्य retain__expiry_check(शून्य)
{
	यदि(db.config->retain_expiry_interval > 0 && db.now_s > अग्र_expire_check){
		retain__expire(&db.retains);
		अग्र_expire_check = db.now_s + db.config->retain_expiry_interval;
	}
}
