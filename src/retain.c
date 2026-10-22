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

#ضمن "config.h"

#ضمن <assert.h>
#ضمن <stdio.h>
#ضمن <string.h>

#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/mqtt_protocol.h"
#ضمن "util_mosq.h"

#ضمن "utlist.h"

ساكن time_t ايندڙ_expire_check = 0;

ساكن بنية mosquitto__retainhier *retain_جوڙيو_hier_داخلا(بنية mosquitto__retainhier *والد, بنية mosquitto__retainhier **sibling, ثابت حرف *topic, uint16_t len)
{
	بنية mosquitto__retainhier *ٻار;

	أكد(sibling);

	ٻار = mosquitto_calloc(1, حجم(بنية mosquitto__retainhier) + len + 1);
	إذا(!ٻار){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		أرجع عدم;
	}
	ٻار->والد = والد;
	ٻار->topic_len = len;
	إذا(len > 0){
		strncpy(ٻار->topic, topic, (size_t)(len+1));
	}

	HASH_جوڙيو(hh, *sibling, topic, ٻار->topic_len, ٻار);

	أرجع ٻار;
}


صحيح retain__init(فراغ)
{
	بنية mosquitto__retainhier *retainhier;

	retainhier = retain_جوڙيو_hier_داخلا(عدم, &db.retains, "", 0);
	إذا(!retainhier){
		أرجع MOSQ_ERR_NOMEM;
	}

	retainhier = retain_جوڙيو_hier_داخلا(عدم, &db.retains, "$SYS", (uint16_t)لڙي_ڊيگهه("$SYS"));
	إذا(!retainhier){
		أرجع MOSQ_ERR_NOMEM;
	}

	أرجع MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحيح mosquitto_persist_retain_نياپو_سيٽ(ثابت حرف *topic, uint64_t بنياد_نياپو_id)
{
	بنية mosquitto_بنياد_نياپو *بنياد_نياپو;
	صحيح rc = MOSQ_ERR_UNKNOWN;
	حرف **ورهايو_topics = عدم;
	حرف *local_topic = عدم;

	إذا(topic == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	HASH_ڳوليو(hh, db.نياپو_store, &بنياد_نياپو_id, حجم(بنياد_نياپو_id), بنياد_نياپو);
	إذا(بنياد_نياپو){
		إذا(sub__topic_tokenise(topic, &local_topic, &ورهايو_topics, عدم)){
			أرجع MOSQ_ERR_NOMEM;
		}

		rc = retain__store(topic, بنياد_نياپو, ورهايو_topics, false);
		mosquitto_free(ورهايو_topics);
		mosquitto_free(local_topic);
	}

	أرجع rc;
}


BROKER_EXPORT صحيح mosquitto_persist_retain_نياپو_delete(ثابت حرف *topic)
{
	بنية mosquitto_بنياد_نياپو بنياد_نياپو;
	صحيح rc = MOSQ_ERR_UNKNOWN;
	حرف **ورهايو_topics = عدم;
	حرف *local_topic = عدم;

	إذا(topic == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	memset(&بنياد_نياپو, 0, حجم(بنياد_نياپو));
	بنياد_نياپو.ref_ڳڻپ = 10; /* Ensure this isn't freed */

	إذا(sub__topic_tokenise(topic, &local_topic, &ورهايو_topics, عدم)){
		أرجع MOSQ_ERR_NOMEM;
	}

	/* With stored->payloadlen == 0, this means the message will be removed */
	rc = retain__store(topic, &بنياد_نياپو, ورهايو_topics, false);
	mosquitto_FREE(ورهايو_topics);
	mosquitto_FREE(local_topic);

	أرجع rc;
}


فراغ retain_clean_خالي_جايون_hierarchy(بنية mosquitto__retainhier *retainhier)
{
	طالما(retainhier){
		إذا(retainhier->children || retainhier->retained || retainhier->والد == عدم){
			/* Entry is being used */
			أرجع;
		}وإلا{
			HASH_DELETE(hh, retainhier->والد->children, retainhier);

			بنية mosquitto__retainhier *والد = retainhier->والد;
			mosquitto_FREE(retainhier);
			retainhier = والد;
		}
	}
}


صحيح retain__store(ثابت حرف *topic, بنية mosquitto_بنياد_نياپو *بنياد_نياپو, حرف **ورهايو_topics, bool persist)
{
	بنية mosquitto__retainhier *retainhier;
	بنية mosquitto__retainhier *branch;
	size_t slen;

	أكد(بنياد_نياپو);
	أكد(ورهايو_topics);

	HASH_ڳوليو(hh, db.retains, ورهايو_topics[0], لڙي_ڊيگهه(ورهايو_topics[0]), retainhier);
	إذا(retainhier == عدم){
		retainhier = retain_جوڙيو_hier_داخلا(عدم, &db.retains, ورهايو_topics[0], (uint16_t)لڙي_ڊيگهه(ورهايو_topics[0]));
		إذا(!retainhier){
			أرجع MOSQ_ERR_NOMEM;
		}
	}

	لكل(صحيح i=0; ورهايو_topics[i] != عدم; i++){
		slen = لڙي_ڊيگهه(ورهايو_topics[i]);
		HASH_ڳوليو(hh, retainhier->children, ورهايو_topics[i], slen, branch);
		إذا(branch == عدم){
			branch = retain_جوڙيو_hier_داخلا(retainhier, &retainhier->children, ورهايو_topics[i], (uint16_t)slen);
			إذا(branch == عدم){
				أرجع MOSQ_ERR_NOMEM;
			}
		}
		retainhier = branch;
	}

#جيڪڏهن_وصف_ٿيل WITH_PERSISTENCE
	إذا(لڙي_ڀيٽ(topic, "$SYS", 4)){
		/* Retained messages count as a persistence change, but only if
		 * they aren't for $SYS. */
		db.persistence_changes++;
	}
#وإلا
	اڻ_استعمال_ٿيل(topic);
#جيڪڏهن_پورو

	إذا(retainhier->retained){
		إذا(retainhier->retained == بنياد_نياپو){
			/* This may occur if multiple persistence providers are used */
			أرجع MOSQ_ERR_SUCCESS;
		}

		إذا(persist && retainhier->retained->معلومات.topic[0] != '$' && بنياد_نياپو->معلومات.payloadlen == 0){
			/* Only delete if another retained message isn't replacing this one */
			plugin_persist_handle_retain_نياپو_delete(retainhier->retained);
		}
		db_نياپو_store_ref_dec(&retainhier->retained);
#جيڪڏهن_وصف_ٿيل WITH_SYS_وڻ
		db.retained_ڳڻپ--;
#جيڪڏهن_پورو
		إذا(بنياد_نياپو->معلومات.payloadlen == 0){
			retainhier->retained = عدم;
			retain_clean_خالي_جايون_hierarchy(retainhier);
		}
	}
	إذا(بنياد_نياپو->معلومات.payloadlen){
		retainhier->retained = بنياد_نياپو;
		db_نياپو_store_ref_inc(retainhier->retained);
		إذا(persist && retainhier->retained->معلومات.topic[0] != '$'){
			plugin_persist_handle_بنياد_نياپو_جوڙيو(retainhier->retained);
			plugin_persist_handle_retain_نياپو_سيٽ(retainhier->retained);
		}
#جيڪڏهن_وصف_ٿيل WITH_SYS_وڻ
		db.retained_ڳڻپ++;
#جيڪڏهن_پورو
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن bool retain_delete_expired_نياپو(بنية mosquitto__retainhier *branch)
{
	إذا(branch->retained && branch->retained->معلومات.expiry_وقت > 0 && db.now_real_s >= branch->retained->معلومات.expiry_وقت){
		plugin_persist_handle_retain_نياپو_delete(branch->retained);
		db_نياپو_store_ref_dec(&branch->retained);
		branch->retained = عدم;
#جيڪڏهن_وصف_ٿيل WITH_SYS_وڻ
		db.retained_ڳڻپ--;
#جيڪڏهن_پورو
		أرجع true;
	}
	أرجع false;
}


ساكن صحيح retain__process(بنية mosquitto__retainhier *branch, بنية mosquitto *context, ثابت بنية mosquitto_subscription *sub)
{
	صحيح rc = 0;
	uint8_t qos, sub_qos;
	uint16_t mid;
	بنية mosquitto_بنياد_نياپو *retained;

	إذا(retain_delete_expired_نياپو(branch)){
		أرجع MOSQ_ERR_SUCCESS;
	}

	retained = branch->retained;

	rc = mosquitto_acl_check(context, retained->معلومات.topic, retained->معلومات.payloadlen, retained->معلومات.payload,
			retained->معلومات.qos, retained->معلومات.retain, retained->معلومات.properties, MOSQ_ACL_READ);
	إذا(rc == MOSQ_ERR_ACL_DENIED){
		أرجع MOSQ_ERR_SUCCESS;
	}وإلا إذا(rc != MOSQ_ERR_SUCCESS){
		أرجع rc;
	}

	/* Check for original source access */
	إذا(db.config->check_retain_ذريعو && retained->origin != mosq_mo_broker && retained->معلومات.ذريعو_id){
		بنية mosquitto retain_ctxt;
		memset(&retain_ctxt, 0, حجم(بنية mosquitto));

		retain_ctxt.id = retained->معلومات.ذريعو_id;
		retain_ctxt.username = retained->معلومات.ذريعو_username;
		retain_ctxt.listener = retained->ذريعو_listener;

		rc = mosquitto_acl_check(&retain_ctxt, retained->معلومات.topic, retained->معلومات.payloadlen, retained->معلومات.payload,
				retained->معلومات.qos, retained->معلومات.retain, retained->معلومات.properties, MOSQ_ACL_WRITE);
		إذا(rc == MOSQ_ERR_ACL_DENIED){
			أرجع MOSQ_ERR_SUCCESS;
		}وإلا إذا(rc != MOSQ_ERR_SUCCESS){
			أرجع rc;
		}
	}

	sub_qos = sub->options & 0x03;
	إذا(db.config->upgrade_outgoing_qos){
		qos = sub_qos;
	}وإلا{
		qos = retained->معلومات.qos;
		إذا(qos > sub_qos){
			qos = sub_qos;
		}
	}
	إذا(qos > 0){
		mid = mosquitto__mid_generate(context);
	}وإلا{
		mid = 0;
	}
	أرجع db_پيغام_وجهو_outgoing(context, 0, mid, qos, true, retained, sub->identifier, false, true);
}


ساكن صحيح retain_ڳولا(بنية mosquitto__retainhier *retainhier, حرف **ورهايو_topics, بنية mosquitto *context, ثابت بنية mosquitto_subscription *sub, صحيح level)
{
	بنية mosquitto__retainhier *branch, *branch_tmp;
	صحيح flag = 0;

	إذا(!strcmp(ورهايو_topics[0], "#") && ورهايو_topics[1] == عدم){
		HASH_ITER(hh, retainhier->children, branch, branch_tmp){
			/* Set flag to indicate that we should check for retained messages
			 * on "foo" when we are subscribing to e.g. "foo/#" and then exit
			 * this function and return to an earlier retain__search().
			 */
			flag = -1;
			إذا(branch->retained){
				retain__process(branch, context, sub);
			}
			إذا(branch->children){
				retain_ڳولا(branch, ورهايو_topics, context, sub, level+1);
			}
		}
	}وإلا{
		إذا(!strcmp(ورهايو_topics[0], "+")){
			HASH_ITER(hh, retainhier->children, branch, branch_tmp){
				إذا(ورهايو_topics[1] != عدم){
					إذا(retain_ڳولا(branch, &(ورهايو_topics[1]), context, sub, level+1) == -1
							|| (ورهايو_topics[1] != عدم && !strcmp(ورهايو_topics[1], "#") && level>0)){

						إذا(branch->retained){
							retain__process(branch, context, sub);
						}
					}
				}وإلا{
					إذا(branch->retained){
						retain__process(branch, context, sub);
					}
				}
			}
		}وإلا{
			HASH_ڳوليو(hh, retainhier->children, ورهايو_topics[0], لڙي_ڊيگهه(ورهايو_topics[0]), branch);
			إذا(branch){
				إذا(ورهايو_topics[1] != عدم){
					إذا(retain_ڳولا(branch, &(ورهايو_topics[1]), context, sub, level+1) == -1
							|| (ورهايو_topics[1] != عدم && !strcmp(ورهايو_topics[1], "#") && level>0)){

						إذا(branch->retained){
							retain__process(branch, context, sub);
						}
					}
				}وإلا{
					إذا(branch->retained){
						retain__process(branch, context, sub);
					}
				}
			}
		}
	}
	أرجع flag;
}


صحيح retain_قطار(بنية mosquitto *context, ثابت بنية mosquitto_subscription *sub)
{
	بنية mosquitto__retainhier *retainhier;
	حرف *local_sub;
	حرف **ورهايو_topics;
	صحيح rc;

	أكد(context);
	أكد(sub);

	إذا(!لڙي_ڀيٽ(sub->topic_filter, "$share/", لڙي_ڊيگهه("$share/"))){
		أرجع MOSQ_ERR_SUCCESS;
	}

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &ورهايو_topics, عدم);
	إذا(rc){
		أرجع rc;
	}

	HASH_ڳوليو(hh, db.retains, ورهايو_topics[0], لڙي_ڊيگهه(ورهايو_topics[0]), retainhier);

	إذا(retainhier){
		retain_ڳولا(retainhier, ورهايو_topics, context, sub, 0);
	}
	mosquitto_FREE(local_sub);
	mosquitto_FREE(ورهايو_topics);

	أرجع MOSQ_ERR_SUCCESS;
}


فراغ retain__expire(بنية mosquitto__retainhier **retainhier)
{
	بنية mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		retain__expire(&peer->children);

		بنية mosquitto__retainhier *والد = peer->والد;
		إذا(retain_delete_expired_نياپو(peer)){
			retain_clean_خالي_جايون_hierarchy(والد);
		}
	}
}


فراغ retain__clean(بنية mosquitto__retainhier **retainhier)
{
	بنية mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		إذا(peer->retained){
			db_نياپو_store_ref_dec(&peer->retained);
		}
		retain__clean(&peer->children);

		HASH_DELETE(hh, *retainhier, peer);
		mosquitto_FREE(peer);
	}
}


فراغ retain__expiry_check(فراغ)
{
	إذا(db.config->retain_expiry_interval > 0 && db.now_s > ايندڙ_expire_check){
		retain__expire(&db.retains);
		ايندڙ_expire_check = db.now_s + db.config->retain_expiry_interval;
	}
}
