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

#که_نه_تعریف_شوی SYS_ونه_H
#تعریف_کړه SYS_ونه_H

#إذا defined(WITH_SYS_ونه) && defined(WITH_BROKER)

/* This ordering *must* match the metrics array in sys_tree.c. */
تعداد mosq_metric_type {
	mosq_gauge_clients_ټول = 0,
	mosq_counter_clients_اعظمي = 1,
	mosq_gauge_clients_disconnected = 2,
	mosq_gauge_clients_connected = 3,
	mosq_counter_clients_expired = 4,
	mosq_gauge_پیغام_store_شمېر = 5,
	mosq_gauge_پیغام_store_بایټونه = 6,
	mosq_gauge_subscriptions = 7,
	mosq_gauge_shared_subscriptions = 8,
	mosq_gauge_retained_messages = 9,
	mosq_gauge_ډېرۍ_current = 10,
	mosq_counter_ډېرۍ_اعظمي = 11,
	mosq_counter_messages_received = 12,
	mosq_counter_messages_لېږل_شوی = 13,
	mosq_counter_بایټونه_received = 14,
	mosq_counter_بایټونه_لېږل_شوی = 15,
	mosq_counter_pub_بایټونه_received = 16,
	mosq_counter_pub_بایټونه_لېږل_شوی = 17,
	mosq_gauge_بهر_packets = 18,
	mosq_gauge_بهر_packet_بایټونه = 19,
	mosq_counter_socket_connections = 20,
	mosq_counter_mqtt_connect_received = 21,
	mosq_counter_mqtt_connect_لېږل_شوی = 22,
	mosq_counter_mqtt_connack_received = 23,
	mosq_counter_mqtt_connack_لېږل_شوی = 24,
	mosq_counter_mqtt_publish_dropped = 25,
	mosq_counter_mqtt_publish_received = 26,
	mosq_counter_mqtt_publish_لېږل_شوی = 27,
	mosq_counter_mqtt_puback_received = 28,
	mosq_counter_mqtt_puback_لېږل_شوی = 29,
	mosq_counter_mqtt_pubrec_received = 30,
	mosq_counter_mqtt_pubrec_لېږل_شوی = 31,
	mosq_counter_mqtt_pubrel_received = 32,
	mosq_counter_mqtt_pubrel_لېږل_شوی = 33,
	mosq_counter_mqtt_pubcomp_received = 34,
	mosq_counter_mqtt_pubcomp_لېږل_شوی = 35,
	mosq_counter_mqtt_subscribe_received = 36,
	mosq_counter_mqtt_subscribe_لېږل_شوی = 37,
	mosq_counter_mqtt_suback_received = 38,
	mosq_counter_mqtt_suback_لېږل_شوی = 39,
	mosq_counter_mqtt_unsubscribe_received = 40,
	mosq_counter_mqtt_unsubscribe_لېږل_شوی = 41,
	mosq_counter_mqtt_unsuback_received = 42,
	mosq_counter_mqtt_unsuback_لېږل_شوی = 43,
	mosq_counter_mqtt_pingreq_received = 44,
	mosq_counter_mqtt_pingreq_لېږل_شوی = 45,
	mosq_counter_mqtt_pingresp_received = 46,
	mosq_counter_mqtt_pingresp_لېږل_شوی = 47,
	mosq_counter_mqtt_disconnect_received = 48,
	mosq_counter_mqtt_disconnect_لېږل_شوی = 49,
	mosq_counter_mqtt_auth_received = 50,
	mosq_counter_mqtt_auth_لېږل_شوی = 51,

	mosq_metric_max,
};

/* This ordering *must* match the metrics_load array in sys_tree.c. */
تعداد mosq_metric_بارول_type {
	mosq_بارول_messages_received_1min = 0,
	mosq_بارول_messages_received_5min = 1,
	mosq_بارول_messages_received_15min = 2,
	mosq_بارول_messages_لېږل_شوی_1min = 3,
	mosq_بارول_messages_لېږل_شوی_5min = 4,
	mosq_بارول_messages_لېږل_شوی_15min = 5,
	mosq_بارول_pub_messages_dropped_1min = 6,
	mosq_بارول_pub_messages_dropped_5min = 7,
	mosq_بارول_pub_messages_dropped_15min = 8,
	mosq_بارول_pub_messages_received_1min = 9,
	mosq_بارول_pub_messages_received_5min = 10,
	mosq_بارول_pub_messages_received_15min = 11,
	mosq_بارول_pub_messages_لېږل_شوی_1min = 12,
	mosq_بارول_pub_messages_لېږل_شوی_5min = 13,
	mosq_بارول_pub_messages_لېږل_شوی_15min = 14,
	mosq_بارول_بایټونه_received_1min = 15,
	mosq_بارول_بایټونه_received_5min = 16,
	mosq_بارول_بایټونه_received_15min = 17,
	mosq_بارول_بایټونه_لېږل_شوی_1min = 18,
	mosq_بارول_بایټونه_لېږل_شوی_5min = 19,
	mosq_بارول_بایټونه_لېږل_شوی_15min = 20,
	mosq_بارول_sockets_1min = 21,
	mosq_بارول_sockets_5min = 22,
	mosq_بارول_sockets_15min = 23,
	mosq_بارول_connections_1min = 24,
	mosq_بارول_connections_5min = 25,
	mosq_بارول_connections_15min = 26,

	mosq_metric_بارول_max,
};

فراغ metrics__int_inc(تعداد mosq_metric_type m, int64_t ارزښت);
فراغ metrics__int_dec(تعداد mosq_metric_type m, int64_t ارزښت);

#وإلا
#  define metrics__int_inc(A, B)
#  define metrics__int_dec(A, B)

#د_که_پای

#د_که_پای
