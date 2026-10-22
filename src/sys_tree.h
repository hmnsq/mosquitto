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

#جے_بنا_تعریف SYS_درخت_H
#تعریف_کرو SYS_درخت_H

#إذا defined(WITH_SYS_درخت) && defined(WITH_BROKER)

/* This ordering *must* match the metrics array in sys_tree.c. */
تعداد mosq_metric_type {
	mosq_gauge_clients_کل = 0,
	mosq_counter_clients_ودھ_توں_ودھ = 1,
	mosq_gauge_clients_disconnected = 2,
	mosq_gauge_clients_connected = 3,
	mosq_counter_clients_expired = 4,
	mosq_gauge_سنیہا_store_گنتی = 5,
	mosq_gauge_سنیہا_store_بائٹاں = 6,
	mosq_gauge_subscriptions = 7,
	mosq_gauge_shared_subscriptions = 8,
	mosq_gauge_retained_messages = 9,
	mosq_gauge_ڈھیر_current = 10,
	mosq_counter_ڈھیر_ودھ_توں_ودھ = 11,
	mosq_counter_messages_received = 12,
	mosq_counter_messages_گھلیا = 13,
	mosq_counter_بائٹاں_received = 14,
	mosq_counter_بائٹاں_گھلیا = 15,
	mosq_counter_pub_بائٹاں_received = 16,
	mosq_counter_pub_بائٹاں_گھلیا = 17,
	mosq_gauge_باہرلا_packets = 18,
	mosq_gauge_باہرلا_packet_بائٹاں = 19,
	mosq_counter_socket_connections = 20,
	mosq_counter_mqtt_connect_received = 21,
	mosq_counter_mqtt_connect_گھلیا = 22,
	mosq_counter_mqtt_connack_received = 23,
	mosq_counter_mqtt_connack_گھلیا = 24,
	mosq_counter_mqtt_publish_dropped = 25,
	mosq_counter_mqtt_publish_received = 26,
	mosq_counter_mqtt_publish_گھلیا = 27,
	mosq_counter_mqtt_puback_received = 28,
	mosq_counter_mqtt_puback_گھلیا = 29,
	mosq_counter_mqtt_pubrec_received = 30,
	mosq_counter_mqtt_pubrec_گھلیا = 31,
	mosq_counter_mqtt_pubrel_received = 32,
	mosq_counter_mqtt_pubrel_گھلیا = 33,
	mosq_counter_mqtt_pubcomp_received = 34,
	mosq_counter_mqtt_pubcomp_گھلیا = 35,
	mosq_counter_mqtt_subscribe_received = 36,
	mosq_counter_mqtt_subscribe_گھلیا = 37,
	mosq_counter_mqtt_suback_received = 38,
	mosq_counter_mqtt_suback_گھلیا = 39,
	mosq_counter_mqtt_unsubscribe_received = 40,
	mosq_counter_mqtt_unsubscribe_گھلیا = 41,
	mosq_counter_mqtt_unsuback_received = 42,
	mosq_counter_mqtt_unsuback_گھلیا = 43,
	mosq_counter_mqtt_pingreq_received = 44,
	mosq_counter_mqtt_pingreq_گھلیا = 45,
	mosq_counter_mqtt_pingresp_received = 46,
	mosq_counter_mqtt_pingresp_گھلیا = 47,
	mosq_counter_mqtt_disconnect_received = 48,
	mosq_counter_mqtt_disconnect_گھلیا = 49,
	mosq_counter_mqtt_auth_received = 50,
	mosq_counter_mqtt_auth_گھلیا = 51,

	mosq_metric_max,
};

/* This ordering *must* match the metrics_load array in sys_tree.c. */
تعداد mosq_metric_لدو_type {
	mosq_لدو_messages_received_1min = 0,
	mosq_لدو_messages_received_5min = 1,
	mosq_لدو_messages_received_15min = 2,
	mosq_لدو_messages_گھلیا_1min = 3,
	mosq_لدو_messages_گھلیا_5min = 4,
	mosq_لدو_messages_گھلیا_15min = 5,
	mosq_لدو_pub_messages_dropped_1min = 6,
	mosq_لدو_pub_messages_dropped_5min = 7,
	mosq_لدو_pub_messages_dropped_15min = 8,
	mosq_لدو_pub_messages_received_1min = 9,
	mosq_لدو_pub_messages_received_5min = 10,
	mosq_لدو_pub_messages_received_15min = 11,
	mosq_لدو_pub_messages_گھلیا_1min = 12,
	mosq_لدو_pub_messages_گھلیا_5min = 13,
	mosq_لدو_pub_messages_گھلیا_15min = 14,
	mosq_لدو_بائٹاں_received_1min = 15,
	mosq_لدو_بائٹاں_received_5min = 16,
	mosq_لدو_بائٹاں_received_15min = 17,
	mosq_لدو_بائٹاں_گھلیا_1min = 18,
	mosq_لدو_بائٹاں_گھلیا_5min = 19,
	mosq_لدو_بائٹاں_گھلیا_15min = 20,
	mosq_لدو_sockets_1min = 21,
	mosq_لدو_sockets_5min = 22,
	mosq_لدو_sockets_15min = 23,
	mosq_لدو_connections_1min = 24,
	mosq_لدو_connections_5min = 25,
	mosq_لدو_connections_15min = 26,

	mosq_metric_لدو_max,
};

فراغ metrics__int_inc(تعداد mosq_metric_type m, int64_t قدر);
فراغ metrics__int_dec(تعداد mosq_metric_type m, int64_t قدر);

#وإلا
#  define metrics__int_inc(A, B)
#  define metrics__int_dec(A, B)

#جے_انت

#جے_انت
