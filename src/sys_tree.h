/*
Copyright (c) 2015-2021 Roger Light <roger@atchoo.org>

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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SYS_ਰੁੱਖ_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SYS_ਰੁੱਖ_H

#ਜੇ defined(WITH_SYS_ਰੁੱਖ) && defined(WITH_BROKER)

/* This ordering *must* match the metrics array in sys_tree.c. */
ਗਿਣਤੀ mosq_metric_type {
	mosq_gauge_clients_ਕੁੱਲ = 0,
	mosq_counter_clients_ਵੱਧ_ਤੋਂ_ਵੱਧ = 1,
	mosq_gauge_clients_disconnected = 2,
	mosq_gauge_clients_connected = 3,
	mosq_counter_clients_expired = 4,
	mosq_gauge_ਸੁਨੇਹਾ_store_ਗਿਣਤੀ = 5,
	mosq_gauge_ਸੁਨੇਹਾ_store_ਬਾਈਟਾਂ = 6,
	mosq_gauge_subscriptions = 7,
	mosq_gauge_shared_subscriptions = 8,
	mosq_gauge_retained_messages = 9,
	mosq_gauge_ਢੇਰ_current = 10,
	mosq_counter_ਢੇਰ_ਵੱਧ_ਤੋਂ_ਵੱਧ = 11,
	mosq_counter_messages_received = 12,
	mosq_counter_messages_ਭੇਜਿਆ = 13,
	mosq_counter_ਬਾਈਟਾਂ_received = 14,
	mosq_counter_ਬਾਈਟਾਂ_ਭੇਜਿਆ = 15,
	mosq_counter_pub_ਬਾਈਟਾਂ_received = 16,
	mosq_counter_pub_ਬਾਈਟਾਂ_ਭੇਜਿਆ = 17,
	mosq_gauge_ਬਾਹਰਲਾ_packets = 18,
	mosq_gauge_ਬਾਹਰਲਾ_packet_ਬਾਈਟਾਂ = 19,
	mosq_counter_socket_connections = 20,
	mosq_counter_mqtt_connect_received = 21,
	mosq_counter_mqtt_connect_ਭੇਜਿਆ = 22,
	mosq_counter_mqtt_connack_received = 23,
	mosq_counter_mqtt_connack_ਭੇਜਿਆ = 24,
	mosq_counter_mqtt_publish_dropped = 25,
	mosq_counter_mqtt_publish_received = 26,
	mosq_counter_mqtt_publish_ਭੇਜਿਆ = 27,
	mosq_counter_mqtt_puback_received = 28,
	mosq_counter_mqtt_puback_ਭੇਜਿਆ = 29,
	mosq_counter_mqtt_pubrec_received = 30,
	mosq_counter_mqtt_pubrec_ਭੇਜਿਆ = 31,
	mosq_counter_mqtt_pubrel_received = 32,
	mosq_counter_mqtt_pubrel_ਭੇਜਿਆ = 33,
	mosq_counter_mqtt_pubcomp_received = 34,
	mosq_counter_mqtt_pubcomp_ਭੇਜਿਆ = 35,
	mosq_counter_mqtt_subscribe_received = 36,
	mosq_counter_mqtt_subscribe_ਭੇਜਿਆ = 37,
	mosq_counter_mqtt_suback_received = 38,
	mosq_counter_mqtt_suback_ਭੇਜਿਆ = 39,
	mosq_counter_mqtt_unsubscribe_received = 40,
	mosq_counter_mqtt_unsubscribe_ਭੇਜਿਆ = 41,
	mosq_counter_mqtt_unsuback_received = 42,
	mosq_counter_mqtt_unsuback_ਭੇਜਿਆ = 43,
	mosq_counter_mqtt_pingreq_received = 44,
	mosq_counter_mqtt_pingreq_ਭੇਜਿਆ = 45,
	mosq_counter_mqtt_pingresp_received = 46,
	mosq_counter_mqtt_pingresp_ਭੇਜਿਆ = 47,
	mosq_counter_mqtt_disconnect_received = 48,
	mosq_counter_mqtt_disconnect_ਭੇਜਿਆ = 49,
	mosq_counter_mqtt_auth_received = 50,
	mosq_counter_mqtt_auth_ਭੇਜਿਆ = 51,

	mosq_metric_max,
};

/* This ordering *must* match the metrics_load array in sys_tree.c. */
ਗਿਣਤੀ mosq_metric_ਲੱਦੋ_type {
	mosq_ਲੱਦੋ_messages_received_1min = 0,
	mosq_ਲੱਦੋ_messages_received_5min = 1,
	mosq_ਲੱਦੋ_messages_received_15min = 2,
	mosq_ਲੱਦੋ_messages_ਭੇਜਿਆ_1min = 3,
	mosq_ਲੱਦੋ_messages_ਭੇਜਿਆ_5min = 4,
	mosq_ਲੱਦੋ_messages_ਭੇਜਿਆ_15min = 5,
	mosq_ਲੱਦੋ_pub_messages_dropped_1min = 6,
	mosq_ਲੱਦੋ_pub_messages_dropped_5min = 7,
	mosq_ਲੱਦੋ_pub_messages_dropped_15min = 8,
	mosq_ਲੱਦੋ_pub_messages_received_1min = 9,
	mosq_ਲੱਦੋ_pub_messages_received_5min = 10,
	mosq_ਲੱਦੋ_pub_messages_received_15min = 11,
	mosq_ਲੱਦੋ_pub_messages_ਭੇਜਿਆ_1min = 12,
	mosq_ਲੱਦੋ_pub_messages_ਭੇਜਿਆ_5min = 13,
	mosq_ਲੱਦੋ_pub_messages_ਭੇਜਿਆ_15min = 14,
	mosq_ਲੱਦੋ_ਬਾਈਟਾਂ_received_1min = 15,
	mosq_ਲੱਦੋ_ਬਾਈਟਾਂ_received_5min = 16,
	mosq_ਲੱਦੋ_ਬਾਈਟਾਂ_received_15min = 17,
	mosq_ਲੱਦੋ_ਬਾਈਟਾਂ_ਭੇਜਿਆ_1min = 18,
	mosq_ਲੱਦੋ_ਬਾਈਟਾਂ_ਭੇਜਿਆ_5min = 19,
	mosq_ਲੱਦੋ_ਬਾਈਟਾਂ_ਭੇਜਿਆ_15min = 20,
	mosq_ਲੱਦੋ_sockets_1min = 21,
	mosq_ਲੱਦੋ_sockets_5min = 22,
	mosq_ਲੱਦੋ_sockets_15min = 23,
	mosq_ਲੱਦੋ_connections_1min = 24,
	mosq_ਲੱਦੋ_connections_5min = 25,
	mosq_ਲੱਦੋ_connections_15min = 26,

	mosq_metric_ਲੱਦੋ_max,
};

ਖਾਲੀ metrics__int_inc(ਗਿਣਤੀ mosq_metric_type m, int64_t ਮੁੱਲ);
ਖਾਲੀ metrics__int_dec(ਗਿਣਤੀ mosq_metric_type m, int64_t ਮੁੱਲ);

#ਨਹੀਂਤਾਂ
#  define metrics__int_inc(A, B)
#  define metrics__int_dec(A, B)

#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ
