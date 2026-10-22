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
	بنية mosquitto_پیغام_انکڑے *msgs;
	bool valid_بائٹاں;
	bool valid_گنتی;

	إذا(dir == mosq_md_باہرلا){
		msgs = &context->msgs_باہرلا;
	}وإلا{
		msgs = &context->msgs_وچ;
	}

	إذا(msgs->inflight_ودھ_توں_ودھ == 0 && db.config->max_inflight_بائٹاں == 0){
		أرجع true;
	}

	إذا(qos == 0){
		/* Deliver QoS 0 messages unless the queue is already full.
		 * For QoS 0 messages the choice is either "inflight" or dropped.
		 * There is no queueing option, unless the client is offline and
		 * queue_qos0_messages is enabled.
		 */
		إذا(db.config->max_queued_messages == 0 && db.config->max_inflight_بائٹاں == 0){
			أرجع true;
		}
		valid_بائٹاں = ((msgs->inflight_بائٹاں - (ssize_t)db.config->max_inflight_بائٹاں) < (ssize_t)db.config->max_queued_بائٹاں);
		إذا(dir == mosq_md_باہرلا){
			valid_گنتی = context->باہرلا_packet_گنتی < db.config->max_queued_messages;
		}وإلا{
			valid_گنتی = msgs->inflight_گنتی - msgs->inflight_ودھ_توں_ودھ < db.config->max_queued_messages;
		}

		إذا(db.config->max_queued_messages == 0){
			أرجع valid_بائٹاں;
		}
		إذا(db.config->max_queued_بائٹاں == 0){
			أرجع valid_گنتی;
		}
	}وإلا{
		valid_بائٹاں = (ssize_t)msgs->inflight_bytes12 < (ssize_t)db.config->max_inflight_بائٹاں;
		valid_گنتی = msgs->inflight_quota > 0;

		إذا(msgs->inflight_ودھ_توں_ودھ == 0){
			أرجع valid_بائٹاں;
		}
		إذا(db.config->max_inflight_بائٹاں == 0){
			أرجع valid_گنتی;
		}
	}

	أرجع valid_بائٹاں && valid_گنتی;
}


/**
 * For a given client context, are more messages allowed to be queued?
 * It is assumed that inflight checks and queue_qos0 checks have already
 * been made.
 * @param context client of interest
 * @param qos destination qos for the packet of interest
 * @return true if queuing is allowed, false if should be dropped
 */
bool db_ready_for_قطار(بنية mosquitto *context, صحيح qos, بنية mosquitto_پیغام_انکڑے *پیغام_انکڑے)
{
	صحيح سروت_گنتی;
	صحيح adjust_گنتی;
	لما سروت_بائٹاں;
	ssize_t adjust_بائٹاں = (ssize_t)db.config->max_inflight_بائٹاں;
	bool valid_بائٹاں;
	bool valid_گنتی;

	إذا(db.config->max_queued_messages == 0 && db.config->max_queued_بائٹاں == 0){
		أرجع true;
	}

	إذا(qos == 0 && db.config->قطار_qos0_messages == false){
		أرجع false; /* This case is handled in db__ready_for_flight() */
	}وإلا{
		سروت_بائٹاں = (ssize_t)پیغام_انکڑے->queued_bytes12;
		سروت_گنتی = پیغام_انکڑے->queued_count12;
	}
	adjust_گنتی = پیغام_انکڑے->inflight_ودھ_توں_ودھ;

	/* nothing in flight for offline clients */
	إذا(!net__is_connected(context)){
		adjust_بائٹاں = 0;
		adjust_گنتی = 0;
	}

	valid_بائٹاں = (سروت_بائٹاں - (ssize_t)adjust_بائٹاں) < (ssize_t)db.config->max_queued_بائٹاں;
	valid_گنتی = سروت_گنتی - adjust_گنتی < db.config->max_queued_messages;

	إذا(db.config->max_queued_بائٹاں == 0){
		أرجع valid_گنتی;
	}
	إذا(db.config->max_queued_messages == 0){
		أرجع valid_بائٹاں;
	}

	أرجع valid_بائٹاں && valid_گنتی;
}


فراغ db_پیغام_جوڑو_منزل_inflight_stats(بنية mosquitto_پیغام_انکڑے *پیغام_انکڑے, بنية mosquitto_client_پیغام *client_پیغام)
{
	پیغام_انکڑے->inflight_گنتی++;
	پیغام_انکڑے->inflight_بائٹاں += client_پیغام->ادھار_پیغام->انکڑے.payloadlen;
	إذا(client_پیغام->انکڑے.qos != 0){
		پیغام_انکڑے->inflight_count12++;
		پیغام_انکڑے->inflight_bytes12 += client_پیغام->ادھار_پیغام->انکڑے.payloadlen;
	}
}


ساكن فراغ db_پیغام_remove_from_inflight_stats(بنية mosquitto_پیغام_انکڑے *پیغام_انکڑے, بنية mosquitto_client_پیغام *client_پیغام)
{
	پیغام_انکڑے->inflight_گنتی--;
	پیغام_انکڑے->inflight_بائٹاں -= client_پیغام->ادھار_پیغام->انکڑے.payloadlen;
	إذا(client_پیغام->انکڑے.qos != 0){
		پیغام_انکڑے->inflight_count12--;
		پیغام_انکڑے->inflight_bytes12 -= client_پیغام->ادھار_پیغام->انکڑے.payloadlen;
	}
}


فراغ db_پیغام_جوڑو_منزل_queued_stats(بنية mosquitto_پیغام_انکڑے *پیغام_انکڑے, بنية mosquitto_client_پیغام *client_پیغام)
{
	پیغام_انکڑے->queued_گنتی++;
	پیغام_انکڑے->queued_بائٹاں += client_پیغام->ادھار_پیغام->انکڑے.payloadlen;
	إذا(client_پیغام->انکڑے.qos != 0){
		پیغام_انکڑے->queued_count12++;
		پیغام_انکڑے->queued_bytes12 += client_پیغام->ادھار_پیغام->انکڑے.payloadlen;
	}
}


ساكن فراغ db_پیغام_remove_from_queued_stats(بنية mosquitto_پیغام_انکڑے *پیغام_انکڑے, بنية mosquitto_client_پیغام *client_پیغام)
{
	پیغام_انکڑے->queued_گنتی--;
	پیغام_انکڑے->queued_بائٹاں -= client_پیغام->ادھار_پیغام->انکڑے.payloadlen;
	إذا(client_پیغام->انکڑے.qos != 0){
		پیغام_انکڑے->queued_count12--;
		پیغام_انکڑے->queued_bytes12 -= client_پیغام->ادھار_پیغام->انکڑے.payloadlen;
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
#جے_تعریف_شدہ WITH_BRIDGE
	db.bridges = عدم;
	db.bridge_گنتی = 0;
#جے_انت

	/* Initialize the hashtable */
	db.clientid_اشاریہ_hash = عدم;

	db.عام_subs = عدم;
	db.shared_subs = عدم;

	sub__init();
	retain__init();

	db.config->security_options.unpwd = عدم;

#جے_تعریف_شدہ WITH_PERSISTENCE
	إذا(persist__restore()){
		أرجع 1;
	}
#جے_انت

	أرجع MOSQ_ERR_SUCCESS;
}


ساكن فراغ subhier_clean(بنية mosquitto__subhier **subhier)
{
	بنية mosquitto__subhier *peer, *subhier_tmp;
	بنية mosquitto__subleaf *پتی, *nextleaf;

	HASH_ITER(hh, *subhier, peer, subhier_tmp){
		پتی = peer->subs;
		طالما(پتی){
			nextleaf = پتی->اگلا;
			mosquitto_FREE(پتی);
			پتی = nextleaf;
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
	db_پیغام_store_clean();

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db_پیغام_store_جوڑو(بنية mosquitto_ادھار_پیغام *ادھار_پیغام)
{
	بنية mosquitto_ادھار_پیغام *لبھیا;
	بنانشانی hashv;

	HASH_قدر(&ادھار_پیغام->انکڑے.store_id, حجم(ادھار_پیغام->انکڑے.store_id), hashv);
	HASH_لبھو_BYHASHVALUE(hh, db.پیغام_store, &ادھار_پیغام->انکڑے.store_id, حجم(ادھار_پیغام->انکڑے.store_id), hashv, لبھیا);
	إذا(لبھیا == عدم){
		HASH_جوڑو_KEYPTR_BYHASHVALUE(hh, db.پیغام_store, &ادھار_پیغام->انکڑے.store_id, حجم(ادھار_پیغام->انکڑے.store_id), hashv, ادھار_پیغام);
		أرجع MOSQ_ERR_SUCCESS;
	}وإلا{
		أرجع MOSQ_ERR_ALREADY_EXISTS;
	}
}


فراغ db_پیغام_store_free(بنية mosquitto_ادھار_پیغام *ادھار_پیغام)
{
	mosquitto_FREE(ادھار_پیغام->انکڑے.سروت_id);
	mosquitto_FREE(ادھار_پیغام->انکڑے.سروت_username);
	إذا(ادھار_پیغام->dest_ids){
		لكل(صحيح i=0; i<ادھار_پیغام->dest_id_گنتی; i++){
			mosquitto_FREE(ادھار_پیغام->dest_ids[i]);
		}
		mosquitto_FREE(ادھار_پیغام->dest_ids);
	}
	mosquitto_FREE(ادھار_پیغام->انکڑے.topic);
	mosquitto_property_free_all(&ادھار_پیغام->انکڑے.properties);
	mosquitto_FREE(ادھار_پیغام->انکڑے.payload);
	mosquitto_FREE(ادھار_پیغام);
}


فراغ db_پیغام_store_remove(بنية mosquitto_ادھار_پیغام *ادھار_پیغام, bool notify)
{
	إذا(ادھار_پیغام == عدم){
		أرجع;
	}
	HASH_DELETE(hh, db.پیغام_store, ادھار_پیغام);
	db.پیغام_store_گنتی--;
	db.پیغام_store_بائٹاں -= ادھار_پیغام->انکڑے.payloadlen;
	إذا(notify == true){
		plugin_persist_handle_ادھار_پیغام_delete(ادھار_پیغام);
	}
	db_پیغام_store_free(ادھار_پیغام);
}


فراغ db_پیغام_store_clean(فراغ)
{
	بنية mosquitto_ادھار_پیغام *ادھار_پیغام, *ادھار_پیغام_tmp;

	HASH_ITER(hh, db.پیغام_store, ادھار_پیغام, ادھار_پیغام_tmp){
		db_پیغام_store_remove(ادھار_پیغام, false);
	}
}


فراغ db_پیغام_store_ref_inc(بنية mosquitto_ادھار_پیغام *ادھار_پیغام)
{
	ادھار_پیغام->ref_گنتی++;
}


فراغ db_پیغام_store_ref_dec(بنية mosquitto_ادھار_پیغام **ادھار_پیغام)
{
	(*ادھار_پیغام)->ref_گنتی--;
	إذا((*ادھار_پیغام)->ref_گنتی == 0){
		db_پیغام_store_remove(*ادھار_پیغام, true);
		*ادھار_پیغام = عدم;
	}
}


فراغ db_پیغام_store_compact(فراغ)
{
	بنية mosquitto_ادھار_پیغام *ادھار_پیغام, *ادھار_پیغام_tmp;

	HASH_ITER(hh, db.پیغام_store, ادھار_پیغام, ادھار_پیغام_tmp){
		إذا(ادھار_پیغام->ref_گنتی < 1){
			db_پیغام_store_remove(ادھار_پیغام, true);
		}
	}
}


ساكن فراغ db_سنیہا_remove_inflight(بنية mosquitto *context, بنية mosquitto_پیغام_انکڑے *پیغام_انکڑے, بنية mosquitto_client_پیغام *شے)
{
	إذا(!context || !پیغام_انکڑے || !شے){
		أرجع;
	}

	plugin_persist_handle_client_پیغام_delete(context, شے);

	DL_DELETE(پیغام_انکڑے->inflight, شے);
	إذا(شے->ادھار_پیغام){
		db_پیغام_remove_from_inflight_stats(پیغام_انکڑے, شے);
		db_پیغام_store_ref_dec(&شے->ادھار_پیغام);
	}

	mosquitto_FREE(شے);
}


ساكن فراغ db_سنیہا_remove_queued(بنية mosquitto *context, بنية mosquitto_پیغام_انکڑے *پیغام_انکڑے, بنية mosquitto_client_پیغام *شے)
{
	إذا(!context || !پیغام_انکڑے || !شے){
		أرجع;
	}

	plugin_persist_handle_client_پیغام_delete(context, شے);

	DL_DELETE(پیغام_انکڑے->queued, شے);
	إذا(شے->ادھار_پیغام){
		db_پیغام_remove_from_queued_stats(پیغام_انکڑے, شے);
		db_پیغام_store_ref_dec(&شے->ادھار_پیغام);
	}

	mosquitto_FREE(شے);
}


ساكن فراغ db_fill_inflight_باہرلا_from_قطار(بنية mosquitto *context)
{
	بنية mosquitto_client_پیغام *client_پیغام, *tmp;

	DL_FOREACH_محفوظ(context->msgs_باہرلا.queued, client_پیغام, tmp){
		إذا(!db__ready_for_flight(context, mosq_md_باہرلا, client_پیغام->انکڑے.qos)){
			أرجع;
		}
		بدل(client_پیغام->انکڑے.qos){
			حالة 0:
				client_پیغام->انکڑے.state = mosq_ms_publish_qos0;
				اكسر;
			حالة 1:
				client_پیغام->انکڑے.state = mosq_ms_publish_qos1;
				اكسر;
			حالة 2:
				client_پیغام->انکڑے.state = mosq_ms_publish_qos2;
				اكسر;
		}
		إذا(client_پیغام->ادھار_پیغام->انکڑے.expiry_ویلا && db.now_real_s > client_پیغام->ادھار_پیغام->انکڑے.expiry_ویلا){
			db_سنیہا_remove_queued(context, &context->msgs_باہرلا, client_پیغام);
			استمر;
		}
		plugin_persist_handle_client_پیغام_update(context, client_پیغام);
		db_سنیہا_قطار_توں_کڈھو_پہلا(context, &context->msgs_باہرلا);
	}
}


فراغ db_سنیہا_قطار_توں_کڈھو_پہلا(بنية mosquitto *context, بنية mosquitto_پیغام_انکڑے *پیغام_انکڑے)
{
	بنية mosquitto_client_پیغام *client_پیغام;

	بن_ورتیا(context);

	client_پیغام = پیغام_انکڑے->queued;
	DL_DELETE(پیغام_انکڑے->queued, client_پیغام);
	DL_APPEND(پیغام_انکڑے->inflight, client_پیغام);
	إذا(پیغام_انکڑے->inflight_quota > 0){
		پیغام_انکڑے->inflight_quota--;
	}

	db_پیغام_remove_from_queued_stats(پیغام_انکڑے, client_پیغام);
	db_پیغام_جوڑو_منزل_inflight_stats(پیغام_انکڑے, client_پیغام);
}


صحيح db_سنیہا_delete_outgoing(بنية mosquitto *context, uint16_t mid, تعداد mosquitto_پیغام_state expect_state, صحيح qos)
{
	بنية mosquitto_client_پیغام *client_پیغام, *tmp;
	bool deleted = false;

	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	DL_FOREACH_محفوظ(context->msgs_باہرلا.inflight, client_پیغام, tmp){
		إذا(client_پیغام->انکڑے.mid == mid){
			إذا(client_پیغام->انکڑے.qos != qos){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when deleting outgoing message.",
						context->id, client_پیغام->انکڑے.qos, qos);
				أرجع MOSQ_ERR_PROTOCOL;
			}وإلا إذا(qos == 2 && client_پیغام->انکڑے.state != expect_state && expect_state != mosq_ms_any){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched state (%d:%d) when deleting outgoing message.",
						context->id, client_پیغام->انکڑے.state, expect_state);
				أرجع MOSQ_ERR_PROTOCOL;
			}
			db_سنیہا_remove_inflight(context, &context->msgs_باہرلا, client_پیغام);
			deleted = true;
			اكسر;
		}
	}

	إذا(deleted == false){
		DL_FOREACH_محفوظ(context->msgs_باہرلا.queued, client_پیغام, tmp){
			إذا(client_پیغام->انکڑے.mid == mid){
				إذا(client_پیغام->انکڑے.qos != qos){
					أرجع MOSQ_ERR_PROTOCOL;
				}وإلا إذا(qos == 2 && client_پیغام->انکڑے.state != expect_state && expect_state != mosq_ms_any){
					أرجع MOSQ_ERR_PROTOCOL;
				}
				db_سنیہا_remove_queued(context, &context->msgs_باہرلا, client_پیغام);
				اكسر;
			}
		}
	}
	db_fill_inflight_باہرلا_from_قطار(context);
#جے_تعریف_شدہ WITH_PERSISTENCE
	db.persistence_changes++;
#جے_انت

	أرجع db_سنیہا_write_inflight_باہرلا_latest(context);
}


/* Only for QoS 2 messages */
صحيح db_سنیہا_داخل_کرو_incoming(بنية mosquitto *context, uint64_t cmsg_id, بنية mosquitto_ادھار_پیغام *ادھار_پیغام, bool persist)
{
	بنية mosquitto_client_پیغام *client_پیغام;
	بنية mosquitto_پیغام_انکڑے *پیغام_انکڑے;
	تعداد mosquitto_پیغام_state state = mosq_ms_invalid;
	صحيح rc = 0;

	أكد(ادھار_پیغام);
	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		أرجع MOSQ_ERR_SUCCESS;

	}
	پیغام_انکڑے = &context->msgs_وچ;

	إذا(db__ready_for_flight(context, mosq_md_وچ, ادھار_پیغام->انکڑے.qos)){
		state = mosq_ms_wait_for_pubrel;
	}وإلا إذا(ادھار_پیغام->انکڑے.qos != 0 && db_ready_for_قطار(context, ادھار_پیغام->انکڑے.qos, پیغام_انکڑے)){
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

#جے_تعریف_شدہ WITH_PERSISTENCE
	إذا(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#جے_انت

	client_پیغام = mosquitto_malloc(حجم(بنية mosquitto_client_پیغام));
	إذا(!client_پیغام){
		أرجع MOSQ_ERR_NOMEM;
	}
	client_پیغام->prev = عدم;
	client_پیغام->اگلا = عدم;
	إذا(cmsg_id){
		client_پیغام->انکڑے.cmsg_id = cmsg_id;
	}وإلا{
		client_پیغام->انکڑے.cmsg_id = ++context->آخری_cmsg_id;
	}
	client_پیغام->ادھار_پیغام = ادھار_پیغام;
	db_پیغام_store_ref_inc(client_پیغام->ادھار_پیغام);
	client_پیغام->انکڑے.mid = ادھار_پیغام->انکڑے.سروت_mid;
	client_پیغام->انکڑے.direction = mosq_md_وچ;
	client_پیغام->انکڑے.state = (uint8_t)state;
	client_پیغام->انکڑے.dup = false;
	إذا(ادھار_پیغام->انکڑے.qos > context->max_qos){
		client_پیغام->انکڑے.qos = context->max_qos;
	}وإلا{
		client_پیغام->انکڑے.qos = ادھار_پیغام->انکڑے.qos;
	}
	client_پیغام->انکڑے.retain = ادھار_پیغام->انکڑے.retain;
	client_پیغام->انکڑے.subscription_identifier = 0;

	إذا(state == mosq_ms_queued){
		DL_APPEND(پیغام_انکڑے->queued, client_پیغام);
		db_پیغام_جوڑو_منزل_queued_stats(پیغام_انکڑے, client_پیغام);
	}وإلا{
		DL_APPEND(پیغام_انکڑے->inflight, client_پیغام);
		db_پیغام_جوڑو_منزل_inflight_stats(پیغام_انکڑے, client_پیغام);
	}

	إذا(persist && context->is_persisted){
		plugin_persist_handle_ادھار_پیغام_جوڑو(client_پیغام->ادھار_پیغام);
		plugin_persist_handle_client_پیغام_جوڑو(context, client_پیغام);
	}

	إذا(client_پیغام->ادھار_پیغام->انکڑے.qos > 0){
		util__decrement_receive_quota(context);
	}
	أرجع rc;
}


صحيح db_سنیہا_داخل_کرو_outgoing(بنية mosquitto *context, uint64_t cmsg_id, uint16_t mid, uint8_t qos, bool retain, بنية mosquitto_ادھار_پیغام *ادھار_پیغام, uint32_t subscription_identifier, bool update, bool persist)
{
	بنية mosquitto_client_پیغام *client_پیغام;
	بنية mosquitto_پیغام_انکڑے *پیغام_انکڑے;
	تعداد mosquitto_پیغام_state state = mosq_ms_invalid;
	صحيح rc = 0;
	حرف **dest_ids;

	أكد(ادھار_پیغام);
	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}
	إذا(!context->id){
		/* Protect against unlikely "client is disconnected but not entirely freed" scenario */
		أرجع MOSQ_ERR_SUCCESS;

	}
	context->stats.messages_گھلیا++;

	پیغام_انکڑے = &context->msgs_باہرلا;

	/* Check whether we've already sent this message to this client
	 * for outgoing messages only.
	 * If retain==true then this is a stale retained message and so should be
	 * sent regardless. FIXME - this does mean retained messages will received
	 * multiple times for overlapping subscriptions, although this is only the
	 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
	 */
	إذا(context->protocol != mosq_p_mqtt5
			&& db.config->allow_duplicate_messages == false
			&& retain == false && ادھار_پیغام->dest_ids){

		لكل(صحيح i=0; i<ادھار_پیغام->dest_id_گنتی; i++){
			إذا(ادھار_پیغام->dest_ids[i] && !strcmp(ادھار_پیغام->dest_ids[i], context->id)){
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
		إذا(db__ready_for_flight(context, mosq_md_باہرلا, qos)){
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
		}وإلا إذا(qos != 0 && db_ready_for_قطار(context, qos, پیغام_انکڑے)){
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
		إذا(db_ready_for_قطار(context, qos, پیغام_انکڑے)){
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

#جے_تعریف_شدہ WITH_PERSISTENCE
	إذا(state == mosq_ms_queued){
		db.persistence_changes++;
	}
#جے_انت

	client_پیغام = mosquitto_malloc(حجم(بنية mosquitto_client_پیغام));
	إذا(!client_پیغام){
		أرجع MOSQ_ERR_NOMEM;
	}
	client_پیغام->prev = عدم;
	client_پیغام->اگلا = عدم;
	إذا(cmsg_id){
		client_پیغام->انکڑے.cmsg_id = cmsg_id;
	}وإلا{
		client_پیغام->انکڑے.cmsg_id = ++context->آخری_cmsg_id;
	}
	client_پیغام->ادھار_پیغام = ادھار_پیغام;
	db_پیغام_store_ref_inc(client_پیغام->ادھار_پیغام);
	client_پیغام->انکڑے.mid = mid;
	client_پیغام->انکڑے.direction = mosq_md_باہرلا;
	client_پیغام->انکڑے.state = (uint8_t)state;
	client_پیغام->انکڑے.dup = false;
	إذا(qos > context->max_qos){
		client_پیغام->انکڑے.qos = context->max_qos;
	}وإلا{
		client_پیغام->انکڑے.qos = qos;
	}
	client_پیغام->انکڑے.retain = retain;
	client_پیغام->انکڑے.subscription_identifier = subscription_identifier;

	إذا(state == mosq_ms_queued){
		DL_APPEND(پیغام_انکڑے->queued, client_پیغام);
		db_پیغام_جوڑو_منزل_queued_stats(پیغام_انکڑے, client_پیغام);
	}وإلا{
		DL_APPEND(پیغام_انکڑے->inflight, client_پیغام);
		db_پیغام_جوڑو_منزل_inflight_stats(پیغام_انکڑے, client_پیغام);
	}

	إذا(persist && context->is_persisted){
		plugin_persist_handle_ادھار_پیغام_جوڑو(client_پیغام->ادھار_پیغام);
		plugin_persist_handle_client_پیغام_جوڑو(context, client_پیغام);
	}

	إذا(db.config->allow_duplicate_messages == false && retain == false){
		/* Record which client ids this message has been sent to so we can avoid duplicates.
		 * Outgoing messages only.
		 * If retain==true then this is a stale retained message and so should be
		 * sent regardless. FIXME - this does mean retained messages will received
		 * multiple times for overlapping subscriptions, although this is only the
		 * case for SUBSCRIPTION with multiple subs in so is a minor concern.
		 */
		dest_ids = mosquitto_realloc(ادھار_پیغام->dest_ids, حجم(حرف *)*(size_t)(ادھار_پیغام->dest_id_گنتی+1));
		إذا(dest_ids){
			ادھار_پیغام->dest_ids = dest_ids;
			ادھار_پیغام->dest_id_گنتی++;
			ادھار_پیغام->dest_ids[ادھار_پیغام->dest_id_گنتی-1] = mosquitto_strdup(context->id);
			إذا(!ادھار_پیغام->dest_ids[ادھار_پیغام->dest_id_گنتی-1]){
				أرجع MOSQ_ERR_NOMEM;
			}
		}وإلا{
			أرجع MOSQ_ERR_NOMEM;
		}
	}
#جے_تعریف_شدہ WITH_BRIDGE
	إذا(context->bridge && context->bridge->شروع_type == bst_lazy
			&& !net__is_connected(context)
			&& context->msgs_باہرلا.inflight_گنتی + context->msgs_باہرلا.queued_گنتی >= context->bridge->حد){

		context->bridge->lazy_reconnect = true;
	}
#جے_انت

	إذا(client_پیغام->انکڑے.qos > 0 && state != mosq_ms_queued){
		util__decrement_send_quota(context);
	}

	إذا(update){
		rc = db_سنیہا_write_inflight_باہرلا_latest(context);
		إذا(rc){
			أرجع rc;
		}
		rc = db_سنیہا_write_queued_باہرلا(context);
		إذا(rc){
			أرجع rc;
		}
	}

	أرجع rc;
}


ساكن مضمن صحيح db_سنیہا_update_outgoing_state(بنية mosquitto *context, بنية mosquitto_client_پیغام *سر,
		uint16_t mid, تعداد mosquitto_پیغام_state state, صحيح qos, bool persist)
{
	بنية mosquitto_client_پیغام *client_پیغام;

	DL_FOREACH(سر, client_پیغام){
		إذا(client_پیغام->انکڑے.mid == mid){
			إذا(client_پیغام->انکڑے.qos != qos){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Mismatched QoS (%d:%d) when updating outgoing message.",
						context->id, client_پیغام->انکڑے.qos, qos);
				أرجع MOSQ_ERR_PROTOCOL;
			}
			client_پیغام->انکڑے.state = (uint8_t)state;
			إذا(persist){
				plugin_persist_handle_client_پیغام_update(context, client_پیغام);
			}
			أرجع MOSQ_ERR_SUCCESS;
		}
	}
	أرجع MOSQ_ERR_NOT_لبھیا;
}


صحيح db_سنیہا_update_outgoing(بنية mosquitto *context, uint16_t mid, تعداد mosquitto_پیغام_state state, صحيح qos, bool persist)
{
	صحيح rc;

	rc = db_سنیہا_update_outgoing_state(context, context->msgs_باہرلا.inflight, mid, state, qos, persist);
	إذا(!persist && rc == MOSQ_ERR_NOT_لبھیا){
		rc = db_سنیہا_update_outgoing_state(context, context->msgs_باہرلا.queued, mid, state, qos, persist);
	}
	أرجع rc;
}


ساكن فراغ db__messages_delete_list(بنية mosquitto_client_پیغام **سر)
{
	بنية mosquitto_client_پیغام *client_پیغام, *tmp;

	DL_FOREACH_محفوظ(*سر, client_پیغام, tmp){
		DL_DELETE(*سر, client_پیغام);
		db_پیغام_store_ref_dec(&client_پیغام->ادھار_پیغام);
		mosquitto_FREE(client_پیغام);
	}
	*سر = عدم;
}


صحيح db__messages_delete_incoming(بنية mosquitto *context)
{
	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_وچ.inflight);
	db__messages_delete_list(&context->msgs_وچ.queued);
	context->msgs_وچ.inflight_بائٹاں = 0;
	context->msgs_وچ.inflight_bytes12 = 0;
	context->msgs_وچ.inflight_گنتی = 0;
	context->msgs_وچ.inflight_count12 = 0;
	context->msgs_وچ.queued_بائٹاں = 0;
	context->msgs_وچ.queued_bytes12 = 0;
	context->msgs_وچ.queued_گنتی = 0;
	context->msgs_وچ.queued_count12 = 0;

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db__messages_delete_outgoing(بنية mosquitto *context)
{
	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	db__messages_delete_list(&context->msgs_باہرلا.inflight);
	db__messages_delete_list(&context->msgs_باہرلا.queued);
	context->msgs_باہرلا.inflight_بائٹاں = 0;
	context->msgs_باہرلا.inflight_bytes12 = 0;
	context->msgs_باہرلا.inflight_گنتی = 0;
	context->msgs_باہرلا.inflight_count12 = 0;
	context->msgs_باہرلا.queued_بائٹاں = 0;
	context->msgs_باہرلا.queued_bytes12 = 0;
	context->msgs_باہرلا.queued_گنتی = 0;
	context->msgs_باہرلا.queued_count12 = 0;

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


صحيح db_messages_easy_قطار(بنية mosquitto *context, ثابت حرف *topic, uint8_t qos, uint32_t payloadlen, ثابت فراغ *payload, صحيح retain, uint32_t سنیہا_expiry_interval, mosquitto_property **properties)
{
	بنية mosquitto_ادھار_پیغام *ادھار_پیغام;
	ثابت حرف *سروت_id;
	تعداد mosquitto_پیغام_origin origin;

	إذا(!topic){
		أرجع MOSQ_ERR_INVAL;
	}

	ادھار_پیغام = mosquitto_calloc(1, حجم(بنية mosquitto_ادھار_پیغام));
	إذا(ادھار_پیغام == عدم){
		أرجع MOSQ_ERR_NOMEM;
	}

	ادھار_پیغام->انکڑے.topic = mosquitto_strdup(topic);
	إذا(ادھار_پیغام->انکڑے.topic == عدم){
		db_پیغام_store_free(ادھار_پیغام);
		أرجع MOSQ_ERR_INVAL;
	}

	ادھار_پیغام->انکڑے.qos = qos;
	إذا(db.config->retain_دستیاب == false){
		ادھار_پیغام->انکڑے.retain = 0;
	}وإلا{
		ادھار_پیغام->انکڑے.retain = retain;
	}

	ادھار_پیغام->انکڑے.payloadlen = payloadlen;
	إذا(payloadlen > 0){
		ادھار_پیغام->انکڑے.payload = mosquitto_malloc(ادھار_پیغام->انکڑے.payloadlen+1);
		إذا(ادھار_پیغام->انکڑے.payload == عدم){
			db_پیغام_store_free(ادھار_پیغام);
			أرجع MOSQ_ERR_NOMEM;
		}
		/* Ensure payload is always zero terminated, this is the reason for the extra byte above */
		((uint8_t *)ادھار_پیغام->انکڑے.payload)[ادھار_پیغام->انکڑے.payloadlen] = 0;
		حافظہ_نقل(ادھار_پیغام->انکڑے.payload, payload, ادھار_پیغام->انکڑے.payloadlen);
	}

	إذا(context && context->id){
		سروت_id = context->id;
	}وإلا{
		سروت_id = "";
	}
	إذا(properties){
		ادھار_پیغام->انکڑے.properties = *properties;
		*properties = عدم;
	}

	إذا(context){
		origin = mosq_mo_client;
	}وإلا{
		origin = mosq_mo_broker;
	}
	إذا(db_سنیہا_store(context, ادھار_پیغام, &سنیہا_expiry_interval, origin)){
		أرجع 1;
	}

	أرجع sub_messages_قطار(سروت_id, ادھار_پیغام->انکڑے.qos, &ادھار_پیغام);
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
uint64_t db_new_پیغام_id(فراغ)
{
#جے_تعریف_شدہ WIN32
	FILETIME ftime;
	uint64_t ftime64;
#وإلا
	بنية timespec ts;
#جے_انت
	uint64_t id;
	uint64_t tmp;
	time_t sec;
	لما nsec;

	id = db.گرہ_id_shifted; /* Top 10-bits */

#جے_تعریف_شدہ WIN32
	Get_System_ویلا_Precise_As_File_ویلا(&ftime);
	ftime64 = (((uint64_t)ftime.dw_بلند_Date_ویلا)<<32) + ftime.dw_کم_Date_ویلا;
	tmp = ftime64 - 116444736000000000LL; /* Convert offset to unix epoch, still in counts of 100ns */
	sec = tmp / 10000000; /* Convert to seconds */
	nsec = (لما)(tmp - sec)*100; /* Remove seconds, convert to counts of 1ns */
#وإلا
	clock_gettime(CLOCK_REALTIME, &ts);
	sec = ts.tv_sec;
	nsec = ts.tv_nsec;
#جے_انت
	tmp = (sec - MOSQ_UUID_EPOCH) & 0x7FFFFFFF;
	id = id | (tmp << 23); /* Seconds, 31-bits (68 years) */

	tmp = (nsec & 0x7FFFFF80); /* top 23-bits of the bottom 30 bits (1 billion ns), ~100 ns resolution */
	id = id | (tmp >> 7);

	إذا(id <= db.آخری_db_id){
		id = db.آخری_db_id + 1;
	}
	db.آخری_db_id = id;

	أرجع id;
}


/* This function requires topic to be allocated on the heap. Once called, it owns topic and will free it on error. Likewise payload and properties. */
صحيح db_سنیہا_store(ثابت بنية mosquitto *سروت, بنية mosquitto_ادھار_پیغام *ادھار_پیغام, uint32_t *سنیہا_expiry_interval, تعداد mosquitto_پیغام_origin origin)
{
	صحيح rc;

	أكد(ادھار_پیغام);

	إذا(سروت && سروت->id){
		ادھار_پیغام->انکڑے.سروت_id = mosquitto_strdup(سروت->id);
	}وإلا{
		ادھار_پیغام->انکڑے.سروت_id = mosquitto_strdup("");
	}
	إذا(!ادھار_پیغام->انکڑے.سروت_id){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Out of memory.");
		db_پیغام_store_free(ادھار_پیغام);
		أرجع MOSQ_ERR_NOMEM;
	}

	إذا(سروت && سروت->username){
		ادھار_پیغام->انکڑے.سروت_username = mosquitto_strdup(سروت->username);
		إذا(!ادھار_پیغام->انکڑے.سروت_username){
			db_پیغام_store_free(ادھار_پیغام);
			أرجع MOSQ_ERR_NOMEM;
		}
	}
	إذا(سروت){
		ادھار_پیغام->سروت_listener = سروت->listener;
	}
	ادھار_پیغام->origin = origin;
	إذا(سنیہا_expiry_interval){
		إذا(*سنیہا_expiry_interval > 0 && *سنیہا_expiry_interval != پیغام_EXPIRY_INFINITE){
			ادھار_پیغام->انکڑے.expiry_ویلا = db.now_real_s + *سنیہا_expiry_interval;
		}وإلا{
			ادھار_پیغام->انکڑے.expiry_ویلا = 0;
		}
	}

	ادھار_پیغام->dest_ids = عدم;
	ادھار_پیغام->dest_id_گنتی = 0;
	db.پیغام_store_گنتی++;
	db.پیغام_store_بائٹاں += ادھار_پیغام->انکڑے.payloadlen;

	إذا(!ادھار_پیغام->انکڑے.store_id){
		ادھار_پیغام->انکڑے.store_id = db_new_پیغام_id();
	}

	rc = db_پیغام_store_جوڑو(ادھار_پیغام);
	إذا(rc){
		db_پیغام_store_free(ادھار_پیغام);
		أرجع rc;
	}

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db_سنیہا_store_لبھو(بنية mosquitto *context, uint16_t mid, بنية mosquitto_client_پیغام **client_پیغام)
{
	بنية mosquitto_client_پیغام *cmsg;

	*client_پیغام = عدم;

	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	DL_FOREACH(context->msgs_وچ.inflight, cmsg){
		إذا(cmsg->ادھار_پیغام->انکڑے.سروت_mid == mid){
			*client_پیغام = cmsg;
			أرجع MOSQ_ERR_SUCCESS;
		}
	}

	DL_FOREACH(context->msgs_وچ.queued, cmsg){
		إذا(cmsg->ادھار_پیغام->انکڑے.سروت_mid == mid){
			*client_پیغام = cmsg;
			أرجع MOSQ_ERR_SUCCESS;
		}
	}

	أرجع 1;
}


/* Called on reconnect to set outgoing messages to a sensible state and force a
 * retry, and to set incoming messages to expect an appropriate retry. */
ساكن صحيح db_سنیہا_reconnect_reset_outgoing(بنية mosquitto *context)
{
	بنية mosquitto_client_پیغام *client_پیغام, *tmp;

	context->msgs_باہرلا.inflight_بائٹاں = 0;
	context->msgs_باہرلا.inflight_bytes12 = 0;
	context->msgs_باہرلا.inflight_گنتی = 0;
	context->msgs_باہرلا.inflight_count12 = 0;
	context->msgs_باہرلا.queued_بائٹاں = 0;
	context->msgs_باہرلا.queued_bytes12 = 0;
	context->msgs_باہرلا.queued_گنتی = 0;
	context->msgs_باہرلا.queued_count12 = 0;
	context->msgs_باہرلا.inflight_quota = context->msgs_باہرلا.inflight_ودھ_توں_ودھ;

	DL_FOREACH_محفوظ(context->msgs_باہرلا.inflight, client_پیغام, tmp){
		db_پیغام_جوڑو_منزل_inflight_stats(&context->msgs_باہرلا, client_پیغام);
		إذا(client_پیغام->انکڑے.qos > 0){
			util__decrement_send_quota(context);
		}

		بدل(client_پیغام->انکڑے.qos){
			حالة 0:
				client_پیغام->انکڑے.state = mosq_ms_publish_qos0;
				اكسر;
			حالة 1:
				client_پیغام->انکڑے.state = mosq_ms_publish_qos1;
				اكسر;
			حالة 2:
				إذا(client_پیغام->انکڑے.state == mosq_ms_wait_for_pubcomp){
					client_پیغام->انکڑے.state = mosq_ms_resend_pubrel;
				}وإلا{
					client_پیغام->انکڑے.state = mosq_ms_publish_qos2;
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
	DL_FOREACH_محفوظ(context->msgs_باہرلا.queued, client_پیغام, tmp){
		db_پیغام_جوڑو_منزل_queued_stats(&context->msgs_باہرلا, client_پیغام);
	}
	db_fill_inflight_باہرلا_from_قطار(context);

	أرجع MOSQ_ERR_SUCCESS;
}


/* Called on reconnect to set incoming messages to expect an appropriate retry. */
ساكن صحيح db_سنیہا_reconnect_reset_incoming(بنية mosquitto *context)
{
	بنية mosquitto_client_پیغام *client_پیغام, *tmp;

	context->msgs_وچ.inflight_بائٹاں = 0;
	context->msgs_وچ.inflight_bytes12 = 0;
	context->msgs_وچ.inflight_گنتی = 0;
	context->msgs_وچ.inflight_count12 = 0;
	context->msgs_وچ.queued_بائٹاں = 0;
	context->msgs_وچ.queued_bytes12 = 0;
	context->msgs_وچ.queued_گنتی = 0;
	context->msgs_وچ.queued_count12 = 0;
	context->msgs_وچ.inflight_quota = context->msgs_وچ.inflight_ودھ_توں_ودھ;

	DL_FOREACH_محفوظ(context->msgs_وچ.inflight, client_پیغام, tmp){
		db_پیغام_جوڑو_منزل_inflight_stats(&context->msgs_وچ, client_پیغام);
		إذا(client_پیغام->انکڑے.qos > 0){
			util__decrement_receive_quota(context);
		}

		إذا(client_پیغام->انکڑے.qos != 2){
			/* Anything <QoS 2 can be completely retried by the client at
			 * no harm. */
			db_سنیہا_remove_inflight(context, &context->msgs_وچ, client_پیغام);
		}وإلا{
			/* Message state can be preserved here because it should match
			 * whatever the client has got. */
			client_پیغام->انکڑے.dup = 0;
		}
	}

	/* Messages received when the client was disconnected are put
	 * in the mosq_ms_queued state. If we don't change them to the
	 * appropriate "publish" state, then the queued messages won't
	 * get sent until the client next receives a message - and they
	 * will be sent out of order.
	 */
	DL_FOREACH_محفوظ(context->msgs_وچ.queued, client_پیغام, tmp){
		client_پیغام->انکڑے.dup = 0;
		db_پیغام_جوڑو_منزل_queued_stats(&context->msgs_وچ, client_پیغام);
		إذا(db__ready_for_flight(context, mosq_md_وچ, client_پیغام->انکڑے.qos)){
			بدل(client_پیغام->انکڑے.qos){
				حالة 0:
					client_پیغام->انکڑے.state = mosq_ms_publish_qos0;
					اكسر;
				حالة 1:
					client_پیغام->انکڑے.state = mosq_ms_publish_qos1;
					اكسر;
				حالة 2:
					client_پیغام->انکڑے.state = mosq_ms_publish_qos2;
					اكسر;
			}
			db_سنیہا_قطار_توں_کڈھو_پہلا(context, &context->msgs_وچ);
			plugin_persist_handle_client_پیغام_update(context, client_پیغام);
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db_سنیہا_reconnect_reset(بنية mosquitto *context)
{
	صحيح rc;

	rc = db_سنیہا_reconnect_reset_outgoing(context);
	إذا(rc){
		أرجع rc;
	}
	أرجع db_سنیہا_reconnect_reset_incoming(context);
}


صحيح db_سنیہا_remove_incoming(بنية mosquitto *context, uint16_t mid)
{
	بنية mosquitto_client_پیغام *client_پیغام, *tmp;

	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	DL_FOREACH_محفوظ(context->msgs_وچ.inflight, client_پیغام, tmp){
		إذا(client_پیغام->انکڑے.mid == mid){
			إذا(client_پیغام->ادھار_پیغام->انکڑے.qos != 2){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when deleting incoming message.",
						context->id, client_پیغام->ادھار_پیغام->انکڑے.qos);
				أرجع MOSQ_ERR_PROTOCOL;
			}
			db_سنیہا_remove_inflight(context, &context->msgs_وچ, client_پیغام);
			أرجع MOSQ_ERR_SUCCESS;
		}
	}

	أرجع MOSQ_ERR_NOT_لبھیا;
}


صحيح db_سنیہا_release_incoming(بنية mosquitto *context, uint16_t mid)
{
	بنية mosquitto_client_پیغام *client_پیغام, *tmp;
	bool deleted = false;
	صحيح rc;

	إذا(!context){
		أرجع MOSQ_ERR_INVAL;
	}

	DL_FOREACH_محفوظ(context->msgs_وچ.inflight, client_پیغام, tmp){
		إذا(client_پیغام->انکڑے.mid == mid){
			إذا(client_پیغام->ادھار_پیغام->انکڑے.qos != 2){
				log__printf(عدم, MOSQ_LOG_INFO, "Protocol error from %s: Incorrect QoS (%d) when releasing incoming message.",
						context->id, client_پیغام->ادھار_پیغام->انکڑے.qos);
				أرجع MOSQ_ERR_PROTOCOL;
			}
			ثابت حرف *topic = client_پیغام->ادھار_پیغام->انکڑے.topic;
			ثابت حرف *سروت_id = client_پیغام->ادھار_پیغام->انکڑے.سروت_id;

			/* topic==NULL should be a QoS 2 message that was
			 * denied/dropped and is being processed so the client doesn't
			 * keep resending it. That means we don't send it to other
			 * clients. */
			إذا(topic == عدم){
				db_سنیہا_remove_inflight(context, &context->msgs_وچ, client_پیغام);
				deleted = true;
			}وإلا{
				rc = sub_messages_قطار(سروت_id, 2, &client_پیغام->ادھار_پیغام);
				إذا(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_NO_SUBSCRIBERS){
					db_سنیہا_remove_inflight(context, &context->msgs_وچ, client_پیغام);
					deleted = true;
				}وإلا{
					أرجع 1;
				}
			}
		}
	}

	DL_FOREACH_محفوظ(context->msgs_وچ.queued, client_پیغام, tmp){
		إذا(db__ready_for_flight(context, mosq_md_وچ, client_پیغام->انکڑے.qos)){
			اكسر;
		}

		إذا(client_پیغام->انکڑے.qos == 2){
			send__pubrec(context, client_پیغام->انکڑے.mid, 0, عدم);
			client_پیغام->انکڑے.state = mosq_ms_wait_for_pubrel;
			db_سنیہا_قطار_توں_کڈھو_پہلا(context, &context->msgs_وچ);
			plugin_persist_handle_client_پیغام_update(context, client_پیغام);
		}
	}
	إذا(deleted){
		أرجع MOSQ_ERR_SUCCESS;
	}وإلا{
		أرجع MOSQ_ERR_NOT_لبھیا;
	}
}


فراغ db__expire_all_messages(بنية mosquitto *context)
{
	بنية mosquitto_client_پیغام *client_پیغام, *tmp;

	DL_FOREACH_محفوظ(context->msgs_باہرلا.inflight, client_پیغام, tmp){
		إذا(client_پیغام->ادھار_پیغام->انکڑے.expiry_ویلا && db.now_real_s > client_پیغام->ادھار_پیغام->انکڑے.expiry_ویلا){
			إذا(client_پیغام->انکڑے.qos > 0){
				util__increment_send_quota(context);
			}
			db_سنیہا_remove_inflight(context, &context->msgs_باہرلا, client_پیغام);
		}
	}
	db_fill_inflight_باہرلا_from_قطار(context);
	DL_FOREACH_محفوظ(context->msgs_باہرلا.queued, client_پیغام, tmp){
		إذا(client_پیغام->ادھار_پیغام->انکڑے.expiry_ویلا && db.now_real_s > client_پیغام->ادھار_پیغام->انکڑے.expiry_ویلا){
			db_سنیہا_remove_queued(context, &context->msgs_باہرلا, client_پیغام);
		}
	}
	DL_FOREACH_محفوظ(context->msgs_وچ.inflight, client_پیغام, tmp){
		إذا(client_پیغام->ادھار_پیغام->انکڑے.expiry_ویلا && db.now_real_s > client_پیغام->ادھار_پیغام->انکڑے.expiry_ویلا){
			إذا(client_پیغام->انکڑے.qos > 0){
				util__increment_receive_quota(context);
			}
			db_سنیہا_remove_inflight(context, &context->msgs_وچ, client_پیغام);
		}
	}
	DL_FOREACH_محفوظ(context->msgs_وچ.queued, client_پیغام, tmp){
		إذا(client_پیغام->ادھار_پیغام->انکڑے.expiry_ویلا && db.now_real_s > client_پیغام->ادھار_پیغام->انکڑے.expiry_ویلا){
			db_سنیہا_remove_queued(context, &context->msgs_وچ, client_پیغام);
		}
	}
}


ساكن فراغ db__client_messages_check_acl(بنية mosquitto *context, بنية mosquitto_پیغام_انکڑے *پیغام_انکڑے, بنية mosquitto_client_پیغام **سر,
		فراغ (*decrement_stats_fn)(بنية mosquitto_پیغام_انکڑے *پیغام_انکڑے, بنية mosquitto_client_پیغام *client_پیغام))
{
	بنية mosquitto_client_پیغام *client_پیغام, *tmp;
	بنية mosquitto_ادھار_پیغام *ادھار_پیغام;
	صحيح access;

	DL_FOREACH_محفوظ((*سر), client_پیغام, tmp){
		ادھار_پیغام = client_پیغام->ادھار_پیغام;
		إذا(client_پیغام->انکڑے.direction == mosq_md_باہرلا){
			access = MOSQ_ACL_READ;
		}وإلا{
			access = MOSQ_ACL_WRITE;
		}
		إذا(mosquitto_acl_check(context, ادھار_پیغام->انکڑے.topic,
				ادھار_پیغام->انکڑے.payloadlen, ادھار_پیغام->انکڑے.payload,
				ادھار_پیغام->انکڑے.qos, ادھار_پیغام->انکڑے.retain,
				ادھار_پیغام->انکڑے.properties, access) != MOSQ_ERR_SUCCESS){

			DL_DELETE((*سر), client_پیغام);
			decrement_stats_fn(پیغام_انکڑے, client_پیغام);
			plugin_persist_handle_client_پیغام_delete(context, client_پیغام);
			db_پیغام_store_ref_dec(&client_پیغام->ادھار_پیغام);
			mosquitto_FREE(client_پیغام);
		}
	}
}


فراغ db_check_acl_دا_all_messages(بنية mosquitto *context)
{
	db__client_messages_check_acl(context, &context->msgs_وچ, &context->msgs_وچ.inflight, &db_پیغام_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_وچ, &context->msgs_وچ.queued, &db_پیغام_remove_from_queued_stats);
	db__client_messages_check_acl(context, &context->msgs_باہرلا, &context->msgs_باہرلا.inflight, &db_پیغام_remove_from_inflight_stats);
	db__client_messages_check_acl(context, &context->msgs_باہرلا, &context->msgs_باہرلا.queued, &db_پیغام_remove_from_queued_stats);
}


ساكن صحيح db_سنیہا_write_inflight_باہرلا_single(بنية mosquitto *context, بنية mosquitto_client_پیغام *client_پیغام)
{
	بنية mosquitto_ادھار_پیغام *ادھار_پیغام;
	mosquitto_property *ادھار_پیغام_props = عدم;
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

	ادھار_پیغام = client_پیغام->ادھار_پیغام;

	expiry_interval = 0;
	إذا(ادھار_پیغام->انکڑے.expiry_ویلا){
		إذا(db.now_real_s > ادھار_پیغام->انکڑے.expiry_ویلا){
			/* Message is expired, must not send. */
			إذا(client_پیغام->انکڑے.direction == mosq_md_باہرلا && client_پیغام->انکڑے.qos > 0){
				util__increment_send_quota(context);
			}
			db_سنیہا_remove_inflight(context, &context->msgs_باہرلا, client_پیغام);
			db_fill_inflight_باہرلا_from_قطار(context);
			أرجع MOSQ_ERR_SUCCESS;
		}وإلا{
			expiry_interval = (uint32_t)(ادھار_پیغام->انکڑے.expiry_ویلا - db.now_real_s);
		}
	}
	mid = client_پیغام->انکڑے.mid;
	retries = client_پیغام->انکڑے.dup;
	retain = client_پیغام->انکڑے.retain;
	topic = ادھار_پیغام->انکڑے.topic;
	qos = (uint8_t)client_پیغام->انکڑے.qos;
	payloadlen = ادھار_پیغام->انکڑے.payloadlen;
	payload = ادھار_پیغام->انکڑے.payload;
	subscription_id = client_پیغام->انکڑے.subscription_identifier;
	ادھار_پیغام_props = ادھار_پیغام->انکڑے.properties;

	بدل(client_پیغام->انکڑے.state){
		حالة mosq_ms_publish_qos0:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, ادھار_پیغام_props, expiry_interval);
			إذا(rc == MOSQ_ERR_SUCCESS || rc == MOSQ_ERR_OVERSIZE_PACKET){
				db_سنیہا_remove_inflight(context, &context->msgs_باہرلا, client_پیغام);
			}وإلا{
				أرجع rc;
			}
			اكسر;

		حالة mosq_ms_publish_qos1:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, ادھار_پیغام_props, expiry_interval);
			إذا(rc == MOSQ_ERR_SUCCESS){
				client_پیغام->انکڑے.dup = 1; /* Any retry attempts are a duplicate. */
				client_پیغام->انکڑے.state = mosq_ms_wait_for_puback;
				plugin_persist_handle_client_پیغام_update(context, client_پیغام);
			}وإلا إذا(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db_سنیہا_remove_inflight(context, &context->msgs_باہرلا, client_پیغام);
			}وإلا{
				أرجع rc;
			}
			اكسر;

		حالة mosq_ms_publish_qos2:
			rc = send__publish(context, mid, topic, payloadlen, payload, qos, retain, retries, subscription_id, ادھار_پیغام_props, expiry_interval);
			إذا(rc == MOSQ_ERR_SUCCESS){
				client_پیغام->انکڑے.dup = 1; /* Any retry attempts are a duplicate. */
				client_پیغام->انکڑے.state = mosq_ms_wait_for_pubrec;
				plugin_persist_handle_client_پیغام_update(context, client_پیغام);
			}وإلا إذا(rc == MOSQ_ERR_OVERSIZE_PACKET){
				db_سنیہا_remove_inflight(context, &context->msgs_باہرلا, client_پیغام);
			}وإلا{
				أرجع rc;
			}
			اكسر;

		حالة mosq_ms_resend_pubrel:
			rc = send__pubrel(context, mid, عدم);
			إذا(!rc){
				client_پیغام->انکڑے.state = mosq_ms_wait_for_pubcomp;
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


صحيح db_سنیہا_write_inflight_باہرلا_all(بنية mosquitto *context)
{
	بنية mosquitto_client_پیغام *client_پیغام, *tmp;
	صحيح rc;

	إذا(context->state != mosq_cs_سرگرم || !net__is_connected(context)){
		أرجع MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_محفوظ(context->msgs_باہرلا.inflight, client_پیغام, tmp){
		rc = db_سنیہا_write_inflight_باہرلا_single(context, client_پیغام);
		إذا(rc){
			أرجع rc;
		}
	}
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db_سنیہا_write_inflight_باہرلا_latest(بنية mosquitto *context)
{
	بنية mosquitto_client_پیغام *client_پیغام, *اگلا;
	صحيح rc;

	إذا(context->state != mosq_cs_سرگرم
			|| !net__is_connected(context)
			|| context->msgs_باہرلا.inflight == عدم){

		أرجع MOSQ_ERR_SUCCESS;
	}

	إذا(context->msgs_باہرلا.inflight->prev == context->msgs_باہرلا.inflight){
		/* Only one message */
		أرجع db_سنیہا_write_inflight_باہرلا_single(context, context->msgs_باہرلا.inflight);
	}

	/* Start at the end of the list and work backwards looking for the first
	 * message in a non-publish state */
	client_پیغام = context->msgs_باہرلا.inflight->prev;
	طالما(client_پیغام != context->msgs_باہرلا.inflight &&
			(client_پیغام->انکڑے.state == mosq_ms_publish_qos0
			|| client_پیغام->انکڑے.state == mosq_ms_publish_qos1
			|| client_پیغام->انکڑے.state == mosq_ms_publish_qos2)){

		client_پیغام = client_پیغام->prev;
	}

	/* Tail is now either the head of the list, if that message is waiting for
	 * publish, or the oldest message not waiting for a publish. In the latter
	 * case, any pending publishes should be next after this message. */
	إذا(client_پیغام != context->msgs_باہرلا.inflight){
		client_پیغام = client_پیغام->اگلا;
	}

	طالما(client_پیغام){
		اگلا = client_پیغام->اگلا;
		rc = db_سنیہا_write_inflight_باہرلا_single(context, client_پیغام);
		إذا(rc){
			أرجع rc;
		}
		client_پیغام = اگلا;
	}
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db_سنیہا_write_queued_وچ(بنية mosquitto *context)
{
	بنية mosquitto_client_پیغام *client_پیغام, *tmp;
	صحيح rc;

	إذا(context->state != mosq_cs_سرگرم){
		أرجع MOSQ_ERR_SUCCESS;
	}

	DL_FOREACH_محفوظ(context->msgs_وچ.queued, client_پیغام, tmp){
		إذا(context->msgs_وچ.inflight_ودھ_توں_ودھ != 0 && context->msgs_وچ.inflight_quota == 0){
			اكسر;
		}

		إذا(client_پیغام->انکڑے.qos == 2){
			client_پیغام->انکڑے.state = mosq_ms_send_pubrec;
			db_سنیہا_قطار_توں_کڈھو_پہلا(context, &context->msgs_وچ);
			rc = send__pubrec(context, client_پیغام->انکڑے.mid, 0, عدم);
			إذا(!rc){
				client_پیغام->انکڑے.state = mosq_ms_wait_for_pubrel;
				plugin_persist_handle_client_پیغام_update(context, client_پیغام);
			}وإلا{
				plugin_persist_handle_client_پیغام_update(context, client_پیغام);
				أرجع rc;
			}
		}
	}
	أرجع MOSQ_ERR_SUCCESS;
}


صحيح db_سنیہا_write_queued_باہرلا(بنية mosquitto *context)
{
	إذا(context->state != mosq_cs_سرگرم){
		أرجع MOSQ_ERR_SUCCESS;
	}

	db_fill_inflight_باہرلا_from_قطار(context);

	أرجع MOSQ_ERR_SUCCESS;
}
