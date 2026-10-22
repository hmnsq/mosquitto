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

#شامل "config.h"

#شامل <assert.h>
#شامل <stdio.h>
#شامل <string.h>

#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto/mqtt_protocol.h"
#شامل "util_mosq.h"

#شامل "utlist.h"

ساکن time_t اگلا_expire_check = 0;

ساکن ساخت mosquitto__retainhier *retain_جوڑو_hier_اندراج(ساخت mosquitto__retainhier *والد, ساخت mosquitto__retainhier **sibling, ثابت حرف *topic, uint16_t len)
{
	ساخت mosquitto__retainhier *بچہ;

	أكد(sibling);

	بچہ = mosquitto_calloc(1, حجم(ساخت mosquitto__retainhier) + len + 1);
	اگر(!بچہ){
		log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
		واپس تہی;
	}
	بچہ->والد = والد;
	بچہ->topic_len = len;
	اگر(len > 0){
		strncpy(بچہ->topic, topic, (size_t)(len+1));
	}

	HASH_جوڑو(hh, *sibling, topic, بچہ->topic_len, بچہ);

	واپس بچہ;
}


صحیح retain__init(خالی)
{
	ساخت mosquitto__retainhier *retainhier;

	retainhier = retain_جوڑو_hier_اندراج(تہی, &db.retains, "", 0);
	اگر(!retainhier){
		واپس MOSQ_ERR_NOMEM;
	}

	retainhier = retain_جوڑو_hier_اندراج(تہی, &db.retains, "$SYS", (uint16_t)لڑی_لمبائی("$SYS"));
	اگر(!retainhier){
		واپس MOSQ_ERR_NOMEM;
	}

	واپس MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT صحیح mosquitto_persist_retain_پیغام_سیٹ(ثابت حرف *topic, uint64_t بنیاد_پیغام_id)
{
	ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام;
	صحیح rc = MOSQ_ERR_UNKNOWN;
	حرف **بانٹو_topics = تہی;
	حرف *local_topic = تہی;

	اگر(topic == تہی){
		واپس MOSQ_ERR_INVAL;
	}

	HASH_ڈھونڈو(hh, db.پیغام_store, &بنیاد_پیغام_id, حجم(بنیاد_پیغام_id), بنیاد_پیغام);
	اگر(بنیاد_پیغام){
		اگر(sub__topic_tokenise(topic, &local_topic, &بانٹو_topics, تہی)){
			واپس MOSQ_ERR_NOMEM;
		}

		rc = retain__store(topic, بنیاد_پیغام, بانٹو_topics, false);
		mosquitto_free(بانٹو_topics);
		mosquitto_free(local_topic);
	}

	واپس rc;
}


BROKER_EXPORT صحیح mosquitto_persist_retain_پیغام_delete(ثابت حرف *topic)
{
	ساخت mosquitto_بنیاد_پیغام بنیاد_پیغام;
	صحیح rc = MOSQ_ERR_UNKNOWN;
	حرف **بانٹو_topics = تہی;
	حرف *local_topic = تہی;

	اگر(topic == تہی){
		واپس MOSQ_ERR_INVAL;
	}

	memset(&بنیاد_پیغام, 0, حجم(بنیاد_پیغام));
	بنیاد_پیغام.ref_گنتی = 10; /* Ensure this isn't freed */

	اگر(sub__topic_tokenise(topic, &local_topic, &بانٹو_topics, تہی)){
		واپس MOSQ_ERR_NOMEM;
	}

	/* With stored->payloadlen == 0, this means the message will be removed */
	rc = retain__store(topic, &بنیاد_پیغام, بانٹو_topics, false);
	mosquitto_FREE(بانٹو_topics);
	mosquitto_FREE(local_topic);

	واپس rc;
}


خالی retain_clean_خالی_خانے_hierarchy(ساخت mosquitto__retainhier *retainhier)
{
	جب(retainhier){
		اگر(retainhier->children || retainhier->retained || retainhier->والد == تہی){
			/* Entry is being used */
			واپس;
		}ورنہ{
			HASH_DELETE(hh, retainhier->والد->children, retainhier);

			ساخت mosquitto__retainhier *والد = retainhier->والد;
			mosquitto_FREE(retainhier);
			retainhier = والد;
		}
	}
}


صحیح retain__store(ثابت حرف *topic, ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام, حرف **بانٹو_topics, bool persist)
{
	ساخت mosquitto__retainhier *retainhier;
	ساخت mosquitto__retainhier *branch;
	size_t slen;

	أكد(بنیاد_پیغام);
	أكد(بانٹو_topics);

	HASH_ڈھونڈو(hh, db.retains, بانٹو_topics[0], لڑی_لمبائی(بانٹو_topics[0]), retainhier);
	اگر(retainhier == تہی){
		retainhier = retain_جوڑو_hier_اندراج(تہی, &db.retains, بانٹو_topics[0], (uint16_t)لڑی_لمبائی(بانٹو_topics[0]));
		اگر(!retainhier){
			واپس MOSQ_ERR_NOMEM;
		}
	}

	برائے(صحیح i=0; بانٹو_topics[i] != تہی; i++){
		slen = لڑی_لمبائی(بانٹو_topics[i]);
		HASH_ڈھونڈو(hh, retainhier->children, بانٹو_topics[i], slen, branch);
		اگر(branch == تہی){
			branch = retain_جوڑو_hier_اندراج(retainhier, &retainhier->children, بانٹو_topics[i], (uint16_t)slen);
			اگر(branch == تہی){
				واپس MOSQ_ERR_NOMEM;
			}
		}
		retainhier = branch;
	}

#اگر_تعریف_شدہ WITH_PERSISTENCE
	اگر(لڑی_موازنہ(topic, "$SYS", 4)){
		/* Retained messages count as a persistence change, but only if
		 * they aren't for $SYS. */
		db.persistence_changes++;
	}
#ورنہ
	غیر_مستعمل(topic);
#اگر_ختم

	اگر(retainhier->retained){
		اگر(retainhier->retained == بنیاد_پیغام){
			/* This may occur if multiple persistence providers are used */
			واپس MOSQ_ERR_SUCCESS;
		}

		اگر(persist && retainhier->retained->ڈیٹا.topic[0] != '$' && بنیاد_پیغام->ڈیٹا.payloadlen == 0){
			/* Only delete if another retained message isn't replacing this one */
			plugin_persist_handle_retain_پیغام_delete(retainhier->retained);
		}
		db_پیغام_store_ref_dec(&retainhier->retained);
#اگر_تعریف_شدہ WITH_SYS_درخت
		db.retained_گنتی--;
#اگر_ختم
		اگر(بنیاد_پیغام->ڈیٹا.payloadlen == 0){
			retainhier->retained = تہی;
			retain_clean_خالی_خانے_hierarchy(retainhier);
		}
	}
	اگر(بنیاد_پیغام->ڈیٹا.payloadlen){
		retainhier->retained = بنیاد_پیغام;
		db_پیغام_store_ref_inc(retainhier->retained);
		اگر(persist && retainhier->retained->ڈیٹا.topic[0] != '$'){
			plugin_persist_handle_بنیاد_پیغام_جوڑو(retainhier->retained);
			plugin_persist_handle_retain_پیغام_سیٹ(retainhier->retained);
		}
#اگر_تعریف_شدہ WITH_SYS_درخت
		db.retained_گنتی++;
#اگر_ختم
	}

	واپس MOSQ_ERR_SUCCESS;
}


ساکن bool retain_delete_expired_پیغام(ساخت mosquitto__retainhier *branch)
{
	اگر(branch->retained && branch->retained->ڈیٹا.expiry_وقت > 0 && db.now_real_s >= branch->retained->ڈیٹا.expiry_وقت){
		plugin_persist_handle_retain_پیغام_delete(branch->retained);
		db_پیغام_store_ref_dec(&branch->retained);
		branch->retained = تہی;
#اگر_تعریف_شدہ WITH_SYS_درخت
		db.retained_گنتی--;
#اگر_ختم
		واپس true;
	}
	واپس false;
}


ساکن صحیح retain__process(ساخت mosquitto__retainhier *branch, ساخت mosquitto *context, ثابت ساخت mosquitto_subscription *sub)
{
	صحیح rc = 0;
	uint8_t qos, sub_qos;
	uint16_t mid;
	ساخت mosquitto_بنیاد_پیغام *retained;

	اگر(retain_delete_expired_پیغام(branch)){
		واپس MOSQ_ERR_SUCCESS;
	}

	retained = branch->retained;

	rc = mosquitto_acl_check(context, retained->ڈیٹا.topic, retained->ڈیٹا.payloadlen, retained->ڈیٹا.payload,
			retained->ڈیٹا.qos, retained->ڈیٹا.retain, retained->ڈیٹا.properties, MOSQ_ACL_READ);
	اگر(rc == MOSQ_ERR_ACL_DENIED){
		واپس MOSQ_ERR_SUCCESS;
	}ورنہ اگر(rc != MOSQ_ERR_SUCCESS){
		واپس rc;
	}

	/* Check for original source access */
	اگر(db.config->check_retain_ماخذ && retained->origin != mosq_mo_broker && retained->ڈیٹا.ماخذ_id){
		ساخت mosquitto retain_ctxt;
		memset(&retain_ctxt, 0, حجم(ساخت mosquitto));

		retain_ctxt.id = retained->ڈیٹا.ماخذ_id;
		retain_ctxt.username = retained->ڈیٹا.ماخذ_username;
		retain_ctxt.listener = retained->ماخذ_listener;

		rc = mosquitto_acl_check(&retain_ctxt, retained->ڈیٹا.topic, retained->ڈیٹا.payloadlen, retained->ڈیٹا.payload,
				retained->ڈیٹا.qos, retained->ڈیٹا.retain, retained->ڈیٹا.properties, MOSQ_ACL_WRITE);
		اگر(rc == MOSQ_ERR_ACL_DENIED){
			واپس MOSQ_ERR_SUCCESS;
		}ورنہ اگر(rc != MOSQ_ERR_SUCCESS){
			واپس rc;
		}
	}

	sub_qos = sub->options & 0x03;
	اگر(db.config->upgrade_outgoing_qos){
		qos = sub_qos;
	}ورنہ{
		qos = retained->ڈیٹا.qos;
		اگر(qos > sub_qos){
			qos = sub_qos;
		}
	}
	اگر(qos > 0){
		mid = mosquitto__mid_generate(context);
	}ورنہ{
		mid = 0;
	}
	واپس db_پیغام_داخل_کرو_outgoing(context, 0, mid, qos, true, retained, sub->identifier, false, true);
}


ساکن صحیح retain_تلاش(ساخت mosquitto__retainhier *retainhier, حرف **بانٹو_topics, ساخت mosquitto *context, ثابت ساخت mosquitto_subscription *sub, صحیح level)
{
	ساخت mosquitto__retainhier *branch, *branch_tmp;
	صحیح flag = 0;

	اگر(!strcmp(بانٹو_topics[0], "#") && بانٹو_topics[1] == تہی){
		HASH_ITER(hh, retainhier->children, branch, branch_tmp){
			/* Set flag to indicate that we should check for retained messages
			 * on "foo" when we are subscribing to e.g. "foo/#" and then exit
			 * this function and return to an earlier retain__search().
			 */
			flag = -1;
			اگر(branch->retained){
				retain__process(branch, context, sub);
			}
			اگر(branch->children){
				retain_تلاش(branch, بانٹو_topics, context, sub, level+1);
			}
		}
	}ورنہ{
		اگر(!strcmp(بانٹو_topics[0], "+")){
			HASH_ITER(hh, retainhier->children, branch, branch_tmp){
				اگر(بانٹو_topics[1] != تہی){
					اگر(retain_تلاش(branch, &(بانٹو_topics[1]), context, sub, level+1) == -1
							|| (بانٹو_topics[1] != تہی && !strcmp(بانٹو_topics[1], "#") && level>0)){

						اگر(branch->retained){
							retain__process(branch, context, sub);
						}
					}
				}ورنہ{
					اگر(branch->retained){
						retain__process(branch, context, sub);
					}
				}
			}
		}ورنہ{
			HASH_ڈھونڈو(hh, retainhier->children, بانٹو_topics[0], لڑی_لمبائی(بانٹو_topics[0]), branch);
			اگر(branch){
				اگر(بانٹو_topics[1] != تہی){
					اگر(retain_تلاش(branch, &(بانٹو_topics[1]), context, sub, level+1) == -1
							|| (بانٹو_topics[1] != تہی && !strcmp(بانٹو_topics[1], "#") && level>0)){

						اگر(branch->retained){
							retain__process(branch, context, sub);
						}
					}
				}ورنہ{
					اگر(branch->retained){
						retain__process(branch, context, sub);
					}
				}
			}
		}
	}
	واپس flag;
}


صحیح retain_قطار(ساخت mosquitto *context, ثابت ساخت mosquitto_subscription *sub)
{
	ساخت mosquitto__retainhier *retainhier;
	حرف *local_sub;
	حرف **بانٹو_topics;
	صحیح rc;

	أكد(context);
	أكد(sub);

	اگر(!لڑی_موازنہ(sub->topic_filter, "$share/", لڑی_لمبائی("$share/"))){
		واپس MOSQ_ERR_SUCCESS;
	}

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &بانٹو_topics, تہی);
	اگر(rc){
		واپس rc;
	}

	HASH_ڈھونڈو(hh, db.retains, بانٹو_topics[0], لڑی_لمبائی(بانٹو_topics[0]), retainhier);

	اگر(retainhier){
		retain_تلاش(retainhier, بانٹو_topics, context, sub, 0);
	}
	mosquitto_FREE(local_sub);
	mosquitto_FREE(بانٹو_topics);

	واپس MOSQ_ERR_SUCCESS;
}


خالی retain__expire(ساخت mosquitto__retainhier **retainhier)
{
	ساخت mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		retain__expire(&peer->children);

		ساخت mosquitto__retainhier *والد = peer->والد;
		اگر(retain_delete_expired_پیغام(peer)){
			retain_clean_خالی_خانے_hierarchy(والد);
		}
	}
}


خالی retain__clean(ساخت mosquitto__retainhier **retainhier)
{
	ساخت mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		اگر(peer->retained){
			db_پیغام_store_ref_dec(&peer->retained);
		}
		retain__clean(&peer->children);

		HASH_DELETE(hh, *retainhier, peer);
		mosquitto_FREE(peer);
	}
}


خالی retain__expiry_check(خالی)
{
	اگر(db.config->retain_expiry_interval > 0 && db.now_s > اگلا_expire_check){
		retain__expire(&db.retains);
		اگلا_expire_check = db.now_s + db.config->retain_expiry_interval;
	}
}
