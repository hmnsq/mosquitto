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

स्थिर समय_प्रकार next_expire_check = 0;

स्थिर संरचना mosquitto__retainhier *retain__add_hier_entry(संरचना mosquitto__retainhier *parent, संरचना mosquitto__retainhier **sibling, अचर वर्ण *topic, uint16_t len)
{
	संरचना mosquitto__retainhier *child;

	प्रतिज्ञा(sibling);

	child = mosquitto_calloc(1, परिमाण(संरचना mosquitto__retainhier) + len + 1);
	यदि(!child){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		फर्काऊ रिक्त;
	}
	child->parent = parent;
	child->topic_len = len;
	यदि(len > 0){
		म_नकलन(child->topic, topic, (माप_प्रकार)(len+1));
	}

	HASH_ADD(hh, *sibling, topic, child->topic_len, child);

	फर्काऊ child;
}


पूर्णाङ्क retain__init(शून्य)
{
	संरचना mosquitto__retainhier *retainhier;

	retainhier = retain__add_hier_entry(रिक्त, &db.retains, "", 0);
	यदि(!retainhier){
		फर्काऊ MOSQ_ERR_NOMEM;
	}

	retainhier = retain__add_hier_entry(रिक्त, &db.retains, "$SYS", (uint16_t)म_माप("$SYS"));
	यदि(!retainhier){
		फर्काऊ MOSQ_ERR_NOMEM;
	}

	फर्काऊ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_persist_retain_सन्देश_set(अचर वर्ण *topic, uint64_t base_सन्देश_id)
{
	संरचना mosquitto_base_सन्देश *base_सन्देश;
	पूर्णाङ्क rc = MOSQ_ERR_UNKNOWN;
	वर्ण **split_topics = रिक्त;
	वर्ण *local_topic = रिक्त;

	यदि(topic == रिक्त){
		फर्काऊ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh, db.सन्देश_store, &base_सन्देश_id, परिमाण(base_सन्देश_id), base_सन्देश);
	यदि(base_सन्देश){
		यदि(sub__topic_tokenise(topic, &local_topic, &split_topics, रिक्त)){
			फर्काऊ MOSQ_ERR_NOMEM;
		}

		rc = retain__store(topic, base_सन्देश, split_topics, false);
		mosquitto_free(split_topics);
		mosquitto_free(local_topic);
	}

	फर्काऊ rc;
}


BROKER_EXPORT पूर्णाङ्क mosquitto_persist_retain_सन्देश_delete(अचर वर्ण *topic)
{
	संरचना mosquitto_base_सन्देश base_सन्देश;
	पूर्णाङ्क rc = MOSQ_ERR_UNKNOWN;
	वर्ण **split_topics = रिक्त;
	वर्ण *local_topic = रिक्त;

	यदि(topic == रिक्त){
		फर्काऊ MOSQ_ERR_INVAL;
	}

	स_रखो(&base_सन्देश, 0, परिमाण(base_सन्देश));
	base_सन्देश.ref_गन्ती = 10; /* Ensure this isn't freed */

	यदि(sub__topic_tokenise(topic, &local_topic, &split_topics, रिक्त)){
		फर्काऊ MOSQ_ERR_NOMEM;
	}

	/* With stored->payloadlen == 0, this means the message will be removed */
	rc = retain__store(topic, &base_सन्देश, split_topics, false);
	mosquitto_FREE(split_topics);
	mosquitto_FREE(local_topic);

	फर्काऊ rc;
}


शून्य retain__clean_empty_hierarchy(संरचना mosquitto__retainhier *retainhier)
{
	जबसम्म(retainhier){
		यदि(retainhier->children || retainhier->retained || retainhier->parent == रिक्त){
			/* Entry is being used */
			फर्काऊ;
		}अन्यथा{
			HASH_DELETE(hh, retainhier->parent->children, retainhier);

			संरचना mosquitto__retainhier *parent = retainhier->parent;
			mosquitto_FREE(retainhier);
			retainhier = parent;
		}
	}
}


पूर्णाङ्क retain__store(अचर वर्ण *topic, संरचना mosquitto_base_सन्देश *base_सन्देश, वर्ण **split_topics, bool persist)
{
	संरचना mosquitto__retainhier *retainhier;
	संरचना mosquitto__retainhier *branch;
	माप_प्रकार slen;

	प्रतिज्ञा(base_सन्देश);
	प्रतिज्ञा(split_topics);

	HASH_FIND(hh, db.retains, split_topics[0], म_माप(split_topics[0]), retainhier);
	यदि(retainhier == रिक्त){
		retainhier = retain__add_hier_entry(रिक्त, &db.retains, split_topics[0], (uint16_t)म_माप(split_topics[0]));
		यदि(!retainhier){
			फर्काऊ MOSQ_ERR_NOMEM;
		}
	}

	लागि(पूर्णाङ्क i=0; split_topics[i] != रिक्त; i++){
		slen = म_माप(split_topics[i]);
		HASH_FIND(hh, retainhier->children, split_topics[i], slen, branch);
		यदि(branch == रिक्त){
			branch = retain__add_hier_entry(retainhier, &retainhier->children, split_topics[i], (uint16_t)slen);
			यदि(branch == रिक्त){
				फर्काऊ MOSQ_ERR_NOMEM;
			}
		}
		retainhier = branch;
	}

#ifdef WITH_PERSISTENCE
	यदि(म_भेदन(topic, "$SYS", 4)){
		/* Retained messages count as a persistence change, but only if
		 * they aren't for $SYS. */
		db.persistence_changes++;
	}
#अन्यथा
	UNUSED(topic);
#endif

	यदि(retainhier->retained){
		यदि(retainhier->retained == base_सन्देश){
			/* This may occur if multiple persistence providers are used */
			फर्काऊ MOSQ_ERR_SUCCESS;
		}

		यदि(persist && retainhier->retained->data.topic[0] != '$' && base_सन्देश->data.payloadlen == 0){
			/* Only delete if another retained message isn't replacing this one */
			plugin_persist_handle_retain_सन्देश_delete(retainhier->retained);
		}
		db_सन्देश_store_ref_dec(&retainhier->retained);
#ifdef WITH_SYS_रूख
		db.retained_गन्ती--;
#endif
		यदि(base_सन्देश->data.payloadlen == 0){
			retainhier->retained = रिक्त;
			retain__clean_empty_hierarchy(retainhier);
		}
	}
	यदि(base_सन्देश->data.payloadlen){
		retainhier->retained = base_सन्देश;
		db_सन्देश_store_ref_inc(retainhier->retained);
		यदि(persist && retainhier->retained->data.topic[0] != '$'){
			plugin_persist_handle_base_सन्देश_add(retainhier->retained);
			plugin_persist_handle_retain_सन्देश_set(retainhier->retained);
		}
#ifdef WITH_SYS_रूख
		db.retained_गन्ती++;
#endif
	}

	फर्काऊ MOSQ_ERR_SUCCESS;
}


स्थिर bool retain_delete_expired_सन्देश(संरचना mosquitto__retainhier *branch)
{
	यदि(branch->retained && branch->retained->data.expiry_time > 0 && db.now_real_s >= branch->retained->data.expiry_time){
		plugin_persist_handle_retain_सन्देश_delete(branch->retained);
		db_सन्देश_store_ref_dec(&branch->retained);
		branch->retained = रिक्त;
#ifdef WITH_SYS_रूख
		db.retained_गन्ती--;
#endif
		फर्काऊ true;
	}
	फर्काऊ false;
}


स्थिर पूर्णाङ्क retain__process(संरचना mosquitto__retainhier *branch, संरचना mosquitto *context, अचर संरचना mosquitto_subscription *sub)
{
	पूर्णाङ्क rc = 0;
	uint8_t qos, sub_qos;
	uint16_t mid;
	संरचना mosquitto_base_सन्देश *retained;

	यदि(retain_delete_expired_सन्देश(branch)){
		फर्काऊ MOSQ_ERR_SUCCESS;
	}

	retained = branch->retained;

	rc = mosquitto_acl_check(context, retained->data.topic, retained->data.payloadlen, retained->data.payload,
			retained->data.qos, retained->data.retain, retained->data.properties, MOSQ_ACL_READ);
	यदि(rc == MOSQ_ERR_ACL_DENIED){
		फर्काऊ MOSQ_ERR_SUCCESS;
	}अन्यथा यदि(rc != MOSQ_ERR_SUCCESS){
		फर्काऊ rc;
	}

	/* Check for original source access */
	यदि(db.config->check_retain_source && retained->origin != mosq_mo_broker && retained->data.source_id){
		संरचना mosquitto retain_ctxt;
		स_रखो(&retain_ctxt, 0, परिमाण(संरचना mosquitto));

		retain_ctxt.id = retained->data.source_id;
		retain_ctxt.username = retained->data.source_username;
		retain_ctxt.listener = retained->source_listener;

		rc = mosquitto_acl_check(&retain_ctxt, retained->data.topic, retained->data.payloadlen, retained->data.payload,
				retained->data.qos, retained->data.retain, retained->data.properties, MOSQ_ACL_WRITE);
		यदि(rc == MOSQ_ERR_ACL_DENIED){
			फर्काऊ MOSQ_ERR_SUCCESS;
		}अन्यथा यदि(rc != MOSQ_ERR_SUCCESS){
			फर्काऊ rc;
		}
	}

	sub_qos = sub->options & 0x03;
	यदि(db.config->upgrade_outgoing_qos){
		qos = sub_qos;
	}अन्यथा{
		qos = retained->data.qos;
		यदि(qos > sub_qos){
			qos = sub_qos;
		}
	}
	यदि(qos > 0){
		mid = mosquitto__mid_generate(context);
	}अन्यथा{
		mid = 0;
	}
	फर्काऊ db_message_घुसाउ_outgoing(context, 0, mid, qos, true, retained, sub->identifier, false, true);
}


स्थिर पूर्णाङ्क retain_खोज(संरचना mosquitto__retainhier *retainhier, वर्ण **split_topics, संरचना mosquitto *context, अचर संरचना mosquitto_subscription *sub, पूर्णाङ्क level)
{
	संरचना mosquitto__retainhier *branch, *branch_tmp;
	पूर्णाङ्क flag = 0;

	यदि(!म_भेद(split_topics[0], "#") && split_topics[1] == रिक्त){
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
				retain_खोज(branch, split_topics, context, sub, level+1);
			}
		}
	}अन्यथा{
		यदि(!म_भेद(split_topics[0], "+")){
			HASH_ITER(hh, retainhier->children, branch, branch_tmp){
				यदि(split_topics[1] != रिक्त){
					यदि(retain_खोज(branch, &(split_topics[1]), context, sub, level+1) == -1
							|| (split_topics[1] != रिक्त && !म_भेद(split_topics[1], "#") && level>0)){

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
			HASH_FIND(hh, retainhier->children, split_topics[0], म_माप(split_topics[0]), branch);
			यदि(branch){
				यदि(split_topics[1] != रिक्त){
					यदि(retain_खोज(branch, &(split_topics[1]), context, sub, level+1) == -1
							|| (split_topics[1] != रिक्त && !म_भेद(split_topics[1], "#") && level>0)){

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
	फर्काऊ flag;
}


पूर्णाङ्क retain__queue(संरचना mosquitto *context, अचर संरचना mosquitto_subscription *sub)
{
	संरचना mosquitto__retainhier *retainhier;
	वर्ण *local_sub;
	वर्ण **split_topics;
	पूर्णाङ्क rc;

	प्रतिज्ञा(context);
	प्रतिज्ञा(sub);

	यदि(!म_भेदन(sub->topic_filter, "$share/", म_माप("$share/"))){
		फर्काऊ MOSQ_ERR_SUCCESS;
	}

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &split_topics, रिक्त);
	यदि(rc){
		फर्काऊ rc;
	}

	HASH_FIND(hh, db.retains, split_topics[0], म_माप(split_topics[0]), retainhier);

	यदि(retainhier){
		retain_खोज(retainhier, split_topics, context, sub, 0);
	}
	mosquitto_FREE(local_sub);
	mosquitto_FREE(split_topics);

	फर्काऊ MOSQ_ERR_SUCCESS;
}


शून्य retain__expire(संरचना mosquitto__retainhier **retainhier)
{
	संरचना mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		retain__expire(&peer->children);

		संरचना mosquitto__retainhier *parent = peer->parent;
		यदि(retain_delete_expired_सन्देश(peer)){
			retain__clean_empty_hierarchy(parent);
		}
	}
}


शून्य retain__clean(संरचना mosquitto__retainhier **retainhier)
{
	संरचना mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		यदि(peer->retained){
			db_सन्देश_store_ref_dec(&peer->retained);
		}
		retain__clean(&peer->children);

		HASH_DELETE(hh, *retainhier, peer);
		mosquitto_FREE(peer);
	}
}


शून्य retain__expiry_check(शून्य)
{
	यदि(db.config->retain_expiry_interval > 0 && db.now_s > next_expire_check){
		retain__expire(&db.retains);
		next_expire_check = db.now_s + db.config->retain_expiry_interval;
	}
}
