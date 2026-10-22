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

स्थिर संरचना mosquitto__retainhier *retain__add_hier_entry(संरचना mosquitto__retainhier *parent, संरचना mosquitto__retainhier **sibling, स्थिरांक अक्षर *topic, uint16_t len)
{
	संरचना mosquitto__retainhier *child;

	प्रतिज्ञा(sibling);

	child = mosquitto_calloc(1, आकार(संरचना mosquitto__retainhier) + len + 1);
	जर(!child){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Out of memory.");
		परत रिक्त;
	}
	child->parent = parent;
	child->topic_len = len;
	जर(len > 0){
		म_नकलन(child->topic, topic, (माप_प्रकार)(len+1));
	}

	HASH_ADD(hh, *sibling, topic, child->topic_len, child);

	परत child;
}


पूर्णांक retain__init(शून्य)
{
	संरचना mosquitto__retainhier *retainhier;

	retainhier = retain__add_hier_entry(रिक्त, &db.retains, "", 0);
	जर(!retainhier){
		परत MOSQ_ERR_NOMEM;
	}

	retainhier = retain__add_hier_entry(रिक्त, &db.retains, "$SYS", (uint16_t)म_माप("$SYS"));
	जर(!retainhier){
		परत MOSQ_ERR_NOMEM;
	}

	परत MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णांक mosquitto_persist_retain_संदेश_set(स्थिरांक अक्षर *topic, uint64_t base_संदेश_id)
{
	संरचना mosquitto_base_संदेश *base_संदेश;
	पूर्णांक rc = MOSQ_ERR_UNKNOWN;
	अक्षर **split_topics = रिक्त;
	अक्षर *local_topic = रिक्त;

	जर(topic == रिक्त){
		परत MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh, db.संदेश_store, &base_संदेश_id, आकार(base_संदेश_id), base_संदेश);
	जर(base_संदेश){
		जर(sub__topic_tokenise(topic, &local_topic, &split_topics, रिक्त)){
			परत MOSQ_ERR_NOMEM;
		}

		rc = retain__store(topic, base_संदेश, split_topics, false);
		mosquitto_free(split_topics);
		mosquitto_free(local_topic);
	}

	परत rc;
}


BROKER_EXPORT पूर्णांक mosquitto_persist_retain_संदेश_delete(स्थिरांक अक्षर *topic)
{
	संरचना mosquitto_base_संदेश base_संदेश;
	पूर्णांक rc = MOSQ_ERR_UNKNOWN;
	अक्षर **split_topics = रिक्त;
	अक्षर *local_topic = रिक्त;

	जर(topic == रिक्त){
		परत MOSQ_ERR_INVAL;
	}

	स_रखो(&base_संदेश, 0, आकार(base_संदेश));
	base_संदेश.ref_मोजणी = 10; /* Ensure this isn't freed */

	जर(sub__topic_tokenise(topic, &local_topic, &split_topics, रिक्त)){
		परत MOSQ_ERR_NOMEM;
	}

	/* With stored->payloadlen == 0, this means the message will be removed */
	rc = retain__store(topic, &base_संदेश, split_topics, false);
	mosquitto_FREE(split_topics);
	mosquitto_FREE(local_topic);

	परत rc;
}


शून्य retain__clean_empty_hierarchy(संरचना mosquitto__retainhier *retainhier)
{
	पर्यंत(retainhier){
		जर(retainhier->children || retainhier->retained || retainhier->parent == रिक्त){
			/* Entry is being used */
			परत;
		}अन्यथा{
			HASH_DELETE(hh, retainhier->parent->children, retainhier);

			संरचना mosquitto__retainhier *parent = retainhier->parent;
			mosquitto_FREE(retainhier);
			retainhier = parent;
		}
	}
}


पूर्णांक retain__store(स्थिरांक अक्षर *topic, संरचना mosquitto_base_संदेश *base_संदेश, अक्षर **split_topics, bool persist)
{
	संरचना mosquitto__retainhier *retainhier;
	संरचना mosquitto__retainhier *branch;
	माप_प्रकार slen;

	प्रतिज्ञा(base_संदेश);
	प्रतिज्ञा(split_topics);

	HASH_FIND(hh, db.retains, split_topics[0], म_माप(split_topics[0]), retainhier);
	जर(retainhier == रिक्त){
		retainhier = retain__add_hier_entry(रिक्त, &db.retains, split_topics[0], (uint16_t)म_माप(split_topics[0]));
		जर(!retainhier){
			परत MOSQ_ERR_NOMEM;
		}
	}

	साठी(पूर्णांक i=0; split_topics[i] != रिक्त; i++){
		slen = म_माप(split_topics[i]);
		HASH_FIND(hh, retainhier->children, split_topics[i], slen, branch);
		जर(branch == रिक्त){
			branch = retain__add_hier_entry(retainhier, &retainhier->children, split_topics[i], (uint16_t)slen);
			जर(branch == रिक्त){
				परत MOSQ_ERR_NOMEM;
			}
		}
		retainhier = branch;
	}

#ifdef WITH_PERSISTENCE
	जर(म_भेदन(topic, "$SYS", 4)){
		/* Retained messages count as a persistence change, but only if
		 * they aren't for $SYS. */
		db.persistence_changes++;
	}
#अन्यथा
	UNUSED(topic);
#endif

	जर(retainhier->retained){
		जर(retainhier->retained == base_संदेश){
			/* This may occur if multiple persistence providers are used */
			परत MOSQ_ERR_SUCCESS;
		}

		जर(persist && retainhier->retained->data.topic[0] != '$' && base_संदेश->data.payloadlen == 0){
			/* Only delete if another retained message isn't replacing this one */
			plugin_persist_handle_retain_संदेश_delete(retainhier->retained);
		}
		db_संदेश_store_ref_dec(&retainhier->retained);
#ifdef WITH_SYS_वृक्ष
		db.retained_मोजणी--;
#endif
		जर(base_संदेश->data.payloadlen == 0){
			retainhier->retained = रिक्त;
			retain__clean_empty_hierarchy(retainhier);
		}
	}
	जर(base_संदेश->data.payloadlen){
		retainhier->retained = base_संदेश;
		db_संदेश_store_ref_inc(retainhier->retained);
		जर(persist && retainhier->retained->data.topic[0] != '$'){
			plugin_persist_handle_base_संदेश_add(retainhier->retained);
			plugin_persist_handle_retain_संदेश_set(retainhier->retained);
		}
#ifdef WITH_SYS_वृक्ष
		db.retained_मोजणी++;
#endif
	}

	परत MOSQ_ERR_SUCCESS;
}


स्थिर bool retain_delete_expired_संदेश(संरचना mosquitto__retainhier *branch)
{
	जर(branch->retained && branch->retained->data.expiry_time > 0 && db.now_real_s >= branch->retained->data.expiry_time){
		plugin_persist_handle_retain_संदेश_delete(branch->retained);
		db_संदेश_store_ref_dec(&branch->retained);
		branch->retained = रिक्त;
#ifdef WITH_SYS_वृक्ष
		db.retained_मोजणी--;
#endif
		परत true;
	}
	परत false;
}


स्थिर पूर्णांक retain__process(संरचना mosquitto__retainhier *branch, संरचना mosquitto *context, स्थिरांक संरचना mosquitto_subscription *sub)
{
	पूर्णांक rc = 0;
	uint8_t qos, sub_qos;
	uint16_t mid;
	संरचना mosquitto_base_संदेश *retained;

	जर(retain_delete_expired_संदेश(branch)){
		परत MOSQ_ERR_SUCCESS;
	}

	retained = branch->retained;

	rc = mosquitto_acl_check(context, retained->data.topic, retained->data.payloadlen, retained->data.payload,
			retained->data.qos, retained->data.retain, retained->data.properties, MOSQ_ACL_READ);
	जर(rc == MOSQ_ERR_ACL_DENIED){
		परत MOSQ_ERR_SUCCESS;
	}अन्यथा जर(rc != MOSQ_ERR_SUCCESS){
		परत rc;
	}

	/* Check for original source access */
	जर(db.config->check_retain_source && retained->origin != mosq_mo_broker && retained->data.source_id){
		संरचना mosquitto retain_ctxt;
		स_रखो(&retain_ctxt, 0, आकार(संरचना mosquitto));

		retain_ctxt.id = retained->data.source_id;
		retain_ctxt.username = retained->data.source_username;
		retain_ctxt.listener = retained->source_listener;

		rc = mosquitto_acl_check(&retain_ctxt, retained->data.topic, retained->data.payloadlen, retained->data.payload,
				retained->data.qos, retained->data.retain, retained->data.properties, MOSQ_ACL_WRITE);
		जर(rc == MOSQ_ERR_ACL_DENIED){
			परत MOSQ_ERR_SUCCESS;
		}अन्यथा जर(rc != MOSQ_ERR_SUCCESS){
			परत rc;
		}
	}

	sub_qos = sub->options & 0x03;
	जर(db.config->upgrade_outgoing_qos){
		qos = sub_qos;
	}अन्यथा{
		qos = retained->data.qos;
		जर(qos > sub_qos){
			qos = sub_qos;
		}
	}
	जर(qos > 0){
		mid = mosquitto__mid_generate(context);
	}अन्यथा{
		mid = 0;
	}
	परत db_message_घाला_outgoing(context, 0, mid, qos, true, retained, sub->identifier, false, true);
}


स्थिर पूर्णांक retain_शोध(संरचना mosquitto__retainhier *retainhier, अक्षर **split_topics, संरचना mosquitto *context, स्थिरांक संरचना mosquitto_subscription *sub, पूर्णांक level)
{
	संरचना mosquitto__retainhier *branch, *branch_tmp;
	पूर्णांक flag = 0;

	जर(!म_भेद(split_topics[0], "#") && split_topics[1] == रिक्त){
		HASH_ITER(hh, retainhier->children, branch, branch_tmp){
			/* Set flag to indicate that we should check for retained messages
			 * on "foo" when we are subscribing to e.g. "foo/#" and then exit
			 * this function and return to an earlier retain__search().
			 */
			flag = -1;
			जर(branch->retained){
				retain__process(branch, context, sub);
			}
			जर(branch->children){
				retain_शोध(branch, split_topics, context, sub, level+1);
			}
		}
	}अन्यथा{
		जर(!म_भेद(split_topics[0], "+")){
			HASH_ITER(hh, retainhier->children, branch, branch_tmp){
				जर(split_topics[1] != रिक्त){
					जर(retain_शोध(branch, &(split_topics[1]), context, sub, level+1) == -1
							|| (split_topics[1] != रिक्त && !म_भेद(split_topics[1], "#") && level>0)){

						जर(branch->retained){
							retain__process(branch, context, sub);
						}
					}
				}अन्यथा{
					जर(branch->retained){
						retain__process(branch, context, sub);
					}
				}
			}
		}अन्यथा{
			HASH_FIND(hh, retainhier->children, split_topics[0], म_माप(split_topics[0]), branch);
			जर(branch){
				जर(split_topics[1] != रिक्त){
					जर(retain_शोध(branch, &(split_topics[1]), context, sub, level+1) == -1
							|| (split_topics[1] != रिक्त && !म_भेद(split_topics[1], "#") && level>0)){

						जर(branch->retained){
							retain__process(branch, context, sub);
						}
					}
				}अन्यथा{
					जर(branch->retained){
						retain__process(branch, context, sub);
					}
				}
			}
		}
	}
	परत flag;
}


पूर्णांक retain__queue(संरचना mosquitto *context, स्थिरांक संरचना mosquitto_subscription *sub)
{
	संरचना mosquitto__retainhier *retainhier;
	अक्षर *local_sub;
	अक्षर **split_topics;
	पूर्णांक rc;

	प्रतिज्ञा(context);
	प्रतिज्ञा(sub);

	जर(!म_भेदन(sub->topic_filter, "$share/", म_माप("$share/"))){
		परत MOSQ_ERR_SUCCESS;
	}

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &split_topics, रिक्त);
	जर(rc){
		परत rc;
	}

	HASH_FIND(hh, db.retains, split_topics[0], म_माप(split_topics[0]), retainhier);

	जर(retainhier){
		retain_शोध(retainhier, split_topics, context, sub, 0);
	}
	mosquitto_FREE(local_sub);
	mosquitto_FREE(split_topics);

	परत MOSQ_ERR_SUCCESS;
}


शून्य retain__expire(संरचना mosquitto__retainhier **retainhier)
{
	संरचना mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		retain__expire(&peer->children);

		संरचना mosquitto__retainhier *parent = peer->parent;
		जर(retain_delete_expired_संदेश(peer)){
			retain__clean_empty_hierarchy(parent);
		}
	}
}


शून्य retain__clean(संरचना mosquitto__retainhier **retainhier)
{
	संरचना mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		जर(peer->retained){
			db_संदेश_store_ref_dec(&peer->retained);
		}
		retain__clean(&peer->children);

		HASH_DELETE(hh, *retainhier, peer);
		mosquitto_FREE(peer);
	}
}


शून्य retain__expiry_check(शून्य)
{
	जर(db.config->retain_expiry_interval > 0 && db.now_s > next_expire_check){
		retain__expire(&db.retains);
		next_expire_check = db.now_s + db.config->retain_expiry_interval;
	}
}
