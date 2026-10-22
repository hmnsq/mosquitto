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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_SYS_ਰੁੱਖ

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <math.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <inttypes.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <limits.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <inttypes.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sys_tree.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BUFLEN 100

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SYS_ਰੁੱਖ_QOS 2

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ METRIC_ਲੱਦੋ_1_MIN 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ METRIC_ਲੱਦੋ_5_MIN 2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ METRIC_ਲੱਦੋ_15_MIN 3

ਬਣਤਰ metric {
	int64_t current;
	int64_t ਅਗਲਾ;
	ਅਚਲ ਅੱਖਰ *topic, *topic_alias;
	bool is_max;
};

ਬਣਤਰ metric_ਲੱਦੋ {
	ਦੁੱਗਣਾ current;
	ਅਚਲ ਅੱਖਰ *topic;
	ਪੂਰਨ ਲੱਦੋ_ref;
	uint8_t interval;
};

ਬਣਤਰ metric metrics[mosq_metric_max] = {
	{ 1, 0, "$SYS/broker/clients/total", ਨਿਰਾ, false }, /* mosq_gauge_clients_total */
	{ 1, 0, "$SYS/broker/clients/maximum", ਨਿਰਾ, true }, /* metric_clients_maximum */
	{ 1, 0, "$SYS/broker/clients/disconnected", "$SYS/broker/clients/inactive", false }, /* mosq_gauge_clients_disconnected */
	{ 1, 0, "$SYS/broker/clients/connected", "$SYS/broker/clients/active", false }, /* mosq_gauge_clients_connected */
	{ 1, 0, "$SYS/broker/clients/expired", ਨਿਰਾ, false }, /* mosq_counter_clients_expired */
	{ 1, 0, "$SYS/broker/messages/stored", "$SYS/broker/store/messages/count", false }, /* mosq_gauge_message_store_count */
	{ 1, 0, "$SYS/broker/store/messages/bytes", ਨਿਰਾ, false }, /* mosq_gauge_message_store_bytes */
	{ 1, 0, "$SYS/broker/subscriptions/count", ਨਿਰਾ, false }, /* mosq_gauge_subscription_count */
	{ 1, 0, "$SYS/broker/shared_subscriptions/count", ਨਿਰਾ, false }, /* mosq_gauge_shared_subscription_count */
	{ 1, 0, "$SYS/broker/retained messages/count", ਨਿਰਾ, false }, /* mosq_gauge_retained_message_count */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_ਸਮ੍ਰਿਤੀ_TRACKING
	{ 1, 0, "$SYS/broker/heap/current", ਨਿਰਾ, false }, /* mosq_gauge_heap_current */
	{ 1, 0, "$SYS/broker/heap/maximum", ਨਿਰਾ, true }, /* mosq_gauge_heap_maximum */
#ਨਹੀਂਤਾਂ
	{ 1, 0, ਨਿਰਾ, ਨਿਰਾ, 0 }, /* mosq_gauge_heap_current */
	{ 1, 0, ਨਿਰਾ, ਨਿਰਾ, 0 }, /* mosq_gauge_heap_maximum */
#ਜੇ_ਅੰਤ
	{ 1, 0, "$SYS/broker/messages/received", ਨਿਰਾ, false }, /* mosq_counter_messages_received */
	{ 1, 0, "$SYS/broker/messages/sent", ਨਿਰਾ, false }, /* mosq_counter_messages_sent */
	{ 1, 0, "$SYS/broker/bytes/received", ਨਿਰਾ, false }, /* mosq_counter_bytes_received */
	{ 1, 0, "$SYS/broker/bytes/sent", ਨਿਰਾ, false }, /* mosq_counter_bytes_sent */
	{ 1, 0, "$SYS/broker/publish/bytes/received", ਨਿਰਾ, false }, /* mosq_counter_pub_bytes_received */
	{ 1, 0, "$SYS/broker/publish/bytes/sent", ਨਿਰਾ, false }, /* mosq_counter_pub_bytes_sent */
	{ 1, 0, "$SYS/broker/packet/out/count", ਨਿਰਾ, false }, /* mosq_gauge_out_packet_count */
	{ 1, 0, "$SYS/broker/packet/out/bytes", ਨਿਰਾ, false }, /* mosq_gauge_out_packet_bytes */
	{ 1, 0, "$SYS/broker/connections/socket/count", ਨਿਰਾ, false }, /* mosq_counter_socket_connections */
	{ 1, 0, ਨਿਰਾ, ਨਿਰਾ, false }, /* mosq_counter_mqtt_connect_received */
	{ 1, 0, ਨਿਰਾ, ਨਿਰਾ, false }, /* mosq_counter_mqtt_connect_sent */
	{ 1, 0, ਨਿਰਾ, ਨਿਰਾ, false }, /* mosq_counter_mqtt_connack_received */
	{ 1, 0, ਨਿਰਾ, ਨਿਰਾ, false }, /* mosq_counter_mqtt_connack_sent */
	{ 1, 0, "$SYS/broker/publish/messages/dropped", ਨਿਰਾ, false }, /* mosq_counter_mqtt_publish_dropped */
	{ 1, 0, "$SYS/broker/publish/messages/received", ਨਿਰਾ, false }, /* mosq_counter_mqtt_publish_received */
	{ 1, 0, "$SYS/broker/publish/messages/sent", ਨਿਰਾ, false }, /* mosq_counter_mqtt_publish_sent */
	{ 1, 0, ਨਿਰਾ, ਨਿਰਾ, false }, /* mosq_counter_mqtt_puback_received */
	{ 1, 0, ਨਿਰਾ, ਨਿਰਾ, false }, /* mosq_counter_mqtt_puback_sent */
	{ 1, 0, ਨਿਰਾ, ਨਿਰਾ, false }, /* mosq_counter_mqtt_pubrec_received */
	{ 1, 0, ਨਿਰਾ, ਨਿਰਾ, false }, /* mosq_counter_mqtt_pubrec_sent */
	{ 1, 0, ਨਿਰਾ, ਨਿਰਾ, false }, /* mosq_counter_mqtt_pubrel_received */
	{ 1, 0, ਨਿਰਾ, ਨਿਰਾ, false }, /* mosq_counter_mqtt_pubrel_sent */
	{ 1, 0, ਨਿਰਾ, ਨਿਰਾ, false }, /* mosq_counter_mqtt_pubcomp_received */
	{ 1, 0, ਨਿਰਾ, ਨਿਰਾ, false }, /* mosq_counter_mqtt_pubcomp_sent */
	{ 1, 0, ਨਿਰਾ, ਨਿਰਾ, false }, /* mosq_counter_mqtt_subscribe_received */
	{ 1, 0, ਨਿਰਾ, ਨਿਰਾ, false }, /* mosq_counter_mqtt_subscribe_sent */
	{ 1, 0, ਨਿਰਾ, ਨਿਰਾ, false }, /* mosq_counter_mqtt_suback_received */
	{ 1, 0, ਨਿਰਾ, ਨਿਰਾ, false }, /* mosq_counter_mqtt_suback_sent */
	{ 1, 0, ਨਿਰਾ, ਨਿਰਾ, false }, /* mosq_counter_mqtt_unsubscribe_received */
	{ 1, 0, ਨਿਰਾ, ਨਿਰਾ, false }, /* mosq_counter_mqtt_unsubscribe_sent */
	{ 1, 0, ਨਿਰਾ, ਨਿਰਾ, false }, /* mosq_counter_mqtt_unsuback_received */
	{ 1, 0, ਨਿਰਾ, ਨਿਰਾ, false }, /* mosq_counter_mqtt_unsuback_sent */
	{ 1, 0, ਨਿਰਾ, ਨਿਰਾ, false }, /* mosq_counter_mqtt_pingreq_received */
	{ 1, 0, ਨਿਰਾ, ਨਿਰਾ, false }, /* mosq_counter_mqtt_pingreq_sent */
	{ 1, 0, ਨਿਰਾ, ਨਿਰਾ, false }, /* mosq_counter_mqtt_pingresp_received */
	{ 1, 0, ਨਿਰਾ, ਨਿਰਾ, false }, /* mosq_counter_mqtt_pingresp_sent */
	{ 1, 0, ਨਿਰਾ, ਨਿਰਾ, false }, /* mosq_counter_mqtt_disconnect_received */
	{ 1, 0, ਨਿਰਾ, ਨਿਰਾ, false }, /* mosq_counter_mqtt_disconnect_sent */
	{ 1, 0, ਨਿਰਾ, ਨਿਰਾ, false }, /* mosq_counter_mqtt_auth_received */
	{ 1, 0, ਨਿਰਾ, ਨਿਰਾ, false }, /* mosq_counter_mqtt_auth_sent */
};

ਬਣਤਰ metric_ਲੱਦੋ metric_loads[mosq_metric_ਲੱਦੋ_max] = {
	{ 0.0, "$SYS/broker/load/messages/received/1min", mosq_counter_messages_received, METRIC_ਲੱਦੋ_1_MIN }, /* metric_load_messages_received_1min */
	{ 0.0, "$SYS/broker/load/messages/received/5min", mosq_counter_messages_received, METRIC_ਲੱਦੋ_5_MIN }, /* metric_load_messages_received_5min */
	{ 0.0, "$SYS/broker/load/messages/received/15min", mosq_counter_messages_received, METRIC_ਲੱਦੋ_15_MIN }, /* metric_load_messages_received_15min */
	{ 0.0, "$SYS/broker/load/messages/sent/1min", mosq_counter_messages_ਭੇਜਿਆ, METRIC_ਲੱਦੋ_1_MIN }, /* metric_load_messages_sent_1min */
	{ 0.0, "$SYS/broker/load/messages/sent/5min", mosq_counter_messages_ਭੇਜਿਆ, METRIC_ਲੱਦੋ_5_MIN }, /* metric_load_messages_sent_5min */
	{ 0.0, "$SYS/broker/load/messages/sent/15min", mosq_counter_messages_ਭੇਜਿਆ, METRIC_ਲੱਦੋ_15_MIN }, /* metric_load_messages_sent_15min */
	{ 0.0, "$SYS/broker/load/publish/dropped/1min", mosq_counter_mqtt_publish_dropped, METRIC_ਲੱਦੋ_1_MIN }, /* metric_load_pub_messages_dropped_1min */
	{ 0.0, "$SYS/broker/load/publish/dropped/5min", mosq_counter_mqtt_publish_dropped, METRIC_ਲੱਦੋ_5_MIN }, /* metric_load_pub_messages_dropped_5min */
	{ 0.0, "$SYS/broker/load/publish/dropped/15min", mosq_counter_mqtt_publish_dropped, METRIC_ਲੱਦੋ_15_MIN }, /* metric_load_pub_messages_dropped_15min */
	{ 0.0, "$SYS/broker/load/publish/received/1min", mosq_counter_mqtt_publish_received, METRIC_ਲੱਦੋ_1_MIN }, /* metric_load_pub_messages_received_1min */
	{ 0.0, "$SYS/broker/load/publish/received/5min", mosq_counter_mqtt_publish_received, METRIC_ਲੱਦੋ_5_MIN }, /* metric_load_pub_messages_received_5min */
	{ 0.0, "$SYS/broker/load/publish/received/15min", mosq_counter_mqtt_publish_received, METRIC_ਲੱਦੋ_15_MIN }, /* metric_load_pub_messages_received_15min */
	{ 0.0, "$SYS/broker/load/publish/sent/1min", mosq_counter_mqtt_publish_ਭੇਜਿਆ, METRIC_ਲੱਦੋ_1_MIN }, /* metric_load_pub_messages_sent_1min */
	{ 0.0, "$SYS/broker/load/publish/sent/5min", mosq_counter_mqtt_publish_ਭੇਜਿਆ, METRIC_ਲੱਦੋ_5_MIN }, /* metric_load_pub_messages_sent_5min */
	{ 0.0, "$SYS/broker/load/publish/sent/15min", mosq_counter_mqtt_publish_ਭੇਜਿਆ, METRIC_ਲੱਦੋ_15_MIN }, /* metric_load_pub_messages_sent_15min */
	{ 0.0, "$SYS/broker/load/bytes/received/1min", mosq_counter_ਬਾਈਟਾਂ_received, METRIC_ਲੱਦੋ_1_MIN }, /* metric_load_bytes_received_1min */
	{ 0.0, "$SYS/broker/load/bytes/received/5min", mosq_counter_ਬਾਈਟਾਂ_received, METRIC_ਲੱਦੋ_5_MIN }, /* metric_load_bytes_received_5min */
	{ 0.0, "$SYS/broker/load/bytes/received/15min", mosq_counter_ਬਾਈਟਾਂ_received, METRIC_ਲੱਦੋ_15_MIN }, /* metric_load_bytes_received_15min */
	{ 0.0, "$SYS/broker/load/bytes/sent/1min", mosq_counter_ਬਾਈਟਾਂ_ਭੇਜਿਆ, METRIC_ਲੱਦੋ_1_MIN }, /* metric_load_bytes_sent_1min */
	{ 0.0, "$SYS/broker/load/bytes/sent/5min", mosq_counter_ਬਾਈਟਾਂ_ਭੇਜਿਆ, METRIC_ਲੱਦੋ_5_MIN }, /* metric_load_bytes_sent_5min */
	{ 0.0, "$SYS/broker/load/bytes/sent/15min", mosq_counter_ਬਾਈਟਾਂ_ਭੇਜਿਆ, METRIC_ਲੱਦੋ_15_MIN }, /* metric_load_bytes_sent_15min */
	{ 0.0, "$SYS/broker/load/sockets/1min", mosq_counter_socket_connections, METRIC_ਲੱਦੋ_1_MIN }, /* metric_load_socket_connections_1min */
	{ 0.0, "$SYS/broker/load/sockets/5min", mosq_counter_socket_connections, METRIC_ਲੱਦੋ_5_MIN }, /* metric_load_socket_connections_5min */
	{ 0.0, "$SYS/broker/load/sockets/15min", mosq_counter_socket_connections, METRIC_ਲੱਦੋ_15_MIN }, /* metric_load_socket_connections_15min */
	{ 0.0, "$SYS/broker/load/connections/1min", mosq_counter_mqtt_connect_received, METRIC_ਲੱਦੋ_1_MIN }, /* metric_load_connections_1min */
	{ 0.0, "$SYS/broker/load/connections/5min", mosq_counter_mqtt_connect_received, METRIC_ਲੱਦੋ_5_MIN }, /* metric_load_connections_5min */
	{ 0.0, "$SYS/broker/load/connections/15min", mosq_counter_mqtt_connect_received, METRIC_ਲੱਦੋ_15_MIN }, /* metric_load_connections_15min */
};

ਸਥਿਰ time_t ਸ਼ੁਰੂ_ਸਮਾਂ = 0;
ਸਥਿਰ time_t ਆਖਰੀ_update = 0;


time_t broker_uptime(ਖਾਲੀ)
{
	ਵਾਪਸ db.now_s - ਸ਼ੁਰੂ_ਸਮਾਂ;
}


ਸਥਿਰ ਖਾਲੀ calc_ਲੱਦੋ(ਅੱਖਰ *buf, ਦੁੱਗਣਾ exponent, ਦੁੱਗਣਾ i_mult, ਬਣਤਰ metric_ਲੱਦੋ *m, bool force)
{
	ਦੁੱਗਣਾ new_ਮੁੱਲ;
	uint32_t len;
	ਦੁੱਗਣਾ interval;

	interval = (ਦੁੱਗਣਾ)(metrics[m->ਲੱਦੋ_ref].ਅਗਲਾ - metrics[m->ਲੱਦੋ_ref].current)*i_mult;
	new_ਮੁੱਲ = interval + exponent*(m->current - interval);
	ਜੇ(fabs(new_ਮੁੱਲ - (m->current)) >= 0.01 || force){
		len = (uint32_t)snprintf(buf, BUFLEN, "%.2f", new_ਮੁੱਲ);
		db_messages_easy_ਕਤਾਰ(ਨਿਰਾ, m->topic, SYS_ਰੁੱਖ_QOS, len, buf, 1, ਸੁਨੇਹਾ_EXPIRY_INFINITE, ਨਿਰਾ);
	}
	m->current = new_ਮੁੱਲ;
}


ਖਾਲੀ sys_ਰੁੱਖ_init(ਖਾਲੀ)
{
	ਅੱਖਰ buf[BUFLEN];
	uint32_t len;

	ਜੇ(db.config->sys_interval == 0){
		ਵਾਪਸ;
	}

	/* Set static $SYS messages */
	len = (uint32_t)snprintf(buf, 64, "mosquitto version %s", ਸੰਸਕਰਣ);
	db_messages_easy_ਕਤਾਰ(ਨਿਰਾ, "$SYS/broker/version", SYS_ਰੁੱਖ_QOS, len, buf, 1, ਸੁਨੇਹਾ_EXPIRY_INFINITE, ਨਿਰਾ);

	ਸ਼ੁਰੂ_ਸਮਾਂ = mosquitto_ਸਮਾਂ();
	ਆਖਰੀ_update = ਸ਼ੁਰੂ_ਸਮਾਂ;

	sys_ਰੁੱਖ_update(true);

	/* Force published load values to 0 */
	ਲਈ(ਪੂਰਨ i=0; i<mosq_metric_ਲੱਦੋ_max; i++){
		ਜੇ(metric_loads[i].interval == METRIC_ਲੱਦੋ_1_MIN){
			calc_ਲੱਦੋ(buf, 0.0, 0.0, &metric_loads[i], true);
		}ਨਹੀਂਤਾਂ ਜੇ(metric_loads[i].interval == METRIC_ਲੱਦੋ_5_MIN){
			calc_ਲੱਦੋ(buf, 0.0, 0.0, &metric_loads[i], true);
		}ਨਹੀਂਤਾਂ{
			calc_ਲੱਦੋ(buf, 0.0, 0.0, &metric_loads[i], true);
		}
	}
}


ਖਾਲੀ metrics__int_inc(ਗਿਣਤੀ mosq_metric_type m, int64_t ਮੁੱਲ)
{
	ਜੇ(m < mosq_metric_max){
		metrics[m].ਅਗਲਾ += ਮੁੱਲ;
	}
}


ਖਾਲੀ metrics__int_dec(ਗਿਣਤੀ mosq_metric_type m, int64_t ਮੁੱਲ)
{
	ਜੇ(m < mosq_metric_max){
		metrics[m].ਅਗਲਾ -= ਮੁੱਲ;
	}
}


/* Send messages for the $SYS hierarchy if the last update is longer than
 * 'interval' seconds ago.
 * 'interval' is the amount of seconds between updates. If 0, then no periodic
 * messages are sent for the $SYS hierarchy.
 * 'start_time' is the result of time() that the broker was started at.
 */
ਖਾਲੀ sys_ਰੁੱਖ_update(bool force)
{
	time_t uptime;
	ਅੱਖਰ buf[BUFLEN];
	uint32_t len;
	time_t ਅਗਲਾ_event;
	ਸਥਿਰ time_t ਆਖਰੀ_update_real = 0;

	ਜੇ(db.config->sys_interval){
		ਅਗਲਾ_event = db.config->sys_interval - db.now_real_s % db.config->sys_interval - 1;
		ਜੇ(ਅਗਲਾ_event <= 0){
			ਅਗਲਾ_event = db.config->sys_interval;
		}
		loop_update_ਅਗਲਾ_event(ਅਗਲਾ_event*1000);
	}

	ਜੇ(db.config->sys_interval
			&& ((db.now_real_s % db.config->sys_interval == 0 && ਆਖਰੀ_update_real != db.now_real_s) || force)){

		uptime = db.now_s - ਸ਼ੁਰੂ_ਸਮਾਂ;
		len = (uint32_t)snprintf(buf, BUFLEN, "%" PRIu64 " seconds", (uint64_t)uptime);
		db_messages_easy_ਕਤਾਰ(ਨਿਰਾ, "$SYS/broker/uptime", SYS_ਰੁੱਖ_QOS, len, buf, 1, ਸੁਨੇਹਾ_EXPIRY_INFINITE, ਨਿਰਾ);

		/*  Update metrics values where not otherwise updated */
		metrics[mosq_gauge_ਸੁਨੇਹਾ_store_ਗਿਣਤੀ].ਅਗਲਾ = db.ਸੁਨੇਹਾ_store_ਗਿਣਤੀ;
		metrics[mosq_gauge_ਸੁਨੇਹਾ_store_ਬਾਈਟਾਂ].ਅਗਲਾ = (int64_t)db.ਸੁਨੇਹਾ_store_ਬਾਈਟਾਂ;
		metrics[mosq_gauge_subscriptions].ਅਗਲਾ = db.subscription_ਗਿਣਤੀ;
		metrics[mosq_gauge_shared_subscriptions].ਅਗਲਾ = db.shared_subscription_ਗਿਣਤੀ;
		metrics[mosq_gauge_retained_messages].ਅਗਲਾ = db.retained_ਗਿਣਤੀ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_ਸਮ੍ਰਿਤੀ_TRACKING
		metrics[mosq_gauge_ਢੇਰ_current].ਅਗਲਾ = (int64_t)mosquitto_ਸਮ੍ਰਿਤੀ_ਵਰਤਿਆ();
		metrics[mosq_counter_ਢੇਰ_ਵੱਧ_ਤੋਂ_ਵੱਧ].ਅਗਲਾ = (int64_t)mosquitto_max_ਸਮ੍ਰਿਤੀ_ਵਰਤਿਆ();
#ਜੇ_ਅੰਤ
		metrics[mosq_gauge_clients_ਕੁੱਲ].ਅਗਲਾ = HASH_CNT(hh_id, db.contexts_by_id);
		metrics[mosq_counter_clients_ਵੱਧ_ਤੋਂ_ਵੱਧ].ਅਗਲਾ = HASH_CNT(hh_id, db.contexts_by_id);
		metrics[mosq_gauge_clients_connected].ਅਗਲਾ = HASH_CNT(hh_sock, db.contexts_by_sock);
		metrics[mosq_gauge_clients_disconnected].ਅਗਲਾ = HASH_CNT(hh_id, db.contexts_by_id) - HASH_CNT(hh_sock, db.contexts_by_sock);

		/* Handle loads first, because they reference other metrics and need next != current */
		ਜੇ(db.now_s > ਆਖਰੀ_update){
			ਦੁੱਗਣਾ i_mult = 60.0/(ਦੁੱਗਣਾ)(db.now_s-ਆਖਰੀ_update);

			ਦੁੱਗਣਾ exponent_1min = ਘਾਤਾਂਕ(-1.0*(ਦੁੱਗਣਾ)(db.now_s-ਆਖਰੀ_update)/60.0);
			ਦੁੱਗਣਾ exponent_5min = ਘਾਤਾਂਕ(-1.0*(ਦੁੱਗਣਾ)(db.now_s-ਆਖਰੀ_update)/300.0);
			ਦੁੱਗਣਾ exponent_15min = ਘਾਤਾਂਕ(-1.0*(ਦੁੱਗਣਾ)(db.now_s-ਆਖਰੀ_update)/900.0);

			ਲਈ(ਪੂਰਨ i=0; i<mosq_metric_ਲੱਦੋ_max; i++){
				ਜੇ(metric_loads[i].interval == METRIC_ਲੱਦੋ_1_MIN){
					calc_ਲੱਦੋ(buf, exponent_1min, i_mult, &metric_loads[i], false);
				}ਨਹੀਂਤਾਂ ਜੇ(metric_loads[i].interval == METRIC_ਲੱਦੋ_5_MIN){
					calc_ਲੱਦੋ(buf, exponent_5min, i_mult, &metric_loads[i], false);
				}ਨਹੀਂਤਾਂ{
					calc_ਲੱਦੋ(buf, exponent_15min, i_mult, &metric_loads[i], false);
				}
			}
		}

		ਲਈ(ਪੂਰਨ i=0; i<mosq_metric_max; i++){
			ਜੇ((metrics[i].is_max && metrics[i].ਅਗਲਾ > metrics[i].current) ||
					(!metrics[i].is_max && metrics[i].ਅਗਲਾ != metrics[i].current)){

				metrics[i].current = metrics[i].ਅਗਲਾ;
				len = (uint32_t)snprintf(buf, BUFLEN, "%lu", metrics[i].current);
				ਜੇ(metrics[i].topic){
					db_messages_easy_ਕਤਾਰ(ਨਿਰਾ, metrics[i].topic, SYS_ਰੁੱਖ_QOS, len, buf, 1, ਸੁਨੇਹਾ_EXPIRY_INFINITE, ਨਿਰਾ);
				}
				ਜੇ(metrics[i].topic_alias){
					db_messages_easy_ਕਤਾਰ(ਨਿਰਾ, metrics[i].topic_alias, SYS_ਰੁੱਖ_QOS, len, buf, 1, ਸੁਨੇਹਾ_EXPIRY_INFINITE, ਨਿਰਾ);
				}
			}
		}

		ਆਖਰੀ_update = db.now_s;
		ਆਖਰੀ_update_real = db.now_real_s;
	}
}

#ਜੇ_ਅੰਤ
