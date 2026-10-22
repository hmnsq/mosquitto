/*
Copyright (c) 2009-2021 Roger Light <roger@atchoo.org>

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
#ضمن <utlist.h>

#ضمن "mosquitto_broker_internal.h"
#ضمن "send_mosq.h"
#ضمن "sys_tree.h"
#ضمن "util_mosq.h"


/**
 * Is this context ready to take more in flight messages right now?
 * @param context the client context of interest
 * @param qos qos for the packet of interest
 * @return true if more in flight are allowed.
 */
bool db__ready_for_flight(بنية mosquitto *context, تعداد mosquitto_پیام_direction dir, صحيح qos)
{
	بنية mosquitto_پیام_داده *msgs;
	bool valid_بایتها;
	bool valid_شمار;

	إذا(dir == mosq_md_بیرون){
		msgs = &context->msgs_بیرون;
	}وإلا{
		msgs = &context->msgs_درون;
	}

	إذا(msgs->inflight_بیشینه == 0 && db.config->max_inflight_بایتها == 0){
		أرجع true;
	}

	إذا(qos == 0){
		/* Deliver QoS 0 messages unless the queue is already full.
		 * For QoS 0 messages the choice is either "inflight" or dropped.
		 * There is no queueing option, unless the client is offline and
		 * queue_qos0_messages is enabled.
		 */
		إذا(db.config->max_queued_messages == 0 && db.config->max_inflight_بایتها == 0){
			أرجع true;
		}
		valid_بایتها = ((msgs->inflight_بایتها - (ssize_t)db.config->max_inflight_بایتها) < (ssize_t)db.config->max_queued_بایتها);
		إذا(dir == mosq_md_بیرون){
			valid_شمار = context->بیرون_packet_شمار < db.config->max_queued_messages;
		}وإلا{
			valid_شمار = msgs->inflight_شمار - msgs->inflight_بیشینه < db.config->max_queued_messages;
		}

		إذا(db.config->max_queued_messages == 0){
			أرجع valid_بایتها;
		}
		إذا(db.config->max_queued_بایتها == 0){
			أرجع valid_شمار;
		}
	}وإلا{
		valid_بایتها = (ssize_t)msgs->inflight_bytes12 < (ssize_t)db.config->max_inflight_بایتها;
		valid_شمار = msgs->inflight_quota > 0;

		إذا(msgs->inflight_بیشینه == 0){
			أرجع valid_بایتها;
		}
		إذا(db.config->max_inflight_بایتها == 0){
			أرجع valid_شمار;
		}
	}

	أرجع valid_بایتها && valid_شمار;
}


/**
 * For a given client context, are more messages allowed to be queued?
 * It is assumed that inflight checks and queue_qos0 checks have already
 * been made.
 * @param context client of interest
 * @param qos destination qos for the packet of interest
 * @return true if queuing is allowed, false if should be dropped
 */
bool db_ready_for_صف(بنية mosquitto *context, صحيح qos, بنية mosquitto_پیام_داده *پیام_داده)
{
	صحيح منبع_شمار;
	صحيح adjust_شمار;
	دراز منبع_بایتها;
	ssize_t adjust_بایتها = (ssize_t)db.config->max_inflight_بایتها;
	bool valid_بایتها;
	bool valid_شمار;

	إذا(db.config->max_queued_messages == 0 && db.config->max_queued_بایتها == 0){
		أرجع true;
	}

	إذا(qos == 0 && db.config->صف_qos0_messages == false){
		أرجع false; /* This case is handled in db__ready_for_flight() */
	}وإلا{
		منبع_بایتها = (ssize_t)پیام_داده->queued_bytes12;
		منبع_شمار = پیام_داده->queued_count12;
	}
	adjust_شمار = پیام_داده->inflight_بیشینه;

	/* nothing in flight for offline clients */
	إذا(!net__is_connected(context)){
		adjust_بایتها = 0;
		adjust_شمار = 0;
	}

	valid_بایتها = (منبع_بایتها - (ssize_t)adjust_بایتها) < (ssize_t)db.config->max_queued_بایتها;
	valid_شمار = منبع_شمار - adjust_شمار < db.config->max_queued_messages;

	إذا(db.config->max_queued_بایتها == 0){
		أرجع valid_شمار;
	}
	إذا(db.config->max_queued_messages == 0){
		أرجع valid_بایتها;
	}

	أرجع valid_بایتها && valid_شمار;
}


فراغ db_پیام_جمع_کن_مقصد_inflight_stats(بنية mosquitto_پیام_داده *پیام_داده, بنية mosquitto_client_پیام *client_پیام)
{
	پیام_داده->inflight_شمار++;
	پیام_داده->inflight_بایتها += client_پیام->پایه_پیام->داده.payloadlen;
	إذا(client_پیام->داده.qos != 0){
		پیام_داده->inflight_count12++;
		پیام_داده->inflight_bytes12 += client_پیام->پایه_پیام->داده.payloadlen;
	}
}


ساكن فراغ db_پیام_remove_from_inflight_stats(بنية mosquitto_پیام_داده *پیام_داده, بنية mosquitto_client_پیام *client_پیام)
{
	پیام_داده->inflight_شمار--;
	پیام_داده->inflight_بایتها -= client_پیام->پایه_پیام->داده.payloadlen;
	إذا(client_پیام->داده.qos != 0){
		پیام_داده->inflight_count12--;
		پیام_داده->inflight_bytes12 -= client_پیام->پایه_پیام->داده.payloadlen;
	}
}


فراغ db_پیام_جمع_کن_مقصد_queued_stats(بنية mosquitto_پیام_داده *پیام_داده, بنية mosquitto_client_پیام *client_پیام)
{
	پیام_داده->queued_شمار++;
	پیام_داده->queued_بایتها += client_پیام->پایه_پیام->داده.payloadlen;
	إذا(client_پیام->داده.qos != 0){
		پیام_داده->queued_count12++;
		پیام_داده->queued_bytes12 += client_پیام->پایه_پیام->داده.payloadlen;
	}
}


ساكن فراغ db_پیام_remove_from_queued_stats(بنية mosquitto_پیام_داده *پیام_داده, بنية mosquitto_client_پیام *client_پیام)
{
	پیام_داده->queued_شمار--;
	پیام_داده->queued_بایتها -= client_پیام->پایه_پیام->داده.payloadlen;
	إذا(client_پیام->داده.qos != 0){
		پیام_داده->queued_count12--;
		پیام_داده->queued_bytes12 -= client_پیام->پایه_پیام->داده.payloadlen;
	}
}


صحيح db__open(بنية mosquitto__config *config)
{
	إذا(!config){
		أرجع MOSQ_ERR_INVAL;
	}

	db.contexts_by_id = عدم;
	db.contexts_by_sock = عدم;
	db.contexts_for_free = عدم;
#اگر_تعریف_شده WITH_BRIDGE
	db.bridges = عدم;
	db.bridge_شمار = 0;
#پایان_اگر

	/* Initialize the hashtable */
	db.clientid_شاخص_hash = عدم;

	db.نرمال_subs = عدم;
	db.shared_subs = عدم;

	sub__init();
	retain__init();

	db.config->security_options.unpwd = عدم;

#اگر_تعریف_شده WITH_PERSISTENCE
	إذا(persist__restore()){
		أرجع 1;
	}
#پایان_اگر

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن فراغ subhier_clean(بنية mosquitto__subhier **subhier)
{
	بنية mosquitto__subhier *peer, *subhier_tmp;
	بنية mosquitto__subleaf *برگ, *nextleaf;

	HASH_ITER(hh, *subhier, peer, subhier_tmp){
		برگ = peer->subs;
		طالما(برگ){
			nextleaf = برگ->بعدی;
			mosquitto_FREE(برگ);
			برگ = nextleaf;
		}
		subhier_clean(&peer->children);

		HASH_DELETE(hh, *subhier, peer);
		mosquitto_FREE(peer);
	}
}


صحيح db__close(فراغ)
{
	subhier_clean(&db.نرمال_subs);
	subhier_clean(&db.shared_subs);
	retain__clean(&db.retains);
	db_پیام_store_clean();

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db_پیام_store_جمع_کن(بنية mosquitto_پایه_پیام *پایه_پیام)
{
	بنية mosquitto_پایه_پیام *یافت;
	بیعلامت hashv;

	HASH_مقدار(&پایه_پیام->داده.store_id, حجم(پایه_پیام->داده.store_id), hashv);
	HASH_پیدا_کن_BYHASHVALUE(hh, db.پیام_store, &پایه_پیام->داده.store_id, حجم(پایه_پیام->داده.store_id), hashv, یافت);
	إذا(یافت == عدم){
		HASH_جمع_کن_KEYPTR_BYHASHVALUE(hh, db.پیام_store, &پایه_پیام->داده.store_id, حجم(پایه_پیام->داده.store_id), hashv, پایه_پیام);
		أرجع MOSQ_ERR_SUCCESS;
	}وإلا{
		أرجع MOSQ_ERR_ALREADY_EXISTS;
	}
}


فراغ db_پیام_store_free(بنية mosquitto_پایه_پیام *پایه_پیام)
{
	mosquitto_FREE(پایه_پیام->داده.منبع_id);
	mosquitto_FREE(پایه_پیام->داده.منبع_username);
	إذا(پایه_پیام->dest_ids){
		لكل(صحيح i=0; i<پایه_پیام->dest_id_شمار; i++){
			mosquitto_FREE(پایه_پیام->dest_ids[i]);
		}
		mosquitto_FREE(پایه_پیام->dest_ids);
	}
	mosquitto_FREE(پایه_پیام->داده.topic);
	mosquitto_property_free_all(&پایه_پیام->داده.properties);
	mosquitto_FREE(پایه_پیام->داده.payload);
	mosquitto_FREE(پایه_پیام);
}


فراغ db_پیام_store_remove(بنية mosquitto_پایه_پیام *پایه_پیام, bool notify)
{
	إذا(پایه_پیام == عدم){
		أرجع;
	}
	HASH_DELETE(hh, db.پیام_store, پایه_پیام);
	db.پیام_store_شمار--;
	db.پیام_store_بایتها -= پایه_پیام->داده.payloadlen;
	إذا(notify == true){
		plugin_persist_handle_پایه_پیام_delete(پایه_پیام);
	}
	db_پیام_store_free(پایه_پیام);
}


فراغ db_پیام_store_clean(فراغ)
{
	بنية mosquitto_پایه_پیام *پایه_پیام, *پایه_پیام_tmp;

	HASH_ITER(hh, db.پیام_store, پایه_پیام, پایه_پیام_tmp){
		db_پیام_store_remove(پایه_پیام, false);
	}
}


فراغ db_پیام_store_ref_inc(بنية mosquitto_پایه_پیام *پایه_پیام)
{
	پایه_پیام->ref_شمار++;
}


فراغ db_پیام_store_ref_dec(بنية mosquitto_پایه_پیام **پایه_پیام)
{
	(*پایه_پیام)->ref_شمار--;
	إذا((*پایه_پیام)->ref_شمار == 0){
		db_پیام_store_remove(*پایه_پیام, true);
		*پایه_پیام = عدم;
	}
}


فراغ db_پیام_store_compact(فراغ)
{
	بنية mosquitto_پایه_پیام *پایه_پیام, *پایه_پیام_tmp;

	HASH_ITER(hh, db.پیام_store, پایه_پیام, پایه_پیام_tmp){
		إذا(پایه_پیام->ref_شمار < 1){
			db_پیام_store_remove(پایه_پیام, true);
		}
	}
}


ساكن فراغ db_پیام_remove_inflight(بنية mosquitto *context, بنية mosquitto_پیام_داده *پیام_داده, بنية mosquitto_client_پیام *قلم)
{
	إذا(!context || !پیام_داده || !قلم){
		أرجع;
	}

	plugin_persist_handle_client_پیام_delete(context, قلم);

	DL_DELETE(پیام_داده->inflight, قلم);
	إذا(قلم->پایه_پیام){
		db_پیام_remove_from_inflight_stats(پیام_داده, قلم);
		db_پیام_store_ref_dec(&قلم->پایه_پیام);
	}

	mosquitto_FREE(قلم);
}


ساكن فراغ db_پیام_remove_queued(بنية mosquitto *context, بنية mosquitto_پیام_داده *پیام_داده, بنية mosquitto_client_پیام *قلم)
{
	إذا(!context || !پیام_داده || !قلم){
		أرجع;
	}

	plugin_persist_handle_client_پیام_delete(context, قلم);

	DL_DELETE(پیام_داده->queued, قلم);
	إذا(قلم->پایه_پیام){
		db_پیام_remove_from_queued_stats(پیام_داده, قلم);
		db_پیام_store_ref_dec(&قلم->پایه_پیام);
	}

	mosquitto_FREE(قلم);
}


ساكن فراغ db_fill_inflight_بیرون_from_صف(بنية mosquitto *context)
{
	بنية mosquitto_client_پیام *client_پیام, *tmp;

	DL_FOREACH_امن(context->msgs_بیرون.queued, client_پیام, tmp){
		إذا(!db__ready_for_flight(context, mosq_md_بیرون, client_پیام->داده.qos)){
			أرجع;
		}
		بدل(client_پیام->داده.qos){
			حالة 0:
				client_پیام->داده.state = mosq_ms_publish_qos0;
				اكسر;
			حالة 1:
				client_پیام->داده.state = mosq_ms_publish_qos1;
				اكسر;
			حالة 2:
				client_پیام->داده.state = mosq_ms_publish_qos2;
				اكسر;
		}
		إذا(client_پیام->پایه_پیام->داده.expiry_زمان && db.now_real_s > client_پیام->پایه_پیام->داده.expiry_زمان){
			db_پیام_remove_queued(context, &context->msgs_بیرون, client_پیام);
			استمر;
		}
		plugin_persist_handle_client_پیام_update(context, client_پیام);
		db_پیام_خارج_از_صف_اولی(context, &context->msgs_بیرون);
	}
}


فراغ db_پیام_خارج_از_صف_اولی(بنية mosquitto *context, بنية mosquitto_پیام_داده *پیام_داده)
{
	بنية mosquitto_client_پیام *client_پیام;

	استفادهنشده(context);

	client_پیام = پیام_داده->queued;
	DL_DELETE(پیام_داده->queued, client_پیام);
	DL_APPEND(پیام_داده->inflight, client_پیام);
	إذا(پیام_داده->inflight_quota > 0){
		پیام_داده->inflight_quota--;
	}

	db_پیام_remove_from_queued_stats(پیام_داده, client_پیام);
	db_پیام_جمع_کن_مقصد_inflight_stats(پیام_داده, client_پیام);
}


صحيح db_پیام_delete_outgoing(بنية mosquitto *context, uint16_t mid, تعداد mosquitto_پیام_state expect_state, صحيح qos)
{
	بنية mosquitto_client_پیام *client_پیام, *tmp;
	bool deleted = false;

	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	DL_FOREACH_امن(context->msgs_بیرون.inflight, client_پیام, tmp){
		إذا(client_پیام->داده.mid == mid){
			إذا(client_پیام->داده.qos != qos){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when deleting outgoing message.",
						context->id, client_پیام->داده.qos, qos);
				أرجع MOSQ_ERR_PROTOCOL;
			}وإلا إذا(qos == 2 && client_پیام->داده.state != expect_state && expect_state != mosq_ms_any){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched state (%d:%d) when deleting outgoing message.",
						context->id, client_پیام->داده.state, expect_state);
				أرجع MOSQ_ERR_PROTOCOL;
			}
			db_پیام_remove_inflight(context, &context->msgs_بیرون, client_پیام);
			deleted = true;
			اكسر;
		}
	}

	إذا(deleted == false){
		DL_FOREACH_امن(context->msgs_بیرون.queued, client_پیام, tmp){
			إذا(client_پیام->داده.mid == mid){
				إذا(client_پیام->داده.qos != qos){
					أرجع MOSQ_ERR_PROTOCOL;
				}وإلا إذا(qos == 2 && client_پیام->داده.state != expect_state && expect_state != mosq_ms_any){
					أرجع MOSQ_ERR_PROTOCOL;
				}
				db_پیام_remove_queued(context, &context->msgs_بیرون, client_پیام);
				اكسر;
			}
		}
	}
	db_fill_inflight_بیرون_from_صف(context);
#اگر_تعریف_شده WITH_PERSISTENCE
	db.persistence_changes++;
#پایان_اگر

	أرجع db_پیام_write_inflight_بیرون_latest(context);
}


/* Only for QoS 2 messages */
صحيح db_پیام_درج_incoming(بنية mosquitto *context, uint64_t cmsg_id, بنية mosquitto_پایه_پیام *پایه_پیام, bool persist)
{
	بنية mosquitto_client_پیام *client_پیام;
	بنية mosquitto_پیام_داده *پیام_داده;
	تعداد mosquitto_پیام_state state = mosq_ms_invalid;
	صحيح rc = 0;

	أكد(پایه_پیام);
	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		أرجع MOSQ_ERR_SUCCESS;

	}
	پیام_داده = &context->msgs_درون;

	إذا(db__ready_for_flight(context, mosq_md_درون, پایه_پیام->داده.qos)){
		state = mosq_ms_wait_for_pubrel;
	}وإلا إذا(پایه_پیام->داده.qos != 0 && db_ready_for_صف(context, پایه_پیام->داده.qos, پیام_داده)){
		state = mosq_ms_queued;
		rc = 2;
	}وإلا{
		/* Dropping message due to full queue. */
		إذا(context->is_dropping == false){
			context->is_dropping = true;
			log__printf(عدم, MOSQ_LOG_NOTICE,
					"Outgoing messages are being dropped for client %s.",
					context->id);
		}
		metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
		context->stats.messages_dropped++;

		أرجع 2;
	}

	أكد(state != mosq_ms_invalid);

#اگر_تعریف_شده WITH_PERSISTENCE
	إذا(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#پایان_اگر

	client_پیام = mosquitto_malloc(حجم(بنية mosquitto_client_پیام));
	إذا(!client_پیام){
		أرجع MOSQ_ERR_NOMEM;
	}
	client_پیام->prev = عدم;
	client_پیام->بعدی = عدم;
	إذا(cmsg_id){
		client_پیام->داده.cmsg_id = cmsg_id;
	}وإلا{
		client_پیام->داده.cmsg_id = ++context->آخرین_cmsg_id;
	}
	client_پیام->پایه_پیام = پایه_پیام;
	db_پیام_store_ref_inc(client_پیام->پایه_پیام);
	client_پیام->داده.mid = پایه_پیام->داده.منبع_mid;
	client_پیام->داده.direction = mosq_md_درون;
	client_پیام->داده.state = (uint8_t)state;
	client_پیام->داده.dup = false;
	إذا(پایه_پیام->داده.qos > context->max_qos){
		client_پیام->داده.qos = context->max_qos;
	}وإلا{
		client_پیام->داده.qos = پایه_پیام->داده.qos;
	}
	client_پیام->داده.retain = پایه_پیام->داده.retain;
	client_پیام->داده.subscription_identifier = 0;

	إذا(state == mosq_ms_queued){
		DL_APPEND(پیام_داده->queued, client_پیام);
		db_پیام_جمع_کن_مقصد_queued_stats(پیام_داده, client_پیام);
	}وإلا{
		DL_APPEND(پیام_داده->inflight, client_پیام);
		db_پیام_جمع_کن_مقصد_inflight_stats(پیام_داده, client_پیام);
	}

	إذا(persist && context->is_persisted){
		plugin_persist_handle_پایه_پیام_جمع_کن(client_پیام->پایه_پیام);
		plugin_persist_handle_client_پیام_جمع_کن(context, client_پیام);
	}

	إذا(client_پیام->پایه_پیام->داده.qos > 0){
		util__decrement_receive_quota(context);
	}
	أرجع rc;
}


صحيح db_پیام_درج_outgoing(بنية mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, بنية mosquitto_پایه_پیام *پایه_پیام, uint32_t subscription_identifier, bool update, bool persist)
{
	بنية mosquitto_client_پیام *client_پیام;
	بنية mosquitto_پیام_داده *پیام_داده;
	تعداد mosquitto_پیام_state state = mosq_ms_invalid;
	صحيح rc = 0;
	حرف **dest_ids;

	أكد(پایه_پیام);
	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		أرجع MOSQ_ERR_SUCCESS;

	}
	context->stats.messages_فرستاده++;

	پیام_داده = &context->msgs_بیرون;

	/* Check whether we've already sent this message to this client
	 * for outgoing messages only.
	 * If retain==true then this is a stale retained message and so should be
	 * sent regardless. FIXME - this does mean retained messages will received
	 * multiple times for overlapping subscriptions, although this is only the
	 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
	 */
	إذا(context->protocol != mosq_p_mqtt5
			&& db.config->allow_duplicate_messages == false
			&& retain == false && پایه_پیام->dest_ids){

		لكل(صحيح i=0; i<پایه_پیام->dest_id_شمار; i++){
			إذا(پایه_پیام->dest_ids[i] && !strcmp(پایه_پیام->dest_ids[i], context->id)){
				/* We have already sent this message to this client. */
				أرجع MOSQ_ERR_SUCCESS;
			}
		}
	}
	إذا(!net__is_connected(context)){
		/* Client is not connected only queue messages with QoS>0. */
		إذا(qos == 0 && !db.config->صف_qos0_messages){
			إذا(!context->bridge){
				أرجع 2;
			}وإلا{
				إذا(context->bridge->شروع_type != bst_lazy){
					أرجع 2;
				}
			}
		}
		إذا(context->bridge && context->bridge->clean_شروع_local == true){
			أرجع 2;
		}
	}

	إذا(net__is_connected(context)){
		إذا(db__ready_for_flight(context, mosq_md_بیرون, qos)){
			بدل(qos){
				حالة 0:
					state = mosq_ms_publish_qos0;
					اكسر;
				حالة 1:
					state = mosq_ms_publish_qos1;
					اكسر;
				حالة 2:
					state = mosq_ms_publish_qos2;
					اكسر;
			}
		}وإلا إذا(qos != 0 && db_ready_for_صف(context, qos, پیام_داده)){
			state = mosq_ms_queued;
			rc = 2;
		}وإلا{
			/* Dropping message due to full queue. */
			إذا(context->is_dropping == false){
				context->is_dropping = true;
				log__printf(عدم, MOSQ_LOG_NOTICE,
						"Outgoing messages are being dropped for client %s.",
						context->id);
			}
			metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
			أرجع 2;
		}
	}وإلا{
		إذا(db_ready_for_صف(context, qos, پیام_داده)){
			state = mosq_ms_queued;
		}وإلا{
			metrics__int_inc(mosq_counter_mqtt_publish_dropped, 1);
			إذا(context->is_dropping == false){
				context->is_dropping = true;
				log__printf(عدم, MOSQ_LOG_NOTICE,
						"Outgoing messages are being dropped for client %s.",
						context->id);
			}
			أرجع 2;
		}
	}
	أكد(state != mosq_ms_invalid);

#اگر_تعریف_شده WITH_PERSISTENCE
	إذا(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#پایان_اگر

	client_پیام = mosquitto_malloc(حجم(بنية mosquitto_client_پیام));
	إذا(!client_پیام){
		أرجع MOSQ_ERR_NOMEM;
	}
	client_پیام->prev = عدم;
	client_پیام->بعدی = عدم;
	إذا(cmsg_id){
		client_پیام->داده.cmsg_id = cmsg_id;
	}وإلا{
		client_پیام->داده.cmsg_id = ++context->آخرین_cmsg_id;
	}
	client_پیام->پایه_پیام = پایه_پیام;
	db_پیام_store_ref_inc(client_پیام->پایه_پیام);
	client_پیام->داده.mid = mid;
	client_پیام->داده.direction = mosq_md_بیرون;
	client_پیام->داده.state = (uint8_t)state;
	client_پیام->داده.dup = false;
	إذا(qos > context->max_qos){
		client_پیام->داده.qos = context->max_qos;
	}وإلا{
		client_پیام->داده.qos = qos;
	}
	client_پیام->داده.retain = retain;
	client_پیام->داده.subscription_identifier = subscription_identifier;

	إذا(state == mosq_ms_queued){
		DL_APPEND(پیام_داده->queued, client_پیام);
		db_پیام_جمع_کن_مقصد_queued_stats(پیام_داده, client_پیام);
	}وإلا{
		DL_APPEND(پیام_داده->inflight, client_پیام);
		db_پیام_جمع_کن_مقصد_inflight_stats(پیام_داده, client_پیام);
	}

	إذا(persist && context->is_persisted){
		plugin_persist_handle_پایه_پیام_جمع_کن(client_پیام->پایه_پیام);
		plugin_persist_handle_client_پیام_جمع_کن(context, client_پیام);
	}

	إذا(db.config->allow_duplicate_messages == false && retain == false){
		/* Record which client ids this message has been sent to so we can avoid duplicates.
		 * Outgoing messages only.
		 * If retain==true then this is a stale retained message and so should be
		 * sent regardless. FIXME - this does mean retained messages will received
		 * multiple times for overlapping subscriptions, although this is only the
		 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
		 */
		dest_ids = mosquitto_realloc(پایه_پیام->dest_ids, حجم(حرف *)*(size_t)(پایه_پیام->dest_id_شمار+1));
		إذا(dest_ids){
			پایه_پیام->dest_ids = dest_ids;
			پایه_پیام->dest_id_شمار++;
			پایه_پیام->dest_ids[پایه_پیام->dest_id_شمار-1] = mosquitto_strdup(context->id);
			إذا(!پایه_پیام->dest_ids[پایه_پیام->dest_id_شمار-1]){
				أرجع MOSQ_ERR_NOMEM;
			}
		}وإلا{
			أرجع MOSQ_ERR_NOMEM;
		}
	}
#اگر_تعریف_شده WITH_BRIDGE
	إذا(context->bridge && context->bridge->شروع_type == bst_lazy
			&& !net__is_connected(context)
			&& context->msgs_بیرون.inflight_شمار + context->msgs_بیرون.queued_شمار >= context->bridge->آستانه){

		context->bridge->lazy_reconnect = true;
	}
#پایان_اگر

	إذا(client_پیام->داده.qos > 0 && state != mosq_ms_queued){
		util__decrement_send_quota(context);
	}

	إذا(update){
		rc = db_پیام_write_inflight_بیرون_latest(context);
		إذا(rc){
			أرجع rc;
		}
		rc = db_پیام_write_queued_بیرون(context);
		إذا(rc){
			أرجع rc;
		}
	}

	أرجع rc;
}


ساكن مضمن صحيح db_پیام_update_outgoing_state(بنية mosquitto *context, بنية mosquitto_client_پیام *سر,
		uint16_t mid, تعداد mosquitto_پیام_state state, صحيح qos, bool persist)
{
	بنية mosquitto_client_پیام *client_پیام;

	DL_FOREACH(سر, client_پیام){
		إذا(client_پیام->داده.mid == mid){
			إذا(client_پیام->داده.qos != qos){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when updating outgoing message.",
						context->id, client_پیام->داده.qos, qos);
				أرجع MOSQ_ERR_PROTOCOL;
			}
			client_پیام->داده.state = (uint8_t)state;
			إذا(persist){
				plugin_persist_handle_client_پیام_update(context, client_پیام);
			}
			أرجع MOSQ_ERR_SUCCESS;
		}
	}
	أرجع MOSQ_ERR_NOT_یافت;
}


صحيح db_پیام_update_outgoing(بنية mosquitto *context, uint16_t mid, تعداد mosquitto_پیام_state state, صحيح qos, bool persist)
{
	صحيح rc;

	rc = db_پیام_update_outgoing_state(context, context->msgs_بیرون.inflight, mid, state, qos, persist);
	إذا(!persist && rc == MOSQ_ERR_NOT_یافت){
		rc = db_پیام_update_outgoing_state(context, context->msgs_بیرون.queued, mid, state, qos, persist);
	}
	أرجع rc;
}


ساكن فراغ db__messages_delete_list(بنية mosquitto_client_پیام **سر)
{
	بنية mosquitto_client_پیام *client_پیام, *tmp;

	DL_FOREACH_امن(*سر, client_پیام, tmp){
		DL_DELETE(*سر, client_پیام);
		db_پیام_store_ref_dec(&client_پیام->پایه_پیام);
		mosquitto_FREE(client_پیام);
	}
	*سر = عدم;
}


صحيح db__messages_delete_incoming(بنية mosquitto *context)
{
	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_درون.inflight);
	db__messages_delete_list(&context->msgs_درون.queued);
	context->msgs_درون.inflight_بایتها = 0;
	context->msgs_درون.inflight_bytes12 = 0;
	context->msgs_درون.inflight_شمار = 0;
	context->msgs_درون.inflight_count12 = 0;
	context->msgs_درون.queued_بایتها = 0;
	context->msgs_درون.queued_bytes12 = 0;
	context->msgs_درون.queued_شمار = 0;
	context->msgs_درون.queued_count12 = 0;

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db__messages_delete_outgoing(بنية mosquitto *context)
{
	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_بیرون.inflight);
	db__messages_delete_list(&context->msgs_بیرون.queued);
	context->msgs_بیرون.inflight_بایتها = 0;
	context->msgs_بیرون.inflight_bytes12 = 0;
	context->msgs_بیرون.inflight_شمار = 0;
	context->msgs_بیرون.inflight_count12 = 0;
	context->msgs_بیرون.queued_بایتها = 0;
	context->msgs_بیرون.queued_bytes12 = 0;
	context->msgs_بیرون.queued_شمار = 0;
	context->msgs_بیرون.queued_count12 = 0;

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db__messages_delete(بنية mosquitto *context, bool force_free)
{
	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	إذا(force_free || context->clean_شروع || (context->bridge && context->bridge->clean_شروع)){
		db__messages_delete_incoming(context);
	}

	إذا(force_free || (context->bridge && context->bridge->clean_شروع_local)
			|| (context->bridge == عدم && context->clean_شروع)){

		db__messages_delete_outgoing(context);
	}

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db_messages_easy_صف(بنية mosquitto *context, ثابت حرف *topic, uint8_t qos, uint32_t payloadlen, ثابت فراغ *payload, صحيح retain, uint32_t پیام_expiry_interval, mosquitto_property **properties)
{
	بنية mosquitto_پایه_پیام *پایه_پیام;
	ثابت حرف *منبع_id;
	تعداد mosquitto_پیام_origin origin;

	إذا(!topic){
		أرجع MOSQ_ERR_INVAL;
	}

	پایه_پیام = mosquitto_calloc(1, حجم(بنية mosquitto_پایه_پیام));
	إذا(پایه_پیام == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}

	پایه_پیام->داده.topic = mosquitto_strdup(topic);
	إذا(پایه_پیام->داده.topic == عدم){
		db_پیام_store_free(پایه_پیام);
		أرجع MOSQ_ERR_INVAL;
	}

	پایه_پیام->داده.qos = qos;
	إذا(db.config->retain_موجود == false){
		پایه_پیام->داده.retain = 0;
	}وإلا{
		پایه_پیام->داده.retain = retain;
	}

	پایه_پیام->داده.payloadlen = payloadlen;
	إذا(payloadlen > 0){
		پایه_پیام->داده.payload = mosquitto_malloc(پایه_پیام->داده.payloadlen+1);
		إذا(پایه_پیام->داده.payload == عدم){
			db_پیام_store_free(پایه_پیام);
			أرجع MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)پایه_پیام->داده.payload)[پایه_پیام->داده.payloadlen] = 0;
		کپی_حافظه(پایه_پیام->داده.payload, payload, پایه_پیام->داده.payloadlen);
	}

	إذا(context && context->id){
		منبع_id = context->id;
	}وإلا{
		منبع_id = "";
	}
	إذا(properties){
		پایه_پیام->داده.properties = *properties;
		*properties = عدم;
	}

	إذا(context){
		origin = mosq_mo_client;
	}وإلا{
		origin = mosq_mo_broker;
	}
	إذا(db_پیام_store(context, پایه_پیام, &پیام_expiry_interval, origin)){
		أرجع 1;
	}

	أرجع sub_messages_صف(منبع_id, پایه_پیام->داده.qos, &پایه_پیام);
}


#تعریف_کن MOSQ_UUID_EPOCH 1637168273


/* db__new_msg_id() attempts to generate a new unique id on the broker, or a
 * number of brokers. It uses the 10-bit node ID, which can be set by plugins
 * to allow different brokers to share the same plugin persistence database
 * without overlapping one another.
 *
 * The message ID is a 64-bit unsigned integer arranged as follows:
 *
 * 10-bit ID  31-bit seconds                 23-bit fractional seconds
 * iiiiiiiiiisssssssssssssssssssssssssssssssnnnnnnnnnnnnnnnnnnnnnnn
 *
 * 10-bit ID gives a total of 1024 brokers can produce unique values (complete overkill)
 * 31-bit seconds gives a roll over date of 68 years after MOSQ_UUID_EPOCH - 2089.
 *    This roll over date would affect messages that have been queued waiting
 *    for a client to receive them, or retained messages only. If either of
 *    those remains for 68 years unchanged, then there will potentially be a
 *    collision. Ideally we need to ensure, however, that the message id is
 *    continually increasing for sorting purposes.
 * 23-bit fractional seconds gives a resolution of 120ns, or 8.4 million
 *    messages per second per broker.
 */
uint64_t db_new_پیام_id(فراغ)
{
#اگر_تعریف_شده WIN32
	FILETIME ftime;
	uint64_t ftime64;
#وإلا
	بنية timespec ts;
#پایان_اگر
	uint64_t id;
	uint64_t tmp;
	time_t sec;
	دراز nsec;

	id = db.گره_id_shifted; /* Top 10-bits */

#اگر_تعریف_شده WIN32
	Get_System_زمان_Precise_As_File_زمان(&ftime);
	ftime64 = (((uint64_t)ftime.dw_بالا_Date_زمان)<<32) + ftime.dw_کم_Date_زمان;
	tmp = ftime64 - 116444736000000000LL; /* Convert offset to unix epoch, still in counts of 100ns */
	sec = tmp / 10000000; /* Convert to seconds */
	nsec = (دراز)(tmp - sec)*100; /* Remove seconds, convert to counts of 1ns */
#وإلا
	clock_gettime(CLOCK_REALTIME, &ts);
	sec = ts.tv_sec;
	nsec = ts.tv_nsec;
#پایان_اگر
	tmp = (sec - MOSQ_UUID_EPOCH) & 0x7FFFFFFF;
	id = id | (tmp << 23); /* Seconds, 31-bits (68 years) */

	tmp = (nsec & 0x7FFFFF80); /* top 23-bits of the bottom 30 bits (1 billion ns), ~100 ns resolution */
	id = id | (tmp >> 7);

	إذا(id <= db.آخرین_db_id){
		id = db.آخرین_db_id + 1;
	}
	db.آخرین_db_id = id;

	أرجع id;
}


/* This function requires topic to be allocated on the heap. Once called, it owns topic and will free it on error. Likewise payload and properties. */
صحيح db_پیام_store(ثابت بنية mosquitto *منبع, بنية mosquitto_پایه_پیام *پایه_پیام, uint32_t *پیام_expiry_interval, تعداد mosquitto_پیام_origin origin)
{
	صحيح rc;

	أكد(پایه_پیام);

	إذا(منبع && منبع->id){
		پایه_پیام->داده.منبع_id = mosquitto_strdup(منبع->id);
	}وإلا{
		پایه_پیام->داده.منبع_id = mosquitto_strdup("");
	}
	إذا(!پایه_پیام->داده.منبع_id){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		db_پیام_store_free(پایه_پیام);
		أرجع MOSQ_ERR_NOMEM;
	}

	إذا(منبع && منبع->username){
		پایه_پیام->داده.منبع_username = mosquitto_strdup(منبع->username);
		إذا(!پایه_پیام->داده.منبع_username){
			db_پیام_store_free(پایه_پیام);
			أرجع MOSQ_ERR_NOMEM;
		}
	}
	إذا(منبع){
		پایه_پیام->منبع_listener = منبع->listener;
	}
	پایه_پیام->origin = origin;
	إذا(پیام_expiry_interval){
		إذا(*پیام_expiry_interval > 0 && *پیام_expiry_interval != پیام_EXPIRY_INFINITE){
			پایه_پیام->داده.expiry_زمان = db.now_real_s + *پیام_expiry_interval;
		}وإلا{
			پایه_پیام->داده.expiry_زمان = 0;
		}
	}

	پایه_پیام->dest_ids = عدم;
	پایه_پیام->dest_id_شمار = 0;
	db.پیام_store_شمار++;
	db.پیام_store_بایتها += پایه_پیام->داده.payloadlen;

	إذا(!پایه_پیام->داده.store_id){
		پایه_پیام->داده.store_id = db_new_پیام_id();
	}

	rc = db_پیام_store_جمع_کن(پایه_پیام);
	إذا(rc){
		db_پیام_store_free(پایه_پیام);
		أرجع rc;
	}

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db_پیام_store_پیدا_کن(بنية mosquitto *context, uint16_t mid, بنية mosquitto_client_پیام **client_پیام)
{
	بنية mosquitto_client_پیام *cmsg;

	*client_پیام = عدم;

	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	DL_FOREACH(context->msgs_درون.inflight, cmsg){
		إذا(cmsg->پایه_پیام->داده.منبع_mid == mid){
			*client_پیام = cmsg;
			أرجع MOSQ_ERR_SUCCESS;
		}
	}

	DL_FOREACH(context->msgs_درون.queued, cmsg){
		إذا(cmsg->پایه_پیام->داده.منبع_mid == mid){
			*client_پیام = cmsg;
			أرجع MOSQ_ERR_SUCCESS;
		}
	}

	أرجع 1;
}


/* Called on reconnect to set outgoing messages to a sensible state and force a
 * retry, and to set incoming messages to expect an appropriate retry. */
ساكن صحيح db_پیام_reconnect_reset_outgoing(بنية mosquitto *context)
{
	بنية mosquitto_client_پیام *client_پیام, *tmp;

	context->msgs_بیرون.inflight_بایتها = 0;
	context->msgs_بیرون.inflight_bytes12 = 0;
	context->msgs_بیرون.inflight_شمار = 0;
	context->msgs_بیرون.inflight_count12 = 0;
	context->msgs_بیرون.queued_بایتها = 0;
	context->msgs_بیرون.queued_bytes12 = 0;
	context->msgs_بیرون.queued_شمار = 0;
	context->msgs_بیرون.queued_count12 = 0;
	context->msgs_بیرون.inflight_quota = context->msgs_بیرون.inflight_بیشینه;

	DL_FOREACH_امن(context->msgs_بیرون.inflight, client_پیام, tmp){
		db_پیام_جمع_کن_مقصد_inflight_stats(&context->msgs_بیرون, client_پیام);
		إذا(client_پیام->داده.qos > 0){
			util__decrement_send_quota(context);
		}

		بدل(client_پیام->داده.qos){
			حالة 0:
				client_پیام->داده.state = mosq_ms_publish_qos0;
				اكسر;
			حالة 1:
				client_پیام->داده.state = mosq_ms_publish_qos1;
				اكسر;
			حالة 2:
				إذا(client_پیام->داده.state == mosq_ms_wait_for_pubcomp){
					client_پیام->داده.state = mosq_ms_resend_pubrel;
				}وإلا{
					client_پیام->داده.state = mosq_ms_publish_qos2;
				}
				اكسر;
		}
		plugin_persist_handle_client_پیام_update(context, client_پیام);
	}
	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_امن(context->msgs_بیرون.queued, client_پیام, tmp){
		db_پیام_جمع_کن_مقصد_queued_stats(&context->msgs_بیرون, client_پیام);
	}
	db_fill_inflight_بیرون_from_صف(context);

	أرجع MOSQ_ERR_SUCCESS;
}


/* Called on reconnect to set incoming messages to expect an appropriate retry. */
ساكن صحيح db_پیام_reconnect_reset_incoming(بنية mosquitto *context)
{
	بنية mosquitto_client_پیام *client_پیام, *tmp;

	context->msgs_درون.inflight_بایتها = 0;
	context->msgs_درون.inflight_bytes12 = 0;
	context->msgs_درون.inflight_شمار = 0;
	context->msgs_درون.inflight_count12 = 0;
	context->msgs_درون.queued_بایتها = 0;
	context->msgs_درون.queued_bytes12 = 0;
	context->msgs_درون.queued_شمار = 0;
	context->msgs_درون.queued_count12 = 0;
	context->msgs_درون.inflight_quota = context->msgs_درون.inflight_بیشینه;

	DL_FOREACH_امن(context->msgs_درون.inflight, client_پیام, tmp){
		db_پیام_جمع_کن_مقصد_inflight_stats(&context->msgs_درون, client_پیام);
		إذا(client_پیام->داده.qos > 0){
			util__decrement_receive_quota(context);
		}

		إذا(client_پیام->داده.qos != 2){
			/* Anything <QoS 2 can be completely retried by the client at
			 * no harm. */
			db_پیام_remove_inflight(context, &context->msgs_درون, client_پیام);
		}وإلا{
			/* Message state can be preserved here because it should match
			 * whatever the client has got. */
			client_پیام->داده.dup = 0;
		}
	}

	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_امن(context->msgs_درون.queued, client_پیام, tmp){
		client_پیام->داده.dup = 0;
		db_پیام_جمع_کن_مقصد_queued_stats(&context->msgs_درون, client_پیام);
		إذا(db__ready_for_flight(context, mosq_md_درون, client_پیام->داده.qos)){
			بدل(client_پیام->داده.qos){
				حالة 0:
					client_پیام->داده.state = mosq_ms_publish_qos0;
					اكسر;
				حالة 1:
					client_پیام->داده.state = mosq_ms_publish_qos1;
					اكسر;
				حالة 2:
					client_پیام->داده.state = mosq_ms_publish_qos2;
					اكسر;
			}
			db_پیام_خارج_از_صف_اولی(context, &context->msgs_درون);
			plugin_persist_handle_client_پیام_update(context, client_پیام);
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db_پیام_reconnect_reset(بنية mosquitto *context)
{
	صحيح rc;

	rc = db_پیام_reconnect_reset_outgoing(context);
	إذا(rc){
		أرجع rc;
	}
	أرجع db_پیام_reconnect_reset_incoming(context);
}


صحيح db_پیام_remove_incoming(بنية mosquitto *context, uint16_t mid)
{
	بنية mosquitto_client_پیام *client_پیام, *tmp;

	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	DL_FOREACH_امن(context->msgs_درون.inflight, client_پیام, tmp){
		إذا(client_پیام->داده.mid == mid){
			إذا(client_پیام->پایه_پیام->داده.qos != 2){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when deleting incoming message.",
						context->id, client_پیام->پایه_پیام->داده.qos);
				أرجع MOSQ_ERR_PROTOCOL;
			}
			db_پیام_remove_inflight(context, &context->msgs_درون, client_پیام);
			أرجع MOSQ_ERR_SUCCESS;
		}
	}

	أرجع MOSQ_ERR_NOT_یافت;
}


صحيح db_پیام_release_incoming(بنية mosquitto *context, uint16_t mid)
{
	بنية mosquitto_client_پیام *client_پیام, *tmp;
	bool deleted = false;
	صحيح rc;

	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	DL_FOREACH_امن(context->msgs_درون.inflight, client_پیام, tmp){
		إذا(client_پیام->داده.mid == mid){
			إذا(client_پیام->پایه_پیام->داده.qos != 2){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when releasing incoming message.",
						context->id, client_پیام->پایه_پیام->داده.qos);
				أرجع MOSQ_ERR_PROTOCOL;
			}
			ثابت حرف *topic = client_پیام->پایه_پیام->داده.topic;
			ثابت حرف *منبع_id = client_پیام->پایه_پیام->داده.منبع_id;

			/* topic==NULL should be a QoS 2 message that was
			 * denied/dropped and is being processed so the client doesn't
			 * keep resending it. That means we don't send it to other
			 * clients. */
			إذا(topic == عدم){
				db_پیام_remove_inflight(context, &context->msgs_درون, client_پیام);
				deleted = true;
			}وإلا{
				rc = sub_messages_صف(منبع_id, 2, &client_پیام->پایه_پیام);
				إذا(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_NO_SUBSCRIBERS){
					db_پیام_remove_inflight(context, &context->msgs_درون, client_پیام);
					deleted = true;
				}وإلا{
					أرجع 1;
				}
			}
		}
	}

	DL_FOREACH_امن(context->msgs_درون.queued, client_پیام, tmp){
		إذا(db__ready_for_flight(context, mosq_md_درون, client_پیام->داده.qos)){
			اكسر;
		}

		إذا(client_پیام->داده.qos == 2){
			send__pubrec(context, client_پیام->داده.mid, 0, عدم);
			client_پیام->داده.state = mosq_ms_wait_for_pubrel;
			db_پیام_خارج_از_صف_اولی(context, &context->msgs_درون);
			plugin_persist_handle_client_پیام_update(context, client_پیام);
		}
	}
	إذا(deleted){
		أرجع MOSQ_ERR_SUCCESS;
	}وإلا{
		أرجع MOSQ_ERR_NOT_یافت;
	}
}


فراغ db__expire_all_messages(بنية mosquitto *context)
{
	بنية mosquitto_client_پیام *client_پیام, *tmp;

	DL_FOREACH_امن(context->msgs_بیرون.inflight, client_پیام, tmp){
		إذا(client_پیام->پایه_پیام->داده.expiry_زمان && db.now_real_s > client_پیام->پایه_پیام->داده.expiry_زمان){
			إذا(client_پیام->داده.qos > 0){
				util__increment_send_quota(context);
			}
			db_پیام_remove_inflight(context, &context->msgs_بیرون, client_پیام);
		}
	}
	db_fill_inflight_بیرون_from_صف(context);
	DL_FOREACH_امن(context->msgs_بیرون.queued, client_پیام, tmp){
		إذا(client_پیام->پایه_پیام->داده.expiry_زمان && db.now_real_s > client_پیام->پایه_پیام->داده.expiry_زمان){
			db_پیام_remove_queued(context, &context->msgs_بیرون, client_پیام);
		}
	}
	DL_FOREACH_امن(context->msgs_درون.inflight, client_پیام, tmp){
		إذا(client_پیام->پایه_پیام->داده.expiry_زمان && db.now_real_s > client_پیام->پایه_پیام->داده.expiry_زمان){
			إذا(client_پیام->داده.qos > 0){
				util__increment_receive_quota(context);
			}
			db_پیام_remove_inflight(context, &context->msgs_درون, client_پیام);
		}
	}
	DL_FOREACH_امن(context->msgs_درون.queued, client_پیام, tmp){
		إذا(client_پیام->پایه_پیام->داده.expiry_زمان && db.now_real_s > client_پیام->پایه_پیام->داده.expiry_زمان){
			db_پیام_remove_queued(context, &context->msgs_درون, client_پیام);
		}
	}
}


ساكن فراغ db__client_messages_check_acl(بنية mosquitto *context, بنية mosquitto_پیام_داده *پیام_داده, بنية mosquitto_client_پیام **سر,
		فراغ (*decrement_stats_fn)(بنية mosquitto_پیام_داده *پیام_داده, بنية mosquitto_client_پیام *client_پیام))
{
	بنية mosquitto_client_پیام *client_پیام, *tmp;
	بنية mosquitto_پایه_پیام *پایه_پیام;
	صحيح access;

	DL_FOREACH_امن((*سر), client_پیام, tmp){
		پایه_پیام = client_پیام->پایه_پیام;
		إذا(client_پیام->داده.direction == mosq_md_بیرون){
			access = MOSQ_ACL_READ;
		}وإلا{
			access = MOSQ_ACL_WRITE;
		}
		إذا(mosquitto_acl_check(context, پایه_پیام->داده.topic,
				پایه_پیام->داده.payloadlen, پایه_پیام->داده.payload,
				پایه_پیام->داده.qos, پایه_پیام->داده.retain,
				پایه_پیام->داده.properties, access) != MOSQ_ERR_SUCCESS){

			DL_DELETE((*سر), client_پیام);
			decrement_stats_fn(پیام_داده, client_پیام);
			plugin_persist_handle_client_پیام_delete(context, client_پیام);
			db_پیام_store_ref_dec(&client_پیام->پایه_پیام);
			mosquitto_FREE(client_پیام);
		}
	}
}


فراغ db_check_acl_از_all_messages(بنية mosquitto *context)
{
	db__client_messages_check_acl(context, &context->msgs_درون, &context->msgs_درون.inflight, &db_پیام_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_درون, &context->msgs_درون.queued, &db_پیام_remove_from_queued_stats);
	db__client_messages_check_acl(context, &context->msgs_بیرون, &context->msgs_بیرون.inflight, &db_پیام_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_بیرون, &context->msgs_بیرون.queued, &db_پیام_remove_from_queued_stats);
}


ساكن صحيح db_پیام_write_inflight_بیرون_single(بنية mosquitto *context, بنية mosquitto_client_پیام *client_پیام)
{
	بنية mosquitto_پایه_پیام *پایه_پیام;
	mosquitto_property *پایه_پیام_props = عدم;
	صحيح rc;
	uint16_t mid;
	صحيح retries;
	صحيح retain;
	ثابت حرف *topic;
	uint8_t qos;
	uint32_t payloadlen;
	ثابت فراغ *payload;
	uint32_t expiry_interval;
	uint32_t subscription_id;

	پایه_پیام = client_پیام->پایه_پیام;

	expiry_interval = 0;
	إذا(پایه_پیام->داده.expiry_زمان){
		إذا(db.now_real_s > پایه_پیام->داده.expiry_زمان){
			/* Message is expired, must not send. */
			إذا(client_پیام->داده.direction == mosq_md_بیرون && client_پیام->داده.qos > 0){
				util__increment_send_quota(context);
			}
			db_پیام_remove_inflight(context, &context->msgs_بیرون, client_پیام);
			db_fill_inflight_بیرون_from_صف(context);
			أرجع MOSQ_ERR_SUCCESS;
		}وإلا{
			expiry_interval = (uint32_t)(پایه_پیام->داده.expiry_زمان - db.now_real_s);
		}
	}
	mid = client_پیام->داده.mid;
	retries = client_پیام->داده.dup;
	retain = client_پیام->داده.retain;
	topic = پایه_پیام->داده.topic;
	qos = (uint8_t)client_پیام->داده.qos;
	payloadlen = پایه_پیام->داده.payloadlen;
	payload = پایه_پیام->داده.payload;
	subscription_id = client_پیام->داده.subscription_identifier;
	پایه_پیام_props = پایه_پیام->داده.properties;

	بدل(client_پیام->داده.state){
		حالة mosq_ms_publish_qos0:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, پایه_پیام_props, expiry_interval);
			إذا(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_OVERSIZE_PACKET){
				db_پیام_remove_inflight(context, &context->msgs_بیرون, client_پیام);
			}وإلا{
				أرجع rc;
			}
			اكسر;

		حالة mosq_ms_publish_qos1:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, پایه_پیام_props, expiry_interval);
			إذا(rc == MOSQ_ERR_SUCCESS){
				client_پیام->داده.dup = 1; /* Any retry attempts are a duplicate. */
				client_پیام->داده.state = mosq_ms_wait_for_puback;
				plugin_persist_handle_client_پیام_update(context, client_پیام);
			}وإلا إذا(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db_پیام_remove_inflight(context, &context->msgs_بیرون, client_پیام);
			}وإلا{
				أرجع rc;
			}
			اكسر;

		حالة mosq_ms_publish_qos2:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, پایه_پیام_props, expiry_interval);
			إذا(rc == MOSQ_ERR_SUCCESS){
				client_پیام->داده.dup = 1; /* Any retry attempts are a duplicate. */
				client_پیام->داده.state = mosq_ms_wait_for_pubrec;
				plugin_persist_handle_client_پیام_update(context, client_پیام);
			}وإلا إذا(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db_پیام_remove_inflight(context, &context->msgs_بیرون, client_پیام);
			}وإلا{
				أرجع rc;
			}
			اكسر;

		حالة mosq_ms_resend_pubrel:
			rc = send__pubrel(context, mid, عدم);
			إذا(!rc){
				client_پیام->داده.state = mosq_ms_wait_for_pubcomp;
				plugin_persist_handle_client_پیام_update(context, client_پیام);
			}وإلا{
				أرجع rc;
			}
			اكسر;

		حالة mosq_ms_invalid:
		حالة mosq_ms_send_pubrec:
		حالة mosq_ms_resend_pubcomp:
		حالة mosq_ms_wait_for_puback:
		حالة mosq_ms_wait_for_pubrec:
		حالة mosq_ms_wait_for_pubrel:
		حالة mosq_ms_wait_for_pubcomp:
		حالة mosq_ms_queued:
			اكسر;
	}
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db_پیام_write_inflight_بیرون_all(بنية mosquitto *context)
{
	بنية mosquitto_client_پیام *client_پیام, *tmp;
	صحيح rc;

	إذا(context->state != mosq_cs_فعال || !net__is_connected(context)){
		أرجع MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_امن(context->msgs_بیرون.inflight, client_پیام, tmp){
		rc = db_پیام_write_inflight_بیرون_single(context, client_پیام);
		إذا(rc){
			أرجع rc;
		}
	}
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db_پیام_write_inflight_بیرون_latest(بنية mosquitto *context)
{
	بنية mosquitto_client_پیام *client_پیام, *بعدی;
	صحيح rc;

	إذا(context->state != mosq_cs_فعال
			|| !net__is_connected(context)
			|| context->msgs_بیرون.inflight == عدم){

		أرجع MOSQ_ERR_SUCCESS;
	}

	إذا(context->msgs_بیرون.inflight->prev == context->msgs_بیرون.inflight){
		/* Only one message */
		أرجع db_پیام_write_inflight_بیرون_single(context, context->msgs_بیرون.inflight);
	}

	/* Start at the end of the list and work backwards looking for the first
	 * message in a non-publish state */
	client_پیام = context->msgs_بیرون.inflight->prev;
	طالما(client_پیام != context->msgs_بیرون.inflight &&
			(client_پیام->داده.state == mosq_ms_publish_qos0
			|| client_پیام->داده.state == mosq_ms_publish_qos1
			|| client_پیام->داده.state == mosq_ms_publish_qos2)){

		client_پیام = client_پیام->prev;
	}

	/* Tail is now either the head of the list, if that message is waiting for
	 * publish, or the oldest message not waiting for a publish. In the latter
	 * case, any pending publishes should be next after this message. */
	إذا(client_پیام != context->msgs_بیرون.inflight){
		client_پیام = client_پیام->بعدی;
	}

	طالما(client_پیام){
		بعدی = client_پیام->بعدی;
		rc = db_پیام_write_inflight_بیرون_single(context, client_پیام);
		إذا(rc){
			أرجع rc;
		}
		client_پیام = بعدی;
	}
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db_پیام_write_queued_درون(بنية mosquitto *context)
{
	بنية mosquitto_client_پیام *client_پیام, *tmp;
	صحيح rc;

	إذا(context->state != mosq_cs_فعال){
		أرجع MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_امن(context->msgs_درون.queued, client_پیام, tmp){
		إذا(context->msgs_درون.inflight_بیشینه != 0 && context->msgs_درون.inflight_quota == 0){
			اكسر;
		}

		إذا(client_پیام->داده.qos == 2){
			client_پیام->داده.state = mosq_ms_send_pubrec;
			db_پیام_خارج_از_صف_اولی(context, &context->msgs_درون);
			rc = send__pubrec(context, client_پیام->داده.mid, 0, عدم);
			إذا(!rc){
				client_پیام->داده.state = mosq_ms_wait_for_pubrel;
				plugin_persist_handle_client_پیام_update(context, client_پیام);
			}وإلا{
				plugin_persist_handle_client_پیام_update(context, client_پیام);
				أرجع rc;
			}
		}
	}
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db_پیام_write_queued_بیرون(بنية mosquitto *context)
{
	إذا(context->state != mosq_cs_فعال){
		أرجع MOSQ_ERR_SUCCESS;
	}

	db_fill_inflight_بیرون_from_صف(context);

	أرجع MOSQ_ERR_SUCCESS;
}
