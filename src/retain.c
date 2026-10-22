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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ଅନ୍ତର୍ଭାଵ <assert.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto/mqtt_protocol.h"
#ଅନ୍ତର୍ଭାଵ "util_mosq.h"

#ଅନ୍ତର୍ଭାଵ "utlist.h"

ସ୍ଥିର time_t next_expire_check = 0;

ସ୍ଥିର ସଂରଚନା mosquitto__retainhier *retain__add_hier_entry(ସଂରଚନା mosquitto__retainhier *parent, ସଂରଚନା mosquitto__retainhier **sibling, ଅଚଳ ଅକ୍ଷର *topic, uint16_t len)
{
	ସଂରଚନା mosquitto__retainhier *child;

	ପ୍ରତିଜ୍ଞା(sibling);

	child = mosquitto_calloc(1, ଆକାର(ସଂରଚନା mosquitto__retainhier) + len + 1);
	ଯଦି(!child){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Out of memory.");
		ଫେରନ୍ତୁ ଖାଲି;
	}
	child->parent = parent;
	child->topic_len = len;
	ଯଦି(len > 0){
		strncpy(child->topic, topic, (size_t)(len+1));
	}

	HASH_ADD(hh, *sibling, topic, child->topic_len, child);

	ଫେରନ୍ତୁ child;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା retain__init(ଶୂନ୍ୟ)
{
	ସଂରଚନା mosquitto__retainhier *retainhier;

	retainhier = retain__add_hier_entry(ଖାଲି, &db.retains, "", 0);
	ଯଦି(!retainhier){
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}

	retainhier = retain__add_hier_entry(ଖାଲି, &db.retains, "$SYS", (uint16_t)strlen("$SYS"));
	ଯଦି(!retainhier){
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_persist_retain_ସନ୍ଦେଶ_set(ଅଚଳ ଅକ୍ଷର *topic, uint64_t base_ସନ୍ଦେଶ_id)
{
	ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = MOSQ_ERR_UNKNOWN;
	ଅକ୍ଷର **split_topics = ଖାଲି;
	ଅକ୍ଷର *local_topic = ଖାଲି;

	ଯଦି(topic == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh, db.ସନ୍ଦେଶ_store, &base_ସନ୍ଦେଶ_id, ଆକାର(base_ସନ୍ଦେଶ_id), base_ସନ୍ଦେଶ);
	ଯଦି(base_ସନ୍ଦେଶ){
		ଯଦି(sub__topic_tokenise(topic, &local_topic, &split_topics, ଖାଲି)){
			ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
		}

		rc = retain__store(topic, base_ସନ୍ଦେଶ, split_topics, false);
		mosquitto_free(split_topics);
		mosquitto_free(local_topic);
	}

	ଫେରନ୍ତୁ rc;
}


BROKER_EXPORT ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_persist_retain_ସନ୍ଦେଶ_delete(ଅଚଳ ଅକ୍ଷର *topic)
{
	ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ base_ସନ୍ଦେଶ;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = MOSQ_ERR_UNKNOWN;
	ଅକ୍ଷର **split_topics = ଖାଲି;
	ଅକ୍ଷର *local_topic = ଖାଲି;

	ଯଦି(topic == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	memset(&base_ସନ୍ଦେଶ, 0, ଆକାର(base_ସନ୍ଦେଶ));
	base_ସନ୍ଦେଶ.ref_ଗଣନା = 10; /* Ensure this isn't freed */

	ଯଦି(sub__topic_tokenise(topic, &local_topic, &split_topics, ଖାଲି)){
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}

	/* With stored->payloadlen == 0, this means the message will be removed */
	rc = retain__store(topic, &base_ସନ୍ଦେଶ, split_topics, false);
	mosquitto_FREE(split_topics);
	mosquitto_FREE(local_topic);

	ଫେରନ୍ତୁ rc;
}


ଶୂନ୍ୟ retain__clean_empty_hierarchy(ସଂରଚନା mosquitto__retainhier *retainhier)
{
	ଯେତେବେଳେ(retainhier){
		ଯଦି(retainhier->children || retainhier->retained || retainhier->parent == ଖାଲି){
			/* Entry is being used */
			ଫେରନ୍ତୁ;
		}ଅନ୍ୟଥା{
			HASH_DELETE(hh, retainhier->parent->children, retainhier);

			ସଂରଚନା mosquitto__retainhier *parent = retainhier->parent;
			mosquitto_FREE(retainhier);
			retainhier = parent;
		}
	}
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା retain__store(ଅଚଳ ଅକ୍ଷର *topic, ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *base_ସନ୍ଦେଶ, ଅକ୍ଷର **split_topics, bool persist)
{
	ସଂରଚନା mosquitto__retainhier *retainhier;
	ସଂରଚନା mosquitto__retainhier *branch;
	size_t slen;

	ପ୍ରତିଜ୍ଞା(base_ସନ୍ଦେଶ);
	ପ୍ରତିଜ୍ଞା(split_topics);

	HASH_FIND(hh, db.retains, split_topics[0], strlen(split_topics[0]), retainhier);
	ଯଦି(retainhier == ଖାଲି){
		retainhier = retain__add_hier_entry(ଖାଲି, &db.retains, split_topics[0], (uint16_t)strlen(split_topics[0]));
		ଯଦି(!retainhier){
			ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
		}
	}

	ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; split_topics[i] != ଖାଲି; i++){
		slen = strlen(split_topics[i]);
		HASH_FIND(hh, retainhier->children, split_topics[i], slen, branch);
		ଯଦି(branch == ଖାଲି){
			branch = retain__add_hier_entry(retainhier, &retainhier->children, split_topics[i], (uint16_t)slen);
			ଯଦି(branch == ଖାଲି){
				ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
			}
		}
		retainhier = branch;
	}

#ifdef WITH_PERSISTENCE
	ଯଦି(strncmp(topic, "$SYS", 4)){
		/* Retained messages count as a persistence change, but only if
		 * they aren't for $SYS. */
		db.persistence_changes++;
	}
#ଅନ୍ୟଥା
	UNUSED(topic);
#endif

	ଯଦି(retainhier->retained){
		ଯଦି(retainhier->retained == base_ସନ୍ଦେଶ){
			/* This may occur if multiple persistence providers are used */
			ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
		}

		ଯଦି(persist && retainhier->retained->data.topic[0] != '$' && base_ସନ୍ଦେଶ->data.payloadlen == 0){
			/* Only delete if another retained message isn't replacing this one */
			plugin_persist_handle_retain_ସନ୍ଦେଶ_delete(retainhier->retained);
		}
		db_ସନ୍ଦେଶ_store_ref_dec(&retainhier->retained);
#ifdef WITH_SYS_ବୃକ୍ଷ
		db.retained_ଗଣନା--;
#endif
		ଯଦି(base_ସନ୍ଦେଶ->data.payloadlen == 0){
			retainhier->retained = ଖାଲି;
			retain__clean_empty_hierarchy(retainhier);
		}
	}
	ଯଦି(base_ସନ୍ଦେଶ->data.payloadlen){
		retainhier->retained = base_ସନ୍ଦେଶ;
		db_ସନ୍ଦେଶ_store_ref_inc(retainhier->retained);
		ଯଦି(persist && retainhier->retained->data.topic[0] != '$'){
			plugin_persist_handle_base_ସନ୍ଦେଶ_add(retainhier->retained);
			plugin_persist_handle_retain_ସନ୍ଦେଶ_set(retainhier->retained);
		}
#ifdef WITH_SYS_ବୃକ୍ଷ
		db.retained_ଗଣନା++;
#endif
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର bool retain_delete_expired_ସନ୍ଦେଶ(ସଂରଚନା mosquitto__retainhier *branch)
{
	ଯଦି(branch->retained && branch->retained->data.expiry_time > 0 && db.now_real_s >= branch->retained->data.expiry_time){
		plugin_persist_handle_retain_ସନ୍ଦେଶ_delete(branch->retained);
		db_ସନ୍ଦେଶ_store_ref_dec(&branch->retained);
		branch->retained = ଖାଲି;
#ifdef WITH_SYS_ବୃକ୍ଷ
		db.retained_ଗଣନା--;
#endif
		ଫେରନ୍ତୁ true;
	}
	ଫେରନ୍ତୁ false;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା retain__process(ସଂରଚନା mosquitto__retainhier *branch, ସଂରଚନା mosquitto *context, ଅଚଳ ସଂରଚନା mosquitto_subscription *sub)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = 0;
	uint8_t qos, sub_qos;
	uint16_t mid;
	ସଂରଚନା mosquitto_base_ସନ୍ଦେଶ *retained;

	ଯଦି(retain_delete_expired_ସନ୍ଦେଶ(branch)){
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}

	retained = branch->retained;

	rc = mosquitto_acl_check(context, retained->data.topic, retained->data.payloadlen, retained->data.payload,
			retained->data.qos, retained->data.retain, retained->data.properties, MOSQ_ACL_READ);
	ଯଦି(rc == MOSQ_ERR_ACL_DENIED){
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}ଅନ୍ୟଥା ଯଦି(rc != MOSQ_ERR_SUCCESS){
		ଫେରନ୍ତୁ rc;
	}

	/* Check for original source access */
	ଯଦି(db.config->check_retain_source && retained->origin != mosq_mo_broker && retained->data.source_id){
		ସଂରଚନା mosquitto retain_ctxt;
		memset(&retain_ctxt, 0, ଆକାର(ସଂରଚନା mosquitto));

		retain_ctxt.id = retained->data.source_id;
		retain_ctxt.username = retained->data.source_username;
		retain_ctxt.listener = retained->source_listener;

		rc = mosquitto_acl_check(&retain_ctxt, retained->data.topic, retained->data.payloadlen, retained->data.payload,
				retained->data.qos, retained->data.retain, retained->data.properties, MOSQ_ACL_WRITE);
		ଯଦି(rc == MOSQ_ERR_ACL_DENIED){
			ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
		}ଅନ୍ୟଥା ଯଦି(rc != MOSQ_ERR_SUCCESS){
			ଫେରନ୍ତୁ rc;
		}
	}

	sub_qos = sub->options & 0x03;
	ଯଦି(db.config->upgrade_outgoing_qos){
		qos = sub_qos;
	}ଅନ୍ୟଥା{
		qos = retained->data.qos;
		ଯଦି(qos > sub_qos){
			qos = sub_qos;
		}
	}
	ଯଦି(qos > 0){
		mid = mosquitto__mid_generate(context);
	}ଅନ୍ୟଥା{
		mid = 0;
	}
	ଫେରନ୍ତୁ db_message_ଭର୍ତ୍ତି_outgoing(context, 0, mid, qos, true, retained, sub->identifier, false, true);
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା retain_ସନ୍ଧାନ(ସଂରଚନା mosquitto__retainhier *retainhier, ଅକ୍ଷର **split_topics, ସଂରଚନା mosquitto *context, ଅଚଳ ସଂରଚନା mosquitto_subscription *sub, ପୂର୍ଣ୍ଣସଂଖ୍ୟା level)
{
	ସଂରଚନା mosquitto__retainhier *branch, *branch_tmp;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା flag = 0;

	ଯଦି(!strcmp(split_topics[0], "#") && split_topics[1] == ଖାଲି){
		HASH_ITER(hh, retainhier->children, branch, branch_tmp){
			/* Set flag to indicate that we should check for retained messages
			 * on "foo" when we are subscribing to e.g. "foo/#" and then exit
			 * this function and return to an earlier retain__search().
			 */
			flag = -1;
			ଯଦି(branch->retained){
				retain__process(branch, context, sub);
			}
			ଯଦି(branch->children){
				retain_ସନ୍ଧାନ(branch, split_topics, context, sub, level+1);
			}
		}
	}ଅନ୍ୟଥା{
		ଯଦି(!strcmp(split_topics[0], "+")){
			HASH_ITER(hh, retainhier->children, branch, branch_tmp){
				ଯଦି(split_topics[1] != ଖାଲି){
					ଯଦି(retain_ସନ୍ଧାନ(branch, &(split_topics[1]), context, sub, level+1) == -1
							|| (split_topics[1] != ଖାଲି && !strcmp(split_topics[1], "#") && level>0)){

						ଯଦି(branch->retained){
							retain__process(branch, context, sub);
						}
					}
				}ଅନ୍ୟଥା{
					ଯଦି(branch->retained){
						retain__process(branch, context, sub);
					}
				}
			}
		}ଅନ୍ୟଥା{
			HASH_FIND(hh, retainhier->children, split_topics[0], strlen(split_topics[0]), branch);
			ଯଦି(branch){
				ଯଦି(split_topics[1] != ଖାଲି){
					ଯଦି(retain_ସନ୍ଧାନ(branch, &(split_topics[1]), context, sub, level+1) == -1
							|| (split_topics[1] != ଖାଲି && !strcmp(split_topics[1], "#") && level>0)){

						ଯଦି(branch->retained){
							retain__process(branch, context, sub);
						}
					}
				}ଅନ୍ୟଥା{
					ଯଦି(branch->retained){
						retain__process(branch, context, sub);
					}
				}
			}
		}
	}
	ଫେରନ୍ତୁ flag;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା retain__queue(ସଂରଚନା mosquitto *context, ଅଚଳ ସଂରଚନା mosquitto_subscription *sub)
{
	ସଂରଚନା mosquitto__retainhier *retainhier;
	ଅକ୍ଷର *local_sub;
	ଅକ୍ଷର **split_topics;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ପ୍ରତିଜ୍ଞା(context);
	ପ୍ରତିଜ୍ଞା(sub);

	ଯଦି(!strncmp(sub->topic_filter, "$share/", strlen("$share/"))){
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &split_topics, ଖାଲି);
	ଯଦି(rc){
		ଫେରନ୍ତୁ rc;
	}

	HASH_FIND(hh, db.retains, split_topics[0], strlen(split_topics[0]), retainhier);

	ଯଦି(retainhier){
		retain_ସନ୍ଧାନ(retainhier, split_topics, context, sub, 0);
	}
	mosquitto_FREE(local_sub);
	mosquitto_FREE(split_topics);

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ଶୂନ୍ୟ retain__expire(ସଂରଚନା mosquitto__retainhier **retainhier)
{
	ସଂରଚନା mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		retain__expire(&peer->children);

		ସଂରଚନା mosquitto__retainhier *parent = peer->parent;
		ଯଦି(retain_delete_expired_ସନ୍ଦେଶ(peer)){
			retain__clean_empty_hierarchy(parent);
		}
	}
}


ଶୂନ୍ୟ retain__clean(ସଂରଚନା mosquitto__retainhier **retainhier)
{
	ସଂରଚନା mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		ଯଦି(peer->retained){
			db_ସନ୍ଦେଶ_store_ref_dec(&peer->retained);
		}
		retain__clean(&peer->children);

		HASH_DELETE(hh, *retainhier, peer);
		mosquitto_FREE(peer);
	}
}


ଶୂନ୍ୟ retain__expiry_check(ଶୂନ୍ୟ)
{
	ଯଦି(db.config->retain_expiry_interval > 0 && db.now_s > next_expire_check){
		retain__expire(&db.retains);
		next_expire_check = db.now_s + db.config->retain_expiry_interval;
	}
}
