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
bool db__ready_for_flight(بنية mosquitto *context, تعداد mosquitto_نياپو_direction dir, صحيح qos)
{
	بنية mosquitto_نياپو_معلومات *msgs;
	bool valid_بائيٽون;
	bool valid_ڳڻپ;

	إذا(dir == mosq_md_ٻاهريون){
		msgs = &context->msgs_ٻاهريون;
	}وإلا{
		msgs = &context->msgs_اندريون;
	}

	إذا(msgs->inflight_وڌ_کان_وڌ == 0 && db.config->max_inflight_بائيٽون == 0){
		أرجع true;
	}

	إذا(qos == 0){
		/* Deliver QoS 0 messages unless the queue is already full.
		 * For QoS 0 messages the choice is either "inflight" or dropped.
		 * There is no queueing option, unless the client is offline and
		 * queue_qos0_messages is enabled.
		 */
		إذا(db.config->max_queued_messages == 0 && db.config->max_inflight_بائيٽون == 0){
			أرجع true;
		}
		valid_بائيٽون = ((msgs->inflight_بائيٽون - (ssize_t)db.config->max_inflight_بائيٽون) < (ssize_t)db.config->max_queued_بائيٽون);
		إذا(dir == mosq_md_ٻاهريون){
			valid_ڳڻپ = context->ٻاهريون_packet_ڳڻپ < db.config->max_queued_messages;
		}وإلا{
			valid_ڳڻپ = msgs->inflight_ڳڻپ - msgs->inflight_وڌ_کان_وڌ < db.config->max_queued_messages;
		}

		إذا(db.config->max_queued_messages == 0){
			أرجع valid_بائيٽون;
		}
		إذا(db.config->max_queued_بائيٽون == 0){
			أرجع valid_ڳڻپ;
		}
	}وإلا{
		valid_بائيٽون = (ssize_t)msgs->inflight_bytes12 < (ssize_t)db.config->max_inflight_بائيٽون;
		valid_ڳڻپ = msgs->inflight_quota > 0;

		إذا(msgs->inflight_وڌ_کان_وڌ == 0){
			أرجع valid_بائيٽون;
		}
		إذا(db.config->max_inflight_بائيٽون == 0){
			أرجع valid_ڳڻپ;
		}
	}

	أرجع valid_بائيٽون && valid_ڳڻپ;
}


/**
 * For a given client context, are more messages allowed to be queued?
 * It is assumed that inflight checks and queue_qos0 checks have already
 * been made.
 * @param context client of interest
 * @param qos destination qos for the packet of interest
 * @return true if queuing is allowed, false if should be dropped
 */
bool db_ready_for_قطار(بنية mosquitto *context, صحيح qos, بنية mosquitto_نياپو_معلومات *نياپو_معلومات)
{
	صحيح ذريعو_ڳڻپ;
	صحيح adjust_ڳڻپ;
	ڊگهو ذريعو_بائيٽون;
	ssize_t adjust_بائيٽون = (ssize_t)db.config->max_inflight_بائيٽون;
	bool valid_بائيٽون;
	bool valid_ڳڻپ;

	إذا(db.config->max_queued_messages == 0 && db.config->max_queued_بائيٽون == 0){
		أرجع true;
	}

	إذا(qos == 0 && db.config->قطار_qos0_messages == false){
		أرجع false; /* This case is handled in db__ready_for_flight() */
	}وإلا{
		ذريعو_بائيٽون = (ssize_t)نياپو_معلومات->queued_bytes12;
		ذريعو_ڳڻپ = نياپو_معلومات->queued_count12;
	}
	adjust_ڳڻپ = نياپو_معلومات->inflight_وڌ_کان_وڌ;

	/* nothing in flight for offline clients */
	إذا(!net__is_connected(context)){
		adjust_بائيٽون = 0;
		adjust_ڳڻپ = 0;
	}

	valid_بائيٽون = (ذريعو_بائيٽون - (ssize_t)adjust_بائيٽون) < (ssize_t)db.config->max_queued_بائيٽون;
	valid_ڳڻپ = ذريعو_ڳڻپ - adjust_ڳڻپ < db.config->max_queued_messages;

	إذا(db.config->max_queued_بائيٽون == 0){
		أرجع valid_ڳڻپ;
	}
	إذا(db.config->max_queued_messages == 0){
		أرجع valid_بائيٽون;
	}

	أرجع valid_بائيٽون && valid_ڳڻپ;
}


فراغ db_نياپو_جوڙيو_منزل_inflight_stats(بنية mosquitto_نياپو_معلومات *نياپو_معلومات, بنية mosquitto_client_نياپو *client_نياپو)
{
	نياپو_معلومات->inflight_ڳڻپ++;
	نياپو_معلومات->inflight_بائيٽون += client_نياپو->بنياد_نياپو->معلومات.payloadlen;
	إذا(client_نياپو->معلومات.qos != 0){
		نياپو_معلومات->inflight_count12++;
		نياپو_معلومات->inflight_bytes12 += client_نياپو->بنياد_نياپو->معلومات.payloadlen;
	}
}


ساكن فراغ db_نياپو_remove_from_inflight_stats(بنية mosquitto_نياپو_معلومات *نياپو_معلومات, بنية mosquitto_client_نياپو *client_نياپو)
{
	نياپو_معلومات->inflight_ڳڻپ--;
	نياپو_معلومات->inflight_بائيٽون -= client_نياپو->بنياد_نياپو->معلومات.payloadlen;
	إذا(client_نياپو->معلومات.qos != 0){
		نياپو_معلومات->inflight_count12--;
		نياپو_معلومات->inflight_bytes12 -= client_نياپو->بنياد_نياپو->معلومات.payloadlen;
	}
}


فراغ db_نياپو_جوڙيو_منزل_queued_stats(بنية mosquitto_نياپو_معلومات *نياپو_معلومات, بنية mosquitto_client_نياپو *client_نياپو)
{
	نياپو_معلومات->queued_ڳڻپ++;
	نياپو_معلومات->queued_بائيٽون += client_نياپو->بنياد_نياپو->معلومات.payloadlen;
	إذا(client_نياپو->معلومات.qos != 0){
		نياپو_معلومات->queued_count12++;
		نياپو_معلومات->queued_bytes12 += client_نياپو->بنياد_نياپو->معلومات.payloadlen;
	}
}


ساكن فراغ db_نياپو_remove_from_queued_stats(بنية mosquitto_نياپو_معلومات *نياپو_معلومات, بنية mosquitto_client_نياپو *client_نياپو)
{
	نياپو_معلومات->queued_ڳڻپ--;
	نياپو_معلومات->queued_بائيٽون -= client_نياپو->بنياد_نياپو->معلومات.payloadlen;
	إذا(client_نياپو->معلومات.qos != 0){
		نياپو_معلومات->queued_count12--;
		نياپو_معلومات->queued_bytes12 -= client_نياپو->بنياد_نياپو->معلومات.payloadlen;
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
#جيڪڏهن_وصف_ٿيل WITH_BRIDGE
	db.bridges = عدم;
	db.bridge_ڳڻپ = 0;
#جيڪڏهن_پورو

	/* Initialize the hashtable */
	db.clientid_اشاريو_hash = عدم;

	db.عام_subs = عدم;
	db.shared_subs = عدم;

	sub__init();
	retain__init();

	db.config->security_options.unpwd = عدم;

#جيڪڏهن_وصف_ٿيل WITH_PERSISTENCE
	إذا(persist__restore()){
		أرجع 1;
	}
#جيڪڏهن_پورو

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن فراغ subhier_clean(بنية mosquitto__subhier **subhier)
{
	بنية mosquitto__subhier *peer, *subhier_tmp;
	بنية mosquitto__subleaf *پن, *nextleaf;

	HASH_ITER(hh, *subhier, peer, subhier_tmp){
		پن = peer->subs;
		طالما(پن){
			nextleaf = پن->ايندڙ;
			mosquitto_FREE(پن);
			پن = nextleaf;
		}
		subhier_clean(&peer->children);

		HASH_DELETE(hh, *subhier, peer);
		mosquitto_FREE(peer);
	}
}


صحيح db__close(فراغ)
{
	subhier_clean(&db.عام_subs);
	subhier_clean(&db.shared_subs);
	retain__clean(&db.retains);
	db_نياپو_store_clean();

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db_نياپو_store_جوڙيو(بنية mosquitto_بنياد_نياپو *بنياد_نياپو)
{
	بنية mosquitto_بنياد_نياپو *مليو;
	بنانشان hashv;

	HASH_قدر(&بنياد_نياپو->معلومات.store_id, حجم(بنياد_نياپو->معلومات.store_id), hashv);
	HASH_ڳوليو_BYHASHVALUE(hh, db.نياپو_store, &بنياد_نياپو->معلومات.store_id, حجم(بنياد_نياپو->معلومات.store_id), hashv, مليو);
	إذا(مليو == عدم){
		HASH_جوڙيو_KEYPTR_BYHASHVALUE(hh, db.نياپو_store, &بنياد_نياپو->معلومات.store_id, حجم(بنياد_نياپو->معلومات.store_id), hashv, بنياد_نياپو);
		أرجع MOSQ_ERR_SUCCESS;
	}وإلا{
		أرجع MOSQ_ERR_ALREADY_EXISTS;
	}
}


فراغ db_نياپو_store_free(بنية mosquitto_بنياد_نياپو *بنياد_نياپو)
{
	mosquitto_FREE(بنياد_نياپو->معلومات.ذريعو_id);
	mosquitto_FREE(بنياد_نياپو->معلومات.ذريعو_username);
	إذا(بنياد_نياپو->dest_ids){
		لكل(صحيح i=0; i<بنياد_نياپو->dest_id_ڳڻپ; i++){
			mosquitto_FREE(بنياد_نياپو->dest_ids[i]);
		}
		mosquitto_FREE(بنياد_نياپو->dest_ids);
	}
	mosquitto_FREE(بنياد_نياپو->معلومات.topic);
	mosquitto_property_free_all(&بنياد_نياپو->معلومات.properties);
	mosquitto_FREE(بنياد_نياپو->معلومات.payload);
	mosquitto_FREE(بنياد_نياپو);
}


فراغ db_نياپو_store_remove(بنية mosquitto_بنياد_نياپو *بنياد_نياپو, bool notify)
{
	إذا(بنياد_نياپو == عدم){
		أرجع;
	}
	HASH_DELETE(hh, db.نياپو_store, بنياد_نياپو);
	db.نياپو_store_ڳڻپ--;
	db.نياپو_store_بائيٽون -= بنياد_نياپو->معلومات.payloadlen;
	إذا(notify == true){
		plugin_persist_handle_بنياد_نياپو_delete(بنياد_نياپو);
	}
	db_نياپو_store_free(بنياد_نياپو);
}


فراغ db_نياپو_store_clean(فراغ)
{
	بنية mosquitto_بنياد_نياپو *بنياد_نياپو, *بنياد_نياپو_tmp;

	HASH_ITER(hh, db.نياپو_store, بنياد_نياپو, بنياد_نياپو_tmp){
		db_نياپو_store_remove(بنياد_نياپو, false);
	}
}


فراغ db_نياپو_store_ref_inc(بنية mosquitto_بنياد_نياپو *بنياد_نياپو)
{
	بنياد_نياپو->ref_ڳڻپ++;
}


فراغ db_نياپو_store_ref_dec(بنية mosquitto_بنياد_نياپو **بنياد_نياپو)
{
	(*بنياد_نياپو)->ref_ڳڻپ--;
	إذا((*بنياد_نياپو)->ref_ڳڻپ == 0){
		db_نياپو_store_remove(*بنياد_نياپو, true);
		*بنياد_نياپو = عدم;
	}
}


فراغ db_نياپو_store_compact(فراغ)
{
	بنية mosquitto_بنياد_نياپو *بنياد_نياپو, *بنياد_نياپو_tmp;

	HASH_ITER(hh, db.نياپو_store, بنياد_نياپو, بنياد_نياپو_tmp){
		إذا(بنياد_نياپو->ref_ڳڻپ < 1){
			db_نياپو_store_remove(بنياد_نياپو, true);
		}
	}
}


ساكن فراغ db_پيغام_remove_inflight(بنية mosquitto *context, بنية mosquitto_نياپو_معلومات *نياپو_معلومات, بنية mosquitto_client_نياپو *شي)
{
	إذا(!context || !نياپو_معلومات || !شي){
		أرجع;
	}

	plugin_persist_handle_client_نياپو_delete(context, شي);

	DL_DELETE(نياپو_معلومات->inflight, شي);
	إذا(شي->بنياد_نياپو){
		db_نياپو_remove_from_inflight_stats(نياپو_معلومات, شي);
		db_نياپو_store_ref_dec(&شي->بنياد_نياپو);
	}

	mosquitto_FREE(شي);
}


ساكن فراغ db_پيغام_remove_queued(بنية mosquitto *context, بنية mosquitto_نياپو_معلومات *نياپو_معلومات, بنية mosquitto_client_نياپو *شي)
{
	إذا(!context || !نياپو_معلومات || !شي){
		أرجع;
	}

	plugin_persist_handle_client_نياپو_delete(context, شي);

	DL_DELETE(نياپو_معلومات->queued, شي);
	إذا(شي->بنياد_نياپو){
		db_نياپو_remove_from_queued_stats(نياپو_معلومات, شي);
		db_نياپو_store_ref_dec(&شي->بنياد_نياپو);
	}

	mosquitto_FREE(شي);
}


ساكن فراغ db_fill_inflight_ٻاهريون_from_قطار(بنية mosquitto *context)
{
	بنية mosquitto_client_نياپو *client_نياپو, *tmp;

	DL_FOREACH_محفوظ(context->msgs_ٻاهريون.queued, client_نياپو, tmp){
		إذا(!db__ready_for_flight(context, mosq_md_ٻاهريون, client_نياپو->معلومات.qos)){
			أرجع;
		}
		بدل(client_نياپو->معلومات.qos){
			حالة 0:
				client_نياپو->معلومات.state = mosq_ms_publish_qos0;
				اكسر;
			حالة 1:
				client_نياپو->معلومات.state = mosq_ms_publish_qos1;
				اكسر;
			حالة 2:
				client_نياپو->معلومات.state = mosq_ms_publish_qos2;
				اكسر;
		}
		إذا(client_نياپو->بنياد_نياپو->معلومات.expiry_وقت && db.now_real_s > client_نياپو->بنياد_نياپو->معلومات.expiry_وقت){
			db_پيغام_remove_queued(context, &context->msgs_ٻاهريون, client_نياپو);
			استمر;
		}
		plugin_persist_handle_client_نياپو_update(context, client_نياپو);
		db_پيغام_قطار_مان_ڪڍو_پهريون(context, &context->msgs_ٻاهريون);
	}
}


فراغ db_پيغام_قطار_مان_ڪڍو_پهريون(بنية mosquitto *context, بنية mosquitto_نياپو_معلومات *نياپو_معلومات)
{
	بنية mosquitto_client_نياپو *client_نياپو;

	اڻ_استعمال_ٿيل(context);

	client_نياپو = نياپو_معلومات->queued;
	DL_DELETE(نياپو_معلومات->queued, client_نياپو);
	DL_APPEND(نياپو_معلومات->inflight, client_نياپو);
	إذا(نياپو_معلومات->inflight_quota > 0){
		نياپو_معلومات->inflight_quota--;
	}

	db_نياپو_remove_from_queued_stats(نياپو_معلومات, client_نياپو);
	db_نياپو_جوڙيو_منزل_inflight_stats(نياپو_معلومات, client_نياپو);
}


صحيح db_پيغام_delete_outgoing(بنية mosquitto *context, uint16_t mid, تعداد mosquitto_نياپو_state expect_state, صحيح qos)
{
	بنية mosquitto_client_نياپو *client_نياپو, *tmp;
	bool deleted = false;

	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	DL_FOREACH_محفوظ(context->msgs_ٻاهريون.inflight, client_نياپو, tmp){
		إذا(client_نياپو->معلومات.mid == mid){
			إذا(client_نياپو->معلومات.qos != qos){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when deleting outgoing message.",
						context->id, client_نياپو->معلومات.qos, qos);
				أرجع MOSQ_ERR_PROTOCOL;
			}وإلا إذا(qos == 2 && client_نياپو->معلومات.state != expect_state && expect_state != mosq_ms_any){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched state (%d:%d) when deleting outgoing message.",
						context->id, client_نياپو->معلومات.state, expect_state);
				أرجع MOSQ_ERR_PROTOCOL;
			}
			db_پيغام_remove_inflight(context, &context->msgs_ٻاهريون, client_نياپو);
			deleted = true;
			اكسر;
		}
	}

	إذا(deleted == false){
		DL_FOREACH_محفوظ(context->msgs_ٻاهريون.queued, client_نياپو, tmp){
			إذا(client_نياپو->معلومات.mid == mid){
				إذا(client_نياپو->معلومات.qos != qos){
					أرجع MOSQ_ERR_PROTOCOL;
				}وإلا إذا(qos == 2 && client_نياپو->معلومات.state != expect_state && expect_state != mosq_ms_any){
					أرجع MOSQ_ERR_PROTOCOL;
				}
				db_پيغام_remove_queued(context, &context->msgs_ٻاهريون, client_نياپو);
				اكسر;
			}
		}
	}
	db_fill_inflight_ٻاهريون_from_قطار(context);
#جيڪڏهن_وصف_ٿيل WITH_PERSISTENCE
	db.persistence_changes++;
#جيڪڏهن_پورو

	أرجع db_پيغام_write_inflight_ٻاهريون_latest(context);
}


/* Only for QoS 2 messages */
صحيح db_پيغام_وجهو_incoming(بنية mosquitto *context, uint64_t cmsg_id, بنية mosquitto_بنياد_نياپو *بنياد_نياپو, bool persist)
{
	بنية mosquitto_client_نياپو *client_نياپو;
	بنية mosquitto_نياپو_معلومات *نياپو_معلومات;
	تعداد mosquitto_نياپو_state state = mosq_ms_invalid;
	صحيح rc = 0;

	أكد(بنياد_نياپو);
	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		أرجع MOSQ_ERR_SUCCESS;

	}
	نياپو_معلومات = &context->msgs_اندريون;

	إذا(db__ready_for_flight(context, mosq_md_اندريون, بنياد_نياپو->معلومات.qos)){
		state = mosq_ms_wait_for_pubrel;
	}وإلا إذا(بنياد_نياپو->معلومات.qos != 0 && db_ready_for_قطار(context, بنياد_نياپو->معلومات.qos, نياپو_معلومات)){
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

#جيڪڏهن_وصف_ٿيل WITH_PERSISTENCE
	إذا(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#جيڪڏهن_پورو

	client_نياپو = mosquitto_malloc(حجم(بنية mosquitto_client_نياپو));
	إذا(!client_نياپو){
		أرجع MOSQ_ERR_NOMEM;
	}
	client_نياپو->prev = عدم;
	client_نياپو->ايندڙ = عدم;
	إذا(cmsg_id){
		client_نياپو->معلومات.cmsg_id = cmsg_id;
	}وإلا{
		client_نياپو->معلومات.cmsg_id = ++context->آخري_cmsg_id;
	}
	client_نياپو->بنياد_نياپو = بنياد_نياپو;
	db_نياپو_store_ref_inc(client_نياپو->بنياد_نياپو);
	client_نياپو->معلومات.mid = بنياد_نياپو->معلومات.ذريعو_mid;
	client_نياپو->معلومات.direction = mosq_md_اندريون;
	client_نياپو->معلومات.state = (uint8_t)state;
	client_نياپو->معلومات.dup = false;
	إذا(بنياد_نياپو->معلومات.qos > context->max_qos){
		client_نياپو->معلومات.qos = context->max_qos;
	}وإلا{
		client_نياپو->معلومات.qos = بنياد_نياپو->معلومات.qos;
	}
	client_نياپو->معلومات.retain = بنياد_نياپو->معلومات.retain;
	client_نياپو->معلومات.subscription_identifier = 0;

	إذا(state == mosq_ms_queued){
		DL_APPEND(نياپو_معلومات->queued, client_نياپو);
		db_نياپو_جوڙيو_منزل_queued_stats(نياپو_معلومات, client_نياپو);
	}وإلا{
		DL_APPEND(نياپو_معلومات->inflight, client_نياپو);
		db_نياپو_جوڙيو_منزل_inflight_stats(نياپو_معلومات, client_نياپو);
	}

	إذا(persist && context->is_persisted){
		plugin_persist_handle_بنياد_نياپو_جوڙيو(client_نياپو->بنياد_نياپو);
		plugin_persist_handle_client_نياپو_جوڙيو(context, client_نياپو);
	}

	إذا(client_نياپو->بنياد_نياپو->معلومات.qos > 0){
		util__decrement_receive_quota(context);
	}
	أرجع rc;
}


صحيح db_پيغام_وجهو_outgoing(بنية mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, بنية mosquitto_بنياد_نياپو *بنياد_نياپو, uint32_t subscription_identifier, bool update, bool persist)
{
	بنية mosquitto_client_نياپو *client_نياپو;
	بنية mosquitto_نياپو_معلومات *نياپو_معلومات;
	تعداد mosquitto_نياپو_state state = mosq_ms_invalid;
	صحيح rc = 0;
	حرف **dest_ids;

	أكد(بنياد_نياپو);
	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		أرجع MOSQ_ERR_SUCCESS;

	}
	context->stats.messages_موڪليل++;

	نياپو_معلومات = &context->msgs_ٻاهريون;

	/* Check whether we've already sent this message to this client
	 * for outgoing messages only.
	 * If retain==true then this is a stale retained message and so should be
	 * sent regardless. FIXME - this does mean retained messages will received
	 * multiple times for overlapping subscriptions, although this is only the
	 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
	 */
	إذا(context->protocol != mosq_p_mqtt5
			&& db.config->allow_duplicate_messages == false
			&& retain == false && بنياد_نياپو->dest_ids){

		لكل(صحيح i=0; i<بنياد_نياپو->dest_id_ڳڻپ; i++){
			إذا(بنياد_نياپو->dest_ids[i] && !strcmp(بنياد_نياپو->dest_ids[i], context->id)){
				/* We have already sent this message to this client. */
				أرجع MOSQ_ERR_SUCCESS;
			}
		}
	}
	إذا(!net__is_connected(context)){
		/* Client is not connected only queue messages with QoS>0. */
		إذا(qos == 0 && !db.config->قطار_qos0_messages){
			إذا(!context->bridge){
				أرجع 2;
			}وإلا{
				إذا(context->bridge->شروعات_type != bst_lazy){
					أرجع 2;
				}
			}
		}
		إذا(context->bridge && context->bridge->clean_شروعات_local == true){
			أرجع 2;
		}
	}

	إذا(net__is_connected(context)){
		إذا(db__ready_for_flight(context, mosq_md_ٻاهريون, qos)){
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
		}وإلا إذا(qos != 0 && db_ready_for_قطار(context, qos, نياپو_معلومات)){
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
		إذا(db_ready_for_قطار(context, qos, نياپو_معلومات)){
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

#جيڪڏهن_وصف_ٿيل WITH_PERSISTENCE
	إذا(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#جيڪڏهن_پورو

	client_نياپو = mosquitto_malloc(حجم(بنية mosquitto_client_نياپو));
	إذا(!client_نياپو){
		أرجع MOSQ_ERR_NOMEM;
	}
	client_نياپو->prev = عدم;
	client_نياپو->ايندڙ = عدم;
	إذا(cmsg_id){
		client_نياپو->معلومات.cmsg_id = cmsg_id;
	}وإلا{
		client_نياپو->معلومات.cmsg_id = ++context->آخري_cmsg_id;
	}
	client_نياپو->بنياد_نياپو = بنياد_نياپو;
	db_نياپو_store_ref_inc(client_نياپو->بنياد_نياپو);
	client_نياپو->معلومات.mid = mid;
	client_نياپو->معلومات.direction = mosq_md_ٻاهريون;
	client_نياپو->معلومات.state = (uint8_t)state;
	client_نياپو->معلومات.dup = false;
	إذا(qos > context->max_qos){
		client_نياپو->معلومات.qos = context->max_qos;
	}وإلا{
		client_نياپو->معلومات.qos = qos;
	}
	client_نياپو->معلومات.retain = retain;
	client_نياپو->معلومات.subscription_identifier = subscription_identifier;

	إذا(state == mosq_ms_queued){
		DL_APPEND(نياپو_معلومات->queued, client_نياپو);
		db_نياپو_جوڙيو_منزل_queued_stats(نياپو_معلومات, client_نياپو);
	}وإلا{
		DL_APPEND(نياپو_معلومات->inflight, client_نياپو);
		db_نياپو_جوڙيو_منزل_inflight_stats(نياپو_معلومات, client_نياپو);
	}

	إذا(persist && context->is_persisted){
		plugin_persist_handle_بنياد_نياپو_جوڙيو(client_نياپو->بنياد_نياپو);
		plugin_persist_handle_client_نياپو_جوڙيو(context, client_نياپو);
	}

	إذا(db.config->allow_duplicate_messages == false && retain == false){
		/* Record which client ids this message has been sent to so we can avoid duplicates.
		 * Outgoing messages only.
		 * If retain==true then this is a stale retained message and so should be
		 * sent regardless. FIXME - this does mean retained messages will received
		 * multiple times for overlapping subscriptions, although this is only the
		 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
		 */
		dest_ids = mosquitto_realloc(بنياد_نياپو->dest_ids, حجم(حرف *)*(size_t)(بنياد_نياپو->dest_id_ڳڻپ+1));
		إذا(dest_ids){
			بنياد_نياپو->dest_ids = dest_ids;
			بنياد_نياپو->dest_id_ڳڻپ++;
			بنياد_نياپو->dest_ids[بنياد_نياپو->dest_id_ڳڻپ-1] = mosquitto_strdup(context->id);
			إذا(!بنياد_نياپو->dest_ids[بنياد_نياپو->dest_id_ڳڻپ-1]){
				أرجع MOSQ_ERR_NOMEM;
			}
		}وإلا{
			أرجع MOSQ_ERR_NOMEM;
		}
	}
#جيڪڏهن_وصف_ٿيل WITH_BRIDGE
	إذا(context->bridge && context->bridge->شروعات_type == bst_lazy
			&& !net__is_connected(context)
			&& context->msgs_ٻاهريون.inflight_ڳڻپ + context->msgs_ٻاهريون.queued_ڳڻپ >= context->bridge->حد){

		context->bridge->lazy_reconnect = true;
	}
#جيڪڏهن_پورو

	إذا(client_نياپو->معلومات.qos > 0 && state != mosq_ms_queued){
		util__decrement_send_quota(context);
	}

	إذا(update){
		rc = db_پيغام_write_inflight_ٻاهريون_latest(context);
		إذا(rc){
			أرجع rc;
		}
		rc = db_پيغام_write_queued_ٻاهريون(context);
		إذا(rc){
			أرجع rc;
		}
	}

	أرجع rc;
}


ساكن مضمن صحيح db_پيغام_update_outgoing_state(بنية mosquitto *context, بنية mosquitto_client_نياپو *سر,
		uint16_t mid, تعداد mosquitto_نياپو_state state, صحيح qos, bool persist)
{
	بنية mosquitto_client_نياپو *client_نياپو;

	DL_FOREACH(سر, client_نياپو){
		إذا(client_نياپو->معلومات.mid == mid){
			إذا(client_نياپو->معلومات.qos != qos){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when updating outgoing message.",
						context->id, client_نياپو->معلومات.qos, qos);
				أرجع MOSQ_ERR_PROTOCOL;
			}
			client_نياپو->معلومات.state = (uint8_t)state;
			إذا(persist){
				plugin_persist_handle_client_نياپو_update(context, client_نياپو);
			}
			أرجع MOSQ_ERR_SUCCESS;
		}
	}
	أرجع MOSQ_ERR_NOT_مليو;
}


صحيح db_پيغام_update_outgoing(بنية mosquitto *context, uint16_t mid, تعداد mosquitto_نياپو_state state, صحيح qos, bool persist)
{
	صحيح rc;

	rc = db_پيغام_update_outgoing_state(context, context->msgs_ٻاهريون.inflight, mid, state, qos, persist);
	إذا(!persist && rc == MOSQ_ERR_NOT_مليو){
		rc = db_پيغام_update_outgoing_state(context, context->msgs_ٻاهريون.queued, mid, state, qos, persist);
	}
	أرجع rc;
}


ساكن فراغ db__messages_delete_list(بنية mosquitto_client_نياپو **سر)
{
	بنية mosquitto_client_نياپو *client_نياپو, *tmp;

	DL_FOREACH_محفوظ(*سر, client_نياپو, tmp){
		DL_DELETE(*سر, client_نياپو);
		db_نياپو_store_ref_dec(&client_نياپو->بنياد_نياپو);
		mosquitto_FREE(client_نياپو);
	}
	*سر = عدم;
}


صحيح db__messages_delete_incoming(بنية mosquitto *context)
{
	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_اندريون.inflight);
	db__messages_delete_list(&context->msgs_اندريون.queued);
	context->msgs_اندريون.inflight_بائيٽون = 0;
	context->msgs_اندريون.inflight_bytes12 = 0;
	context->msgs_اندريون.inflight_ڳڻپ = 0;
	context->msgs_اندريون.inflight_count12 = 0;
	context->msgs_اندريون.queued_بائيٽون = 0;
	context->msgs_اندريون.queued_bytes12 = 0;
	context->msgs_اندريون.queued_ڳڻپ = 0;
	context->msgs_اندريون.queued_count12 = 0;

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db__messages_delete_outgoing(بنية mosquitto *context)
{
	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_ٻاهريون.inflight);
	db__messages_delete_list(&context->msgs_ٻاهريون.queued);
	context->msgs_ٻاهريون.inflight_بائيٽون = 0;
	context->msgs_ٻاهريون.inflight_bytes12 = 0;
	context->msgs_ٻاهريون.inflight_ڳڻپ = 0;
	context->msgs_ٻاهريون.inflight_count12 = 0;
	context->msgs_ٻاهريون.queued_بائيٽون = 0;
	context->msgs_ٻاهريون.queued_bytes12 = 0;
	context->msgs_ٻاهريون.queued_ڳڻپ = 0;
	context->msgs_ٻاهريون.queued_count12 = 0;

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db__messages_delete(بنية mosquitto *context, bool force_free)
{
	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	إذا(force_free || context->clean_شروعات || (context->bridge && context->bridge->clean_شروعات)){
		db__messages_delete_incoming(context);
	}

	إذا(force_free || (context->bridge && context->bridge->clean_شروعات_local)
			|| (context->bridge == عدم && context->clean_شروعات)){

		db__messages_delete_outgoing(context);
	}

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db_messages_easy_قطار(بنية mosquitto *context, ثابت حرف *topic, uint8_t qos, uint32_t payloadlen, ثابت فراغ *payload, صحيح retain, uint32_t پيغام_expiry_interval, mosquitto_property **properties)
{
	بنية mosquitto_بنياد_نياپو *بنياد_نياپو;
	ثابت حرف *ذريعو_id;
	تعداد mosquitto_نياپو_origin origin;

	إذا(!topic){
		أرجع MOSQ_ERR_INVAL;
	}

	بنياد_نياپو = mosquitto_calloc(1, حجم(بنية mosquitto_بنياد_نياپو));
	إذا(بنياد_نياپو == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}

	بنياد_نياپو->معلومات.topic = mosquitto_strdup(topic);
	إذا(بنياد_نياپو->معلومات.topic == عدم){
		db_نياپو_store_free(بنياد_نياپو);
		أرجع MOSQ_ERR_INVAL;
	}

	بنياد_نياپو->معلومات.qos = qos;
	إذا(db.config->retain_موجود == false){
		بنياد_نياپو->معلومات.retain = 0;
	}وإلا{
		بنياد_نياپو->معلومات.retain = retain;
	}

	بنياد_نياپو->معلومات.payloadlen = payloadlen;
	إذا(payloadlen > 0){
		بنياد_نياپو->معلومات.payload = mosquitto_malloc(بنياد_نياپو->معلومات.payloadlen+1);
		إذا(بنياد_نياپو->معلومات.payload == عدم){
			db_نياپو_store_free(بنياد_نياپو);
			أرجع MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)بنياد_نياپو->معلومات.payload)[بنياد_نياپو->معلومات.payloadlen] = 0;
		يادگيري_نقل(بنياد_نياپو->معلومات.payload, payload, بنياد_نياپو->معلومات.payloadlen);
	}

	إذا(context && context->id){
		ذريعو_id = context->id;
	}وإلا{
		ذريعو_id = "";
	}
	إذا(properties){
		بنياد_نياپو->معلومات.properties = *properties;
		*properties = عدم;
	}

	إذا(context){
		origin = mosq_mo_client;
	}وإلا{
		origin = mosq_mo_broker;
	}
	إذا(db_پيغام_store(context, بنياد_نياپو, &پيغام_expiry_interval, origin)){
		أرجع 1;
	}

	أرجع sub_messages_قطار(ذريعو_id, بنياد_نياپو->معلومات.qos, &بنياد_نياپو);
}


#وصف_ڪريو MOSQ_UUID_EPOCH 1637168273


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
uint64_t db_new_نياپو_id(فراغ)
{
#جيڪڏهن_وصف_ٿيل WIN32
	FILETIME ftime;
	uint64_t ftime64;
#وإلا
	بنية timespec ts;
#جيڪڏهن_پورو
	uint64_t id;
	uint64_t tmp;
	time_t sec;
	ڊگهو nsec;

	id = db.ڳنڍ_id_shifted; /* Top 10-bits */

#جيڪڏهن_وصف_ٿيل WIN32
	Get_System_وقت_Precise_As_File_وقت(&ftime);
	ftime64 = (((uint64_t)ftime.dw_مٿاهون_Date_وقت)<<32) + ftime.dw_هيٺيون_Date_وقت;
	tmp = ftime64 - 116444736000000000LL; /* Convert offset to unix epoch, still in counts of 100ns */
	sec = tmp / 10000000; /* Convert to seconds */
	nsec = (ڊگهو)(tmp - sec)*100; /* Remove seconds, convert to counts of 1ns */
#وإلا
	clock_gettime(CLOCK_REALTIME, &ts);
	sec = ts.tv_sec;
	nsec = ts.tv_nsec;
#جيڪڏهن_پورو
	tmp = (sec - MOSQ_UUID_EPOCH) & 0x7FFFFFFF;
	id = id | (tmp << 23); /* Seconds, 31-bits (68 years) */

	tmp = (nsec & 0x7FFFFF80); /* top 23-bits of the bottom 30 bits (1 billion ns), ~100 ns resolution */
	id = id | (tmp >> 7);

	إذا(id <= db.آخري_db_id){
		id = db.آخري_db_id + 1;
	}
	db.آخري_db_id = id;

	أرجع id;
}


/* This function requires topic to be allocated on the heap. Once called, it owns topic and will free it on error. Likewise payload and properties. */
صحيح db_پيغام_store(ثابت بنية mosquitto *ذريعو, بنية mosquitto_بنياد_نياپو *بنياد_نياپو, uint32_t *پيغام_expiry_interval, تعداد mosquitto_نياپو_origin origin)
{
	صحيح rc;

	أكد(بنياد_نياپو);

	إذا(ذريعو && ذريعو->id){
		بنياد_نياپو->معلومات.ذريعو_id = mosquitto_strdup(ذريعو->id);
	}وإلا{
		بنياد_نياپو->معلومات.ذريعو_id = mosquitto_strdup("");
	}
	إذا(!بنياد_نياپو->معلومات.ذريعو_id){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		db_نياپو_store_free(بنياد_نياپو);
		أرجع MOSQ_ERR_NOMEM;
	}

	إذا(ذريعو && ذريعو->username){
		بنياد_نياپو->معلومات.ذريعو_username = mosquitto_strdup(ذريعو->username);
		إذا(!بنياد_نياپو->معلومات.ذريعو_username){
			db_نياپو_store_free(بنياد_نياپو);
			أرجع MOSQ_ERR_NOMEM;
		}
	}
	إذا(ذريعو){
		بنياد_نياپو->ذريعو_listener = ذريعو->listener;
	}
	بنياد_نياپو->origin = origin;
	إذا(پيغام_expiry_interval){
		إذا(*پيغام_expiry_interval > 0 && *پيغام_expiry_interval != نياپو_EXPIRY_INFINITE){
			بنياد_نياپو->معلومات.expiry_وقت = db.now_real_s + *پيغام_expiry_interval;
		}وإلا{
			بنياد_نياپو->معلومات.expiry_وقت = 0;
		}
	}

	بنياد_نياپو->dest_ids = عدم;
	بنياد_نياپو->dest_id_ڳڻپ = 0;
	db.نياپو_store_ڳڻپ++;
	db.نياپو_store_بائيٽون += بنياد_نياپو->معلومات.payloadlen;

	إذا(!بنياد_نياپو->معلومات.store_id){
		بنياد_نياپو->معلومات.store_id = db_new_نياپو_id();
	}

	rc = db_نياپو_store_جوڙيو(بنياد_نياپو);
	إذا(rc){
		db_نياپو_store_free(بنياد_نياپو);
		أرجع rc;
	}

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db_پيغام_store_ڳوليو(بنية mosquitto *context, uint16_t mid, بنية mosquitto_client_نياپو **client_نياپو)
{
	بنية mosquitto_client_نياپو *cmsg;

	*client_نياپو = عدم;

	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	DL_FOREACH(context->msgs_اندريون.inflight, cmsg){
		إذا(cmsg->بنياد_نياپو->معلومات.ذريعو_mid == mid){
			*client_نياپو = cmsg;
			أرجع MOSQ_ERR_SUCCESS;
		}
	}

	DL_FOREACH(context->msgs_اندريون.queued, cmsg){
		إذا(cmsg->بنياد_نياپو->معلومات.ذريعو_mid == mid){
			*client_نياپو = cmsg;
			أرجع MOSQ_ERR_SUCCESS;
		}
	}

	أرجع 1;
}


/* Called on reconnect to set outgoing messages to a sensible state and force a
 * retry, and to set incoming messages to expect an appropriate retry. */
ساكن صحيح db_پيغام_reconnect_reset_outgoing(بنية mosquitto *context)
{
	بنية mosquitto_client_نياپو *client_نياپو, *tmp;

	context->msgs_ٻاهريون.inflight_بائيٽون = 0;
	context->msgs_ٻاهريون.inflight_bytes12 = 0;
	context->msgs_ٻاهريون.inflight_ڳڻپ = 0;
	context->msgs_ٻاهريون.inflight_count12 = 0;
	context->msgs_ٻاهريون.queued_بائيٽون = 0;
	context->msgs_ٻاهريون.queued_bytes12 = 0;
	context->msgs_ٻاهريون.queued_ڳڻپ = 0;
	context->msgs_ٻاهريون.queued_count12 = 0;
	context->msgs_ٻاهريون.inflight_quota = context->msgs_ٻاهريون.inflight_وڌ_کان_وڌ;

	DL_FOREACH_محفوظ(context->msgs_ٻاهريون.inflight, client_نياپو, tmp){
		db_نياپو_جوڙيو_منزل_inflight_stats(&context->msgs_ٻاهريون, client_نياپو);
		إذا(client_نياپو->معلومات.qos > 0){
			util__decrement_send_quota(context);
		}

		بدل(client_نياپو->معلومات.qos){
			حالة 0:
				client_نياپو->معلومات.state = mosq_ms_publish_qos0;
				اكسر;
			حالة 1:
				client_نياپو->معلومات.state = mosq_ms_publish_qos1;
				اكسر;
			حالة 2:
				إذا(client_نياپو->معلومات.state == mosq_ms_wait_for_pubcomp){
					client_نياپو->معلومات.state = mosq_ms_resend_pubrel;
				}وإلا{
					client_نياپو->معلومات.state = mosq_ms_publish_qos2;
				}
				اكسر;
		}
		plugin_persist_handle_client_نياپو_update(context, client_نياپو);
	}
	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_محفوظ(context->msgs_ٻاهريون.queued, client_نياپو, tmp){
		db_نياپو_جوڙيو_منزل_queued_stats(&context->msgs_ٻاهريون, client_نياپو);
	}
	db_fill_inflight_ٻاهريون_from_قطار(context);

	أرجع MOSQ_ERR_SUCCESS;
}


/* Called on reconnect to set incoming messages to expect an appropriate retry. */
ساكن صحيح db_پيغام_reconnect_reset_incoming(بنية mosquitto *context)
{
	بنية mosquitto_client_نياپو *client_نياپو, *tmp;

	context->msgs_اندريون.inflight_بائيٽون = 0;
	context->msgs_اندريون.inflight_bytes12 = 0;
	context->msgs_اندريون.inflight_ڳڻپ = 0;
	context->msgs_اندريون.inflight_count12 = 0;
	context->msgs_اندريون.queued_بائيٽون = 0;
	context->msgs_اندريون.queued_bytes12 = 0;
	context->msgs_اندريون.queued_ڳڻپ = 0;
	context->msgs_اندريون.queued_count12 = 0;
	context->msgs_اندريون.inflight_quota = context->msgs_اندريون.inflight_وڌ_کان_وڌ;

	DL_FOREACH_محفوظ(context->msgs_اندريون.inflight, client_نياپو, tmp){
		db_نياپو_جوڙيو_منزل_inflight_stats(&context->msgs_اندريون, client_نياپو);
		إذا(client_نياپو->معلومات.qos > 0){
			util__decrement_receive_quota(context);
		}

		إذا(client_نياپو->معلومات.qos != 2){
			/* Anything <QoS 2 can be completely retried by the client at
			 * no harm. */
			db_پيغام_remove_inflight(context, &context->msgs_اندريون, client_نياپو);
		}وإلا{
			/* Message state can be preserved here because it should match
			 * whatever the client has got. */
			client_نياپو->معلومات.dup = 0;
		}
	}

	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_محفوظ(context->msgs_اندريون.queued, client_نياپو, tmp){
		client_نياپو->معلومات.dup = 0;
		db_نياپو_جوڙيو_منزل_queued_stats(&context->msgs_اندريون, client_نياپو);
		إذا(db__ready_for_flight(context, mosq_md_اندريون, client_نياپو->معلومات.qos)){
			بدل(client_نياپو->معلومات.qos){
				حالة 0:
					client_نياپو->معلومات.state = mosq_ms_publish_qos0;
					اكسر;
				حالة 1:
					client_نياپو->معلومات.state = mosq_ms_publish_qos1;
					اكسر;
				حالة 2:
					client_نياپو->معلومات.state = mosq_ms_publish_qos2;
					اكسر;
			}
			db_پيغام_قطار_مان_ڪڍو_پهريون(context, &context->msgs_اندريون);
			plugin_persist_handle_client_نياپو_update(context, client_نياپو);
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db_پيغام_reconnect_reset(بنية mosquitto *context)
{
	صحيح rc;

	rc = db_پيغام_reconnect_reset_outgoing(context);
	إذا(rc){
		أرجع rc;
	}
	أرجع db_پيغام_reconnect_reset_incoming(context);
}


صحيح db_پيغام_remove_incoming(بنية mosquitto *context, uint16_t mid)
{
	بنية mosquitto_client_نياپو *client_نياپو, *tmp;

	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	DL_FOREACH_محفوظ(context->msgs_اندريون.inflight, client_نياپو, tmp){
		إذا(client_نياپو->معلومات.mid == mid){
			إذا(client_نياپو->بنياد_نياپو->معلومات.qos != 2){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when deleting incoming message.",
						context->id, client_نياپو->بنياد_نياپو->معلومات.qos);
				أرجع MOSQ_ERR_PROTOCOL;
			}
			db_پيغام_remove_inflight(context, &context->msgs_اندريون, client_نياپو);
			أرجع MOSQ_ERR_SUCCESS;
		}
	}

	أرجع MOSQ_ERR_NOT_مليو;
}


صحيح db_پيغام_release_incoming(بنية mosquitto *context, uint16_t mid)
{
	بنية mosquitto_client_نياپو *client_نياپو, *tmp;
	bool deleted = false;
	صحيح rc;

	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	DL_FOREACH_محفوظ(context->msgs_اندريون.inflight, client_نياپو, tmp){
		إذا(client_نياپو->معلومات.mid == mid){
			إذا(client_نياپو->بنياد_نياپو->معلومات.qos != 2){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when releasing incoming message.",
						context->id, client_نياپو->بنياد_نياپو->معلومات.qos);
				أرجع MOSQ_ERR_PROTOCOL;
			}
			ثابت حرف *topic = client_نياپو->بنياد_نياپو->معلومات.topic;
			ثابت حرف *ذريعو_id = client_نياپو->بنياد_نياپو->معلومات.ذريعو_id;

			/* topic==NULL should be a QoS 2 message that was
			 * denied/dropped and is being processed so the client doesn't
			 * keep resending it. That means we don't send it to other
			 * clients. */
			إذا(topic == عدم){
				db_پيغام_remove_inflight(context, &context->msgs_اندريون, client_نياپو);
				deleted = true;
			}وإلا{
				rc = sub_messages_قطار(ذريعو_id, 2, &client_نياپو->بنياد_نياپو);
				إذا(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_NO_SUBSCRIBERS){
					db_پيغام_remove_inflight(context, &context->msgs_اندريون, client_نياپو);
					deleted = true;
				}وإلا{
					أرجع 1;
				}
			}
		}
	}

	DL_FOREACH_محفوظ(context->msgs_اندريون.queued, client_نياپو, tmp){
		إذا(db__ready_for_flight(context, mosq_md_اندريون, client_نياپو->معلومات.qos)){
			اكسر;
		}

		إذا(client_نياپو->معلومات.qos == 2){
			send__pubrec(context, client_نياپو->معلومات.mid, 0, عدم);
			client_نياپو->معلومات.state = mosq_ms_wait_for_pubrel;
			db_پيغام_قطار_مان_ڪڍو_پهريون(context, &context->msgs_اندريون);
			plugin_persist_handle_client_نياپو_update(context, client_نياپو);
		}
	}
	إذا(deleted){
		أرجع MOSQ_ERR_SUCCESS;
	}وإلا{
		أرجع MOSQ_ERR_NOT_مليو;
	}
}


فراغ db__expire_all_messages(بنية mosquitto *context)
{
	بنية mosquitto_client_نياپو *client_نياپو, *tmp;

	DL_FOREACH_محفوظ(context->msgs_ٻاهريون.inflight, client_نياپو, tmp){
		إذا(client_نياپو->بنياد_نياپو->معلومات.expiry_وقت && db.now_real_s > client_نياپو->بنياد_نياپو->معلومات.expiry_وقت){
			إذا(client_نياپو->معلومات.qos > 0){
				util__increment_send_quota(context);
			}
			db_پيغام_remove_inflight(context, &context->msgs_ٻاهريون, client_نياپو);
		}
	}
	db_fill_inflight_ٻاهريون_from_قطار(context);
	DL_FOREACH_محفوظ(context->msgs_ٻاهريون.queued, client_نياپو, tmp){
		إذا(client_نياپو->بنياد_نياپو->معلومات.expiry_وقت && db.now_real_s > client_نياپو->بنياد_نياپو->معلومات.expiry_وقت){
			db_پيغام_remove_queued(context, &context->msgs_ٻاهريون, client_نياپو);
		}
	}
	DL_FOREACH_محفوظ(context->msgs_اندريون.inflight, client_نياپو, tmp){
		إذا(client_نياپو->بنياد_نياپو->معلومات.expiry_وقت && db.now_real_s > client_نياپو->بنياد_نياپو->معلومات.expiry_وقت){
			إذا(client_نياپو->معلومات.qos > 0){
				util__increment_receive_quota(context);
			}
			db_پيغام_remove_inflight(context, &context->msgs_اندريون, client_نياپو);
		}
	}
	DL_FOREACH_محفوظ(context->msgs_اندريون.queued, client_نياپو, tmp){
		إذا(client_نياپو->بنياد_نياپو->معلومات.expiry_وقت && db.now_real_s > client_نياپو->بنياد_نياپو->معلومات.expiry_وقت){
			db_پيغام_remove_queued(context, &context->msgs_اندريون, client_نياپو);
		}
	}
}


ساكن فراغ db__client_messages_check_acl(بنية mosquitto *context, بنية mosquitto_نياپو_معلومات *نياپو_معلومات, بنية mosquitto_client_نياپو **سر,
		فراغ (*decrement_stats_fn)(بنية mosquitto_نياپو_معلومات *نياپو_معلومات, بنية mosquitto_client_نياپو *client_نياپو))
{
	بنية mosquitto_client_نياپو *client_نياپو, *tmp;
	بنية mosquitto_بنياد_نياپو *بنياد_نياپو;
	صحيح access;

	DL_FOREACH_محفوظ((*سر), client_نياپو, tmp){
		بنياد_نياپو = client_نياپو->بنياد_نياپو;
		إذا(client_نياپو->معلومات.direction == mosq_md_ٻاهريون){
			access = MOSQ_ACL_READ;
		}وإلا{
			access = MOSQ_ACL_WRITE;
		}
		إذا(mosquitto_acl_check(context, بنياد_نياپو->معلومات.topic,
				بنياد_نياپو->معلومات.payloadlen, بنياد_نياپو->معلومات.payload,
				بنياد_نياپو->معلومات.qos, بنياد_نياپو->معلومات.retain,
				بنياد_نياپو->معلومات.properties, access) != MOSQ_ERR_SUCCESS){

			DL_DELETE((*سر), client_نياپو);
			decrement_stats_fn(نياپو_معلومات, client_نياپو);
			plugin_persist_handle_client_نياپو_delete(context, client_نياپو);
			db_نياپو_store_ref_dec(&client_نياپو->بنياد_نياپو);
			mosquitto_FREE(client_نياپو);
		}
	}
}


فراغ db_check_acl_جو_all_messages(بنية mosquitto *context)
{
	db__client_messages_check_acl(context, &context->msgs_اندريون, &context->msgs_اندريون.inflight, &db_نياپو_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_اندريون, &context->msgs_اندريون.queued, &db_نياپو_remove_from_queued_stats);
	db__client_messages_check_acl(context, &context->msgs_ٻاهريون, &context->msgs_ٻاهريون.inflight, &db_نياپو_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_ٻاهريون, &context->msgs_ٻاهريون.queued, &db_نياپو_remove_from_queued_stats);
}


ساكن صحيح db_پيغام_write_inflight_ٻاهريون_single(بنية mosquitto *context, بنية mosquitto_client_نياپو *client_نياپو)
{
	بنية mosquitto_بنياد_نياپو *بنياد_نياپو;
	mosquitto_property *بنياد_نياپو_props = عدم;
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

	بنياد_نياپو = client_نياپو->بنياد_نياپو;

	expiry_interval = 0;
	إذا(بنياد_نياپو->معلومات.expiry_وقت){
		إذا(db.now_real_s > بنياد_نياپو->معلومات.expiry_وقت){
			/* Message is expired, must not send. */
			إذا(client_نياپو->معلومات.direction == mosq_md_ٻاهريون && client_نياپو->معلومات.qos > 0){
				util__increment_send_quota(context);
			}
			db_پيغام_remove_inflight(context, &context->msgs_ٻاهريون, client_نياپو);
			db_fill_inflight_ٻاهريون_from_قطار(context);
			أرجع MOSQ_ERR_SUCCESS;
		}وإلا{
			expiry_interval = (uint32_t)(بنياد_نياپو->معلومات.expiry_وقت - db.now_real_s);
		}
	}
	mid = client_نياپو->معلومات.mid;
	retries = client_نياپو->معلومات.dup;
	retain = client_نياپو->معلومات.retain;
	topic = بنياد_نياپو->معلومات.topic;
	qos = (uint8_t)client_نياپو->معلومات.qos;
	payloadlen = بنياد_نياپو->معلومات.payloadlen;
	payload = بنياد_نياپو->معلومات.payload;
	subscription_id = client_نياپو->معلومات.subscription_identifier;
	بنياد_نياپو_props = بنياد_نياپو->معلومات.properties;

	بدل(client_نياپو->معلومات.state){
		حالة mosq_ms_publish_qos0:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, بنياد_نياپو_props, expiry_interval);
			إذا(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_OVERSIZE_PACKET){
				db_پيغام_remove_inflight(context, &context->msgs_ٻاهريون, client_نياپو);
			}وإلا{
				أرجع rc;
			}
			اكسر;

		حالة mosq_ms_publish_qos1:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, بنياد_نياپو_props, expiry_interval);
			إذا(rc == MOSQ_ERR_SUCCESS){
				client_نياپو->معلومات.dup = 1; /* Any retry attempts are a duplicate. */
				client_نياپو->معلومات.state = mosq_ms_wait_for_puback;
				plugin_persist_handle_client_نياپو_update(context, client_نياپو);
			}وإلا إذا(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db_پيغام_remove_inflight(context, &context->msgs_ٻاهريون, client_نياپو);
			}وإلا{
				أرجع rc;
			}
			اكسر;

		حالة mosq_ms_publish_qos2:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, بنياد_نياپو_props, expiry_interval);
			إذا(rc == MOSQ_ERR_SUCCESS){
				client_نياپو->معلومات.dup = 1; /* Any retry attempts are a duplicate. */
				client_نياپو->معلومات.state = mosq_ms_wait_for_pubrec;
				plugin_persist_handle_client_نياپو_update(context, client_نياپو);
			}وإلا إذا(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db_پيغام_remove_inflight(context, &context->msgs_ٻاهريون, client_نياپو);
			}وإلا{
				أرجع rc;
			}
			اكسر;

		حالة mosq_ms_resend_pubrel:
			rc = send__pubrel(context, mid, عدم);
			إذا(!rc){
				client_نياپو->معلومات.state = mosq_ms_wait_for_pubcomp;
				plugin_persist_handle_client_نياپو_update(context, client_نياپو);
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


صحيح db_پيغام_write_inflight_ٻاهريون_all(بنية mosquitto *context)
{
	بنية mosquitto_client_نياپو *client_نياپو, *tmp;
	صحيح rc;

	إذا(context->state != mosq_cs_سرگرم || !net__is_connected(context)){
		أرجع MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_محفوظ(context->msgs_ٻاهريون.inflight, client_نياپو, tmp){
		rc = db_پيغام_write_inflight_ٻاهريون_single(context, client_نياپو);
		إذا(rc){
			أرجع rc;
		}
	}
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db_پيغام_write_inflight_ٻاهريون_latest(بنية mosquitto *context)
{
	بنية mosquitto_client_نياپو *client_نياپو, *ايندڙ;
	صحيح rc;

	إذا(context->state != mosq_cs_سرگرم
			|| !net__is_connected(context)
			|| context->msgs_ٻاهريون.inflight == عدم){

		أرجع MOSQ_ERR_SUCCESS;
	}

	إذا(context->msgs_ٻاهريون.inflight->prev == context->msgs_ٻاهريون.inflight){
		/* Only one message */
		أرجع db_پيغام_write_inflight_ٻاهريون_single(context, context->msgs_ٻاهريون.inflight);
	}

	/* Start at the end of the list and work backwards looking for the first
	 * message in a non-publish state */
	client_نياپو = context->msgs_ٻاهريون.inflight->prev;
	طالما(client_نياپو != context->msgs_ٻاهريون.inflight &&
			(client_نياپو->معلومات.state == mosq_ms_publish_qos0
			|| client_نياپو->معلومات.state == mosq_ms_publish_qos1
			|| client_نياپو->معلومات.state == mosq_ms_publish_qos2)){

		client_نياپو = client_نياپو->prev;
	}

	/* Tail is now either the head of the list, if that message is waiting for
	 * publish, or the oldest message not waiting for a publish. In the latter
	 * case, any pending publishes should be next after this message. */
	إذا(client_نياپو != context->msgs_ٻاهريون.inflight){
		client_نياپو = client_نياپو->ايندڙ;
	}

	طالما(client_نياپو){
		ايندڙ = client_نياپو->ايندڙ;
		rc = db_پيغام_write_inflight_ٻاهريون_single(context, client_نياپو);
		إذا(rc){
			أرجع rc;
		}
		client_نياپو = ايندڙ;
	}
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db_پيغام_write_queued_اندريون(بنية mosquitto *context)
{
	بنية mosquitto_client_نياپو *client_نياپو, *tmp;
	صحيح rc;

	إذا(context->state != mosq_cs_سرگرم){
		أرجع MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_محفوظ(context->msgs_اندريون.queued, client_نياپو, tmp){
		إذا(context->msgs_اندريون.inflight_وڌ_کان_وڌ != 0 && context->msgs_اندريون.inflight_quota == 0){
			اكسر;
		}

		إذا(client_نياپو->معلومات.qos == 2){
			client_نياپو->معلومات.state = mosq_ms_send_pubrec;
			db_پيغام_قطار_مان_ڪڍو_پهريون(context, &context->msgs_اندريون);
			rc = send__pubrec(context, client_نياپو->معلومات.mid, 0, عدم);
			إذا(!rc){
				client_نياپو->معلومات.state = mosq_ms_wait_for_pubrel;
				plugin_persist_handle_client_نياپو_update(context, client_نياپو);
			}وإلا{
				plugin_persist_handle_client_نياپو_update(context, client_نياپو);
				أرجع rc;
			}
		}
	}
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db_پيغام_write_queued_ٻاهريون(بنية mosquitto *context)
{
	إذا(context->state != mosq_cs_سرگرم){
		أرجع MOSQ_ERR_SUCCESS;
	}

	db_fill_inflight_ٻاهريون_from_قطار(context);

	أرجع MOSQ_ERR_SUCCESS;
}
