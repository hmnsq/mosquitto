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

#اگر_غیر_تعریف_شدہ SYS_درخت_H
#تعریف_کرو SYS_درخت_H

#اگر defined(WITH_SYS_درخت) && defined(WITH_BROKER)

/* This ordering *must* match the metrics array in sys_tree.c. */
شمار mosq_metric_type {
	mosq_gauge_clients_کل = 0,
	mosq_شمار_clients_زیادہ_سے_زیادہ = 1,
	mosq_gauge_clients_disconnected = 2,
	mosq_gauge_clients_connected = 3,
	mosq_شمار_clients_expired = 4,
	mosq_gauge_پیغام_store_گنتی = 5,
	mosq_gauge_پیغام_store_بائٹیں = 6,
	mosq_gauge_subscriptions = 7,
	mosq_gauge_shared_subscriptions = 8,
	mosq_gauge_retained_messages = 9,
	mosq_gauge_ڈھیر_current = 10,
	mosq_شمار_ڈھیر_زیادہ_سے_زیادہ = 11,
	mosq_شمار_messages_received = 12,
	mosq_شمار_messages_بھیجا = 13,
	mosq_شمار_بائٹیں_received = 14,
	mosq_شمار_بائٹیں_بھیجا = 15,
	mosq_شمار_pub_بائٹیں_received = 16,
	mosq_شمار_pub_بائٹیں_بھیجا = 17,
	mosq_gauge_برآمد_packets = 18,
	mosq_gauge_برآمد_packet_بائٹیں = 19,
	mosq_شمار_socket_connections = 20,
	mosq_شمار_mqtt_connect_received = 21,
	mosq_شمار_mqtt_connect_بھیجا = 22,
	mosq_شمار_mqtt_connack_received = 23,
	mosq_شمار_mqtt_connack_بھیجا = 24,
	mosq_شمار_mqtt_publish_dropped = 25,
	mosq_شمار_mqtt_publish_received = 26,
	mosq_شمار_mqtt_publish_بھیجا = 27,
	mosq_شمار_mqtt_puback_received = 28,
	mosq_شمار_mqtt_puback_بھیجا = 29,
	mosq_شمار_mqtt_pubrec_received = 30,
	mosq_شمار_mqtt_pubrec_بھیجا = 31,
	mosq_شمار_mqtt_pubrel_received = 32,
	mosq_شمار_mqtt_pubrel_بھیجا = 33,
	mosq_شمار_mqtt_pubcomp_received = 34,
	mosq_شمار_mqtt_pubcomp_بھیجا = 35,
	mosq_شمار_mqtt_subscribe_received = 36,
	mosq_شمار_mqtt_subscribe_بھیجا = 37,
	mosq_شمار_mqtt_suback_received = 38,
	mosq_شمار_mqtt_suback_بھیجا = 39,
	mosq_شمار_mqtt_unsubscribe_received = 40,
	mosq_شمار_mqtt_unsubscribe_بھیجا = 41,
	mosq_شمار_mqtt_unsuback_received = 42,
	mosq_شمار_mqtt_unsuback_بھیجا = 43,
	mosq_شمار_mqtt_pingreq_received = 44,
	mosq_شمار_mqtt_pingreq_بھیجا = 45,
	mosq_شمار_mqtt_pingresp_received = 46,
	mosq_شمار_mqtt_pingresp_بھیجا = 47,
	mosq_شمار_mqtt_disconnect_received = 48,
	mosq_شمار_mqtt_disconnect_بھیجا = 49,
	mosq_شمار_mqtt_auth_received = 50,
	mosq_شمار_mqtt_auth_بھیجا = 51,

	mosq_metric_max,
};

/* This ordering *must* match the metrics_load array in sys_tree.c. */
شمار mosq_metric_بھرو_type {
	mosq_بھرو_messages_received_1min = 0,
	mosq_بھرو_messages_received_5min = 1,
	mosq_بھرو_messages_received_15min = 2,
	mosq_بھرو_messages_بھیجا_1min = 3,
	mosq_بھرو_messages_بھیجا_5min = 4,
	mosq_بھرو_messages_بھیجا_15min = 5,
	mosq_بھرو_pub_messages_dropped_1min = 6,
	mosq_بھرو_pub_messages_dropped_5min = 7,
	mosq_بھرو_pub_messages_dropped_15min = 8,
	mosq_بھرو_pub_messages_received_1min = 9,
	mosq_بھرو_pub_messages_received_5min = 10,
	mosq_بھرو_pub_messages_received_15min = 11,
	mosq_بھرو_pub_messages_بھیجا_1min = 12,
	mosq_بھرو_pub_messages_بھیجا_5min = 13,
	mosq_بھرو_pub_messages_بھیجا_15min = 14,
	mosq_بھرو_بائٹیں_received_1min = 15,
	mosq_بھرو_بائٹیں_received_5min = 16,
	mosq_بھرو_بائٹیں_received_15min = 17,
	mosq_بھرو_بائٹیں_بھیجا_1min = 18,
	mosq_بھرو_بائٹیں_بھیجا_5min = 19,
	mosq_بھرو_بائٹیں_بھیجا_15min = 20,
	mosq_بھرو_sockets_1min = 21,
	mosq_بھرو_sockets_5min = 22,
	mosq_بھرو_sockets_15min = 23,
	mosq_بھرو_connections_1min = 24,
	mosq_بھرو_connections_5min = 25,
	mosq_بھرو_connections_15min = 26,

	mosq_metric_بھرو_max,
};

خالی metrics__int_inc(شمار mosq_metric_type m, int64_t قدر);
خالی metrics__int_dec(شمار mosq_metric_type m, int64_t قدر);

#ورنہ
#  define metrics__int_inc(A, B)
#  define metrics__int_dec(A, B)

#اگر_ختم

#اگر_ختم
