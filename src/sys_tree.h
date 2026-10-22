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

#నిర్వచితం_కాకపోతే SYS_వృక్షం_H
#నిర్వచించు SYS_వృక్షం_H

#ఒకవేళ defined(WITH_SYS_వృక్షం) && defined(WITH_BROKER)

/* This ordering *must* match the metrics array in sys_tree.c. */
గణన mosq_metric_type {
	mosq_gauge_clients_మొత్తం = 0,
	mosq_counter_clients_గరిష్ఠ = 1,
	mosq_gauge_clients_disconnected = 2,
	mosq_gauge_clients_connected = 3,
	mosq_counter_clients_expired = 4,
	mosq_gauge_సందేశం_store_లెక్క = 5,
	mosq_gauge_సందేశం_store_బైట్లు = 6,
	mosq_gauge_subscriptions = 7,
	mosq_gauge_shared_subscriptions = 8,
	mosq_gauge_retained_messages = 9,
	mosq_gauge_కుప్ప_current = 10,
	mosq_counter_కుప్ప_గరిష్ఠ = 11,
	mosq_counter_messages_received = 12,
	mosq_counter_messages_పంపిన = 13,
	mosq_counter_బైట్లు_received = 14,
	mosq_counter_బైట్లు_పంపిన = 15,
	mosq_counter_pub_బైట్లు_received = 16,
	mosq_counter_pub_బైట్లు_పంపిన = 17,
	mosq_gauge_బహిర్గతం_packets = 18,
	mosq_gauge_బహిర్గతం_packet_బైట్లు = 19,
	mosq_counter_socket_connections = 20,
	mosq_counter_mqtt_connect_received = 21,
	mosq_counter_mqtt_connect_పంపిన = 22,
	mosq_counter_mqtt_connack_received = 23,
	mosq_counter_mqtt_connack_పంపిన = 24,
	mosq_counter_mqtt_publish_dropped = 25,
	mosq_counter_mqtt_publish_received = 26,
	mosq_counter_mqtt_publish_పంపిన = 27,
	mosq_counter_mqtt_puback_received = 28,
	mosq_counter_mqtt_puback_పంపిన = 29,
	mosq_counter_mqtt_pubrec_received = 30,
	mosq_counter_mqtt_pubrec_పంపిన = 31,
	mosq_counter_mqtt_pubrel_received = 32,
	mosq_counter_mqtt_pubrel_పంపిన = 33,
	mosq_counter_mqtt_pubcomp_received = 34,
	mosq_counter_mqtt_pubcomp_పంపిన = 35,
	mosq_counter_mqtt_subscribe_received = 36,
	mosq_counter_mqtt_subscribe_పంపిన = 37,
	mosq_counter_mqtt_suback_received = 38,
	mosq_counter_mqtt_suback_పంపిన = 39,
	mosq_counter_mqtt_unsubscribe_received = 40,
	mosq_counter_mqtt_unsubscribe_పంపిన = 41,
	mosq_counter_mqtt_unsuback_received = 42,
	mosq_counter_mqtt_unsuback_పంపిన = 43,
	mosq_counter_mqtt_pingreq_received = 44,
	mosq_counter_mqtt_pingreq_పంపిన = 45,
	mosq_counter_mqtt_pingresp_received = 46,
	mosq_counter_mqtt_pingresp_పంపిన = 47,
	mosq_counter_mqtt_disconnect_received = 48,
	mosq_counter_mqtt_disconnect_పంపిన = 49,
	mosq_counter_mqtt_auth_received = 50,
	mosq_counter_mqtt_auth_పంపిన = 51,

	mosq_metric_max,
};

/* This ordering *must* match the metrics_load array in sys_tree.c. */
గణన mosq_metric_ఎక్కించు_type {
	mosq_ఎక్కించు_messages_received_1min = 0,
	mosq_ఎక్కించు_messages_received_5min = 1,
	mosq_ఎక్కించు_messages_received_15min = 2,
	mosq_ఎక్కించు_messages_పంపిన_1min = 3,
	mosq_ఎక్కించు_messages_పంపిన_5min = 4,
	mosq_ఎక్కించు_messages_పంపిన_15min = 5,
	mosq_ఎక్కించు_pub_messages_dropped_1min = 6,
	mosq_ఎక్కించు_pub_messages_dropped_5min = 7,
	mosq_ఎక్కించు_pub_messages_dropped_15min = 8,
	mosq_ఎక్కించు_pub_messages_received_1min = 9,
	mosq_ఎక్కించు_pub_messages_received_5min = 10,
	mosq_ఎక్కించు_pub_messages_received_15min = 11,
	mosq_ఎక్కించు_pub_messages_పంపిన_1min = 12,
	mosq_ఎక్కించు_pub_messages_పంపిన_5min = 13,
	mosq_ఎక్కించు_pub_messages_పంపిన_15min = 14,
	mosq_ఎక్కించు_బైట్లు_received_1min = 15,
	mosq_ఎక్కించు_బైట్లు_received_5min = 16,
	mosq_ఎక్కించు_బైట్లు_received_15min = 17,
	mosq_ఎక్కించు_బైట్లు_పంపిన_1min = 18,
	mosq_ఎక్కించు_బైట్లు_పంపిన_5min = 19,
	mosq_ఎక్కించు_బైట్లు_పంపిన_15min = 20,
	mosq_ఎక్కించు_sockets_1min = 21,
	mosq_ఎక్కించు_sockets_5min = 22,
	mosq_ఎక్కించు_sockets_15min = 23,
	mosq_ఎక్కించు_connections_1min = 24,
	mosq_ఎక్కించు_connections_5min = 25,
	mosq_ఎక్కించు_connections_15min = 26,

	mosq_metric_ఎక్కించు_max,
};

శూన్య metrics__int_inc(గణన mosq_metric_type m, int64_t విలువ);
శూన్య metrics__int_dec(గణన mosq_metric_type m, int64_t విలువ);

#లేకపోతే
#  define metrics__int_inc(A, B)
#  define metrics__int_dec(A, B)

#అయితే_ముగింపు

#అయితే_ముగింపు
