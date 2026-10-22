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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_SYS_ವೃಕ್ಷ

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ <math.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <inttypes.h>
#ಅನ್ತರ್ಭಾವ <limits.h>
#ಅನ್ತರ್ಭಾವ <inttypes.h>

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "sys_tree.h"

#ವ್ಯಾಖ್ಯಾನಿಸು BUFLEN 100

#ವ್ಯಾಖ್ಯಾನಿಸು SYS_ವೃಕ್ಷ_QOS 2

#ವ್ಯಾಖ್ಯಾನಿಸು METRIC_ತುಂಬು_1_MIN 1
#ವ್ಯಾಖ್ಯಾನಿಸು METRIC_ತುಂಬು_5_MIN 2
#ವ್ಯಾಖ್ಯಾನಿಸು METRIC_ತುಂಬು_15_MIN 3

ರಚನೆ metric {
	int64_t current;
	int64_t ಮುಂದಿನ;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *topic, *topic_alias;
	bool is_max;
};

ರಚನೆ metric_ತುಂಬು {
	ದ್ವಿಗುಣ current;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *topic;
	ಪೂರ್ಣಾಂಕ ತುಂಬು_ref;
	uint8_t interval;
};

ರಚನೆ metric metrics[mosq_metric_max] = {
	{ 1, 0, "$SYS/broker/clients/total", ಖಾಲಿ, false }, /* mosq_gauge_clients_total */
	{ 1, 0, "$SYS/broker/clients/maximum", ಖಾಲಿ, true }, /* metric_clients_maximum */
	{ 1, 0, "$SYS/broker/clients/disconnected", "$SYS/broker/clients/inactive", false }, /* mosq_gauge_clients_disconnected */
	{ 1, 0, "$SYS/broker/clients/connected", "$SYS/broker/clients/active", false }, /* mosq_gauge_clients_connected */
	{ 1, 0, "$SYS/broker/clients/expired", ಖಾಲಿ, false }, /* mosq_counter_clients_expired */
	{ 1, 0, "$SYS/broker/messages/stored", "$SYS/broker/store/messages/count", false }, /* mosq_gauge_message_store_count */
	{ 1, 0, "$SYS/broker/store/messages/bytes", ಖಾಲಿ, false }, /* mosq_gauge_message_store_bytes */
	{ 1, 0, "$SYS/broker/subscriptions/count", ಖಾಲಿ, false }, /* mosq_gauge_subscription_count */
	{ 1, 0, "$SYS/broker/shared_subscriptions/count", ಖಾಲಿ, false }, /* mosq_gauge_shared_subscription_count */
	{ 1, 0, "$SYS/broker/retained messages/count", ಖಾಲಿ, false }, /* mosq_gauge_retained_message_count */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_ಸ್ಮರಣೆ_TRACKING
	{ 1, 0, "$SYS/broker/heap/current", ಖಾಲಿ, false }, /* mosq_gauge_heap_current */
	{ 1, 0, "$SYS/broker/heap/maximum", ಖಾಲಿ, true }, /* mosq_gauge_heap_maximum */
#ಇಲ್ಲದಿದ್ದರೆ
	{ 1, 0, ಖಾಲಿ, ಖಾಲಿ, 0 }, /* mosq_gauge_heap_current */
	{ 1, 0, ಖಾಲಿ, ಖಾಲಿ, 0 }, /* mosq_gauge_heap_maximum */
#ಆದರೆ_ಅಂತ್ಯ
	{ 1, 0, "$SYS/broker/messages/received", ಖಾಲಿ, false }, /* mosq_counter_messages_received */
	{ 1, 0, "$SYS/broker/messages/sent", ಖಾಲಿ, false }, /* mosq_counter_messages_sent */
	{ 1, 0, "$SYS/broker/bytes/received", ಖಾಲಿ, false }, /* mosq_counter_bytes_received */
	{ 1, 0, "$SYS/broker/bytes/sent", ಖಾಲಿ, false }, /* mosq_counter_bytes_sent */
	{ 1, 0, "$SYS/broker/publish/bytes/received", ಖಾಲಿ, false }, /* mosq_counter_pub_bytes_received */
	{ 1, 0, "$SYS/broker/publish/bytes/sent", ಖಾಲಿ, false }, /* mosq_counter_pub_bytes_sent */
	{ 1, 0, "$SYS/broker/packet/out/count", ಖಾಲಿ, false }, /* mosq_gauge_out_packet_count */
	{ 1, 0, "$SYS/broker/packet/out/bytes", ಖಾಲಿ, false }, /* mosq_gauge_out_packet_bytes */
	{ 1, 0, "$SYS/broker/connections/socket/count", ಖಾಲಿ, false }, /* mosq_counter_socket_connections */
	{ 1, 0, ಖಾಲಿ, ಖಾಲಿ, false }, /* mosq_counter_mqtt_connect_received */
	{ 1, 0, ಖಾಲಿ, ಖಾಲಿ, false }, /* mosq_counter_mqtt_connect_sent */
	{ 1, 0, ಖಾಲಿ, ಖಾಲಿ, false }, /* mosq_counter_mqtt_connack_received */
	{ 1, 0, ಖಾಲಿ, ಖಾಲಿ, false }, /* mosq_counter_mqtt_connack_sent */
	{ 1, 0, "$SYS/broker/publish/messages/dropped", ಖಾಲಿ, false }, /* mosq_counter_mqtt_publish_dropped */
	{ 1, 0, "$SYS/broker/publish/messages/received", ಖಾಲಿ, false }, /* mosq_counter_mqtt_publish_received */
	{ 1, 0, "$SYS/broker/publish/messages/sent", ಖಾಲಿ, false }, /* mosq_counter_mqtt_publish_sent */
	{ 1, 0, ಖಾಲಿ, ಖಾಲಿ, false }, /* mosq_counter_mqtt_puback_received */
	{ 1, 0, ಖಾಲಿ, ಖಾಲಿ, false }, /* mosq_counter_mqtt_puback_sent */
	{ 1, 0, ಖಾಲಿ, ಖಾಲಿ, false }, /* mosq_counter_mqtt_pubrec_received */
	{ 1, 0, ಖಾಲಿ, ಖಾಲಿ, false }, /* mosq_counter_mqtt_pubrec_sent */
	{ 1, 0, ಖಾಲಿ, ಖಾಲಿ, false }, /* mosq_counter_mqtt_pubrel_received */
	{ 1, 0, ಖಾಲಿ, ಖಾಲಿ, false }, /* mosq_counter_mqtt_pubrel_sent */
	{ 1, 0, ಖಾಲಿ, ಖಾಲಿ, false }, /* mosq_counter_mqtt_pubcomp_received */
	{ 1, 0, ಖಾಲಿ, ಖಾಲಿ, false }, /* mosq_counter_mqtt_pubcomp_sent */
	{ 1, 0, ಖಾಲಿ, ಖಾಲಿ, false }, /* mosq_counter_mqtt_subscribe_received */
	{ 1, 0, ಖಾಲಿ, ಖಾಲಿ, false }, /* mosq_counter_mqtt_subscribe_sent */
	{ 1, 0, ಖಾಲಿ, ಖಾಲಿ, false }, /* mosq_counter_mqtt_suback_received */
	{ 1, 0, ಖಾಲಿ, ಖಾಲಿ, false }, /* mosq_counter_mqtt_suback_sent */
	{ 1, 0, ಖಾಲಿ, ಖಾಲಿ, false }, /* mosq_counter_mqtt_unsubscribe_received */
	{ 1, 0, ಖಾಲಿ, ಖಾಲಿ, false }, /* mosq_counter_mqtt_unsubscribe_sent */
	{ 1, 0, ಖಾಲಿ, ಖಾಲಿ, false }, /* mosq_counter_mqtt_unsuback_received */
	{ 1, 0, ಖಾಲಿ, ಖಾಲಿ, false }, /* mosq_counter_mqtt_unsuback_sent */
	{ 1, 0, ಖಾಲಿ, ಖಾಲಿ, false }, /* mosq_counter_mqtt_pingreq_received */
	{ 1, 0, ಖಾಲಿ, ಖಾಲಿ, false }, /* mosq_counter_mqtt_pingreq_sent */
	{ 1, 0, ಖಾಲಿ, ಖಾಲಿ, false }, /* mosq_counter_mqtt_pingresp_received */
	{ 1, 0, ಖಾಲಿ, ಖಾಲಿ, false }, /* mosq_counter_mqtt_pingresp_sent */
	{ 1, 0, ಖಾಲಿ, ಖಾಲಿ, false }, /* mosq_counter_mqtt_disconnect_received */
	{ 1, 0, ಖಾಲಿ, ಖಾಲಿ, false }, /* mosq_counter_mqtt_disconnect_sent */
	{ 1, 0, ಖಾಲಿ, ಖಾಲಿ, false }, /* mosq_counter_mqtt_auth_received */
	{ 1, 0, ಖಾಲಿ, ಖಾಲಿ, false }, /* mosq_counter_mqtt_auth_sent */
};

ರಚನೆ metric_ತುಂಬು metric_loads[mosq_metric_ತುಂಬು_max] = {
	{ 0.0, "$SYS/broker/load/messages/received/1min", mosq_counter_messages_received, METRIC_ತುಂಬು_1_MIN }, /* metric_load_messages_received_1min */
	{ 0.0, "$SYS/broker/load/messages/received/5min", mosq_counter_messages_received, METRIC_ತುಂಬು_5_MIN }, /* metric_load_messages_received_5min */
	{ 0.0, "$SYS/broker/load/messages/received/15min", mosq_counter_messages_received, METRIC_ತುಂಬು_15_MIN }, /* metric_load_messages_received_15min */
	{ 0.0, "$SYS/broker/load/messages/sent/1min", mosq_counter_messages_ಕಳುಹಿಸಿದ, METRIC_ತುಂಬು_1_MIN }, /* metric_load_messages_sent_1min */
	{ 0.0, "$SYS/broker/load/messages/sent/5min", mosq_counter_messages_ಕಳುಹಿಸಿದ, METRIC_ತುಂಬು_5_MIN }, /* metric_load_messages_sent_5min */
	{ 0.0, "$SYS/broker/load/messages/sent/15min", mosq_counter_messages_ಕಳುಹಿಸಿದ, METRIC_ತುಂಬು_15_MIN }, /* metric_load_messages_sent_15min */
	{ 0.0, "$SYS/broker/load/publish/dropped/1min", mosq_counter_mqtt_publish_dropped, METRIC_ತುಂಬು_1_MIN }, /* metric_load_pub_messages_dropped_1min */
	{ 0.0, "$SYS/broker/load/publish/dropped/5min", mosq_counter_mqtt_publish_dropped, METRIC_ತುಂಬು_5_MIN }, /* metric_load_pub_messages_dropped_5min */
	{ 0.0, "$SYS/broker/load/publish/dropped/15min", mosq_counter_mqtt_publish_dropped, METRIC_ತುಂಬು_15_MIN }, /* metric_load_pub_messages_dropped_15min */
	{ 0.0, "$SYS/broker/load/publish/received/1min", mosq_counter_mqtt_publish_received, METRIC_ತುಂಬು_1_MIN }, /* metric_load_pub_messages_received_1min */
	{ 0.0, "$SYS/broker/load/publish/received/5min", mosq_counter_mqtt_publish_received, METRIC_ತುಂಬು_5_MIN }, /* metric_load_pub_messages_received_5min */
	{ 0.0, "$SYS/broker/load/publish/received/15min", mosq_counter_mqtt_publish_received, METRIC_ತುಂಬು_15_MIN }, /* metric_load_pub_messages_received_15min */
	{ 0.0, "$SYS/broker/load/publish/sent/1min", mosq_counter_mqtt_publish_ಕಳುಹಿಸಿದ, METRIC_ತುಂಬು_1_MIN }, /* metric_load_pub_messages_sent_1min */
	{ 0.0, "$SYS/broker/load/publish/sent/5min", mosq_counter_mqtt_publish_ಕಳುಹಿಸಿದ, METRIC_ತುಂಬು_5_MIN }, /* metric_load_pub_messages_sent_5min */
	{ 0.0, "$SYS/broker/load/publish/sent/15min", mosq_counter_mqtt_publish_ಕಳುಹಿಸಿದ, METRIC_ತುಂಬು_15_MIN }, /* metric_load_pub_messages_sent_15min */
	{ 0.0, "$SYS/broker/load/bytes/received/1min", mosq_counter_ಬೈಟುಗಳು_received, METRIC_ತುಂಬು_1_MIN }, /* metric_load_bytes_received_1min */
	{ 0.0, "$SYS/broker/load/bytes/received/5min", mosq_counter_ಬೈಟುಗಳು_received, METRIC_ತುಂಬು_5_MIN }, /* metric_load_bytes_received_5min */
	{ 0.0, "$SYS/broker/load/bytes/received/15min", mosq_counter_ಬೈಟುಗಳು_received, METRIC_ತುಂಬು_15_MIN }, /* metric_load_bytes_received_15min */
	{ 0.0, "$SYS/broker/load/bytes/sent/1min", mosq_counter_ಬೈಟುಗಳು_ಕಳುಹಿಸಿದ, METRIC_ತುಂಬು_1_MIN }, /* metric_load_bytes_sent_1min */
	{ 0.0, "$SYS/broker/load/bytes/sent/5min", mosq_counter_ಬೈಟುಗಳು_ಕಳುಹಿಸಿದ, METRIC_ತುಂಬು_5_MIN }, /* metric_load_bytes_sent_5min */
	{ 0.0, "$SYS/broker/load/bytes/sent/15min", mosq_counter_ಬೈಟುಗಳು_ಕಳುಹಿಸಿದ, METRIC_ತುಂಬು_15_MIN }, /* metric_load_bytes_sent_15min */
	{ 0.0, "$SYS/broker/load/sockets/1min", mosq_counter_socket_connections, METRIC_ತುಂಬು_1_MIN }, /* metric_load_socket_connections_1min */
	{ 0.0, "$SYS/broker/load/sockets/5min", mosq_counter_socket_connections, METRIC_ತುಂಬು_5_MIN }, /* metric_load_socket_connections_5min */
	{ 0.0, "$SYS/broker/load/sockets/15min", mosq_counter_socket_connections, METRIC_ತುಂಬು_15_MIN }, /* metric_load_socket_connections_15min */
	{ 0.0, "$SYS/broker/load/connections/1min", mosq_counter_mqtt_connect_received, METRIC_ತುಂಬು_1_MIN }, /* metric_load_connections_1min */
	{ 0.0, "$SYS/broker/load/connections/5min", mosq_counter_mqtt_connect_received, METRIC_ತುಂಬು_5_MIN }, /* metric_load_connections_5min */
	{ 0.0, "$SYS/broker/load/connections/15min", mosq_counter_mqtt_connect_received, METRIC_ತುಂಬು_15_MIN }, /* metric_load_connections_15min */
};

ಸ್ಥಿರ time_t ಆರಂಭ_ಸಮಯ = 0;
ಸ್ಥಿರ time_t ಕೊನೆಯ_update = 0;


time_t broker_uptime(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ db.now_s - ಆರಂಭ_ಸಮಯ;
}


ಸ್ಥಿರ ಶೂನ್ಯ calc_ತುಂಬು(ಅಕ್ಷರ *buf, ದ್ವಿಗುಣ exponent, ದ್ವಿಗುಣ i_mult, ರಚನೆ metric_ತುಂಬು *m, bool force)
{
	ದ್ವಿಗುಣ new_ಮೌಲ್ಯ;
	uint32_t len;
	ದ್ವಿಗುಣ interval;

	interval = (ದ್ವಿಗುಣ)(metrics[m->ತುಂಬು_ref].ಮುಂದಿನ - metrics[m->ತುಂಬು_ref].current)*i_mult;
	new_ಮೌಲ್ಯ = interval + exponent*(m->current - interval);
	ಆದರೆ(fabs(new_ಮೌಲ್ಯ - (m->current)) >= 0.01 || force){
		len = (uint32_t)snprintf(buf, BUFLEN, "%.2f", new_ಮೌಲ್ಯ);
		db_messages_easy_ಸರತಿ(ಖಾಲಿ, m->topic, SYS_ವೃಕ್ಷ_QOS, len, buf, 1, ಸಂದೇಶ_EXPIRY_INFINITE, ಖಾಲಿ);
	}
	m->current = new_ಮೌಲ್ಯ;
}


ಶೂನ್ಯ sys_ವೃಕ್ಷ_init(ಶೂನ್ಯ)
{
	ಅಕ್ಷರ buf[BUFLEN];
	uint32_t len;

	ಆದರೆ(db.config->sys_interval == 0){
		ಹಿಂತಿರುಗಿ;
	}

	/* Set static $SYS messages */
	len = (uint32_t)snprintf(buf, 64, "mosquitto version %s", ಆವೃತ್ತಿ);
	db_messages_easy_ಸರತಿ(ಖಾಲಿ, "$SYS/broker/version", SYS_ವೃಕ್ಷ_QOS, len, buf, 1, ಸಂದೇಶ_EXPIRY_INFINITE, ಖಾಲಿ);

	ಆರಂಭ_ಸಮಯ = mosquitto_ಸಮಯ();
	ಕೊನೆಯ_update = ಆರಂಭ_ಸಮಯ;

	sys_ವೃಕ್ಷ_update(true);

	/* Force published load values to 0 */
	ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<mosq_metric_ತುಂಬು_max; i++){
		ಆದರೆ(metric_loads[i].interval == METRIC_ತುಂಬು_1_MIN){
			calc_ತುಂಬು(buf, 0.0, 0.0, &metric_loads[i], true);
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(metric_loads[i].interval == METRIC_ತುಂಬು_5_MIN){
			calc_ತುಂಬು(buf, 0.0, 0.0, &metric_loads[i], true);
		}ಇಲ್ಲದಿದ್ದರೆ{
			calc_ತುಂಬು(buf, 0.0, 0.0, &metric_loads[i], true);
		}
	}
}


ಶೂನ್ಯ metrics__int_inc(ಎಣಿಕೆ mosq_metric_type m, int64_t ಮೌಲ್ಯ)
{
	ಆದರೆ(m < mosq_metric_max){
		metrics[m].ಮುಂದಿನ += ಮೌಲ್ಯ;
	}
}


ಶೂನ್ಯ metrics__int_dec(ಎಣಿಕೆ mosq_metric_type m, int64_t ಮೌಲ್ಯ)
{
	ಆದರೆ(m < mosq_metric_max){
		metrics[m].ಮುಂದಿನ -= ಮೌಲ್ಯ;
	}
}


/* Send messages for the $SYS hierarchy if the last update is longer than
 * 'interval' seconds ago.
 * 'interval' is the amount of seconds between updates. If 0, then no periodic
 * messages are sent for the $SYS hierarchy.
 * 'start_time' is the result of time() that the broker was started at.
 */
ಶೂನ್ಯ sys_ವೃಕ್ಷ_update(bool force)
{
	time_t uptime;
	ಅಕ್ಷರ buf[BUFLEN];
	uint32_t len;
	time_t ಮುಂದಿನ_event;
	ಸ್ಥಿರ time_t ಕೊನೆಯ_update_real = 0;

	ಆದರೆ(db.config->sys_interval){
		ಮುಂದಿನ_event = db.config->sys_interval - db.now_real_s % db.config->sys_interval - 1;
		ಆದರೆ(ಮುಂದಿನ_event <= 0){
			ಮುಂದಿನ_event = db.config->sys_interval;
		}
		loop_update_ಮುಂದಿನ_event(ಮುಂದಿನ_event*1000);
	}

	ಆದರೆ(db.config->sys_interval
			&& ((db.now_real_s % db.config->sys_interval == 0 && ಕೊನೆಯ_update_real != db.now_real_s) || force)){

		uptime = db.now_s - ಆರಂಭ_ಸಮಯ;
		len = (uint32_t)snprintf(buf, BUFLEN, "%" PRIu64 " seconds", (uint64_t)uptime);
		db_messages_easy_ಸರತಿ(ಖಾಲಿ, "$SYS/broker/uptime", SYS_ವೃಕ್ಷ_QOS, len, buf, 1, ಸಂದೇಶ_EXPIRY_INFINITE, ಖಾಲಿ);

		/*  Update metrics values where not otherwise updated */
		metrics[mosq_gauge_ಸಂದೇಶ_store_ಎಣಿಕೆ].ಮುಂದಿನ = db.ಸಂದೇಶ_store_ಎಣಿಕೆ;
		metrics[mosq_gauge_ಸಂದೇಶ_store_ಬೈಟುಗಳು].ಮುಂದಿನ = (int64_t)db.ಸಂದೇಶ_store_ಬೈಟುಗಳು;
		metrics[mosq_gauge_subscriptions].ಮುಂದಿನ = db.subscription_ಎಣಿಕೆ;
		metrics[mosq_gauge_shared_subscriptions].ಮುಂದಿನ = db.shared_subscription_ಎಣಿಕೆ;
		metrics[mosq_gauge_retained_messages].ಮುಂದಿನ = db.retained_ಎಣಿಕೆ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_ಸ್ಮರಣೆ_TRACKING
		metrics[mosq_gauge_ರಾಶಿ_current].ಮುಂದಿನ = (int64_t)mosquitto_ಸ್ಮರಣೆ_ಬಳಸಿದ();
		metrics[mosq_counter_ರಾಶಿ_ಗರಿಷ್ಠ].ಮುಂದಿನ = (int64_t)mosquitto_max_ಸ್ಮರಣೆ_ಬಳಸಿದ();
#ಆದರೆ_ಅಂತ್ಯ
		metrics[mosq_gauge_clients_ಒಟ್ಟು].ಮುಂದಿನ = HASH_CNT(hh_id, db.contexts_by_id);
		metrics[mosq_counter_clients_ಗರಿಷ್ಠ].ಮುಂದಿನ = HASH_CNT(hh_id, db.contexts_by_id);
		metrics[mosq_gauge_clients_connected].ಮುಂದಿನ = HASH_CNT(hh_sock, db.contexts_by_sock);
		metrics[mosq_gauge_clients_disconnected].ಮುಂದಿನ = HASH_CNT(hh_id, db.contexts_by_id) - HASH_CNT(hh_sock, db.contexts_by_sock);

		/* Handle loads first, because they reference other metrics and need next != current */
		ಆದರೆ(db.now_s > ಕೊನೆಯ_update){
			ದ್ವಿಗುಣ i_mult = 60.0/(ದ್ವಿಗುಣ)(db.now_s-ಕೊನೆಯ_update);

			ದ್ವಿಗುಣ exponent_1min = ಘಾತಾಂಕ(-1.0*(ದ್ವಿಗುಣ)(db.now_s-ಕೊನೆಯ_update)/60.0);
			ದ್ವಿಗುಣ exponent_5min = ಘಾತಾಂಕ(-1.0*(ದ್ವಿಗುಣ)(db.now_s-ಕೊನೆಯ_update)/300.0);
			ದ್ವಿಗುಣ exponent_15min = ಘಾತಾಂಕ(-1.0*(ದ್ವಿಗುಣ)(db.now_s-ಕೊನೆಯ_update)/900.0);

			ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<mosq_metric_ತುಂಬು_max; i++){
				ಆದರೆ(metric_loads[i].interval == METRIC_ತುಂಬು_1_MIN){
					calc_ತುಂಬು(buf, exponent_1min, i_mult, &metric_loads[i], false);
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(metric_loads[i].interval == METRIC_ತುಂಬು_5_MIN){
					calc_ತುಂಬು(buf, exponent_5min, i_mult, &metric_loads[i], false);
				}ಇಲ್ಲದಿದ್ದರೆ{
					calc_ತುಂಬು(buf, exponent_15min, i_mult, &metric_loads[i], false);
				}
			}
		}

		ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<mosq_metric_max; i++){
			ಆದರೆ((metrics[i].is_max && metrics[i].ಮುಂದಿನ > metrics[i].current) ||
					(!metrics[i].is_max && metrics[i].ಮುಂದಿನ != metrics[i].current)){

				metrics[i].current = metrics[i].ಮುಂದಿನ;
				len = (uint32_t)snprintf(buf, BUFLEN, "%lu", metrics[i].current);
				ಆದರೆ(metrics[i].topic){
					db_messages_easy_ಸರತಿ(ಖಾಲಿ, metrics[i].topic, SYS_ವೃಕ್ಷ_QOS, len, buf, 1, ಸಂದೇಶ_EXPIRY_INFINITE, ಖಾಲಿ);
				}
				ಆದರೆ(metrics[i].topic_alias){
					db_messages_easy_ಸರತಿ(ಖಾಲಿ, metrics[i].topic_alias, SYS_ವೃಕ್ಷ_QOS, len, buf, 1, ಸಂದೇಶ_EXPIRY_INFINITE, ಖಾಲಿ);
				}
			}
		}

		ಕೊನೆಯ_update = db.now_s;
		ಕೊನೆಯ_update_real = db.now_real_s;
	}
}

#ಆದರೆ_ಅಂತ್ಯ
