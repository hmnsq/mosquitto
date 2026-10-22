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

#شامل "config.h"

#شامل <assert.h>
#شامل <stdio.h>
#شامل <utlist.h>

#شامل "mosquitto_broker_internal.h"
#شامل "send_mosq.h"
#شامل "sys_tree.h"
#شامل "util_mosq.h"


/**
 * Is this context ready to take more in flight messages right now?
 * @param context the client context of interest
 * @param qos qos for the packet of interest
 * @return true if more in flight are allowed.
 */
bool db__ready_for_flight(ساخت mosquitto *context, شمار mosquitto_پیغام_direction dir, صحیح qos)
{
	ساخت mosquitto_پیغام_ڈیٹا *msgs;
	bool valid_بائٹیں;
	bool valid_گنتی;

	اگر(dir == mosq_md_برآمد){
		msgs = &context->msgs_برآمد;
	}ورنہ{
		msgs = &context->msgs_اندر;
	}

	اگر(msgs->inflight_زیادہ_سے_زیادہ == 0 && db.config->max_inflight_بائٹیں == 0){
		واپس true;
	}

	اگر(qos == 0){
		/* Deliver QoS 0 messages unless the queue is already full.
		 * For QoS 0 messages the choice is either "inflight" or dropped.
		 * There is no queueing option, unless the client is offline and
		 * queue_qos0_messages is enabled.
		 */
		اگر(db.config->max_queued_messages == 0 && db.config->max_inflight_بائٹیں == 0){
			واپس true;
		}
		valid_بائٹیں = ((msgs->inflight_بائٹیں - (ssize_t)db.config->max_inflight_بائٹیں) < (ssize_t)db.config->max_queued_بائٹیں);
		اگر(dir == mosq_md_برآمد){
			valid_گنتی = context->برآمد_packet_گنتی < db.config->max_queued_messages;
		}ورنہ{
			valid_گنتی = msgs->inflight_گنتی - msgs->inflight_زیادہ_سے_زیادہ < db.config->max_queued_messages;
		}

		اگر(db.config->max_queued_messages == 0){
			واپس valid_بائٹیں;
		}
		اگر(db.config->max_queued_بائٹیں == 0){
			واپس valid_گنتی;
		}
	}ورنہ{
		valid_بائٹیں = (ssize_t)msgs->inflight_bytes12 < (ssize_t)db.config->max_inflight_بائٹیں;
		valid_گنتی = msgs->inflight_quota > 0;

		اگر(msgs->inflight_زیادہ_سے_زیادہ == 0){
			واپس valid_بائٹیں;
		}
		اگر(db.config->max_inflight_بائٹیں == 0){
			واپس valid_گنتی;
		}
	}

	واپس valid_بائٹیں && valid_گنتی;
}


/**
 * For a given client context, are more messages allowed to be queued?
 * It is assumed that inflight checks and queue_qos0 checks have already
 * been made.
 * @param context client of interest
 * @param qos destination qos for the packet of interest
 * @return true if queuing is allowed, false if should be dropped
 */
bool db_ready_for_قطار(ساخت mosquitto *context, صحیح qos, ساخت mosquitto_پیغام_ڈیٹا *پیغام_ڈیٹا)
{
	صحیح ماخذ_گنتی;
	صحیح adjust_گنتی;
	طویل ماخذ_بائٹیں;
	ssize_t adjust_بائٹیں = (ssize_t)db.config->max_inflight_بائٹیں;
	bool valid_بائٹیں;
	bool valid_گنتی;

	اگر(db.config->max_queued_messages == 0 && db.config->max_queued_بائٹیں == 0){
		واپس true;
	}

	اگر(qos == 0 && db.config->قطار_qos0_messages == false){
		واپس false; /* This case is handled in db__ready_for_flight() */
	}ورنہ{
		ماخذ_بائٹیں = (ssize_t)پیغام_ڈیٹا->queued_bytes12;
		ماخذ_گنتی = پیغام_ڈیٹا->queued_count12;
	}
	adjust_گنتی = پیغام_ڈیٹا->inflight_زیادہ_سے_زیادہ;

	/* nothing in flight for offline clients */
	اگر(!net__is_connected(context)){
		adjust_بائٹیں = 0;
		adjust_گنتی = 0;
	}

	valid_بائٹیں = (ماخذ_بائٹیں - (ssize_t)adjust_بائٹیں) < (ssize_t)db.config->max_queued_بائٹیں;
	valid_گنتی = ماخذ_گنتی - adjust_گنتی < db.config->max_queued_messages;

	اگر(db.config->max_queued_بائٹیں == 0){
		واپس valid_گنتی;
	}
	اگر(db.config->max_queued_messages == 0){
		واپس valid_بائٹیں;
	}

	واپس valid_بائٹیں && valid_گنتی;
}


خالی db_پیغام_جوڑو_مقصد_inflight_stats(ساخت mosquitto_پیغام_ڈیٹا *پیغام_ڈیٹا, ساخت mosquitto_client_پیغام *client_پیغام)
{
	پیغام_ڈیٹا->inflight_گنتی++;
	پیغام_ڈیٹا->inflight_بائٹیں += client_پیغام->بنیاد_پیغام->ڈیٹا.payloadlen;
	اگر(client_پیغام->ڈیٹا.qos != 0){
		پیغام_ڈیٹا->inflight_count12++;
		پیغام_ڈیٹا->inflight_bytes12 += client_پیغام->بنیاد_پیغام->ڈیٹا.payloadlen;
	}
}


ساکن خالی db_پیغام_remove_from_inflight_stats(ساخت mosquitto_پیغام_ڈیٹا *پیغام_ڈیٹا, ساخت mosquitto_client_پیغام *client_پیغام)
{
	پیغام_ڈیٹا->inflight_گنتی--;
	پیغام_ڈیٹا->inflight_بائٹیں -= client_پیغام->بنیاد_پیغام->ڈیٹا.payloadlen;
	اگر(client_پیغام->ڈیٹا.qos != 0){
		پیغام_ڈیٹا->inflight_count12--;
		پیغام_ڈیٹا->inflight_bytes12 -= client_پیغام->بنیاد_پیغام->ڈیٹا.payloadlen;
	}
}


خالی db_پیغام_جوڑو_مقصد_queued_stats(ساخت mosquitto_پیغام_ڈیٹا *پیغام_ڈیٹا, ساخت mosquitto_client_پیغام *client_پیغام)
{
	پیغام_ڈیٹا->queued_گنتی++;
	پیغام_ڈیٹا->queued_بائٹیں += client_پیغام->بنیاد_پیغام->ڈیٹا.payloadlen;
	اگر(client_پیغام->ڈیٹا.qos != 0){
		پیغام_ڈیٹا->queued_count12++;
		پیغام_ڈیٹا->queued_bytes12 += client_پیغام->بنیاد_پیغام->ڈیٹا.payloadlen;
	}
}


ساکن خالی db_پیغام_remove_from_queued_stats(ساخت mosquitto_پیغام_ڈیٹا *پیغام_ڈیٹا, ساخت mosquitto_client_پیغام *client_پیغام)
{
	پیغام_ڈیٹا->queued_گنتی--;
	پیغام_ڈیٹا->queued_بائٹیں -= client_پیغام->بنیاد_پیغام->ڈیٹا.payloadlen;
	اگر(client_پیغام->ڈیٹا.qos != 0){
		پیغام_ڈیٹا->queued_count12--;
		پیغام_ڈیٹا->queued_bytes12 -= client_پیغام->بنیاد_پیغام->ڈیٹا.payloadlen;
	}
}


صحیح db__open(ساخت mosquitto__config *config)
{
	اگر(!config){
		واپس MOSQ_ERR_INVAL;
	}

	db.contexts_by_id = تہی;
	db.contexts_by_sock = تہی;
	db.contexts_for_free = تہی;
#اگر_تعریف_شدہ WITH_BRIDGE
	db.bridges = تہی;
	db.bridge_گنتی = 0;
#اگر_ختم

	/* Initialize the hashtable */
	db.clientid_اشاریہ_hash = تہی;

	db.معمول_subs = تہی;
	db.shared_subs = تہی;

	sub__init();
	retain__init();

	db.config->security_options.unpwd = تہی;

#اگر_تعریف_شدہ WITH_PERSISTENCE
	اگر(persist__restore()){
		واپس 1;
	}
#اگر_ختم

	واپس MOSQ_ERR_SUCCESS;
}


ساکن خالی subhier_clean(ساخت mosquitto__subhier **subhier)
{
	ساخت mosquitto__subhier *peer, *subhier_tmp;
	ساخت mosquitto__subleaf *برگ, *nextleaf;

	HASH_ITER(hh, *subhier, peer, subhier_tmp){
		برگ = peer->subs;
		جب(برگ){
			nextleaf = برگ->اگلا;
			mosquitto_FREE(برگ);
			برگ = nextleaf;
		}
		subhier_clean(&peer->children);

		HASH_DELETE(hh, *subhier, peer);
		mosquitto_FREE(peer);
	}
}


صحیح db__close(خالی)
{
	subhier_clean(&db.معمول_subs);
	subhier_clean(&db.shared_subs);
	retain__clean(&db.retains);
	db_پیغام_store_clean();

	واپس MOSQ_ERR_SUCCESS;
}


صحیح db_پیغام_store_جوڑو(ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام)
{
	ساخت mosquitto_بنیاد_پیغام *ملا;
	غیرعلامتی hashv;

	HASH_قدر(&بنیاد_پیغام->ڈیٹا.store_id, حجم(بنیاد_پیغام->ڈیٹا.store_id), hashv);
	HASH_ڈھونڈو_BYHASHVALUE(hh, db.پیغام_store, &بنیاد_پیغام->ڈیٹا.store_id, حجم(بنیاد_پیغام->ڈیٹا.store_id), hashv, ملا);
	اگر(ملا == تہی){
		HASH_جوڑو_KEYPTR_BYHASHVALUE(hh, db.پیغام_store, &بنیاد_پیغام->ڈیٹا.store_id, حجم(بنیاد_پیغام->ڈیٹا.store_id), hashv, بنیاد_پیغام);
		واپس MOSQ_ERR_SUCCESS;
	}ورنہ{
		واپس MOSQ_ERR_ALREADY_EXISTS;
	}
}


خالی db_پیغام_store_free(ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام)
{
	mosquitto_FREE(بنیاد_پیغام->ڈیٹا.ماخذ_id);
	mosquitto_FREE(بنیاد_پیغام->ڈیٹا.ماخذ_username);
	اگر(بنیاد_پیغام->dest_ids){
		برائے(صحیح i=0; i<بنیاد_پیغام->dest_id_گنتی; i++){
			mosquitto_FREE(بنیاد_پیغام->dest_ids[i]);
		}
		mosquitto_FREE(بنیاد_پیغام->dest_ids);
	}
	mosquitto_FREE(بنیاد_پیغام->ڈیٹا.topic);
	mosquitto_property_free_all(&بنیاد_پیغام->ڈیٹا.properties);
	mosquitto_FREE(بنیاد_پیغام->ڈیٹا.payload);
	mosquitto_FREE(بنیاد_پیغام);
}


خالی db_پیغام_store_remove(ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام, bool notify)
{
	اگر(بنیاد_پیغام == تہی){
		واپس;
	}
	HASH_DELETE(hh, db.پیغام_store, بنیاد_پیغام);
	db.پیغام_store_گنتی--;
	db.پیغام_store_بائٹیں -= بنیاد_پیغام->ڈیٹا.payloadlen;
	اگر(notify == true){
		plugin_persist_handle_بنیاد_پیغام_delete(بنیاد_پیغام);
	}
	db_پیغام_store_free(بنیاد_پیغام);
}


خالی db_پیغام_store_clean(خالی)
{
	ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام, *بنیاد_پیغام_tmp;

	HASH_ITER(hh, db.پیغام_store, بنیاد_پیغام, بنیاد_پیغام_tmp){
		db_پیغام_store_remove(بنیاد_پیغام, false);
	}
}


خالی db_پیغام_store_ref_inc(ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام)
{
	بنیاد_پیغام->ref_گنتی++;
}


خالی db_پیغام_store_ref_dec(ساخت mosquitto_بنیاد_پیغام **بنیاد_پیغام)
{
	(*بنیاد_پیغام)->ref_گنتی--;
	اگر((*بنیاد_پیغام)->ref_گنتی == 0){
		db_پیغام_store_remove(*بنیاد_پیغام, true);
		*بنیاد_پیغام = تہی;
	}
}


خالی db_پیغام_store_compact(خالی)
{
	ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام, *بنیاد_پیغام_tmp;

	HASH_ITER(hh, db.پیغام_store, بنیاد_پیغام, بنیاد_پیغام_tmp){
		اگر(بنیاد_پیغام->ref_گنتی < 1){
			db_پیغام_store_remove(بنیاد_پیغام, true);
		}
	}
}


ساکن خالی db_پیغام_remove_inflight(ساخت mosquitto *context, ساخت mosquitto_پیغام_ڈیٹا *پیغام_ڈیٹا, ساخت mosquitto_client_پیغام *شے)
{
	اگر(!context || !پیغام_ڈیٹا || !شے){
		واپس;
	}

	plugin_persist_handle_client_پیغام_delete(context, شے);

	DL_DELETE(پیغام_ڈیٹا->inflight, شے);
	اگر(شے->بنیاد_پیغام){
		db_پیغام_remove_from_inflight_stats(پیغام_ڈیٹا, شے);
		db_پیغام_store_ref_dec(&شے->بنیاد_پیغام);
	}

	mosquitto_FREE(شے);
}


ساکن خالی db_پیغام_remove_queued(ساخت mosquitto *context, ساخت mosquitto_پیغام_ڈیٹا *پیغام_ڈیٹا, ساخت mosquitto_client_پیغام *شے)
{
	اگر(!context || !پیغام_ڈیٹا || !شے){
		واپس;
	}

	plugin_persist_handle_client_پیغام_delete(context, شے);

	DL_DELETE(پیغام_ڈیٹا->queued, شے);
	اگر(شے->بنیاد_پیغام){
		db_پیغام_remove_from_queued_stats(پیغام_ڈیٹا, شے);
		db_پیغام_store_ref_dec(&شے->بنیاد_پیغام);
	}

	mosquitto_FREE(شے);
}


ساکن خالی db_fill_inflight_برآمد_from_قطار(ساخت mosquitto *context)
{
	ساخت mosquitto_client_پیغام *client_پیغام, *tmp;

	DL_FOREACH_محفوظ(context->msgs_برآمد.queued, client_پیغام, tmp){
		اگر(!db__ready_for_flight(context, mosq_md_برآمد, client_پیغام->ڈیٹا.qos)){
			واپس;
		}
		انتخاب(client_پیغام->ڈیٹا.qos){
			حالت 0:
				client_پیغام->ڈیٹا.state = mosq_ms_publish_qos0;
				توڑو;
			حالت 1:
				client_پیغام->ڈیٹا.state = mosq_ms_publish_qos1;
				توڑو;
			حالت 2:
				client_پیغام->ڈیٹا.state = mosq_ms_publish_qos2;
				توڑو;
		}
		اگر(client_پیغام->بنیاد_پیغام->ڈیٹا.expiry_وقت && db.now_real_s > client_پیغام->بنیاد_پیغام->ڈیٹا.expiry_وقت){
			db_پیغام_remove_queued(context, &context->msgs_برآمد, client_پیغام);
			جاری;
		}
		plugin_persist_handle_client_پیغام_update(context, client_پیغام);
		db_پیغام_قطار_سے_نکالو_پہلا(context, &context->msgs_برآمد);
	}
}


خالی db_پیغام_قطار_سے_نکالو_پہلا(ساخت mosquitto *context, ساخت mosquitto_پیغام_ڈیٹا *پیغام_ڈیٹا)
{
	ساخت mosquitto_client_پیغام *client_پیغام;

	غیر_مستعمل(context);

	client_پیغام = پیغام_ڈیٹا->queued;
	DL_DELETE(پیغام_ڈیٹا->queued, client_پیغام);
	DL_APPEND(پیغام_ڈیٹا->inflight, client_پیغام);
	اگر(پیغام_ڈیٹا->inflight_quota > 0){
		پیغام_ڈیٹا->inflight_quota--;
	}

	db_پیغام_remove_from_queued_stats(پیغام_ڈیٹا, client_پیغام);
	db_پیغام_جوڑو_مقصد_inflight_stats(پیغام_ڈیٹا, client_پیغام);
}


صحیح db_پیغام_delete_outgoing(ساخت mosquitto *context, uint16_t mid, شمار mosquitto_پیغام_state expect_state, صحیح qos)
{
	ساخت mosquitto_client_پیغام *client_پیغام, *tmp;
	bool deleted = false;

	اگر(!context){
		واپس MOSQ_ERR_INVAL;
	}

	DL_FOREACH_محفوظ(context->msgs_برآمد.inflight, client_پیغام, tmp){
		اگر(client_پیغام->ڈیٹا.mid == mid){
			اگر(client_پیغام->ڈیٹا.qos != qos){
				log__printf(تہی, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when deleting outgoing message.",
						context->id, client_پیغام->ڈیٹا.qos, qos);
				واپس MOSQ_ERR_PROTOCOL;
			}ورنہ اگر(qos == 2 && client_پیغام->ڈیٹا.state != expect_state && expect_state != mosq_ms_any){
				log__printf(تہی, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched state (%d:%d) when deleting outgoing message.",
						context->id, client_پیغام->ڈیٹا.state, expect_state);
				واپس MOSQ_ERR_PROTOCOL;
			}
			db_پیغام_remove_inflight(context, &context->msgs_برآمد, client_پیغام);
			deleted = true;
			توڑو;
		}
	}

	اگر(deleted == false){
		DL_FOREACH_محفوظ(context->msgs_برآمد.queued, client_پیغام, tmp){
			اگر(client_پیغام->ڈیٹا.mid == mid){
				اگر(client_پیغام->ڈیٹا.qos != qos){
					واپس MOSQ_ERR_PROTOCOL;
				}ورنہ اگر(qos == 2 && client_پیغام->ڈیٹا.state != expect_state && expect_state != mosq_ms_any){
					واپس MOSQ_ERR_PROTOCOL;
				}
				db_پیغام_remove_queued(context, &context->msgs_برآمد, client_پیغام);
				توڑو;
			}
		}
	}
	db_fill_inflight_برآمد_from_قطار(context);
#اگر_تعریف_شدہ WITH_PERSISTENCE
	db.persistence_changes++;
#اگر_ختم

	واپس db_پیغام_write_inflight_برآمد_latest(context);
}


/* Only for QoS 2 messages */
صحیح db_پیغام_داخل_کرو_incoming(ساخت mosquitto *context, uint64_t cmsg_id, ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام, bool persist)
{
	ساخت mosquitto_client_پیغام *client_پیغام;
	ساخت mosquitto_پیغام_ڈیٹا *پیغام_ڈیٹا;
	شمار mosquitto_پیغام_state state = mosq_ms_invalid;
	صحیح rc = 0;

	أكد(بنیاد_پیغام);
	اگر(!context){
		واپس MOSQ_ERR_INVAL;
	}
	اگر(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		واپس MOSQ_ERR_SUCCESS;

	}
	پیغام_ڈیٹا = &context->msgs_اندر;

	اگر(db__ready_for_flight(context, mosq_md_اندر, بنیاد_پیغام->ڈیٹا.qos)){
		state = mosq_ms_wait_for_pubrel;
	}ورنہ اگر(بنیاد_پیغام->ڈیٹا.qos != 0 && db_ready_for_قطار(context, بنیاد_پیغام->ڈیٹا.qos, پیغام_ڈیٹا)){
		state = mosq_ms_queued;
		rc = 2;
	}ورنہ{
		/* Dropping message due to full queue. */
		اگر(context->is_dropping == false){
			context->is_dropping = true;
			log__printf(تہی, MOSQ_LOG_NOTICE,
					"Outgoing messages are being dropped for client %s.",
					context->id);
		}
		metrics__int_inc(mosq_شمار_mqtt_publish_dropped, 1);
		context->stats.messages_dropped++;

		واپس 2;
	}

	أكد(state != mosq_ms_invalid);

#اگر_تعریف_شدہ WITH_PERSISTENCE
	اگر(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#اگر_ختم

	client_پیغام = mosquitto_malloc(حجم(ساخت mosquitto_client_پیغام));
	اگر(!client_پیغام){
		واپس MOSQ_ERR_NOMEM;
	}
	client_پیغام->پچھلا = تہی;
	client_پیغام->اگلا = تہی;
	اگر(cmsg_id){
		client_پیغام->ڈیٹا.cmsg_id = cmsg_id;
	}ورنہ{
		client_پیغام->ڈیٹا.cmsg_id = ++context->آخری_cmsg_id;
	}
	client_پیغام->بنیاد_پیغام = بنیاد_پیغام;
	db_پیغام_store_ref_inc(client_پیغام->بنیاد_پیغام);
	client_پیغام->ڈیٹا.mid = بنیاد_پیغام->ڈیٹا.ماخذ_mid;
	client_پیغام->ڈیٹا.direction = mosq_md_اندر;
	client_پیغام->ڈیٹا.state = (uint8_t)state;
	client_پیغام->ڈیٹا.dup = false;
	اگر(بنیاد_پیغام->ڈیٹا.qos > context->max_qos){
		client_پیغام->ڈیٹا.qos = context->max_qos;
	}ورنہ{
		client_پیغام->ڈیٹا.qos = بنیاد_پیغام->ڈیٹا.qos;
	}
	client_پیغام->ڈیٹا.retain = بنیاد_پیغام->ڈیٹا.retain;
	client_پیغام->ڈیٹا.subscription_identifier = 0;

	اگر(state == mosq_ms_queued){
		DL_APPEND(پیغام_ڈیٹا->queued, client_پیغام);
		db_پیغام_جوڑو_مقصد_queued_stats(پیغام_ڈیٹا, client_پیغام);
	}ورنہ{
		DL_APPEND(پیغام_ڈیٹا->inflight, client_پیغام);
		db_پیغام_جوڑو_مقصد_inflight_stats(پیغام_ڈیٹا, client_پیغام);
	}

	اگر(persist && context->is_persisted){
		plugin_persist_handle_بنیاد_پیغام_جوڑو(client_پیغام->بنیاد_پیغام);
		plugin_persist_handle_client_پیغام_جوڑو(context, client_پیغام);
	}

	اگر(client_پیغام->بنیاد_پیغام->ڈیٹا.qos > 0){
		util__decrement_receive_quota(context);
	}
	واپس rc;
}


صحیح db_پیغام_داخل_کرو_outgoing(ساخت mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام, uint32_t subscription_identifier, bool update, bool persist)
{
	ساخت mosquitto_client_پیغام *client_پیغام;
	ساخت mosquitto_پیغام_ڈیٹا *پیغام_ڈیٹا;
	شمار mosquitto_پیغام_state state = mosq_ms_invalid;
	صحیح rc = 0;
	حرف **dest_ids;

	أكد(بنیاد_پیغام);
	اگر(!context){
		واپس MOSQ_ERR_INVAL;
	}
	اگر(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		واپس MOSQ_ERR_SUCCESS;

	}
	context->stats.messages_بھیجا++;

	پیغام_ڈیٹا = &context->msgs_برآمد;

	/* Check whether we've already sent this message to this client
	 * for outgoing messages only.
	 * If retain==true then this is a stale retained message and so should be
	 * sent regardless. FIXME - this does mean retained messages will received
	 * multiple times for overlapping subscriptions, although this is only the
	 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
	 */
	اگر(context->protocol != mosq_p_mqtt5
			&& db.config->allow_duplicate_messages == false
			&& retain == false && بنیاد_پیغام->dest_ids){

		برائے(صحیح i=0; i<بنیاد_پیغام->dest_id_گنتی; i++){
			اگر(بنیاد_پیغام->dest_ids[i] && !strcmp(بنیاد_پیغام->dest_ids[i], context->id)){
				/* We have already sent this message to this client. */
				واپس MOSQ_ERR_SUCCESS;
			}
		}
	}
	اگر(!net__is_connected(context)){
		/* Client is not connected only queue messages with QoS>0. */
		اگر(qos == 0 && !db.config->قطار_qos0_messages){
			اگر(!context->bridge){
				واپس 2;
			}ورنہ{
				اگر(context->bridge->آغاز_type != bst_lazy){
					واپس 2;
				}
			}
		}
		اگر(context->bridge && context->bridge->clean_آغاز_local == true){
			واپس 2;
		}
	}

	اگر(net__is_connected(context)){
		اگر(db__ready_for_flight(context, mosq_md_برآمد, qos)){
			انتخاب(qos){
				حالت 0:
					state = mosq_ms_publish_qos0;
					توڑو;
				حالت 1:
					state = mosq_ms_publish_qos1;
					توڑو;
				حالت 2:
					state = mosq_ms_publish_qos2;
					توڑو;
			}
		}ورنہ اگر(qos != 0 && db_ready_for_قطار(context, qos, پیغام_ڈیٹا)){
			state = mosq_ms_queued;
			rc = 2;
		}ورنہ{
			/* Dropping message due to full queue. */
			اگر(context->is_dropping == false){
				context->is_dropping = true;
				log__printf(تہی, MOSQ_LOG_NOTICE,
						"Outgoing messages are being dropped for client %s.",
						context->id);
			}
			metrics__int_inc(mosq_شمار_mqtt_publish_dropped, 1);
			واپس 2;
		}
	}ورنہ{
		اگر(db_ready_for_قطار(context, qos, پیغام_ڈیٹا)){
			state = mosq_ms_queued;
		}ورنہ{
			metrics__int_inc(mosq_شمار_mqtt_publish_dropped, 1);
			اگر(context->is_dropping == false){
				context->is_dropping = true;
				log__printf(تہی, MOSQ_LOG_NOTICE,
						"Outgoing messages are being dropped for client %s.",
						context->id);
			}
			واپس 2;
		}
	}
	أكد(state != mosq_ms_invalid);

#اگر_تعریف_شدہ WITH_PERSISTENCE
	اگر(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#اگر_ختم

	client_پیغام = mosquitto_malloc(حجم(ساخت mosquitto_client_پیغام));
	اگر(!client_پیغام){
		واپس MOSQ_ERR_NOMEM;
	}
	client_پیغام->پچھلا = تہی;
	client_پیغام->اگلا = تہی;
	اگر(cmsg_id){
		client_پیغام->ڈیٹا.cmsg_id = cmsg_id;
	}ورنہ{
		client_پیغام->ڈیٹا.cmsg_id = ++context->آخری_cmsg_id;
	}
	client_پیغام->بنیاد_پیغام = بنیاد_پیغام;
	db_پیغام_store_ref_inc(client_پیغام->بنیاد_پیغام);
	client_پیغام->ڈیٹا.mid = mid;
	client_پیغام->ڈیٹا.direction = mosq_md_برآمد;
	client_پیغام->ڈیٹا.state = (uint8_t)state;
	client_پیغام->ڈیٹا.dup = false;
	اگر(qos > context->max_qos){
		client_پیغام->ڈیٹا.qos = context->max_qos;
	}ورنہ{
		client_پیغام->ڈیٹا.qos = qos;
	}
	client_پیغام->ڈیٹا.retain = retain;
	client_پیغام->ڈیٹا.subscription_identifier = subscription_identifier;

	اگر(state == mosq_ms_queued){
		DL_APPEND(پیغام_ڈیٹا->queued, client_پیغام);
		db_پیغام_جوڑو_مقصد_queued_stats(پیغام_ڈیٹا, client_پیغام);
	}ورنہ{
		DL_APPEND(پیغام_ڈیٹا->inflight, client_پیغام);
		db_پیغام_جوڑو_مقصد_inflight_stats(پیغام_ڈیٹا, client_پیغام);
	}

	اگر(persist && context->is_persisted){
		plugin_persist_handle_بنیاد_پیغام_جوڑو(client_پیغام->بنیاد_پیغام);
		plugin_persist_handle_client_پیغام_جوڑو(context, client_پیغام);
	}

	اگر(db.config->allow_duplicate_messages == false && retain == false){
		/* Record which client ids this message has been sent to so we can avoid duplicates.
		 * Outgoing messages only.
		 * If retain==true then this is a stale retained message and so should be
		 * sent regardless. FIXME - this does mean retained messages will received
		 * multiple times for overlapping subscriptions, although this is only the
		 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
		 */
		dest_ids = mosquitto_realloc(بنیاد_پیغام->dest_ids, حجم(حرف *)*(size_t)(بنیاد_پیغام->dest_id_گنتی+1));
		اگر(dest_ids){
			بنیاد_پیغام->dest_ids = dest_ids;
			بنیاد_پیغام->dest_id_گنتی++;
			بنیاد_پیغام->dest_ids[بنیاد_پیغام->dest_id_گنتی-1] = mosquitto_strdup(context->id);
			اگر(!بنیاد_پیغام->dest_ids[بنیاد_پیغام->dest_id_گنتی-1]){
				واپس MOSQ_ERR_NOMEM;
			}
		}ورنہ{
			واپس MOSQ_ERR_NOMEM;
		}
	}
#اگر_تعریف_شدہ WITH_BRIDGE
	اگر(context->bridge && context->bridge->آغاز_type == bst_lazy
			&& !net__is_connected(context)
			&& context->msgs_برآمد.inflight_گنتی + context->msgs_برآمد.queued_گنتی >= context->bridge->حد){

		context->bridge->lazy_reconnect = true;
	}
#اگر_ختم

	اگر(client_پیغام->ڈیٹا.qos > 0 && state != mosq_ms_queued){
		util__decrement_send_quota(context);
	}

	اگر(update){
		rc = db_پیغام_write_inflight_برآمد_latest(context);
		اگر(rc){
			واپس rc;
		}
		rc = db_پیغام_write_queued_برآمد(context);
		اگر(rc){
			واپس rc;
		}
	}

	واپس rc;
}


ساکن اندرونی صحیح db_پیغام_update_outgoing_state(ساخت mosquitto *context, ساخت mosquitto_client_پیغام *سر,
		uint16_t mid, شمار mosquitto_پیغام_state state, صحیح qos, bool persist)
{
	ساخت mosquitto_client_پیغام *client_پیغام;

	DL_FOREACH(سر, client_پیغام){
		اگر(client_پیغام->ڈیٹا.mid == mid){
			اگر(client_پیغام->ڈیٹا.qos != qos){
				log__printf(تہی, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when updating outgoing message.",
						context->id, client_پیغام->ڈیٹا.qos, qos);
				واپس MOSQ_ERR_PROTOCOL;
			}
			client_پیغام->ڈیٹا.state = (uint8_t)state;
			اگر(persist){
				plugin_persist_handle_client_پیغام_update(context, client_پیغام);
			}
			واپس MOSQ_ERR_SUCCESS;
		}
	}
	واپس MOSQ_ERR_NOT_ملا;
}


صحیح db_پیغام_update_outgoing(ساخت mosquitto *context, uint16_t mid, شمار mosquitto_پیغام_state state, صحیح qos, bool persist)
{
	صحیح rc;

	rc = db_پیغام_update_outgoing_state(context, context->msgs_برآمد.inflight, mid, state, qos, persist);
	اگر(!persist && rc == MOSQ_ERR_NOT_ملا){
		rc = db_پیغام_update_outgoing_state(context, context->msgs_برآمد.queued, mid, state, qos, persist);
	}
	واپس rc;
}


ساکن خالی db__messages_delete_list(ساخت mosquitto_client_پیغام **سر)
{
	ساخت mosquitto_client_پیغام *client_پیغام, *tmp;

	DL_FOREACH_محفوظ(*سر, client_پیغام, tmp){
		DL_DELETE(*سر, client_پیغام);
		db_پیغام_store_ref_dec(&client_پیغام->بنیاد_پیغام);
		mosquitto_FREE(client_پیغام);
	}
	*سر = تہی;
}


صحیح db__messages_delete_incoming(ساخت mosquitto *context)
{
	اگر(!context){
		واپس MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_اندر.inflight);
	db__messages_delete_list(&context->msgs_اندر.queued);
	context->msgs_اندر.inflight_بائٹیں = 0;
	context->msgs_اندر.inflight_bytes12 = 0;
	context->msgs_اندر.inflight_گنتی = 0;
	context->msgs_اندر.inflight_count12 = 0;
	context->msgs_اندر.queued_بائٹیں = 0;
	context->msgs_اندر.queued_bytes12 = 0;
	context->msgs_اندر.queued_گنتی = 0;
	context->msgs_اندر.queued_count12 = 0;

	واپس MOSQ_ERR_SUCCESS;
}


صحیح db__messages_delete_outgoing(ساخت mosquitto *context)
{
	اگر(!context){
		واپس MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_برآمد.inflight);
	db__messages_delete_list(&context->msgs_برآمد.queued);
	context->msgs_برآمد.inflight_بائٹیں = 0;
	context->msgs_برآمد.inflight_bytes12 = 0;
	context->msgs_برآمد.inflight_گنتی = 0;
	context->msgs_برآمد.inflight_count12 = 0;
	context->msgs_برآمد.queued_بائٹیں = 0;
	context->msgs_برآمد.queued_bytes12 = 0;
	context->msgs_برآمد.queued_گنتی = 0;
	context->msgs_برآمد.queued_count12 = 0;

	واپس MOSQ_ERR_SUCCESS;
}


صحیح db__messages_delete(ساخت mosquitto *context, bool force_free)
{
	اگر(!context){
		واپس MOSQ_ERR_INVAL;
	}

	اگر(force_free || context->clean_آغاز || (context->bridge && context->bridge->clean_آغاز)){
		db__messages_delete_incoming(context);
	}

	اگر(force_free || (context->bridge && context->bridge->clean_آغاز_local)
			|| (context->bridge == تہی && context->clean_آغاز)){

		db__messages_delete_outgoing(context);
	}

	واپس MOSQ_ERR_SUCCESS;
}


صحیح db_messages_easy_قطار(ساخت mosquitto *context, ثابت حرف *topic, uint8_t qos, uint32_t payloadlen, ثابت خالی *payload, صحیح retain, uint32_t پیغام_expiry_interval, mosquitto_property **properties)
{
	ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام;
	ثابت حرف *ماخذ_id;
	شمار mosquitto_پیغام_origin origin;

	اگر(!topic){
		واپس MOSQ_ERR_INVAL;
	}

	بنیاد_پیغام = mosquitto_calloc(1, حجم(ساخت mosquitto_بنیاد_پیغام));
	اگر(بنیاد_پیغام == تہی){
		واپس MOSQ_ERR_NOMEM;
	}

	بنیاد_پیغام->ڈیٹا.topic = mosquitto_strdup(topic);
	اگر(بنیاد_پیغام->ڈیٹا.topic == تہی){
		db_پیغام_store_free(بنیاد_پیغام);
		واپس MOSQ_ERR_INVAL;
	}

	بنیاد_پیغام->ڈیٹا.qos = qos;
	اگر(db.config->retain_دستیاب == false){
		بنیاد_پیغام->ڈیٹا.retain = 0;
	}ورنہ{
		بنیاد_پیغام->ڈیٹا.retain = retain;
	}

	بنیاد_پیغام->ڈیٹا.payloadlen = payloadlen;
	اگر(payloadlen > 0){
		بنیاد_پیغام->ڈیٹا.payload = mosquitto_malloc(بنیاد_پیغام->ڈیٹا.payloadlen+1);
		اگر(بنیاد_پیغام->ڈیٹا.payload == تہی){
			db_پیغام_store_free(بنیاد_پیغام);
			واپس MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)بنیاد_پیغام->ڈیٹا.payload)[بنیاد_پیغام->ڈیٹا.payloadlen] = 0;
		حافظہ_نقل(بنیاد_پیغام->ڈیٹا.payload, payload, بنیاد_پیغام->ڈیٹا.payloadlen);
	}

	اگر(context && context->id){
		ماخذ_id = context->id;
	}ورنہ{
		ماخذ_id = "";
	}
	اگر(properties){
		بنیاد_پیغام->ڈیٹا.properties = *properties;
		*properties = تہی;
	}

	اگر(context){
		origin = mosq_mo_client;
	}ورنہ{
		origin = mosq_mo_broker;
	}
	اگر(db_پیغام_store(context, بنیاد_پیغام, &پیغام_expiry_interval, origin)){
		واپس 1;
	}

	واپس sub_messages_قطار(ماخذ_id, بنیاد_پیغام->ڈیٹا.qos, &بنیاد_پیغام);
}


#تعریف_کرو MOSQ_UUID_EPOCH 1637168273


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
uint64_t db_new_پیغام_id(خالی)
{
#اگر_تعریف_شدہ WIN32
	FILETIME ftime;
	uint64_t ftime64;
#ورنہ
	ساخت timespec ts;
#اگر_ختم
	uint64_t id;
	uint64_t tmp;
	time_t sec;
	طویل nsec;

	id = db.گرہ_id_shifted; /* Top 10-bits */

#اگر_تعریف_شدہ WIN32
	Get_System_وقت_Precise_As_File_وقت(&ftime);
	ftime64 = (((uint64_t)ftime.dw_بلند_Date_وقت)<<32) + ftime.dw_کم_Date_وقت;
	tmp = ftime64 - 116444736000000000LL; /* Convert offset to unix epoch, still in counts of 100ns */
	sec = tmp / 10000000; /* Convert to seconds */
	nsec = (طویل)(tmp - sec)*100; /* Remove seconds, convert to counts of 1ns */
#ورنہ
	clock_gettime(CLOCK_REALTIME, &ts);
	sec = ts.tv_sec;
	nsec = ts.tv_nsec;
#اگر_ختم
	tmp = (sec - MOSQ_UUID_EPOCH) & 0x7FFFFFFF;
	id = id | (tmp << 23); /* Seconds, 31-bits (68 years) */

	tmp = (nsec & 0x7FFFFF80); /* top 23-bits of the bottom 30 bits (1 billion ns), ~100 ns resolution */
	id = id | (tmp >> 7);

	اگر(id <= db.آخری_db_id){
		id = db.آخری_db_id + 1;
	}
	db.آخری_db_id = id;

	واپس id;
}


/* This function requires topic to be allocated on the heap. Once called, it owns topic and will free it on error. Likewise payload and properties. */
صحیح db_پیغام_store(ثابت ساخت mosquitto *ماخذ, ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام, uint32_t *پیغام_expiry_interval, شمار mosquitto_پیغام_origin origin)
{
	صحیح rc;

	أكد(بنیاد_پیغام);

	اگر(ماخذ && ماخذ->id){
		بنیاد_پیغام->ڈیٹا.ماخذ_id = mosquitto_strdup(ماخذ->id);
	}ورنہ{
		بنیاد_پیغام->ڈیٹا.ماخذ_id = mosquitto_strdup("");
	}
	اگر(!بنیاد_پیغام->ڈیٹا.ماخذ_id){
		log__printf(تہی, MOSQ_LOG_ERR, "Error: Out of memory.");
		db_پیغام_store_free(بنیاد_پیغام);
		واپس MOSQ_ERR_NOMEM;
	}

	اگر(ماخذ && ماخذ->username){
		بنیاد_پیغام->ڈیٹا.ماخذ_username = mosquitto_strdup(ماخذ->username);
		اگر(!بنیاد_پیغام->ڈیٹا.ماخذ_username){
			db_پیغام_store_free(بنیاد_پیغام);
			واپس MOSQ_ERR_NOMEM;
		}
	}
	اگر(ماخذ){
		بنیاد_پیغام->ماخذ_listener = ماخذ->listener;
	}
	بنیاد_پیغام->origin = origin;
	اگر(پیغام_expiry_interval){
		اگر(*پیغام_expiry_interval > 0 && *پیغام_expiry_interval != پیغام_EXPIRY_INFINITE){
			بنیاد_پیغام->ڈیٹا.expiry_وقت = db.now_real_s + *پیغام_expiry_interval;
		}ورنہ{
			بنیاد_پیغام->ڈیٹا.expiry_وقت = 0;
		}
	}

	بنیاد_پیغام->dest_ids = تہی;
	بنیاد_پیغام->dest_id_گنتی = 0;
	db.پیغام_store_گنتی++;
	db.پیغام_store_بائٹیں += بنیاد_پیغام->ڈیٹا.payloadlen;

	اگر(!بنیاد_پیغام->ڈیٹا.store_id){
		بنیاد_پیغام->ڈیٹا.store_id = db_new_پیغام_id();
	}

	rc = db_پیغام_store_جوڑو(بنیاد_پیغام);
	اگر(rc){
		db_پیغام_store_free(بنیاد_پیغام);
		واپس rc;
	}

	واپس MOSQ_ERR_SUCCESS;
}


صحیح db_پیغام_store_ڈھونڈو(ساخت mosquitto *context, uint16_t mid, ساخت mosquitto_client_پیغام **client_پیغام)
{
	ساخت mosquitto_client_پیغام *cmsg;

	*client_پیغام = تہی;

	اگر(!context){
		واپس MOSQ_ERR_INVAL;
	}

	DL_FOREACH(context->msgs_اندر.inflight, cmsg){
		اگر(cmsg->بنیاد_پیغام->ڈیٹا.ماخذ_mid == mid){
			*client_پیغام = cmsg;
			واپس MOSQ_ERR_SUCCESS;
		}
	}

	DL_FOREACH(context->msgs_اندر.queued, cmsg){
		اگر(cmsg->بنیاد_پیغام->ڈیٹا.ماخذ_mid == mid){
			*client_پیغام = cmsg;
			واپس MOSQ_ERR_SUCCESS;
		}
	}

	واپس 1;
}


/* Called on reconnect to set outgoing messages to a sensible state and force a
 * retry, and to set incoming messages to expect an appropriate retry. */
ساکن صحیح db_پیغام_reconnect_reset_outgoing(ساخت mosquitto *context)
{
	ساخت mosquitto_client_پیغام *client_پیغام, *tmp;

	context->msgs_برآمد.inflight_بائٹیں = 0;
	context->msgs_برآمد.inflight_bytes12 = 0;
	context->msgs_برآمد.inflight_گنتی = 0;
	context->msgs_برآمد.inflight_count12 = 0;
	context->msgs_برآمد.queued_بائٹیں = 0;
	context->msgs_برآمد.queued_bytes12 = 0;
	context->msgs_برآمد.queued_گنتی = 0;
	context->msgs_برآمد.queued_count12 = 0;
	context->msgs_برآمد.inflight_quota = context->msgs_برآمد.inflight_زیادہ_سے_زیادہ;

	DL_FOREACH_محفوظ(context->msgs_برآمد.inflight, client_پیغام, tmp){
		db_پیغام_جوڑو_مقصد_inflight_stats(&context->msgs_برآمد, client_پیغام);
		اگر(client_پیغام->ڈیٹا.qos > 0){
			util__decrement_send_quota(context);
		}

		انتخاب(client_پیغام->ڈیٹا.qos){
			حالت 0:
				client_پیغام->ڈیٹا.state = mosq_ms_publish_qos0;
				توڑو;
			حالت 1:
				client_پیغام->ڈیٹا.state = mosq_ms_publish_qos1;
				توڑو;
			حالت 2:
				اگر(client_پیغام->ڈیٹا.state == mosq_ms_wait_for_pubcomp){
					client_پیغام->ڈیٹا.state = mosq_ms_resend_pubrel;
				}ورنہ{
					client_پیغام->ڈیٹا.state = mosq_ms_publish_qos2;
				}
				توڑو;
		}
		plugin_persist_handle_client_پیغام_update(context, client_پیغام);
	}
	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_محفوظ(context->msgs_برآمد.queued, client_پیغام, tmp){
		db_پیغام_جوڑو_مقصد_queued_stats(&context->msgs_برآمد, client_پیغام);
	}
	db_fill_inflight_برآمد_from_قطار(context);

	واپس MOSQ_ERR_SUCCESS;
}


/* Called on reconnect to set incoming messages to expect an appropriate retry. */
ساکن صحیح db_پیغام_reconnect_reset_incoming(ساخت mosquitto *context)
{
	ساخت mosquitto_client_پیغام *client_پیغام, *tmp;

	context->msgs_اندر.inflight_بائٹیں = 0;
	context->msgs_اندر.inflight_bytes12 = 0;
	context->msgs_اندر.inflight_گنتی = 0;
	context->msgs_اندر.inflight_count12 = 0;
	context->msgs_اندر.queued_بائٹیں = 0;
	context->msgs_اندر.queued_bytes12 = 0;
	context->msgs_اندر.queued_گنتی = 0;
	context->msgs_اندر.queued_count12 = 0;
	context->msgs_اندر.inflight_quota = context->msgs_اندر.inflight_زیادہ_سے_زیادہ;

	DL_FOREACH_محفوظ(context->msgs_اندر.inflight, client_پیغام, tmp){
		db_پیغام_جوڑو_مقصد_inflight_stats(&context->msgs_اندر, client_پیغام);
		اگر(client_پیغام->ڈیٹا.qos > 0){
			util__decrement_receive_quota(context);
		}

		اگر(client_پیغام->ڈیٹا.qos != 2){
			/* Anything <QoS 2 can be completely retried by the client at
			 * no harm. */
			db_پیغام_remove_inflight(context, &context->msgs_اندر, client_پیغام);
		}ورنہ{
			/* Message state can be preserved here because it should match
			 * whatever the client has got. */
			client_پیغام->ڈیٹا.dup = 0;
		}
	}

	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_محفوظ(context->msgs_اندر.queued, client_پیغام, tmp){
		client_پیغام->ڈیٹا.dup = 0;
		db_پیغام_جوڑو_مقصد_queued_stats(&context->msgs_اندر, client_پیغام);
		اگر(db__ready_for_flight(context, mosq_md_اندر, client_پیغام->ڈیٹا.qos)){
			انتخاب(client_پیغام->ڈیٹا.qos){
				حالت 0:
					client_پیغام->ڈیٹا.state = mosq_ms_publish_qos0;
					توڑو;
				حالت 1:
					client_پیغام->ڈیٹا.state = mosq_ms_publish_qos1;
					توڑو;
				حالت 2:
					client_پیغام->ڈیٹا.state = mosq_ms_publish_qos2;
					توڑو;
			}
			db_پیغام_قطار_سے_نکالو_پہلا(context, &context->msgs_اندر);
			plugin_persist_handle_client_پیغام_update(context, client_پیغام);
		}
	}

	واپس MOSQ_ERR_SUCCESS;
}


صحیح db_پیغام_reconnect_reset(ساخت mosquitto *context)
{
	صحیح rc;

	rc = db_پیغام_reconnect_reset_outgoing(context);
	اگر(rc){
		واپس rc;
	}
	واپس db_پیغام_reconnect_reset_incoming(context);
}


صحیح db_پیغام_remove_incoming(ساخت mosquitto *context, uint16_t mid)
{
	ساخت mosquitto_client_پیغام *client_پیغام, *tmp;

	اگر(!context){
		واپس MOSQ_ERR_INVAL;
	}

	DL_FOREACH_محفوظ(context->msgs_اندر.inflight, client_پیغام, tmp){
		اگر(client_پیغام->ڈیٹا.mid == mid){
			اگر(client_پیغام->بنیاد_پیغام->ڈیٹا.qos != 2){
				log__printf(تہی, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when deleting incoming message.",
						context->id, client_پیغام->بنیاد_پیغام->ڈیٹا.qos);
				واپس MOSQ_ERR_PROTOCOL;
			}
			db_پیغام_remove_inflight(context, &context->msgs_اندر, client_پیغام);
			واپس MOSQ_ERR_SUCCESS;
		}
	}

	واپس MOSQ_ERR_NOT_ملا;
}


صحیح db_پیغام_release_incoming(ساخت mosquitto *context, uint16_t mid)
{
	ساخت mosquitto_client_پیغام *client_پیغام, *tmp;
	bool deleted = false;
	صحیح rc;

	اگر(!context){
		واپس MOSQ_ERR_INVAL;
	}

	DL_FOREACH_محفوظ(context->msgs_اندر.inflight, client_پیغام, tmp){
		اگر(client_پیغام->ڈیٹا.mid == mid){
			اگر(client_پیغام->بنیاد_پیغام->ڈیٹا.qos != 2){
				log__printf(تہی, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when releasing incoming message.",
						context->id, client_پیغام->بنیاد_پیغام->ڈیٹا.qos);
				واپس MOSQ_ERR_PROTOCOL;
			}
			ثابت حرف *topic = client_پیغام->بنیاد_پیغام->ڈیٹا.topic;
			ثابت حرف *ماخذ_id = client_پیغام->بنیاد_پیغام->ڈیٹا.ماخذ_id;

			/* topic==NULL should be a QoS 2 message that was
			 * denied/dropped and is being processed so the client doesn't
			 * keep resending it. That means we don't send it to other
			 * clients. */
			اگر(topic == تہی){
				db_پیغام_remove_inflight(context, &context->msgs_اندر, client_پیغام);
				deleted = true;
			}ورنہ{
				rc = sub_messages_قطار(ماخذ_id, 2, &client_پیغام->بنیاد_پیغام);
				اگر(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_NO_SUBSCRIBERS){
					db_پیغام_remove_inflight(context, &context->msgs_اندر, client_پیغام);
					deleted = true;
				}ورنہ{
					واپس 1;
				}
			}
		}
	}

	DL_FOREACH_محفوظ(context->msgs_اندر.queued, client_پیغام, tmp){
		اگر(db__ready_for_flight(context, mosq_md_اندر, client_پیغام->ڈیٹا.qos)){
			توڑو;
		}

		اگر(client_پیغام->ڈیٹا.qos == 2){
			send__pubrec(context, client_پیغام->ڈیٹا.mid, 0, تہی);
			client_پیغام->ڈیٹا.state = mosq_ms_wait_for_pubrel;
			db_پیغام_قطار_سے_نکالو_پہلا(context, &context->msgs_اندر);
			plugin_persist_handle_client_پیغام_update(context, client_پیغام);
		}
	}
	اگر(deleted){
		واپس MOSQ_ERR_SUCCESS;
	}ورنہ{
		واپس MOSQ_ERR_NOT_ملا;
	}
}


خالی db__expire_all_messages(ساخت mosquitto *context)
{
	ساخت mosquitto_client_پیغام *client_پیغام, *tmp;

	DL_FOREACH_محفوظ(context->msgs_برآمد.inflight, client_پیغام, tmp){
		اگر(client_پیغام->بنیاد_پیغام->ڈیٹا.expiry_وقت && db.now_real_s > client_پیغام->بنیاد_پیغام->ڈیٹا.expiry_وقت){
			اگر(client_پیغام->ڈیٹا.qos > 0){
				util__increment_send_quota(context);
			}
			db_پیغام_remove_inflight(context, &context->msgs_برآمد, client_پیغام);
		}
	}
	db_fill_inflight_برآمد_from_قطار(context);
	DL_FOREACH_محفوظ(context->msgs_برآمد.queued, client_پیغام, tmp){
		اگر(client_پیغام->بنیاد_پیغام->ڈیٹا.expiry_وقت && db.now_real_s > client_پیغام->بنیاد_پیغام->ڈیٹا.expiry_وقت){
			db_پیغام_remove_queued(context, &context->msgs_برآمد, client_پیغام);
		}
	}
	DL_FOREACH_محفوظ(context->msgs_اندر.inflight, client_پیغام, tmp){
		اگر(client_پیغام->بنیاد_پیغام->ڈیٹا.expiry_وقت && db.now_real_s > client_پیغام->بنیاد_پیغام->ڈیٹا.expiry_وقت){
			اگر(client_پیغام->ڈیٹا.qos > 0){
				util__increment_receive_quota(context);
			}
			db_پیغام_remove_inflight(context, &context->msgs_اندر, client_پیغام);
		}
	}
	DL_FOREACH_محفوظ(context->msgs_اندر.queued, client_پیغام, tmp){
		اگر(client_پیغام->بنیاد_پیغام->ڈیٹا.expiry_وقت && db.now_real_s > client_پیغام->بنیاد_پیغام->ڈیٹا.expiry_وقت){
			db_پیغام_remove_queued(context, &context->msgs_اندر, client_پیغام);
		}
	}
}


ساکن خالی db__client_messages_check_acl(ساخت mosquitto *context, ساخت mosquitto_پیغام_ڈیٹا *پیغام_ڈیٹا, ساخت mosquitto_client_پیغام **سر,
		خالی (*decrement_stats_fn)(ساخت mosquitto_پیغام_ڈیٹا *پیغام_ڈیٹا, ساخت mosquitto_client_پیغام *client_پیغام))
{
	ساخت mosquitto_client_پیغام *client_پیغام, *tmp;
	ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام;
	صحیح access;

	DL_FOREACH_محفوظ((*سر), client_پیغام, tmp){
		بنیاد_پیغام = client_پیغام->بنیاد_پیغام;
		اگر(client_پیغام->ڈیٹا.direction == mosq_md_برآمد){
			access = MOSQ_ACL_READ;
		}ورنہ{
			access = MOSQ_ACL_WRITE;
		}
		اگر(mosquitto_acl_check(context, بنیاد_پیغام->ڈیٹا.topic,
				بنیاد_پیغام->ڈیٹا.payloadlen, بنیاد_پیغام->ڈیٹا.payload,
				بنیاد_پیغام->ڈیٹا.qos, بنیاد_پیغام->ڈیٹا.retain,
				بنیاد_پیغام->ڈیٹا.properties, access) != MOSQ_ERR_SUCCESS){

			DL_DELETE((*سر), client_پیغام);
			decrement_stats_fn(پیغام_ڈیٹا, client_پیغام);
			plugin_persist_handle_client_پیغام_delete(context, client_پیغام);
			db_پیغام_store_ref_dec(&client_پیغام->بنیاد_پیغام);
			mosquitto_FREE(client_پیغام);
		}
	}
}


خالی db_check_acl_کا_all_messages(ساخت mosquitto *context)
{
	db__client_messages_check_acl(context, &context->msgs_اندر, &context->msgs_اندر.inflight, &db_پیغام_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_اندر, &context->msgs_اندر.queued, &db_پیغام_remove_from_queued_stats);
	db__client_messages_check_acl(context, &context->msgs_برآمد, &context->msgs_برآمد.inflight, &db_پیغام_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_برآمد, &context->msgs_برآمد.queued, &db_پیغام_remove_from_queued_stats);
}


ساکن صحیح db_پیغام_write_inflight_برآمد_single(ساخت mosquitto *context, ساخت mosquitto_client_پیغام *client_پیغام)
{
	ساخت mosquitto_بنیاد_پیغام *بنیاد_پیغام;
	mosquitto_property *بنیاد_پیغام_props = تہی;
	صحیح rc;
	uint16_t mid;
	صحیح retries;
	صحیح retain;
	ثابت حرف *topic;
	uint8_t qos;
	uint32_t payloadlen;
	ثابت خالی *payload;
	uint32_t expiry_interval;
	uint32_t subscription_id;

	بنیاد_پیغام = client_پیغام->بنیاد_پیغام;

	expiry_interval = 0;
	اگر(بنیاد_پیغام->ڈیٹا.expiry_وقت){
		اگر(db.now_real_s > بنیاد_پیغام->ڈیٹا.expiry_وقت){
			/* Message is expired, must not send. */
			اگر(client_پیغام->ڈیٹا.direction == mosq_md_برآمد && client_پیغام->ڈیٹا.qos > 0){
				util__increment_send_quota(context);
			}
			db_پیغام_remove_inflight(context, &context->msgs_برآمد, client_پیغام);
			db_fill_inflight_برآمد_from_قطار(context);
			واپس MOSQ_ERR_SUCCESS;
		}ورنہ{
			expiry_interval = (uint32_t)(بنیاد_پیغام->ڈیٹا.expiry_وقت - db.now_real_s);
		}
	}
	mid = client_پیغام->ڈیٹا.mid;
	retries = client_پیغام->ڈیٹا.dup;
	retain = client_پیغام->ڈیٹا.retain;
	topic = بنیاد_پیغام->ڈیٹا.topic;
	qos = (uint8_t)client_پیغام->ڈیٹا.qos;
	payloadlen = بنیاد_پیغام->ڈیٹا.payloadlen;
	payload = بنیاد_پیغام->ڈیٹا.payload;
	subscription_id = client_پیغام->ڈیٹا.subscription_identifier;
	بنیاد_پیغام_props = بنیاد_پیغام->ڈیٹا.properties;

	انتخاب(client_پیغام->ڈیٹا.state){
		حالت mosq_ms_publish_qos0:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, بنیاد_پیغام_props, expiry_interval);
			اگر(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_OVERSIZE_PACKET){
				db_پیغام_remove_inflight(context, &context->msgs_برآمد, client_پیغام);
			}ورنہ{
				واپس rc;
			}
			توڑو;

		حالت mosq_ms_publish_qos1:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, بنیاد_پیغام_props, expiry_interval);
			اگر(rc == MOSQ_ERR_SUCCESS){
				client_پیغام->ڈیٹا.dup = 1; /* Any retry attempts are a duplicate. */
				client_پیغام->ڈیٹا.state = mosq_ms_wait_for_puback;
				plugin_persist_handle_client_پیغام_update(context, client_پیغام);
			}ورنہ اگر(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db_پیغام_remove_inflight(context, &context->msgs_برآمد, client_پیغام);
			}ورنہ{
				واپس rc;
			}
			توڑو;

		حالت mosq_ms_publish_qos2:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, بنیاد_پیغام_props, expiry_interval);
			اگر(rc == MOSQ_ERR_SUCCESS){
				client_پیغام->ڈیٹا.dup = 1; /* Any retry attempts are a duplicate. */
				client_پیغام->ڈیٹا.state = mosq_ms_wait_for_pubrec;
				plugin_persist_handle_client_پیغام_update(context, client_پیغام);
			}ورنہ اگر(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db_پیغام_remove_inflight(context, &context->msgs_برآمد, client_پیغام);
			}ورنہ{
				واپس rc;
			}
			توڑو;

		حالت mosq_ms_resend_pubrel:
			rc = send__pubrel(context, mid, تہی);
			اگر(!rc){
				client_پیغام->ڈیٹا.state = mosq_ms_wait_for_pubcomp;
				plugin_persist_handle_client_پیغام_update(context, client_پیغام);
			}ورنہ{
				واپس rc;
			}
			توڑو;

		حالت mosq_ms_invalid:
		حالت mosq_ms_send_pubrec:
		حالت mosq_ms_resend_pubcomp:
		حالت mosq_ms_wait_for_puback:
		حالت mosq_ms_wait_for_pubrec:
		حالت mosq_ms_wait_for_pubrel:
		حالت mosq_ms_wait_for_pubcomp:
		حالت mosq_ms_queued:
			توڑو;
	}
	واپس MOSQ_ERR_SUCCESS;
}


صحیح db_پیغام_write_inflight_برآمد_all(ساخت mosquitto *context)
{
	ساخت mosquitto_client_پیغام *client_پیغام, *tmp;
	صحیح rc;

	اگر(context->state != mosq_cs_فعال || !net__is_connected(context)){
		واپس MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_محفوظ(context->msgs_برآمد.inflight, client_پیغام, tmp){
		rc = db_پیغام_write_inflight_برآمد_single(context, client_پیغام);
		اگر(rc){
			واپس rc;
		}
	}
	واپس MOSQ_ERR_SUCCESS;
}


صحیح db_پیغام_write_inflight_برآمد_latest(ساخت mosquitto *context)
{
	ساخت mosquitto_client_پیغام *client_پیغام, *اگلا;
	صحیح rc;

	اگر(context->state != mosq_cs_فعال
			|| !net__is_connected(context)
			|| context->msgs_برآمد.inflight == تہی){

		واپس MOSQ_ERR_SUCCESS;
	}

	اگر(context->msgs_برآمد.inflight->پچھلا == context->msgs_برآمد.inflight){
		/* Only one message */
		واپس db_پیغام_write_inflight_برآمد_single(context, context->msgs_برآمد.inflight);
	}

	/* Start at the end of the list and work backwards looking for the first
	 * message in a non-publish state */
	client_پیغام = context->msgs_برآمد.inflight->پچھلا;
	جب(client_پیغام != context->msgs_برآمد.inflight &&
			(client_پیغام->ڈیٹا.state == mosq_ms_publish_qos0
			|| client_پیغام->ڈیٹا.state == mosq_ms_publish_qos1
			|| client_پیغام->ڈیٹا.state == mosq_ms_publish_qos2)){

		client_پیغام = client_پیغام->پچھلا;
	}

	/* Tail is now either the head of the list, if that message is waiting for
	 * publish, or the oldest message not waiting for a publish. In the latter
	 * case, any pending publishes should be next after this message. */
	اگر(client_پیغام != context->msgs_برآمد.inflight){
		client_پیغام = client_پیغام->اگلا;
	}

	جب(client_پیغام){
		اگلا = client_پیغام->اگلا;
		rc = db_پیغام_write_inflight_برآمد_single(context, client_پیغام);
		اگر(rc){
			واپس rc;
		}
		client_پیغام = اگلا;
	}
	واپس MOSQ_ERR_SUCCESS;
}


صحیح db_پیغام_write_queued_اندر(ساخت mosquitto *context)
{
	ساخت mosquitto_client_پیغام *client_پیغام, *tmp;
	صحیح rc;

	اگر(context->state != mosq_cs_فعال){
		واپس MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_محفوظ(context->msgs_اندر.queued, client_پیغام, tmp){
		اگر(context->msgs_اندر.inflight_زیادہ_سے_زیادہ != 0 && context->msgs_اندر.inflight_quota == 0){
			توڑو;
		}

		اگر(client_پیغام->ڈیٹا.qos == 2){
			client_پیغام->ڈیٹا.state = mosq_ms_send_pubrec;
			db_پیغام_قطار_سے_نکالو_پہلا(context, &context->msgs_اندر);
			rc = send__pubrec(context, client_پیغام->ڈیٹا.mid, 0, تہی);
			اگر(!rc){
				client_پیغام->ڈیٹا.state = mosq_ms_wait_for_pubrel;
				plugin_persist_handle_client_پیغام_update(context, client_پیغام);
			}ورنہ{
				plugin_persist_handle_client_پیغام_update(context, client_پیغام);
				واپس rc;
			}
		}
	}
	واپس MOSQ_ERR_SUCCESS;
}


صحیح db_پیغام_write_queued_برآمد(ساخت mosquitto *context)
{
	اگر(context->state != mosq_cs_فعال){
		واپس MOSQ_ERR_SUCCESS;
	}

	db_fill_inflight_برآمد_from_قطار(context);

	واپس MOSQ_ERR_SUCCESS;
}
