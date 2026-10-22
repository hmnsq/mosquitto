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

#جيڪڏهن_وصف_نه_ٿيل SYS_وڻ_H
#وصف_ڪريو SYS_وڻ_H

#إذا defined(WITH_SYS_وڻ) && defined(WITH_BROKER)

/* This ordering *must* match the metrics array in sys_tree.c. */
تعداد mosq_metric_type {
	mosq_gauge_clients_ڪل = 0,
	mosq_counter_clients_وڌ_کان_وڌ = 1,
	mosq_gauge_clients_disconnected = 2,
	mosq_gauge_clients_connected = 3,
	mosq_counter_clients_expired = 4,
	mosq_gauge_پيغام_store_ڳڻپ = 5,
	mosq_gauge_پيغام_store_بائيٽون = 6,
	mosq_gauge_subscriptions = 7,
	mosq_gauge_shared_subscriptions = 8,
	mosq_gauge_retained_messages = 9,
	mosq_gauge_ڍير_current = 10,
	mosq_counter_ڍير_وڌ_کان_وڌ = 11,
	mosq_counter_messages_received = 12,
	mosq_counter_messages_موڪليل = 13,
	mosq_counter_بائيٽون_received = 14,
	mosq_counter_بائيٽون_موڪليل = 15,
	mosq_counter_pub_بائيٽون_received = 16,
	mosq_counter_pub_بائيٽون_موڪليل = 17,
	mosq_gauge_ٻاهريون_packets = 18,
	mosq_gauge_ٻاهريون_packet_بائيٽون = 19,
	mosq_counter_socket_connections = 20,
	mosq_counter_mqtt_connect_received = 21,
	mosq_counter_mqtt_connect_موڪليل = 22,
	mosq_counter_mqtt_connack_received = 23,
	mosq_counter_mqtt_connack_موڪليل = 24,
	mosq_counter_mqtt_publish_dropped = 25,
	mosq_counter_mqtt_publish_received = 26,
	mosq_counter_mqtt_publish_موڪليل = 27,
	mosq_counter_mqtt_puback_received = 28,
	mosq_counter_mqtt_puback_موڪليل = 29,
	mosq_counter_mqtt_pubrec_received = 30,
	mosq_counter_mqtt_pubrec_موڪليل = 31,
	mosq_counter_mqtt_pubrel_received = 32,
	mosq_counter_mqtt_pubrel_موڪليل = 33,
	mosq_counter_mqtt_pubcomp_received = 34,
	mosq_counter_mqtt_pubcomp_موڪليل = 35,
	mosq_counter_mqtt_subscribe_received = 36,
	mosq_counter_mqtt_subscribe_موڪليل = 37,
	mosq_counter_mqtt_suback_received = 38,
	mosq_counter_mqtt_suback_موڪليل = 39,
	mosq_counter_mqtt_unsubscribe_received = 40,
	mosq_counter_mqtt_unsubscribe_موڪليل = 41,
	mosq_counter_mqtt_unsuback_received = 42,
	mosq_counter_mqtt_unsuback_موڪليل = 43,
	mosq_counter_mqtt_pingreq_received = 44,
	mosq_counter_mqtt_pingreq_موڪليل = 45,
	mosq_counter_mqtt_pingresp_received = 46,
	mosq_counter_mqtt_pingresp_موڪليل = 47,
	mosq_counter_mqtt_disconnect_received = 48,
	mosq_counter_mqtt_disconnect_موڪليل = 49,
	mosq_counter_mqtt_auth_received = 50,
	mosq_counter_mqtt_auth_موڪليل = 51,

	mosq_metric_max,
};

/* This ordering *must* match the metrics_load array in sys_tree.c. */
تعداد mosq_metric_ڀريو_type {
	mosq_ڀريو_messages_received_1min = 0,
	mosq_ڀريو_messages_received_5min = 1,
	mosq_ڀريو_messages_received_15min = 2,
	mosq_ڀريو_messages_موڪليل_1min = 3,
	mosq_ڀريو_messages_موڪليل_5min = 4,
	mosq_ڀريو_messages_موڪليل_15min = 5,
	mosq_ڀريو_pub_messages_dropped_1min = 6,
	mosq_ڀريو_pub_messages_dropped_5min = 7,
	mosq_ڀريو_pub_messages_dropped_15min = 8,
	mosq_ڀريو_pub_messages_received_1min = 9,
	mosq_ڀريو_pub_messages_received_5min = 10,
	mosq_ڀريو_pub_messages_received_15min = 11,
	mosq_ڀريو_pub_messages_موڪليل_1min = 12,
	mosq_ڀريو_pub_messages_موڪليل_5min = 13,
	mosq_ڀريو_pub_messages_موڪليل_15min = 14,
	mosq_ڀريو_بائيٽون_received_1min = 15,
	mosq_ڀريو_بائيٽون_received_5min = 16,
	mosq_ڀريو_بائيٽون_received_15min = 17,
	mosq_ڀريو_بائيٽون_موڪليل_1min = 18,
	mosq_ڀريو_بائيٽون_موڪليل_5min = 19,
	mosq_ڀريو_بائيٽون_موڪليل_15min = 20,
	mosq_ڀريو_sockets_1min = 21,
	mosq_ڀريو_sockets_5min = 22,
	mosq_ڀريو_sockets_15min = 23,
	mosq_ڀريو_connections_1min = 24,
	mosq_ڀريو_connections_5min = 25,
	mosq_ڀريو_connections_15min = 26,

	mosq_metric_ڀريو_max,
};

فراغ metrics__int_inc(تعداد mosq_metric_type m, int64_t قدر);
فراغ metrics__int_dec(تعداد mosq_metric_type m, int64_t قدر);

#وإلا
#  define metrics__int_inc(A, B)
#  define metrics__int_dec(A, B)

#جيڪڏهن_پورو

#جيڪڏهن_پورو
