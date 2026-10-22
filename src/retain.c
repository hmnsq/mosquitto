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

ساكن time_t اگلا_expire_check = 0;

ساكن بنية mosquitto__retainhier *retain_جوڑو_hier_اندراج(بنية mosquitto__retainhier *ماپا, بنية mosquitto__retainhier **sibling, ثابت حرف *topic, uint16_t len)
{
	بنية mosquitto__retainhier *بچہ;

	أكد(sibling);

	بچہ = mosquitto_calloc(1, حجم(بنية mosquitto__retainhier) + len + 1);
	إذا(!بچہ){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		أرجع عدم;
	}
	بچہ->ماپا = ماپا;
	بچہ->topic_len = len;
	إذا(len > 0){
		strncpy(بچہ->topic, topic, (size_t)(len+1));
	}

	HASH_جوڑو(hh, *sibling, topic, بچہ->topic_len, بچہ);

	أرجع بچہ;
}


صحيح retain__init(فراغ)
{
	بنية mosquitto__retainhier *retainhier;

	retainhier = retain_جوڑو_hier_اندراج(عدم, &db.retains, "", 0);
	إذا(!retainhier){
		أرجع MOSQ_ERR_NOMEM;
	}

	retainhier = retain_جوڑو_hier_اندراج(عدم, &db.retains, "$SYS", (uint16_t)لڑی_لمبائی("$SYS"));
	إذا(!retainhier){
		أرجع MOSQ_ERR_NOMEM;
	}

	أرجع MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحيح mosquitto_persist_retain_پیغام_سیٹ(ثابت حرف *topic, uint64_t ادھار_پیغام_id)
{
	بنية mosquitto_ادھار_پیغام *ادھار_پیغام;
	صحيح rc = MOSQ_ERR_UNKNOWN;
	حرف **ونڈو_topics = عدم;
	حرف *local_topic = عدم;

	إذا(topic == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	HASH_لبھو(hh, db.پیغام_store, &ادھار_پیغام_id, حجم(ادھار_پیغام_id), ادھار_پیغام);
	إذا(ادھار_پیغام){
		إذا(sub__topic_tokenise(topic, &local_topic, &ونڈو_topics, عدم)){
			أرجع MOSQ_ERR_NOMEM;
		}

		rc = retain__store(topic, ادھار_پیغام, ونڈو_topics, false);
		mosquitto_free(ونڈو_topics);
		mosquitto_free(local_topic);
	}

	أرجع rc;
}


BROKER_EXPORT صحيح mosquitto_persist_retain_پیغام_delete(ثابت حرف *topic)
{
	بنية mosquitto_ادھار_پیغام ادھار_پیغام;
	صحيح rc = MOSQ_ERR_UNKNOWN;
	حرف **ونڈو_topics = عدم;
	حرف *local_topic = عدم;

	إذا(topic == عدم){
		أرجع MOSQ_ERR_INVAL;
	}

	memset(&ادھار_پیغام, 0, حجم(ادھار_پیغام));
	ادھار_پیغام.ref_گنتی = 10; /* Ensure this isn't freed */

	إذا(sub__topic_tokenise(topic, &local_topic, &ونڈو_topics, عدم)){
		أرجع MOSQ_ERR_NOMEM;
	}

	/* With stored->payloadlen == 0, this means the message will be removed */
	rc = retain__store(topic, &ادھار_پیغام, ونڈو_topics, false);
	mosquitto_FREE(ونڈو_topics);
	mosquitto_FREE(local_topic);

	أرجع rc;
}


فراغ retain_clean_خالی_تھاواں_hierarchy(بنية mosquitto__retainhier *retainhier)
{
	طالما(retainhier){
		إذا(retainhier->children || retainhier->retained || retainhier->ماپا == عدم){
			/* Entry is being used */
			أرجع;
		}وإلا{
			HASH_DELETE(hh, retainhier->ماپا->children, retainhier);

			بنية mosquitto__retainhier *ماپا = retainhier->ماپا;
			mosquitto_FREE(retainhier);
			retainhier = ماپا;
		}
	}
}


صحيح retain__store(ثابت حرف *topic, بنية mosquitto_ادھار_پیغام *ادھار_پیغام, حرف **ونڈو_topics, bool persist)
{
	بنية mosquitto__retainhier *retainhier;
	بنية mosquitto__retainhier *branch;
	size_t slen;

	أكد(ادھار_پیغام);
	أكد(ونڈو_topics);

	HASH_لبھو(hh, db.retains, ونڈو_topics[0], لڑی_لمبائی(ونڈو_topics[0]), retainhier);
	إذا(retainhier == عدم){
		retainhier = retain_جوڑو_hier_اندراج(عدم, &db.retains, ونڈو_topics[0], (uint16_t)لڑی_لمبائی(ونڈو_topics[0]));
		إذا(!retainhier){
			أرجع MOSQ_ERR_NOMEM;
		}
	}

	لكل(صحيح i=0; ونڈو_topics[i] != عدم; i++){
		slen = لڑی_لمبائی(ونڈو_topics[i]);
		HASH_لبھو(hh, retainhier->children, ونڈو_topics[i], slen, branch);
		إذا(branch == عدم){
			branch = retain_جوڑو_hier_اندراج(retainhier, &retainhier->children, ونڈو_topics[i], (uint16_t)slen);
			إذا(branch == عدم){
				أرجع MOSQ_ERR_NOMEM;
			}
		}
		retainhier = branch;
	}

#جے_تعریف_شدہ WITH_PERSISTENCE
	إذا(لڑی_مقابلہ(topic, "$SYS", 4)){
		/* Retained messages count as a persistence change, but only if
		 * they aren't for $SYS. */
		db.persistence_changes++;
	}
#وإلا
	بن_ورتیا(topic);
#جے_انت

	إذا(retainhier->retained){
		إذا(retainhier->retained == ادھار_پیغام){
			/* This may occur if multiple persistence providers are used */
			أرجع MOSQ_ERR_SUCCESS;
		}

		إذا(persist && retainhier->retained->انکڑے.topic[0] != '$' && ادھار_پیغام->انکڑے.payloadlen == 0){
			/* Only delete if another retained message isn't replacing this one */
			plugin_persist_handle_retain_پیغام_delete(retainhier->retained);
		}
		db_پیغام_store_ref_dec(&retainhier->retained);
#جے_تعریف_شدہ WITH_SYS_درخت
		db.retained_گنتی--;
#جے_انت
		إذا(ادھار_پیغام->انکڑے.payloadlen == 0){
			retainhier->retained = عدم;
			retain_clean_خالی_تھاواں_hierarchy(retainhier);
		}
	}
	إذا(ادھار_پیغام->انکڑے.payloadlen){
		retainhier->retained = ادھار_پیغام;
		db_پیغام_store_ref_inc(retainhier->retained);
		إذا(persist && retainhier->retained->انکڑے.topic[0] != '$'){
			plugin_persist_handle_ادھار_پیغام_جوڑو(retainhier->retained);
			plugin_persist_handle_retain_پیغام_سیٹ(retainhier->retained);
		}
#جے_تعریف_شدہ WITH_SYS_درخت
		db.retained_گنتی++;
#جے_انت
	}

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن bool retain_delete_expired_پیغام(بنية mosquitto__retainhier *branch)
{
	إذا(branch->retained && branch->retained->انکڑے.expiry_ویلا > 0 && db.now_real_s >= branch->retained->انکڑے.expiry_ویلا){
		plugin_persist_handle_retain_پیغام_delete(branch->retained);
		db_پیغام_store_ref_dec(&branch->retained);
		branch->retained = عدم;
#جے_تعریف_شدہ WITH_SYS_درخت
		db.retained_گنتی--;
#جے_انت
		أرجع true;
	}
	أرجع false;
}


ساكن صحيح retain__process(بنية mosquitto__retainhier *branch, بنية mosquitto *context, ثابت بنية mosquitto_subscription *sub)
{
	صحيح rc = 0;
	uint8_t qos, sub_qos;
	uint16_t mid;
	بنية mosquitto_ادھار_پیغام *retained;

	إذا(retain_delete_expired_پیغام(branch)){
		أرجع MOSQ_ERR_SUCCESS;
	}

	retained = branch->retained;

	rc = mosquitto_acl_check(context, retained->انکڑے.topic, retained->انکڑے.payloadlen, retained->انکڑے.payload,
			retained->انکڑے.qos, retained->انکڑے.retain, retained->انکڑے.properties, MOSQ_ACL_READ);
	إذا(rc == MOSQ_ERR_ACL_DENIED){
		أرجع MOSQ_ERR_SUCCESS;
	}وإلا إذا(rc != MOSQ_ERR_SUCCESS){
		أرجع rc;
	}

	/* Check for original source access */
	إذا(db.config->check_retain_سروت && retained->origin != mosq_mo_broker && retained->انکڑے.سروت_id){
		بنية mosquitto retain_ctxt;
		memset(&retain_ctxt, 0, حجم(بنية mosquitto));

		retain_ctxt.id = retained->انکڑے.سروت_id;
		retain_ctxt.username = retained->انکڑے.سروت_username;
		retain_ctxt.listener = retained->سروت_listener;

		rc = mosquitto_acl_check(&retain_ctxt, retained->انکڑے.topic, retained->انکڑے.payloadlen, retained->انکڑے.payload,
				retained->انکڑے.qos, retained->انکڑے.retain, retained->انکڑے.properties, MOSQ_ACL_WRITE);
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
		qos = retained->انکڑے.qos;
		إذا(qos > sub_qos){
			qos = sub_qos;
		}
	}
	إذا(qos > 0){
		mid = mosquitto__mid_generate(context);
	}وإلا{
		mid = 0;
	}
	أرجع db_سنیہا_داخل_کرو_outgoing(context, 0, mid, qos, true, retained, sub->identifier, false, true);
}


ساكن صحيح retain_تلاش(بنية mosquitto__retainhier *retainhier, حرف **ونڈو_topics, بنية mosquitto *context, ثابت بنية mosquitto_subscription *sub, صحيح level)
{
	بنية mosquitto__retainhier *branch, *branch_tmp;
	صحيح flag = 0;

	إذا(!strcmp(ونڈو_topics[0], "#") && ونڈو_topics[1] == عدم){
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
				retain_تلاش(branch, ونڈو_topics, context, sub, level+1);
			}
		}
	}وإلا{
		إذا(!strcmp(ونڈو_topics[0], "+")){
			HASH_ITER(hh, retainhier->children, branch, branch_tmp){
				إذا(ونڈو_topics[1] != عدم){
					إذا(retain_تلاش(branch, &(ونڈو_topics[1]), context, sub, level+1) == -1
							|| (ونڈو_topics[1] != عدم && !strcmp(ونڈو_topics[1], "#") && level>0)){

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
			HASH_لبھو(hh, retainhier->children, ونڈو_topics[0], لڑی_لمبائی(ونڈو_topics[0]), branch);
			إذا(branch){
				إذا(ونڈو_topics[1] != عدم){
					إذا(retain_تلاش(branch, &(ونڈو_topics[1]), context, sub, level+1) == -1
							|| (ونڈو_topics[1] != عدم && !strcmp(ونڈو_topics[1], "#") && level>0)){

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
	حرف **ونڈو_topics;
	صحيح rc;

	أكد(context);
	أكد(sub);

	إذا(!لڑی_مقابلہ(sub->topic_filter, "$share/", لڑی_لمبائی("$share/"))){
		أرجع MOSQ_ERR_SUCCESS;
	}

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &ونڈو_topics, عدم);
	إذا(rc){
		أرجع rc;
	}

	HASH_لبھو(hh, db.retains, ونڈو_topics[0], لڑی_لمبائی(ونڈو_topics[0]), retainhier);

	إذا(retainhier){
		retain_تلاش(retainhier, ونڈو_topics, context, sub, 0);
	}
	mosquitto_FREE(local_sub);
	mosquitto_FREE(ونڈو_topics);

	أرجع MOSQ_ERR_SUCCESS;
}


فراغ retain__expire(بنية mosquitto__retainhier **retainhier)
{
	بنية mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		retain__expire(&peer->children);

		بنية mosquitto__retainhier *ماپا = peer->ماپا;
		إذا(retain_delete_expired_پیغام(peer)){
			retain_clean_خالی_تھاواں_hierarchy(ماپا);
		}
	}
}


فراغ retain__clean(بنية mosquitto__retainhier **retainhier)
{
	بنية mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		إذا(peer->retained){
			db_پیغام_store_ref_dec(&peer->retained);
		}
		retain__clean(&peer->children);

		HASH_DELETE(hh, *retainhier, peer);
		mosquitto_FREE(peer);
	}
}


فراغ retain__expiry_check(فراغ)
{
	إذا(db.config->retain_expiry_interval > 0 && db.now_s > اگلا_expire_check){
		retain__expire(&db.retains);
		اگلا_expire_check = db.now_s + db.config->retain_expiry_interval;
	}
}
