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
bool db__ready_for_flight(بنية mosquitto *context, تعداد mosquitto_پیغام_direction dir, صحيح qos)
{
	بنية mosquitto_پیغام_معلومات *msgs;
	bool valid_بایټونه;
	bool valid_شمېر;

	إذا(dir == mosq_md_بهر){
		msgs = &context->msgs_بهر;
	}وإلا{
		msgs = &context->msgs_دننه;
	}

	إذا(msgs->inflight_اعظمي == 0 && db.config->max_inflight_بایټونه == 0){
		أرجع true;
	}

	إذا(qos == 0){
		/* Deliver QoS 0 messages unless the queue is already full.
		 * For QoS 0 messages the choice is either "inflight" or dropped.
		 * There is no queueing option, unless the client is offline and
		 * queue_qos0_messages is enabled.
		 */
		إذا(db.config->max_queued_messages == 0 && db.config->max_inflight_بایټونه == 0){
			أرجع true;
		}
		valid_بایټونه = ((msgs->inflight_بایټونه - (ssize_t)db.config->max_inflight_بایټونه) < (ssize_t)db.config->max_queued_بایټونه);
		إذا(dir == mosq_md_بهر){
			valid_شمېر = context->بهر_packet_شمېر < db.config->max_queued_messages;
		}وإلا{
			valid_شمېر = msgs->inflight_شمېر - msgs->inflight_اعظمي < db.config->max_queued_messages;
		}

		إذا(db.config->max_queued_messages == 0){
			أرجع valid_بایټونه;
		}
		إذا(db.config->max_queued_بایټونه == 0){
			أرجع valid_شمېر;
		}
	}وإلا{
		valid_بایټونه = (ssize_t)msgs->inflight_bytes12 < (ssize_t)db.config->max_inflight_بایټونه;
		valid_شمېر = msgs->inflight_quota > 0;

		إذا(msgs->inflight_اعظمي == 0){
			أرجع valid_بایټونه;
		}
		إذا(db.config->max_inflight_بایټونه == 0){
			أرجع valid_شمېر;
		}
	}

	أرجع valid_بایټونه && valid_شمېر;
}


/**
 * For a given client context, are more messages allowed to be queued?
 * It is assumed that inflight checks and queue_qos0 checks have already
 * been made.
 * @param context client of interest
 * @param qos destination qos for the packet of interest
 * @return true if queuing is allowed, false if should be dropped
 */
bool db_ready_for_کتار(بنية mosquitto *context, صحيح qos, بنية mosquitto_پیغام_معلومات *پیغام_معلومات)
{
	صحيح سرچینه_شمېر;
	صحيح adjust_شمېر;
	اوږد سرچینه_بایټونه;
	ssize_t adjust_بایټونه = (ssize_t)db.config->max_inflight_بایټونه;
	bool valid_بایټونه;
	bool valid_شمېر;

	إذا(db.config->max_queued_messages == 0 && db.config->max_queued_بایټونه == 0){
		أرجع true;
	}

	إذا(qos == 0 && db.config->کتار_qos0_messages == false){
		أرجع false; /* This case is handled in db__ready_for_flight() */
	}وإلا{
		سرچینه_بایټونه = (ssize_t)پیغام_معلومات->queued_bytes12;
		سرچینه_شمېر = پیغام_معلومات->queued_count12;
	}
	adjust_شمېر = پیغام_معلومات->inflight_اعظمي;

	/* nothing in flight for offline clients */
	إذا(!net__is_connected(context)){
		adjust_بایټونه = 0;
		adjust_شمېر = 0;
	}

	valid_بایټونه = (سرچینه_بایټونه - (ssize_t)adjust_بایټونه) < (ssize_t)db.config->max_queued_بایټونه;
	valid_شمېر = سرچینه_شمېر - adjust_شمېر < db.config->max_queued_messages;

	إذا(db.config->max_queued_بایټونه == 0){
		أرجع valid_شمېر;
	}
	إذا(db.config->max_queued_messages == 0){
		أرجع valid_بایټونه;
	}

	أرجع valid_بایټونه && valid_شمېر;
}


فراغ db_پیغام_جمع_کړه_منزل_inflight_stats(بنية mosquitto_پیغام_معلومات *پیغام_معلومات, بنية mosquitto_client_پیغام *client_پیغام)
{
	پیغام_معلومات->inflight_شمېر++;
	پیغام_معلومات->inflight_بایټونه += client_پیغام->بنسټ_پیغام->معلومات.payloadlen;
	إذا(client_پیغام->معلومات.qos != 0){
		پیغام_معلومات->inflight_count12++;
		پیغام_معلومات->inflight_bytes12 += client_پیغام->بنسټ_پیغام->معلومات.payloadlen;
	}
}


ساكن فراغ db_پیغام_remove_from_inflight_stats(بنية mosquitto_پیغام_معلومات *پیغام_معلومات, بنية mosquitto_client_پیغام *client_پیغام)
{
	پیغام_معلومات->inflight_شمېر--;
	پیغام_معلومات->inflight_بایټونه -= client_پیغام->بنسټ_پیغام->معلومات.payloadlen;
	إذا(client_پیغام->معلومات.qos != 0){
		پیغام_معلومات->inflight_count12--;
		پیغام_معلومات->inflight_bytes12 -= client_پیغام->بنسټ_پیغام->معلومات.payloadlen;
	}
}


فراغ db_پیغام_جمع_کړه_منزل_queued_stats(بنية mosquitto_پیغام_معلومات *پیغام_معلومات, بنية mosquitto_client_پیغام *client_پیغام)
{
	پیغام_معلومات->queued_شمېر++;
	پیغام_معلومات->queued_بایټونه += client_پیغام->بنسټ_پیغام->معلومات.payloadlen;
	إذا(client_پیغام->معلومات.qos != 0){
		پیغام_معلومات->queued_count12++;
		پیغام_معلومات->queued_bytes12 += client_پیغام->بنسټ_پیغام->معلومات.payloadlen;
	}
}


ساكن فراغ db_پیغام_remove_from_queued_stats(بنية mosquitto_پیغام_معلومات *پیغام_معلومات, بنية mosquitto_client_پیغام *client_پیغام)
{
	پیغام_معلومات->queued_شمېر--;
	پیغام_معلومات->queued_بایټونه -= client_پیغام->بنسټ_پیغام->معلومات.payloadlen;
	إذا(client_پیغام->معلومات.qos != 0){
		پیغام_معلومات->queued_count12--;
		پیغام_معلومات->queued_bytes12 -= client_پیغام->بنسټ_پیغام->معلومات.payloadlen;
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
#که_تعریف_شوی WITH_BRIDGE
	db.bridges = عدم;
	db.bridge_شمېر = 0;
#د_که_پای

	/* Initialize the hashtable */
	db.clientid_شاخص_hash = عدم;

	db.نورمال_subs = عدم;
	db.shared_subs = عدم;

	sub__init();
	retain__init();

	db.config->security_options.unpwd = عدم;

#که_تعریف_شوی WITH_PERSISTENCE
	إذا(persist__restore()){
		أرجع 1;
	}
#د_که_پای

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن فراغ subhier_clean(بنية mosquitto__subhier **subhier)
{
	بنية mosquitto__subhier *peer, *subhier_tmp;
	بنية mosquitto__subleaf *ونې_پاڼه, *nextleaf;

	HASH_ITER(hh, *subhier, peer, subhier_tmp){
		ونې_پاڼه = peer->subs;
		طالما(ونې_پاڼه){
			nextleaf = ونې_پاڼه->بل;
			mosquitto_FREE(ونې_پاڼه);
			ونې_پاڼه = nextleaf;
		}
		subhier_clean(&peer->children);

		HASH_DELETE(hh, *subhier, peer);
		mosquitto_FREE(peer);
	}
}


صحيح db__close(فراغ)
{
	subhier_clean(&db.نورمال_subs);
	subhier_clean(&db.shared_subs);
	retain__clean(&db.retains);
	db_پیغام_store_clean();

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db_پیغام_store_جمع_کړه(بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام)
{
	بنية mosquitto_بنسټ_پیغام *وموندل;
	بینښه hashv;

	HASH_ارزښت(&بنسټ_پیغام->معلومات.store_id, حجم(بنسټ_پیغام->معلومات.store_id), hashv);
	HASH_پیدا_کړه_BYHASHVALUE(hh, db.پیغام_store, &بنسټ_پیغام->معلومات.store_id, حجم(بنسټ_پیغام->معلومات.store_id), hashv, وموندل);
	إذا(وموندل == عدم){
		HASH_جمع_کړه_KEYPTR_BYHASHVALUE(hh, db.پیغام_store, &بنسټ_پیغام->معلومات.store_id, حجم(بنسټ_پیغام->معلومات.store_id), hashv, بنسټ_پیغام);
		أرجع MOSQ_ERR_SUCCESS;
	}وإلا{
		أرجع MOSQ_ERR_ALREADY_EXISTS;
	}
}


فراغ db_پیغام_store_free(بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام)
{
	mosquitto_FREE(بنسټ_پیغام->معلومات.سرچینه_id);
	mosquitto_FREE(بنسټ_پیغام->معلومات.سرچینه_username);
	إذا(بنسټ_پیغام->dest_ids){
		لكل(صحيح i=0; i<بنسټ_پیغام->dest_id_شمېر; i++){
			mosquitto_FREE(بنسټ_پیغام->dest_ids[i]);
		}
		mosquitto_FREE(بنسټ_پیغام->dest_ids);
	}
	mosquitto_FREE(بنسټ_پیغام->معلومات.topic);
	mosquitto_property_free_all(&بنسټ_پیغام->معلومات.properties);
	mosquitto_FREE(بنسټ_پیغام->معلومات.payload);
	mosquitto_FREE(بنسټ_پیغام);
}


فراغ db_پیغام_store_remove(بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام, bool notify)
{
	إذا(بنسټ_پیغام == عدم){
		أرجع;
	}
	HASH_DELETE(hh, db.پیغام_store, بنسټ_پیغام);
	db.پیغام_store_شمېر--;
	db.پیغام_store_بایټونه -= بنسټ_پیغام->معلومات.payloadlen;
	إذا(notify == true){
		plugin_persist_handle_بنسټ_پیغام_delete(بنسټ_پیغام);
	}
	db_پیغام_store_free(بنسټ_پیغام);
}


فراغ db_پیغام_store_clean(فراغ)
{
	بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام, *بنسټ_پیغام_tmp;

	HASH_ITER(hh, db.پیغام_store, بنسټ_پیغام, بنسټ_پیغام_tmp){
		db_پیغام_store_remove(بنسټ_پیغام, false);
	}
}


فراغ db_پیغام_store_ref_inc(بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام)
{
	بنسټ_پیغام->ref_شمېر++;
}


فراغ db_پیغام_store_ref_dec(بنية mosquitto_بنسټ_پیغام **بنسټ_پیغام)
{
	(*بنسټ_پیغام)->ref_شمېر--;
	إذا((*بنسټ_پیغام)->ref_شمېر == 0){
		db_پیغام_store_remove(*بنسټ_پیغام, true);
		*بنسټ_پیغام = عدم;
	}
}


فراغ db_پیغام_store_compact(فراغ)
{
	بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام, *بنسټ_پیغام_tmp;

	HASH_ITER(hh, db.پیغام_store, بنسټ_پیغام, بنسټ_پیغام_tmp){
		إذا(بنسټ_پیغام->ref_شمېر < 1){
			db_پیغام_store_remove(بنسټ_پیغام, true);
		}
	}
}


ساكن فراغ db_پیغام_remove_inflight(بنية mosquitto *context, بنية mosquitto_پیغام_معلومات *پیغام_معلومات, بنية mosquitto_client_پیغام *توکی)
{
	إذا(!context || !پیغام_معلومات || !توکی){
		أرجع;
	}

	plugin_persist_handle_client_پیغام_delete(context, توکی);

	DL_DELETE(پیغام_معلومات->inflight, توکی);
	إذا(توکی->بنسټ_پیغام){
		db_پیغام_remove_from_inflight_stats(پیغام_معلومات, توکی);
		db_پیغام_store_ref_dec(&توکی->بنسټ_پیغام);
	}

	mosquitto_FREE(توکی);
}


ساكن فراغ db_پیغام_remove_queued(بنية mosquitto *context, بنية mosquitto_پیغام_معلومات *پیغام_معلومات, بنية mosquitto_client_پیغام *توکی)
{
	إذا(!context || !پیغام_معلومات || !توکی){
		أرجع;
	}

	plugin_persist_handle_client_پیغام_delete(context, توکی);

	DL_DELETE(پیغام_معلومات->queued, توکی);
	إذا(توکی->بنسټ_پیغام){
		db_پیغام_remove_from_queued_stats(پیغام_معلومات, توکی);
		db_پیغام_store_ref_dec(&توکی->بنسټ_پیغام);
	}

	mosquitto_FREE(توکی);
}


ساكن فراغ db_fill_inflight_بهر_from_کتار(بنية mosquitto *context)
{
	بنية mosquitto_client_پیغام *client_پیغام, *tmp;

	DL_FOREACH_خوندي(context->msgs_بهر.queued, client_پیغام, tmp){
		إذا(!db__ready_for_flight(context, mosq_md_بهر, client_پیغام->معلومات.qos)){
			أرجع;
		}
		بدل(client_پیغام->معلومات.qos){
			حالة 0:
				client_پیغام->معلومات.state = mosq_ms_publish_qos0;
				اكسر;
			حالة 1:
				client_پیغام->معلومات.state = mosq_ms_publish_qos1;
				اكسر;
			حالة 2:
				client_پیغام->معلومات.state = mosq_ms_publish_qos2;
				اكسر;
		}
		إذا(client_پیغام->بنسټ_پیغام->معلومات.expiry_وخت && db.now_real_s > client_پیغام->بنسټ_پیغام->معلومات.expiry_وخت){
			db_پیغام_remove_queued(context, &context->msgs_بهر, client_پیغام);
			استمر;
		}
		plugin_persist_handle_client_پیغام_update(context, client_پیغام);
		db_پیغام_له_کتار_ایستل_لومړی(context, &context->msgs_بهر);
	}
}


فراغ db_پیغام_له_کتار_ایستل_لومړی(بنية mosquitto *context, بنية mosquitto_پیغام_معلومات *پیغام_معلومات)
{
	بنية mosquitto_client_پیغام *client_پیغام;

	ناکارول_شوی(context);

	client_پیغام = پیغام_معلومات->queued;
	DL_DELETE(پیغام_معلومات->queued, client_پیغام);
	DL_APPEND(پیغام_معلومات->inflight, client_پیغام);
	إذا(پیغام_معلومات->inflight_quota > 0){
		پیغام_معلومات->inflight_quota--;
	}

	db_پیغام_remove_from_queued_stats(پیغام_معلومات, client_پیغام);
	db_پیغام_جمع_کړه_منزل_inflight_stats(پیغام_معلومات, client_پیغام);
}


صحيح db_پیغام_delete_outgoing(بنية mosquitto *context, uint16_t mid, تعداد mosquitto_پیغام_state expect_state, صحيح qos)
{
	بنية mosquitto_client_پیغام *client_پیغام, *tmp;
	bool deleted = false;

	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	DL_FOREACH_خوندي(context->msgs_بهر.inflight, client_پیغام, tmp){
		إذا(client_پیغام->معلومات.mid == mid){
			إذا(client_پیغام->معلومات.qos != qos){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when deleting outgoing message.",
						context->id, client_پیغام->معلومات.qos, qos);
				أرجع MOSQ_ERR_PROTOCOL;
			}وإلا إذا(qos == 2 && client_پیغام->معلومات.state != expect_state && expect_state != mosq_ms_any){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched state (%d:%d) when deleting outgoing message.",
						context->id, client_پیغام->معلومات.state, expect_state);
				أرجع MOSQ_ERR_PROTOCOL;
			}
			db_پیغام_remove_inflight(context, &context->msgs_بهر, client_پیغام);
			deleted = true;
			اكسر;
		}
	}

	إذا(deleted == false){
		DL_FOREACH_خوندي(context->msgs_بهر.queued, client_پیغام, tmp){
			إذا(client_پیغام->معلومات.mid == mid){
				إذا(client_پیغام->معلومات.qos != qos){
					أرجع MOSQ_ERR_PROTOCOL;
				}وإلا إذا(qos == 2 && client_پیغام->معلومات.state != expect_state && expect_state != mosq_ms_any){
					أرجع MOSQ_ERR_PROTOCOL;
				}
				db_پیغام_remove_queued(context, &context->msgs_بهر, client_پیغام);
				اكسر;
			}
		}
	}
	db_fill_inflight_بهر_from_کتار(context);
#که_تعریف_شوی WITH_PERSISTENCE
	db.persistence_changes++;
#د_که_پای

	أرجع db_پیغام_write_inflight_بهر_latest(context);
}


/* Only for QoS 2 messages */
صحيح db_پیغام_ورننباسه_incoming(بنية mosquitto *context, uint64_t cmsg_id, بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام, bool persist)
{
	بنية mosquitto_client_پیغام *client_پیغام;
	بنية mosquitto_پیغام_معلومات *پیغام_معلومات;
	تعداد mosquitto_پیغام_state state = mosq_ms_invalid;
	صحيح rc = 0;

	أكد(بنسټ_پیغام);
	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		أرجع MOSQ_ERR_SUCCESS;

	}
	پیغام_معلومات = &context->msgs_دننه;

	إذا(db__ready_for_flight(context, mosq_md_دننه, بنسټ_پیغام->معلومات.qos)){
		state = mosq_ms_wait_for_pubrel;
	}وإلا إذا(بنسټ_پیغام->معلومات.qos != 0 && db_ready_for_کتار(context, بنسټ_پیغام->معلومات.qos, پیغام_معلومات)){
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

#که_تعریف_شوی WITH_PERSISTENCE
	إذا(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#د_که_پای

	client_پیغام = mosquitto_malloc(حجم(بنية mosquitto_client_پیغام));
	إذا(!client_پیغام){
		أرجع MOSQ_ERR_NOMEM;
	}
	client_پیغام->prev = عدم;
	client_پیغام->بل = عدم;
	إذا(cmsg_id){
		client_پیغام->معلومات.cmsg_id = cmsg_id;
	}وإلا{
		client_پیغام->معلومات.cmsg_id = ++context->وروستی_cmsg_id;
	}
	client_پیغام->بنسټ_پیغام = بنسټ_پیغام;
	db_پیغام_store_ref_inc(client_پیغام->بنسټ_پیغام);
	client_پیغام->معلومات.mid = بنسټ_پیغام->معلومات.سرچینه_mid;
	client_پیغام->معلومات.direction = mosq_md_دننه;
	client_پیغام->معلومات.state = (uint8_t)state;
	client_پیغام->معلومات.dup = false;
	إذا(بنسټ_پیغام->معلومات.qos > context->max_qos){
		client_پیغام->معلومات.qos = context->max_qos;
	}وإلا{
		client_پیغام->معلومات.qos = بنسټ_پیغام->معلومات.qos;
	}
	client_پیغام->معلومات.retain = بنسټ_پیغام->معلومات.retain;
	client_پیغام->معلومات.subscription_identifier = 0;

	إذا(state == mosq_ms_queued){
		DL_APPEND(پیغام_معلومات->queued, client_پیغام);
		db_پیغام_جمع_کړه_منزل_queued_stats(پیغام_معلومات, client_پیغام);
	}وإلا{
		DL_APPEND(پیغام_معلومات->inflight, client_پیغام);
		db_پیغام_جمع_کړه_منزل_inflight_stats(پیغام_معلومات, client_پیغام);
	}

	إذا(persist && context->is_persisted){
		plugin_persist_handle_بنسټ_پیغام_جمع_کړه(client_پیغام->بنسټ_پیغام);
		plugin_persist_handle_client_پیغام_جمع_کړه(context, client_پیغام);
	}

	إذا(client_پیغام->بنسټ_پیغام->معلومات.qos > 0){
		util__decrement_receive_quota(context);
	}
	أرجع rc;
}


صحيح db_پیغام_ورننباسه_outgoing(بنية mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام, uint32_t subscription_identifier, bool update, bool persist)
{
	بنية mosquitto_client_پیغام *client_پیغام;
	بنية mosquitto_پیغام_معلومات *پیغام_معلومات;
	تعداد mosquitto_پیغام_state state = mosq_ms_invalid;
	صحيح rc = 0;
	حرف **dest_ids;

	أكد(بنسټ_پیغام);
	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		أرجع MOSQ_ERR_SUCCESS;

	}
	context->stats.messages_لېږل_شوی++;

	پیغام_معلومات = &context->msgs_بهر;

	/* Check whether we've already sent this message to this client
	 * for outgoing messages only.
	 * If retain==true then this is a stale retained message and so should be
	 * sent regardless. FIXME - this does mean retained messages will received
	 * multiple times for overlapping subscriptions, although this is only the
	 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
	 */
	إذا(context->protocol != mosq_p_mqtt5
			&& db.config->allow_duplicate_messages == false
			&& retain == false && بنسټ_پیغام->dest_ids){

		لكل(صحيح i=0; i<بنسټ_پیغام->dest_id_شمېر; i++){
			إذا(بنسټ_پیغام->dest_ids[i] && !strcmp(بنسټ_پیغام->dest_ids[i], context->id)){
				/* We have already sent this message to this client. */
				أرجع MOSQ_ERR_SUCCESS;
			}
		}
	}
	إذا(!net__is_connected(context)){
		/* Client is not connected only queue messages with QoS>0. */
		إذا(qos == 0 && !db.config->کتار_qos0_messages){
			إذا(!context->bridge){
				أرجع 2;
			}وإلا{
				إذا(context->bridge->پیل_type != bst_lazy){
					أرجع 2;
				}
			}
		}
		إذا(context->bridge && context->bridge->clean_پیل_local == true){
			أرجع 2;
		}
	}

	إذا(net__is_connected(context)){
		إذا(db__ready_for_flight(context, mosq_md_بهر, qos)){
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
		}وإلا إذا(qos != 0 && db_ready_for_کتار(context, qos, پیغام_معلومات)){
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
		إذا(db_ready_for_کتار(context, qos, پیغام_معلومات)){
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

#که_تعریف_شوی WITH_PERSISTENCE
	إذا(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#د_که_پای

	client_پیغام = mosquitto_malloc(حجم(بنية mosquitto_client_پیغام));
	إذا(!client_پیغام){
		أرجع MOSQ_ERR_NOMEM;
	}
	client_پیغام->prev = عدم;
	client_پیغام->بل = عدم;
	إذا(cmsg_id){
		client_پیغام->معلومات.cmsg_id = cmsg_id;
	}وإلا{
		client_پیغام->معلومات.cmsg_id = ++context->وروستی_cmsg_id;
	}
	client_پیغام->بنسټ_پیغام = بنسټ_پیغام;
	db_پیغام_store_ref_inc(client_پیغام->بنسټ_پیغام);
	client_پیغام->معلومات.mid = mid;
	client_پیغام->معلومات.direction = mosq_md_بهر;
	client_پیغام->معلومات.state = (uint8_t)state;
	client_پیغام->معلومات.dup = false;
	إذا(qos > context->max_qos){
		client_پیغام->معلومات.qos = context->max_qos;
	}وإلا{
		client_پیغام->معلومات.qos = qos;
	}
	client_پیغام->معلومات.retain = retain;
	client_پیغام->معلومات.subscription_identifier = subscription_identifier;

	إذا(state == mosq_ms_queued){
		DL_APPEND(پیغام_معلومات->queued, client_پیغام);
		db_پیغام_جمع_کړه_منزل_queued_stats(پیغام_معلومات, client_پیغام);
	}وإلا{
		DL_APPEND(پیغام_معلومات->inflight, client_پیغام);
		db_پیغام_جمع_کړه_منزل_inflight_stats(پیغام_معلومات, client_پیغام);
	}

	إذا(persist && context->is_persisted){
		plugin_persist_handle_بنسټ_پیغام_جمع_کړه(client_پیغام->بنسټ_پیغام);
		plugin_persist_handle_client_پیغام_جمع_کړه(context, client_پیغام);
	}

	إذا(db.config->allow_duplicate_messages == false && retain == false){
		/* Record which client ids this message has been sent to so we can avoid duplicates.
		 * Outgoing messages only.
		 * If retain==true then this is a stale retained message and so should be
		 * sent regardless. FIXME - this does mean retained messages will received
		 * multiple times for overlapping subscriptions, although this is only the
		 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
		 */
		dest_ids = mosquitto_realloc(بنسټ_پیغام->dest_ids, حجم(حرف *)*(size_t)(بنسټ_پیغام->dest_id_شمېر+1));
		إذا(dest_ids){
			بنسټ_پیغام->dest_ids = dest_ids;
			بنسټ_پیغام->dest_id_شمېر++;
			بنسټ_پیغام->dest_ids[بنسټ_پیغام->dest_id_شمېر-1] = mosquitto_strdup(context->id);
			إذا(!بنسټ_پیغام->dest_ids[بنسټ_پیغام->dest_id_شمېر-1]){
				أرجع MOSQ_ERR_NOMEM;
			}
		}وإلا{
			أرجع MOSQ_ERR_NOMEM;
		}
	}
#که_تعریف_شوی WITH_BRIDGE
	إذا(context->bridge && context->bridge->پیل_type == bst_lazy
			&& !net__is_connected(context)
			&& context->msgs_بهر.inflight_شمېر + context->msgs_بهر.queued_شمېر >= context->bridge->پوله){

		context->bridge->lazy_reconnect = true;
	}
#د_که_پای

	إذا(client_پیغام->معلومات.qos > 0 && state != mosq_ms_queued){
		util__decrement_send_quota(context);
	}

	إذا(update){
		rc = db_پیغام_write_inflight_بهر_latest(context);
		إذا(rc){
			أرجع rc;
		}
		rc = db_پیغام_write_queued_بهر(context);
		إذا(rc){
			أرجع rc;
		}
	}

	أرجع rc;
}


ساكن مضمن صحيح db_پیغام_update_outgoing_state(بنية mosquitto *context, بنية mosquitto_client_پیغام *سر,
		uint16_t mid, تعداد mosquitto_پیغام_state state, صحيح qos, bool persist)
{
	بنية mosquitto_client_پیغام *client_پیغام;

	DL_FOREACH(سر, client_پیغام){
		إذا(client_پیغام->معلومات.mid == mid){
			إذا(client_پیغام->معلومات.qos != qos){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when updating outgoing message.",
						context->id, client_پیغام->معلومات.qos, qos);
				أرجع MOSQ_ERR_PROTOCOL;
			}
			client_پیغام->معلومات.state = (uint8_t)state;
			إذا(persist){
				plugin_persist_handle_client_پیغام_update(context, client_پیغام);
			}
			أرجع MOSQ_ERR_SUCCESS;
		}
	}
	أرجع MOSQ_ERR_NOT_وموندل;
}


صحيح db_پیغام_update_outgoing(بنية mosquitto *context, uint16_t mid, تعداد mosquitto_پیغام_state state, صحيح qos, bool persist)
{
	صحيح rc;

	rc = db_پیغام_update_outgoing_state(context, context->msgs_بهر.inflight, mid, state, qos, persist);
	إذا(!persist && rc == MOSQ_ERR_NOT_وموندل){
		rc = db_پیغام_update_outgoing_state(context, context->msgs_بهر.queued, mid, state, qos, persist);
	}
	أرجع rc;
}


ساكن فراغ db__messages_delete_list(بنية mosquitto_client_پیغام **سر)
{
	بنية mosquitto_client_پیغام *client_پیغام, *tmp;

	DL_FOREACH_خوندي(*سر, client_پیغام, tmp){
		DL_DELETE(*سر, client_پیغام);
		db_پیغام_store_ref_dec(&client_پیغام->بنسټ_پیغام);
		mosquitto_FREE(client_پیغام);
	}
	*سر = عدم;
}


صحيح db__messages_delete_incoming(بنية mosquitto *context)
{
	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_دننه.inflight);
	db__messages_delete_list(&context->msgs_دننه.queued);
	context->msgs_دننه.inflight_بایټونه = 0;
	context->msgs_دننه.inflight_bytes12 = 0;
	context->msgs_دننه.inflight_شمېر = 0;
	context->msgs_دننه.inflight_count12 = 0;
	context->msgs_دننه.queued_بایټونه = 0;
	context->msgs_دننه.queued_bytes12 = 0;
	context->msgs_دننه.queued_شمېر = 0;
	context->msgs_دننه.queued_count12 = 0;

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db__messages_delete_outgoing(بنية mosquitto *context)
{
	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_بهر.inflight);
	db__messages_delete_list(&context->msgs_بهر.queued);
	context->msgs_بهر.inflight_بایټونه = 0;
	context->msgs_بهر.inflight_bytes12 = 0;
	context->msgs_بهر.inflight_شمېر = 0;
	context->msgs_بهر.inflight_count12 = 0;
	context->msgs_بهر.queued_بایټونه = 0;
	context->msgs_بهر.queued_bytes12 = 0;
	context->msgs_بهر.queued_شمېر = 0;
	context->msgs_بهر.queued_count12 = 0;

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db__messages_delete(بنية mosquitto *context, bool force_free)
{
	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	إذا(force_free || context->clean_پیل || (context->bridge && context->bridge->clean_پیل)){
		db__messages_delete_incoming(context);
	}

	إذا(force_free || (context->bridge && context->bridge->clean_پیل_local)
			|| (context->bridge == عدم && context->clean_پیل)){

		db__messages_delete_outgoing(context);
	}

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db_messages_easy_کتار(بنية mosquitto *context, ثابت حرف *topic, uint8_t qos, uint32_t payloadlen, ثابت فراغ *payload, صحيح retain, uint32_t پیغام_expiry_interval, mosquitto_property **properties)
{
	بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام;
	ثابت حرف *سرچینه_id;
	تعداد mosquitto_پیغام_origin origin;

	إذا(!topic){
		أرجع MOSQ_ERR_INVAL;
	}

	بنسټ_پیغام = mosquitto_calloc(1, حجم(بنية mosquitto_بنسټ_پیغام));
	إذا(بنسټ_پیغام == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}

	بنسټ_پیغام->معلومات.topic = mosquitto_strdup(topic);
	إذا(بنسټ_پیغام->معلومات.topic == عدم){
		db_پیغام_store_free(بنسټ_پیغام);
		أرجع MOSQ_ERR_INVAL;
	}

	بنسټ_پیغام->معلومات.qos = qos;
	إذا(db.config->retain_شته == false){
		بنسټ_پیغام->معلومات.retain = 0;
	}وإلا{
		بنسټ_پیغام->معلومات.retain = retain;
	}

	بنسټ_پیغام->معلومات.payloadlen = payloadlen;
	إذا(payloadlen > 0){
		بنسټ_پیغام->معلومات.payload = mosquitto_malloc(بنسټ_پیغام->معلومات.payloadlen+1);
		إذا(بنسټ_پیغام->معلومات.payload == عدم){
			db_پیغام_store_free(بنسټ_پیغام);
			أرجع MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)بنسټ_پیغام->معلومات.payload)[بنسټ_پیغام->معلومات.payloadlen] = 0;
		حافظه_نقل(بنسټ_پیغام->معلومات.payload, payload, بنسټ_پیغام->معلومات.payloadlen);
	}

	إذا(context && context->id){
		سرچینه_id = context->id;
	}وإلا{
		سرچینه_id = "";
	}
	إذا(properties){
		بنسټ_پیغام->معلومات.properties = *properties;
		*properties = عدم;
	}

	إذا(context){
		origin = mosq_mo_client;
	}وإلا{
		origin = mosq_mo_broker;
	}
	إذا(db_پیغام_store(context, بنسټ_پیغام, &پیغام_expiry_interval, origin)){
		أرجع 1;
	}

	أرجع sub_messages_کتار(سرچینه_id, بنسټ_پیغام->معلومات.qos, &بنسټ_پیغام);
}


#تعریف_کړه MOSQ_UUID_EPOCH 1637168273


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
uint64_t db_new_پیغام_id(فراغ)
{
#که_تعریف_شوی WIN32
	FILETIME ftime;
	uint64_t ftime64;
#وإلا
	بنية timespec ts;
#د_که_پای
	uint64_t id;
	uint64_t tmp;
	time_t sec;
	اوږد nsec;

	id = db.غوټه_id_shifted; /* Top 10-bits */

#که_تعریف_شوی WIN32
	Get_System_وخت_Precise_As_File_وخت(&ftime);
	ftime64 = (((uint64_t)ftime.dw_لوړ_Date_وخت)<<32) + ftime.dw_ټیټ_Date_وخت;
	tmp = ftime64 - 116444736000000000LL; /* Convert offset to unix epoch, still in counts of 100ns */
	sec = tmp / 10000000; /* Convert to seconds */
	nsec = (اوږد)(tmp - sec)*100; /* Remove seconds, convert to counts of 1ns */
#وإلا
	clock_gettime(CLOCK_REALTIME, &ts);
	sec = ts.tv_sec;
	nsec = ts.tv_nsec;
#د_که_پای
	tmp = (sec - MOSQ_UUID_EPOCH) & 0x7FFFFFFF;
	id = id | (tmp << 23); /* Seconds, 31-bits (68 years) */

	tmp = (nsec & 0x7FFFFF80); /* top 23-bits of the bottom 30 bits (1 billion ns), ~100 ns resolution */
	id = id | (tmp >> 7);

	إذا(id <= db.وروستی_db_id){
		id = db.وروستی_db_id + 1;
	}
	db.وروستی_db_id = id;

	أرجع id;
}


/* This function requires topic to be allocated on the heap. Once called, it owns topic and will free it on error. Likewise payload and properties. */
صحيح db_پیغام_store(ثابت بنية mosquitto *سرچینه, بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام, uint32_t *پیغام_expiry_interval, تعداد mosquitto_پیغام_origin origin)
{
	صحيح rc;

	أكد(بنسټ_پیغام);

	إذا(سرچینه && سرچینه->id){
		بنسټ_پیغام->معلومات.سرچینه_id = mosquitto_strdup(سرچینه->id);
	}وإلا{
		بنسټ_پیغام->معلومات.سرچینه_id = mosquitto_strdup("");
	}
	إذا(!بنسټ_پیغام->معلومات.سرچینه_id){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		db_پیغام_store_free(بنسټ_پیغام);
		أرجع MOSQ_ERR_NOMEM;
	}

	إذا(سرچینه && سرچینه->username){
		بنسټ_پیغام->معلومات.سرچینه_username = mosquitto_strdup(سرچینه->username);
		إذا(!بنسټ_پیغام->معلومات.سرچینه_username){
			db_پیغام_store_free(بنسټ_پیغام);
			أرجع MOSQ_ERR_NOMEM;
		}
	}
	إذا(سرچینه){
		بنسټ_پیغام->سرچینه_listener = سرچینه->listener;
	}
	بنسټ_پیغام->origin = origin;
	إذا(پیغام_expiry_interval){
		إذا(*پیغام_expiry_interval > 0 && *پیغام_expiry_interval != پیغام_EXPIRY_INFINITE){
			بنسټ_پیغام->معلومات.expiry_وخت = db.now_real_s + *پیغام_expiry_interval;
		}وإلا{
			بنسټ_پیغام->معلومات.expiry_وخت = 0;
		}
	}

	بنسټ_پیغام->dest_ids = عدم;
	بنسټ_پیغام->dest_id_شمېر = 0;
	db.پیغام_store_شمېر++;
	db.پیغام_store_بایټونه += بنسټ_پیغام->معلومات.payloadlen;

	إذا(!بنسټ_پیغام->معلومات.store_id){
		بنسټ_پیغام->معلومات.store_id = db_new_پیغام_id();
	}

	rc = db_پیغام_store_جمع_کړه(بنسټ_پیغام);
	إذا(rc){
		db_پیغام_store_free(بنسټ_پیغام);
		أرجع rc;
	}

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db_پیغام_store_پیدا_کړه(بنية mosquitto *context, uint16_t mid, بنية mosquitto_client_پیغام **client_پیغام)
{
	بنية mosquitto_client_پیغام *cmsg;

	*client_پیغام = عدم;

	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	DL_FOREACH(context->msgs_دننه.inflight, cmsg){
		إذا(cmsg->بنسټ_پیغام->معلومات.سرچینه_mid == mid){
			*client_پیغام = cmsg;
			أرجع MOSQ_ERR_SUCCESS;
		}
	}

	DL_FOREACH(context->msgs_دننه.queued, cmsg){
		إذا(cmsg->بنسټ_پیغام->معلومات.سرچینه_mid == mid){
			*client_پیغام = cmsg;
			أرجع MOSQ_ERR_SUCCESS;
		}
	}

	أرجع 1;
}


/* Called on reconnect to set outgoing messages to a sensible state and force a
 * retry, and to set incoming messages to expect an appropriate retry. */
ساكن صحيح db_پیغام_reconnect_reset_outgoing(بنية mosquitto *context)
{
	بنية mosquitto_client_پیغام *client_پیغام, *tmp;

	context->msgs_بهر.inflight_بایټونه = 0;
	context->msgs_بهر.inflight_bytes12 = 0;
	context->msgs_بهر.inflight_شمېر = 0;
	context->msgs_بهر.inflight_count12 = 0;
	context->msgs_بهر.queued_بایټونه = 0;
	context->msgs_بهر.queued_bytes12 = 0;
	context->msgs_بهر.queued_شمېر = 0;
	context->msgs_بهر.queued_count12 = 0;
	context->msgs_بهر.inflight_quota = context->msgs_بهر.inflight_اعظمي;

	DL_FOREACH_خوندي(context->msgs_بهر.inflight, client_پیغام, tmp){
		db_پیغام_جمع_کړه_منزل_inflight_stats(&context->msgs_بهر, client_پیغام);
		إذا(client_پیغام->معلومات.qos > 0){
			util__decrement_send_quota(context);
		}

		بدل(client_پیغام->معلومات.qos){
			حالة 0:
				client_پیغام->معلومات.state = mosq_ms_publish_qos0;
				اكسر;
			حالة 1:
				client_پیغام->معلومات.state = mosq_ms_publish_qos1;
				اكسر;
			حالة 2:
				إذا(client_پیغام->معلومات.state == mosq_ms_wait_for_pubcomp){
					client_پیغام->معلومات.state = mosq_ms_resend_pubrel;
				}وإلا{
					client_پیغام->معلومات.state = mosq_ms_publish_qos2;
				}
				اكسر;
		}
		plugin_persist_handle_client_پیغام_update(context, client_پیغام);
	}
	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_خوندي(context->msgs_بهر.queued, client_پیغام, tmp){
		db_پیغام_جمع_کړه_منزل_queued_stats(&context->msgs_بهر, client_پیغام);
	}
	db_fill_inflight_بهر_from_کتار(context);

	أرجع MOSQ_ERR_SUCCESS;
}


/* Called on reconnect to set incoming messages to expect an appropriate retry. */
ساكن صحيح db_پیغام_reconnect_reset_incoming(بنية mosquitto *context)
{
	بنية mosquitto_client_پیغام *client_پیغام, *tmp;

	context->msgs_دننه.inflight_بایټونه = 0;
	context->msgs_دننه.inflight_bytes12 = 0;
	context->msgs_دننه.inflight_شمېر = 0;
	context->msgs_دننه.inflight_count12 = 0;
	context->msgs_دننه.queued_بایټونه = 0;
	context->msgs_دننه.queued_bytes12 = 0;
	context->msgs_دننه.queued_شمېر = 0;
	context->msgs_دننه.queued_count12 = 0;
	context->msgs_دننه.inflight_quota = context->msgs_دننه.inflight_اعظمي;

	DL_FOREACH_خوندي(context->msgs_دننه.inflight, client_پیغام, tmp){
		db_پیغام_جمع_کړه_منزل_inflight_stats(&context->msgs_دننه, client_پیغام);
		إذا(client_پیغام->معلومات.qos > 0){
			util__decrement_receive_quota(context);
		}

		إذا(client_پیغام->معلومات.qos != 2){
			/* Anything <QoS 2 can be completely retried by the client at
			 * no harm. */
			db_پیغام_remove_inflight(context, &context->msgs_دننه, client_پیغام);
		}وإلا{
			/* Message state can be preserved here because it should match
			 * whatever the client has got. */
			client_پیغام->معلومات.dup = 0;
		}
	}

	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_خوندي(context->msgs_دننه.queued, client_پیغام, tmp){
		client_پیغام->معلومات.dup = 0;
		db_پیغام_جمع_کړه_منزل_queued_stats(&context->msgs_دننه, client_پیغام);
		إذا(db__ready_for_flight(context, mosq_md_دننه, client_پیغام->معلومات.qos)){
			بدل(client_پیغام->معلومات.qos){
				حالة 0:
					client_پیغام->معلومات.state = mosq_ms_publish_qos0;
					اكسر;
				حالة 1:
					client_پیغام->معلومات.state = mosq_ms_publish_qos1;
					اكسر;
				حالة 2:
					client_پیغام->معلومات.state = mosq_ms_publish_qos2;
					اكسر;
			}
			db_پیغام_له_کتار_ایستل_لومړی(context, &context->msgs_دننه);
			plugin_persist_handle_client_پیغام_update(context, client_پیغام);
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db_پیغام_reconnect_reset(بنية mosquitto *context)
{
	صحيح rc;

	rc = db_پیغام_reconnect_reset_outgoing(context);
	إذا(rc){
		أرجع rc;
	}
	أرجع db_پیغام_reconnect_reset_incoming(context);
}


صحيح db_پیغام_remove_incoming(بنية mosquitto *context, uint16_t mid)
{
	بنية mosquitto_client_پیغام *client_پیغام, *tmp;

	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	DL_FOREACH_خوندي(context->msgs_دننه.inflight, client_پیغام, tmp){
		إذا(client_پیغام->معلومات.mid == mid){
			إذا(client_پیغام->بنسټ_پیغام->معلومات.qos != 2){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when deleting incoming message.",
						context->id, client_پیغام->بنسټ_پیغام->معلومات.qos);
				أرجع MOSQ_ERR_PROTOCOL;
			}
			db_پیغام_remove_inflight(context, &context->msgs_دننه, client_پیغام);
			أرجع MOSQ_ERR_SUCCESS;
		}
	}

	أرجع MOSQ_ERR_NOT_وموندل;
}


صحيح db_پیغام_release_incoming(بنية mosquitto *context, uint16_t mid)
{
	بنية mosquitto_client_پیغام *client_پیغام, *tmp;
	bool deleted = false;
	صحيح rc;

	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	DL_FOREACH_خوندي(context->msgs_دننه.inflight, client_پیغام, tmp){
		إذا(client_پیغام->معلومات.mid == mid){
			إذا(client_پیغام->بنسټ_پیغام->معلومات.qos != 2){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when releasing incoming message.",
						context->id, client_پیغام->بنسټ_پیغام->معلومات.qos);
				أرجع MOSQ_ERR_PROTOCOL;
			}
			ثابت حرف *topic = client_پیغام->بنسټ_پیغام->معلومات.topic;
			ثابت حرف *سرچینه_id = client_پیغام->بنسټ_پیغام->معلومات.سرچینه_id;

			/* topic==NULL should be a QoS 2 message that was
			 * denied/dropped and is being processed so the client doesn't
			 * keep resending it. That means we don't send it to other
			 * clients. */
			إذا(topic == عدم){
				db_پیغام_remove_inflight(context, &context->msgs_دننه, client_پیغام);
				deleted = true;
			}وإلا{
				rc = sub_messages_کتار(سرچینه_id, 2, &client_پیغام->بنسټ_پیغام);
				إذا(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_NO_SUBSCRIBERS){
					db_پیغام_remove_inflight(context, &context->msgs_دننه, client_پیغام);
					deleted = true;
				}وإلا{
					أرجع 1;
				}
			}
		}
	}

	DL_FOREACH_خوندي(context->msgs_دننه.queued, client_پیغام, tmp){
		إذا(db__ready_for_flight(context, mosq_md_دننه, client_پیغام->معلومات.qos)){
			اكسر;
		}

		إذا(client_پیغام->معلومات.qos == 2){
			send__pubrec(context, client_پیغام->معلومات.mid, 0, عدم);
			client_پیغام->معلومات.state = mosq_ms_wait_for_pubrel;
			db_پیغام_له_کتار_ایستل_لومړی(context, &context->msgs_دننه);
			plugin_persist_handle_client_پیغام_update(context, client_پیغام);
		}
	}
	إذا(deleted){
		أرجع MOSQ_ERR_SUCCESS;
	}وإلا{
		أرجع MOSQ_ERR_NOT_وموندل;
	}
}


فراغ db__expire_all_messages(بنية mosquitto *context)
{
	بنية mosquitto_client_پیغام *client_پیغام, *tmp;

	DL_FOREACH_خوندي(context->msgs_بهر.inflight, client_پیغام, tmp){
		إذا(client_پیغام->بنسټ_پیغام->معلومات.expiry_وخت && db.now_real_s > client_پیغام->بنسټ_پیغام->معلومات.expiry_وخت){
			إذا(client_پیغام->معلومات.qos > 0){
				util__increment_send_quota(context);
			}
			db_پیغام_remove_inflight(context, &context->msgs_بهر, client_پیغام);
		}
	}
	db_fill_inflight_بهر_from_کتار(context);
	DL_FOREACH_خوندي(context->msgs_بهر.queued, client_پیغام, tmp){
		إذا(client_پیغام->بنسټ_پیغام->معلومات.expiry_وخت && db.now_real_s > client_پیغام->بنسټ_پیغام->معلومات.expiry_وخت){
			db_پیغام_remove_queued(context, &context->msgs_بهر, client_پیغام);
		}
	}
	DL_FOREACH_خوندي(context->msgs_دننه.inflight, client_پیغام, tmp){
		إذا(client_پیغام->بنسټ_پیغام->معلومات.expiry_وخت && db.now_real_s > client_پیغام->بنسټ_پیغام->معلومات.expiry_وخت){
			إذا(client_پیغام->معلومات.qos > 0){
				util__increment_receive_quota(context);
			}
			db_پیغام_remove_inflight(context, &context->msgs_دننه, client_پیغام);
		}
	}
	DL_FOREACH_خوندي(context->msgs_دننه.queued, client_پیغام, tmp){
		إذا(client_پیغام->بنسټ_پیغام->معلومات.expiry_وخت && db.now_real_s > client_پیغام->بنسټ_پیغام->معلومات.expiry_وخت){
			db_پیغام_remove_queued(context, &context->msgs_دننه, client_پیغام);
		}
	}
}


ساكن فراغ db__client_messages_check_acl(بنية mosquitto *context, بنية mosquitto_پیغام_معلومات *پیغام_معلومات, بنية mosquitto_client_پیغام **سر,
		فراغ (*decrement_stats_fn)(بنية mosquitto_پیغام_معلومات *پیغام_معلومات, بنية mosquitto_client_پیغام *client_پیغام))
{
	بنية mosquitto_client_پیغام *client_پیغام, *tmp;
	بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام;
	صحيح access;

	DL_FOREACH_خوندي((*سر), client_پیغام, tmp){
		بنسټ_پیغام = client_پیغام->بنسټ_پیغام;
		إذا(client_پیغام->معلومات.direction == mosq_md_بهر){
			access = MOSQ_ACL_READ;
		}وإلا{
			access = MOSQ_ACL_WRITE;
		}
		إذا(mosquitto_acl_check(context, بنسټ_پیغام->معلومات.topic,
				بنسټ_پیغام->معلومات.payloadlen, بنسټ_پیغام->معلومات.payload,
				بنسټ_پیغام->معلومات.qos, بنسټ_پیغام->معلومات.retain,
				بنسټ_پیغام->معلومات.properties, access) != MOSQ_ERR_SUCCESS){

			DL_DELETE((*سر), client_پیغام);
			decrement_stats_fn(پیغام_معلومات, client_پیغام);
			plugin_persist_handle_client_پیغام_delete(context, client_پیغام);
			db_پیغام_store_ref_dec(&client_پیغام->بنسټ_پیغام);
			mosquitto_FREE(client_پیغام);
		}
	}
}


فراغ db_check_acl_د_all_messages(بنية mosquitto *context)
{
	db__client_messages_check_acl(context, &context->msgs_دننه, &context->msgs_دننه.inflight, &db_پیغام_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_دننه, &context->msgs_دننه.queued, &db_پیغام_remove_from_queued_stats);
	db__client_messages_check_acl(context, &context->msgs_بهر, &context->msgs_بهر.inflight, &db_پیغام_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_بهر, &context->msgs_بهر.queued, &db_پیغام_remove_from_queued_stats);
}


ساكن صحيح db_پیغام_write_inflight_بهر_single(بنية mosquitto *context, بنية mosquitto_client_پیغام *client_پیغام)
{
	بنية mosquitto_بنسټ_پیغام *بنسټ_پیغام;
	mosquitto_property *بنسټ_پیغام_props = عدم;
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

	بنسټ_پیغام = client_پیغام->بنسټ_پیغام;

	expiry_interval = 0;
	إذا(بنسټ_پیغام->معلومات.expiry_وخت){
		إذا(db.now_real_s > بنسټ_پیغام->معلومات.expiry_وخت){
			/* Message is expired, must not send. */
			إذا(client_پیغام->معلومات.direction == mosq_md_بهر && client_پیغام->معلومات.qos > 0){
				util__increment_send_quota(context);
			}
			db_پیغام_remove_inflight(context, &context->msgs_بهر, client_پیغام);
			db_fill_inflight_بهر_from_کتار(context);
			أرجع MOSQ_ERR_SUCCESS;
		}وإلا{
			expiry_interval = (uint32_t)(بنسټ_پیغام->معلومات.expiry_وخت - db.now_real_s);
		}
	}
	mid = client_پیغام->معلومات.mid;
	retries = client_پیغام->معلومات.dup;
	retain = client_پیغام->معلومات.retain;
	topic = بنسټ_پیغام->معلومات.topic;
	qos = (uint8_t)client_پیغام->معلومات.qos;
	payloadlen = بنسټ_پیغام->معلومات.payloadlen;
	payload = بنسټ_پیغام->معلومات.payload;
	subscription_id = client_پیغام->معلومات.subscription_identifier;
	بنسټ_پیغام_props = بنسټ_پیغام->معلومات.properties;

	بدل(client_پیغام->معلومات.state){
		حالة mosq_ms_publish_qos0:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, بنسټ_پیغام_props, expiry_interval);
			إذا(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_OVERSIZE_PACKET){
				db_پیغام_remove_inflight(context, &context->msgs_بهر, client_پیغام);
			}وإلا{
				أرجع rc;
			}
			اكسر;

		حالة mosq_ms_publish_qos1:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, بنسټ_پیغام_props, expiry_interval);
			إذا(rc == MOSQ_ERR_SUCCESS){
				client_پیغام->معلومات.dup = 1; /* Any retry attempts are a duplicate. */
				client_پیغام->معلومات.state = mosq_ms_wait_for_puback;
				plugin_persist_handle_client_پیغام_update(context, client_پیغام);
			}وإلا إذا(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db_پیغام_remove_inflight(context, &context->msgs_بهر, client_پیغام);
			}وإلا{
				أرجع rc;
			}
			اكسر;

		حالة mosq_ms_publish_qos2:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, بنسټ_پیغام_props, expiry_interval);
			إذا(rc == MOSQ_ERR_SUCCESS){
				client_پیغام->معلومات.dup = 1; /* Any retry attempts are a duplicate. */
				client_پیغام->معلومات.state = mosq_ms_wait_for_pubrec;
				plugin_persist_handle_client_پیغام_update(context, client_پیغام);
			}وإلا إذا(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db_پیغام_remove_inflight(context, &context->msgs_بهر, client_پیغام);
			}وإلا{
				أرجع rc;
			}
			اكسر;

		حالة mosq_ms_resend_pubrel:
			rc = send__pubrel(context, mid, عدم);
			إذا(!rc){
				client_پیغام->معلومات.state = mosq_ms_wait_for_pubcomp;
				plugin_persist_handle_client_پیغام_update(context, client_پیغام);
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


صحيح db_پیغام_write_inflight_بهر_all(بنية mosquitto *context)
{
	بنية mosquitto_client_پیغام *client_پیغام, *tmp;
	صحيح rc;

	إذا(context->state != mosq_cs_فعال || !net__is_connected(context)){
		أرجع MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_خوندي(context->msgs_بهر.inflight, client_پیغام, tmp){
		rc = db_پیغام_write_inflight_بهر_single(context, client_پیغام);
		إذا(rc){
			أرجع rc;
		}
	}
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db_پیغام_write_inflight_بهر_latest(بنية mosquitto *context)
{
	بنية mosquitto_client_پیغام *client_پیغام, *بل;
	صحيح rc;

	إذا(context->state != mosq_cs_فعال
			|| !net__is_connected(context)
			|| context->msgs_بهر.inflight == عدم){

		أرجع MOSQ_ERR_SUCCESS;
	}

	إذا(context->msgs_بهر.inflight->prev == context->msgs_بهر.inflight){
		/* Only one message */
		أرجع db_پیغام_write_inflight_بهر_single(context, context->msgs_بهر.inflight);
	}

	/* Start at the end of the list and work backwards looking for the first
	 * message in a non-publish state */
	client_پیغام = context->msgs_بهر.inflight->prev;
	طالما(client_پیغام != context->msgs_بهر.inflight &&
			(client_پیغام->معلومات.state == mosq_ms_publish_qos0
			|| client_پیغام->معلومات.state == mosq_ms_publish_qos1
			|| client_پیغام->معلومات.state == mosq_ms_publish_qos2)){

		client_پیغام = client_پیغام->prev;
	}

	/* Tail is now either the head of the list, if that message is waiting for
	 * publish, or the oldest message not waiting for a publish. In the latter
	 * case, any pending publishes should be next after this message. */
	إذا(client_پیغام != context->msgs_بهر.inflight){
		client_پیغام = client_پیغام->بل;
	}

	طالما(client_پیغام){
		بل = client_پیغام->بل;
		rc = db_پیغام_write_inflight_بهر_single(context, client_پیغام);
		إذا(rc){
			أرجع rc;
		}
		client_پیغام = بل;
	}
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db_پیغام_write_queued_دننه(بنية mosquitto *context)
{
	بنية mosquitto_client_پیغام *client_پیغام, *tmp;
	صحيح rc;

	إذا(context->state != mosq_cs_فعال){
		أرجع MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_خوندي(context->msgs_دننه.queued, client_پیغام, tmp){
		إذا(context->msgs_دننه.inflight_اعظمي != 0 && context->msgs_دننه.inflight_quota == 0){
			اكسر;
		}

		إذا(client_پیغام->معلومات.qos == 2){
			client_پیغام->معلومات.state = mosq_ms_send_pubrec;
			db_پیغام_له_کتار_ایستل_لومړی(context, &context->msgs_دننه);
			rc = send__pubrec(context, client_پیغام->معلومات.mid, 0, عدم);
			إذا(!rc){
				client_پیغام->معلومات.state = mosq_ms_wait_for_pubrel;
				plugin_persist_handle_client_پیغام_update(context, client_پیغام);
			}وإلا{
				plugin_persist_handle_client_پیغام_update(context, client_پیغام);
				أرجع rc;
			}
		}
	}
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db_پیغام_write_queued_بهر(بنية mosquitto *context)
{
	إذا(context->state != mosq_cs_فعال){
		أرجع MOSQ_ERR_SUCCESS;
	}

	db_fill_inflight_بهر_from_کتار(context);

	أرجع MOSQ_ERR_SUCCESS;
}
