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

#അന്തര്ഭാവ "config.h"

#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <string.h>

#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "mosquitto/mqtt_protocol.h"
#അന്തര്ഭാവ "util_mosq.h"

#അന്തര്ഭാവ "utlist.h"

സ്ഥിതം time_t next_expire_check = 0;

സ്ഥിതം ഘടന mosquitto__retainhier *retain__add_hier_entry(ഘടന mosquitto__retainhier *parent, ഘടന mosquitto__retainhier **sibling, സ്ഥിരം അക്ഷരം *topic, uint16_t len)
{
	ഘടന mosquitto__retainhier *child;

	പ്രതിജ്ഞാ(sibling);

	child = mosquitto_calloc(1, വലുപ്പം(ഘടന mosquitto__retainhier) + len + 1);
	എങ്കിൽ(!child){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Out of memory.");
		തിരികെ ശൂന്യ;
	}
	child->parent = parent;
	child->topic_len = len;
	എങ്കിൽ(len > 0){
		strncpy(child->topic, topic, (size_t)(len+1));
	}

	HASH_ADD(hh, *sibling, topic, child->topic_len, child);

	തിരികെ child;
}


പൂർണ്ണസംഖ്യ retain__init(ശൂന്യം)
{
	ഘടന mosquitto__retainhier *retainhier;

	retainhier = retain__add_hier_entry(ശൂന്യ, &db.retains, "", 0);
	എങ്കിൽ(!retainhier){
		തിരികെ MOSQ_ERR_NOMEM;
	}

	retainhier = retain__add_hier_entry(ശൂന്യ, &db.retains, "$SYS", (uint16_t)strlen("$SYS"));
	എങ്കിൽ(!retainhier){
		തിരികെ MOSQ_ERR_NOMEM;
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT പൂർണ്ണസംഖ്യ mosquitto_persist_retain_സന്ദേശം_set(സ്ഥിരം അക്ഷരം *topic, uint64_t base_സന്ദേശം_id)
{
	ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം;
	പൂർണ്ണസംഖ്യ rc = MOSQ_ERR_UNKNOWN;
	അക്ഷരം **split_topics = ശൂന്യ;
	അക്ഷരം *local_topic = ശൂന്യ;

	എങ്കിൽ(topic == ശൂന്യ){
		തിരികെ MOSQ_ERR_INVAL;
	}

	HASH_FIND(hh, db.സന്ദേശം_store, &base_സന്ദേശം_id, വലുപ്പം(base_സന്ദേശം_id), base_സന്ദേശം);
	എങ്കിൽ(base_സന്ദേശം){
		എങ്കിൽ(sub__topic_tokenise(topic, &local_topic, &split_topics, ശൂന്യ)){
			തിരികെ MOSQ_ERR_NOMEM;
		}

		rc = retain__store(topic, base_സന്ദേശം, split_topics, false);
		mosquitto_free(split_topics);
		mosquitto_free(local_topic);
	}

	തിരികെ rc;
}


BROKER_EXPORT പൂർണ്ണസംഖ്യ mosquitto_persist_retain_സന്ദേശം_delete(സ്ഥിരം അക്ഷരം *topic)
{
	ഘടന mosquitto_base_സന്ദേശം base_സന്ദേശം;
	പൂർണ്ണസംഖ്യ rc = MOSQ_ERR_UNKNOWN;
	അക്ഷരം **split_topics = ശൂന്യ;
	അക്ഷരം *local_topic = ശൂന്യ;

	എങ്കിൽ(topic == ശൂന്യ){
		തിരികെ MOSQ_ERR_INVAL;
	}

	memset(&base_സന്ദേശം, 0, വലുപ്പം(base_സന്ദേശം));
	base_സന്ദേശം.ref_എണ്ണം = 10; /* Ensure this isn't freed */

	എങ്കിൽ(sub__topic_tokenise(topic, &local_topic, &split_topics, ശൂന്യ)){
		തിരികെ MOSQ_ERR_NOMEM;
	}

	/* With stored->payloadlen == 0, this means the message will be removed */
	rc = retain__store(topic, &base_സന്ദേശം, split_topics, false);
	mosquitto_FREE(split_topics);
	mosquitto_FREE(local_topic);

	തിരികെ rc;
}


ശൂന്യം retain__clean_empty_hierarchy(ഘടന mosquitto__retainhier *retainhier)
{
	ആകുമ്പോൾ(retainhier){
		എങ്കിൽ(retainhier->children || retainhier->retained || retainhier->parent == ശൂന്യ){
			/* Entry is being used */
			തിരികെ;
		}അല്ലെങ്കിൽ{
			HASH_DELETE(hh, retainhier->parent->children, retainhier);

			ഘടന mosquitto__retainhier *parent = retainhier->parent;
			mosquitto_FREE(retainhier);
			retainhier = parent;
		}
	}
}


പൂർണ്ണസംഖ്യ retain__store(സ്ഥിരം അക്ഷരം *topic, ഘടന mosquitto_base_സന്ദേശം *base_സന്ദേശം, അക്ഷരം **split_topics, bool persist)
{
	ഘടന mosquitto__retainhier *retainhier;
	ഘടന mosquitto__retainhier *branch;
	size_t slen;

	പ്രതിജ്ഞാ(base_സന്ദേശം);
	പ്രതിജ്ഞാ(split_topics);

	HASH_FIND(hh, db.retains, split_topics[0], strlen(split_topics[0]), retainhier);
	എങ്കിൽ(retainhier == ശൂന്യ){
		retainhier = retain__add_hier_entry(ശൂന്യ, &db.retains, split_topics[0], (uint16_t)strlen(split_topics[0]));
		എങ്കിൽ(!retainhier){
			തിരികെ MOSQ_ERR_NOMEM;
		}
	}

	വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; split_topics[i] != ശൂന്യ; i++){
		slen = strlen(split_topics[i]);
		HASH_FIND(hh, retainhier->children, split_topics[i], slen, branch);
		എങ്കിൽ(branch == ശൂന്യ){
			branch = retain__add_hier_entry(retainhier, &retainhier->children, split_topics[i], (uint16_t)slen);
			എങ്കിൽ(branch == ശൂന്യ){
				തിരികെ MOSQ_ERR_NOMEM;
			}
		}
		retainhier = branch;
	}

#ifdef WITH_PERSISTENCE
	എങ്കിൽ(strncmp(topic, "$SYS", 4)){
		/* Retained messages count as a persistence change, but only if
		 * they aren't for $SYS. */
		db.persistence_changes++;
	}
#അല്ലെങ്കിൽ
	UNUSED(topic);
#endif

	എങ്കിൽ(retainhier->retained){
		എങ്കിൽ(retainhier->retained == base_സന്ദേശം){
			/* This may occur if multiple persistence providers are used */
			തിരികെ MOSQ_ERR_SUCCESS;
		}

		എങ്കിൽ(persist && retainhier->retained->data.topic[0] != '$' && base_സന്ദേശം->data.payloadlen == 0){
			/* Only delete if another retained message isn't replacing this one */
			plugin_persist_handle_retain_സന്ദേശം_delete(retainhier->retained);
		}
		db_സന്ദേശം_store_ref_dec(&retainhier->retained);
#ifdef WITH_SYS_വൃക്ഷം
		db.retained_എണ്ണം--;
#endif
		എങ്കിൽ(base_സന്ദേശം->data.payloadlen == 0){
			retainhier->retained = ശൂന്യ;
			retain__clean_empty_hierarchy(retainhier);
		}
	}
	എങ്കിൽ(base_സന്ദേശം->data.payloadlen){
		retainhier->retained = base_സന്ദേശം;
		db_സന്ദേശം_store_ref_inc(retainhier->retained);
		എങ്കിൽ(persist && retainhier->retained->data.topic[0] != '$'){
			plugin_persist_handle_base_സന്ദേശം_add(retainhier->retained);
			plugin_persist_handle_retain_സന്ദേശം_set(retainhier->retained);
		}
#ifdef WITH_SYS_വൃക്ഷം
		db.retained_എണ്ണം++;
#endif
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം bool retain_delete_expired_സന്ദേശം(ഘടന mosquitto__retainhier *branch)
{
	എങ്കിൽ(branch->retained && branch->retained->data.expiry_time > 0 && db.now_real_s >= branch->retained->data.expiry_time){
		plugin_persist_handle_retain_സന്ദേശം_delete(branch->retained);
		db_സന്ദേശം_store_ref_dec(&branch->retained);
		branch->retained = ശൂന്യ;
#ifdef WITH_SYS_വൃക്ഷം
		db.retained_എണ്ണം--;
#endif
		തിരികെ true;
	}
	തിരികെ false;
}


സ്ഥിതം പൂർണ്ണസംഖ്യ retain__process(ഘടന mosquitto__retainhier *branch, ഘടന mosquitto *context, സ്ഥിരം ഘടന mosquitto_subscription *sub)
{
	പൂർണ്ണസംഖ്യ rc = 0;
	uint8_t qos, sub_qos;
	uint16_t mid;
	ഘടന mosquitto_base_സന്ദേശം *retained;

	എങ്കിൽ(retain_delete_expired_സന്ദേശം(branch)){
		തിരികെ MOSQ_ERR_SUCCESS;
	}

	retained = branch->retained;

	rc = mosquitto_acl_check(context, retained->data.topic, retained->data.payloadlen, retained->data.payload,
			retained->data.qos, retained->data.retain, retained->data.properties, MOSQ_ACL_READ);
	എങ്കിൽ(rc == MOSQ_ERR_ACL_DENIED){
		തിരികെ MOSQ_ERR_SUCCESS;
	}അല്ലെങ്കിൽ എങ്കിൽ(rc != MOSQ_ERR_SUCCESS){
		തിരികെ rc;
	}

	/* Check for original source access */
	എങ്കിൽ(db.config->check_retain_source && retained->origin != mosq_mo_broker && retained->data.source_id){
		ഘടന mosquitto retain_ctxt;
		memset(&retain_ctxt, 0, വലുപ്പം(ഘടന mosquitto));

		retain_ctxt.id = retained->data.source_id;
		retain_ctxt.username = retained->data.source_username;
		retain_ctxt.listener = retained->source_listener;

		rc = mosquitto_acl_check(&retain_ctxt, retained->data.topic, retained->data.payloadlen, retained->data.payload,
				retained->data.qos, retained->data.retain, retained->data.properties, MOSQ_ACL_WRITE);
		എങ്കിൽ(rc == MOSQ_ERR_ACL_DENIED){
			തിരികെ MOSQ_ERR_SUCCESS;
		}അല്ലെങ്കിൽ എങ്കിൽ(rc != MOSQ_ERR_SUCCESS){
			തിരികെ rc;
		}
	}

	sub_qos = sub->options & 0x03;
	എങ്കിൽ(db.config->upgrade_outgoing_qos){
		qos = sub_qos;
	}അല്ലെങ്കിൽ{
		qos = retained->data.qos;
		എങ്കിൽ(qos > sub_qos){
			qos = sub_qos;
		}
	}
	എങ്കിൽ(qos > 0){
		mid = mosquitto__mid_generate(context);
	}അല്ലെങ്കിൽ{
		mid = 0;
	}
	തിരികെ db_message_തിരുകുക_outgoing(context, 0, mid, qos, true, retained, sub->identifier, false, true);
}


സ്ഥിതം പൂർണ്ണസംഖ്യ retain_തിരയൽ(ഘടന mosquitto__retainhier *retainhier, അക്ഷരം **split_topics, ഘടന mosquitto *context, സ്ഥിരം ഘടന mosquitto_subscription *sub, പൂർണ്ണസംഖ്യ level)
{
	ഘടന mosquitto__retainhier *branch, *branch_tmp;
	പൂർണ്ണസംഖ്യ flag = 0;

	എങ്കിൽ(!strcmp(split_topics[0], "#") && split_topics[1] == ശൂന്യ){
		HASH_ITER(hh, retainhier->children, branch, branch_tmp){
			/* Set flag to indicate that we should check for retained messages
			 * on "foo" when we are subscribing to e.g. "foo/#" and then exit
			 * this function and return to an earlier retain__search().
			 */
			flag = -1;
			എങ്കിൽ(branch->retained){
				retain__process(branch, context, sub);
			}
			എങ്കിൽ(branch->children){
				retain_തിരയൽ(branch, split_topics, context, sub, level+1);
			}
		}
	}അല്ലെങ്കിൽ{
		എങ്കിൽ(!strcmp(split_topics[0], "+")){
			HASH_ITER(hh, retainhier->children, branch, branch_tmp){
				എങ്കിൽ(split_topics[1] != ശൂന്യ){
					എങ്കിൽ(retain_തിരയൽ(branch, &(split_topics[1]), context, sub, level+1) == -1
							|| (split_topics[1] != ശൂന്യ && !strcmp(split_topics[1], "#") && level>0)){

						എങ്കിൽ(branch->retained){
							retain__process(branch, context, sub);
						}
					}
				}അല്ലെങ്കിൽ{
					എങ്കിൽ(branch->retained){
						retain__process(branch, context, sub);
					}
				}
			}
		}അല്ലെങ്കിൽ{
			HASH_FIND(hh, retainhier->children, split_topics[0], strlen(split_topics[0]), branch);
			എങ്കിൽ(branch){
				എങ്കിൽ(split_topics[1] != ശൂന്യ){
					എങ്കിൽ(retain_തിരയൽ(branch, &(split_topics[1]), context, sub, level+1) == -1
							|| (split_topics[1] != ശൂന്യ && !strcmp(split_topics[1], "#") && level>0)){

						എങ്കിൽ(branch->retained){
							retain__process(branch, context, sub);
						}
					}
				}അല്ലെങ്കിൽ{
					എങ്കിൽ(branch->retained){
						retain__process(branch, context, sub);
					}
				}
			}
		}
	}
	തിരികെ flag;
}


പൂർണ്ണസംഖ്യ retain__queue(ഘടന mosquitto *context, സ്ഥിരം ഘടന mosquitto_subscription *sub)
{
	ഘടന mosquitto__retainhier *retainhier;
	അക്ഷരം *local_sub;
	അക്ഷരം **split_topics;
	പൂർണ്ണസംഖ്യ rc;

	പ്രതിജ്ഞാ(context);
	പ്രതിജ്ഞാ(sub);

	എങ്കിൽ(!strncmp(sub->topic_filter, "$share/", strlen("$share/"))){
		തിരികെ MOSQ_ERR_SUCCESS;
	}

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &split_topics, ശൂന്യ);
	എങ്കിൽ(rc){
		തിരികെ rc;
	}

	HASH_FIND(hh, db.retains, split_topics[0], strlen(split_topics[0]), retainhier);

	എങ്കിൽ(retainhier){
		retain_തിരയൽ(retainhier, split_topics, context, sub, 0);
	}
	mosquitto_FREE(local_sub);
	mosquitto_FREE(split_topics);

	തിരികെ MOSQ_ERR_SUCCESS;
}


ശൂന്യം retain__expire(ഘടന mosquitto__retainhier **retainhier)
{
	ഘടന mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		retain__expire(&peer->children);

		ഘടന mosquitto__retainhier *parent = peer->parent;
		എങ്കിൽ(retain_delete_expired_സന്ദേശം(peer)){
			retain__clean_empty_hierarchy(parent);
		}
	}
}


ശൂന്യം retain__clean(ഘടന mosquitto__retainhier **retainhier)
{
	ഘടന mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		എങ്കിൽ(peer->retained){
			db_സന്ദേശം_store_ref_dec(&peer->retained);
		}
		retain__clean(&peer->children);

		HASH_DELETE(hh, *retainhier, peer);
		mosquitto_FREE(peer);
	}
}


ശൂന്യം retain__expiry_check(ശൂന്യം)
{
	എങ്കിൽ(db.config->retain_expiry_interval > 0 && db.now_s > next_expire_check){
		retain__expire(&db.retains);
		next_expire_check = db.now_s + db.config->retain_expiry_interval;
	}
}
