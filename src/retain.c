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

#समावेश "config.h"

#समावेश <assert.h>
#समावेश <stdio.h>
#समावेश <string.h>

#समावेश "mosquitto_broker_internal.h"
#समावेश "mosquitto/mqtt_protocol.h"
#समावेश "util_mosq.h"

#समावेश "utlist.h"

अटल समय_प्रकार अग्र_expire_check = 0;

अटल काष्ठा mosquitto__retainhier *retain__add_hier_entry(काष्ठा mosquitto__retainhier *parent, काष्ठा mosquitto__retainhier **sibling, स्थिर अक्षर *topic, uint16_t len)
{
	काष्ठा mosquitto__retainhier *child;

	निश्चित(sibling);

	child = mosquitto_calloc(1, माप(काष्ठा mosquitto__retainhier) + len + 1);
	अगर(!child){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: Out of memory.");
		वापस शून्य;
	}
	child->parent = parent;
	child->topic_len = len;
	अगर(len > 0){
		म_नकलन(child->topic, topic, (माप_प्रकार)(len+1));
	}

	HASH_ADD(hh, *sibling, topic, child->topic_len, child);

	वापस child;
}


पूर्णांक retain__init(व्योम)
{
	काष्ठा mosquitto__retainhier *retainhier;

	retainhier = retain__add_hier_entry(शून्य, &db.retains, "", 0);
	अगर(!retainhier){
		वापस MOSQ_ERR_NOMEM;
	}

	retainhier = retain__add_hier_entry(शून्य, &db.retains, "$SYS", (uint16_t)म_माप("$SYS"));
	अगर(!retainhier){
		वापस MOSQ_ERR_NOMEM;
	}

	वापस MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT पूर्णांक mosquitto_persist_retain_संदेश_set(स्थिर अक्षर *topic, uint64_t base_संदेश_id)
{
	काष्ठा mosquitto_base_संदेश *base_संदेश;
	पूर्णांक rc = MOSQ_ERR_UNKNOWN;
	अक्षर **split_topics = शून्य;
	अक्षर *local_topic = शून्य;

	अगर(topic == शून्य){
		वापस MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh, db.संदेश_store, &base_संदेश_id, माप(base_संदेश_id), base_संदेश);
	अगर(base_संदेश){
		अगर(sub__topic_tokenise(topic, &local_topic, &split_topics, शून्य)){
			वापस MOSQ_ERR_NOMEM;
		}

		rc = retain__store(topic, base_संदेश, split_topics, false);
		mosquitto_free(split_topics);
		mosquitto_free(local_topic);
	}

	वापस rc;
}


BROKER_EXPORT पूर्णांक mosquitto_persist_retain_संदेश_delete(स्थिर अक्षर *topic)
{
	काष्ठा mosquitto_base_संदेश base_संदेश;
	पूर्णांक rc = MOSQ_ERR_UNKNOWN;
	अक्षर **split_topics = शून्य;
	अक्षर *local_topic = शून्य;

	अगर(topic == शून्य){
		वापस MOSQ_ERR_INVAL;
	}

	स_रखो(&base_संदेश, 0, माप(base_संदेश));
	base_संदेश.ref_गिनती = 10; /* Ensure this isn't freed */

	अगर(sub__topic_tokenise(topic, &local_topic, &split_topics, शून्य)){
		वापस MOSQ_ERR_NOMEM;
	}

	/* With stored->payloadlen == 0, this means the message will be removed */
	rc = retain__store(topic, &base_संदेश, split_topics, false);
	mosquitto_FREE(split_topics);
	mosquitto_FREE(local_topic);

	वापस rc;
}


व्योम retain__clean_empty_hierarchy(काष्ठा mosquitto__retainhier *retainhier)
{
	जबतक(retainhier){
		अगर(retainhier->children || retainhier->retained || retainhier->parent == शून्य){
			/* Entry is being used */
			वापस;
		}अन्यथा{
			HASH_DELETE(hh, retainhier->parent->children, retainhier);

			काष्ठा mosquitto__retainhier *parent = retainhier->parent;
			mosquitto_FREE(retainhier);
			retainhier = parent;
		}
	}
}


पूर्णांक retain__store(स्थिर अक्षर *topic, काष्ठा mosquitto_base_संदेश *base_संदेश, अक्षर **split_topics, bool persist)
{
	काष्ठा mosquitto__retainhier *retainhier;
	काष्ठा mosquitto__retainhier *branch;
	माप_प्रकार slen;

	निश्चित(base_संदेश);
	निश्चित(split_topics);

	HASH_FIND(hh, db.retains, split_topics[0], म_माप(split_topics[0]), retainhier);
	अगर(retainhier == शून्य){
		retainhier = retain__add_hier_entry(शून्य, &db.retains, split_topics[0], (uint16_t)म_माप(split_topics[0]));
		अगर(!retainhier){
			वापस MOSQ_ERR_NOMEM;
		}
	}

	क्रम(पूर्णांक i=0; split_topics[i] != शून्य; i++){
		slen = म_माप(split_topics[i]);
		HASH_FIND(hh, retainhier->children, split_topics[i], slen, branch);
		अगर(branch == शून्य){
			branch = retain__add_hier_entry(retainhier, &retainhier->children, split_topics[i], (uint16_t)slen);
			अगर(branch == शून्य){
				वापस MOSQ_ERR_NOMEM;
			}
		}
		retainhier = branch;
	}

#अगर_घोषित WITH_PERSISTENCE
	अगर(म_भेदन(topic, "$SYS", 4)){
		/* Retained messages count as a persistence change, but only if
		 * they aren't for $SYS. */
		db.persistence_changes++;
	}
#अन्यथा
	UNUSED(topic);
#पूर्ण_अगर

	अगर(retainhier->retained){
		अगर(retainhier->retained == base_संदेश){
			/* This may occur if multiple persistence providers are used */
			वापस MOSQ_ERR_SUCCESS;
		}

		अगर(persist && retainhier->retained->तथ्य.topic[0] != '$' && base_संदेश->तथ्य.payloadlen == 0){
			/* Only delete if another retained message isn't replacing this one */
			plugin_persist_handle_retain_संदेश_delete(retainhier->retained);
		}
		db_संदेश_store_ref_dec(&retainhier->retained);
#अगर_घोषित WITH_SYS_वृक्ष
		db.retained_गिनती--;
#पूर्ण_अगर
		अगर(base_संदेश->तथ्य.payloadlen == 0){
			retainhier->retained = शून्य;
			retain__clean_empty_hierarchy(retainhier);
		}
	}
	अगर(base_संदेश->तथ्य.payloadlen){
		retainhier->retained = base_संदेश;
		db_संदेश_store_ref_inc(retainhier->retained);
		अगर(persist && retainhier->retained->तथ्य.topic[0] != '$'){
			plugin_persist_handle_base_संदेश_add(retainhier->retained);
			plugin_persist_handle_retain_संदेश_set(retainhier->retained);
		}
#अगर_घोषित WITH_SYS_वृक्ष
		db.retained_गिनती++;
#पूर्ण_अगर
	}

	वापस MOSQ_ERR_SUCCESS;
}


अटल bool retain_delete_expired_संदेश(काष्ठा mosquitto__retainhier *branch)
{
	अगर(branch->retained && branch->retained->तथ्य.expiry_time > 0 && db.now_real_s >= branch->retained->तथ्य.expiry_time){
		plugin_persist_handle_retain_संदेश_delete(branch->retained);
		db_संदेश_store_ref_dec(&branch->retained);
		branch->retained = शून्य;
#अगर_घोषित WITH_SYS_वृक्ष
		db.retained_गिनती--;
#पूर्ण_अगर
		वापस true;
	}
	वापस false;
}


अटल पूर्णांक retain__process(काष्ठा mosquitto__retainhier *branch, काष्ठा mosquitto *context, स्थिर काष्ठा mosquitto_subscription *sub)
{
	पूर्णांक rc = 0;
	uint8_t qos, sub_qos;
	uint16_t mid;
	काष्ठा mosquitto_base_संदेश *retained;

	अगर(retain_delete_expired_संदेश(branch)){
		वापस MOSQ_ERR_SUCCESS;
	}

	retained = branch->retained;

	rc = mosquitto_acl_check(context, retained->तथ्य.topic, retained->तथ्य.payloadlen, retained->तथ्य.payload,
			retained->तथ्य.qos, retained->तथ्य.retain, retained->तथ्य.properties, MOSQ_ACL_READ);
	अगर(rc == MOSQ_ERR_ACL_DENIED){
		वापस MOSQ_ERR_SUCCESS;
	}अन्यथा अगर(rc != MOSQ_ERR_SUCCESS){
		वापस rc;
	}

	/* Check for original source access */
	अगर(db.config->check_retain_source && retained->origin != mosq_mo_broker && retained->तथ्य.source_id){
		काष्ठा mosquitto retain_ctxt;
		स_रखो(&retain_ctxt, 0, माप(काष्ठा mosquitto));

		retain_ctxt.id = retained->तथ्य.source_id;
		retain_ctxt.username = retained->तथ्य.source_username;
		retain_ctxt.listener = retained->source_listener;

		rc = mosquitto_acl_check(&retain_ctxt, retained->तथ्य.topic, retained->तथ्य.payloadlen, retained->तथ्य.payload,
				retained->तथ्य.qos, retained->तथ्य.retain, retained->तथ्य.properties, MOSQ_ACL_WRITE);
		अगर(rc == MOSQ_ERR_ACL_DENIED){
			वापस MOSQ_ERR_SUCCESS;
		}अन्यथा अगर(rc != MOSQ_ERR_SUCCESS){
			वापस rc;
		}
	}

	sub_qos = sub->options & 0x03;
	अगर(db.config->upgrade_outgoing_qos){
		qos = sub_qos;
	}अन्यथा{
		qos = retained->तथ्य.qos;
		अगर(qos > sub_qos){
			qos = sub_qos;
		}
	}
	अगर(qos > 0){
		mid = mosquitto__mid_generate(context);
	}अन्यथा{
		mid = 0;
	}
	वापस db_message_जोड़ो_outgoing(context, 0, mid, qos, true, retained, sub->identifier, false, true);
}


अटल पूर्णांक retain_खोज(काष्ठा mosquitto__retainhier *retainhier, अक्षर **split_topics, काष्ठा mosquitto *context, स्थिर काष्ठा mosquitto_subscription *sub, पूर्णांक level)
{
	काष्ठा mosquitto__retainhier *branch, *branch_tmp;
	पूर्णांक flag = 0;

	अगर(!म_भेद(split_topics[0], "#") && split_topics[1] == शून्य){
		HASH_ITER(hh, retainhier->children, branch, branch_tmp){
			/* Set flag to indicate that we should check for retained messages
			 * on "foo" when we are subscribing to e.g. "foo/#" and then exit
			 * this function and return to an earlier retain__search().
			 */
			flag = -1;
			अगर(branch->retained){
				retain__process(branch, context, sub);
			}
			अगर(branch->children){
				retain_खोज(branch, split_topics, context, sub, level+1);
			}
		}
	}अन्यथा{
		अगर(!म_भेद(split_topics[0], "+")){
			HASH_ITER(hh, retainhier->children, branch, branch_tmp){
				अगर(split_topics[1] != शून्य){
					अगर(retain_खोज(branch, &(split_topics[1]), context, sub, level+1) == -1
							|| (split_topics[1] != शून्य && !म_भेद(split_topics[1], "#") && level>0)){

						अगर(branch->retained){
							retain__process(branch, context, sub);
						}
					}
				}अन्यथा{
					अगर(branch->retained){
						retain__process(branch, context, sub);
					}
				}
			}
		}अन्यथा{
			HASH_FIND(hh, retainhier->children, split_topics[0], म_माप(split_topics[0]), branch);
			अगर(branch){
				अगर(split_topics[1] != शून्य){
					अगर(retain_खोज(branch, &(split_topics[1]), context, sub, level+1) == -1
							|| (split_topics[1] != शून्य && !म_भेद(split_topics[1], "#") && level>0)){

						अगर(branch->retained){
							retain__process(branch, context, sub);
						}
					}
				}अन्यथा{
					अगर(branch->retained){
						retain__process(branch, context, sub);
					}
				}
			}
		}
	}
	वापस flag;
}


पूर्णांक retain__queue(काष्ठा mosquitto *context, स्थिर काष्ठा mosquitto_subscription *sub)
{
	काष्ठा mosquitto__retainhier *retainhier;
	अक्षर *local_sub;
	अक्षर **split_topics;
	पूर्णांक rc;

	निश्चित(context);
	निश्चित(sub);

	अगर(!म_भेदन(sub->topic_filter, "$share/", म_माप("$share/"))){
		वापस MOSQ_ERR_SUCCESS;
	}

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &split_topics, शून्य);
	अगर(rc){
		वापस rc;
	}

	HASH_FIND(hh, db.retains, split_topics[0], म_माप(split_topics[0]), retainhier);

	अगर(retainhier){
		retain_खोज(retainhier, split_topics, context, sub, 0);
	}
	mosquitto_FREE(local_sub);
	mosquitto_FREE(split_topics);

	वापस MOSQ_ERR_SUCCESS;
}


व्योम retain__expire(काष्ठा mosquitto__retainhier **retainhier)
{
	काष्ठा mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		retain__expire(&peer->children);

		काष्ठा mosquitto__retainhier *parent = peer->parent;
		अगर(retain_delete_expired_संदेश(peer)){
			retain__clean_empty_hierarchy(parent);
		}
	}
}


व्योम retain__clean(काष्ठा mosquitto__retainhier **retainhier)
{
	काष्ठा mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		अगर(peer->retained){
			db_संदेश_store_ref_dec(&peer->retained);
		}
		retain__clean(&peer->children);

		HASH_DELETE(hh, *retainhier, peer);
		mosquitto_FREE(peer);
	}
}


व्योम retain__expiry_check(व्योम)
{
	अगर(db.config->retain_expiry_interval > 0 && db.now_s > अग्र_expire_check){
		retain__expire(&db.retains);
		अग्र_expire_check = db.now_s + db.config->retain_expiry_interval;
	}
}
