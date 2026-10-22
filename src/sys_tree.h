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

#اگر_تعریف_نشده SYS_درخت_H
#تعریف_کن SYS_درخت_H

#إذا defined(WITH_SYS_درخت) && defined(WITH_BROKER)

/* This ordering *must* match the metrics array in sys_tree.c. */
تعداد mosq_metric_type {
	mosq_gauge_clients_مجموع = 0,
	mosq_counter_clients_بیشینه = 1,
	mosq_gauge_clients_disconnected = 2,
	mosq_gauge_clients_connected = 3,
	mosq_counter_clients_expired = 4,
	mosq_gauge_پیام_store_شمار = 5,
	mosq_gauge_پیام_store_بایتها = 6,
	mosq_gauge_subscriptions = 7,
	mosq_gauge_shared_subscriptions = 8,
	mosq_gauge_retained_messages = 9,
	mosq_gauge_انباشته_current = 10,
	mosq_counter_انباشته_بیشینه = 11,
	mosq_counter_messages_received = 12,
	mosq_counter_messages_فرستاده = 13,
	mosq_counter_بایتها_received = 14,
	mosq_counter_بایتها_فرستاده = 15,
	mosq_counter_pub_بایتها_received = 16,
	mosq_counter_pub_بایتها_فرستاده = 17,
	mosq_gauge_بیرون_packets = 18,
	mosq_gauge_بیرون_packet_بایتها = 19,
	mosq_counter_socket_connections = 20,
	mosq_counter_mqtt_connect_received = 21,
	mosq_counter_mqtt_connect_فرستاده = 22,
	mosq_counter_mqtt_connack_received = 23,
	mosq_counter_mqtt_connack_فرستاده = 24,
	mosq_counter_mqtt_publish_dropped = 25,
	mosq_counter_mqtt_publish_received = 26,
	mosq_counter_mqtt_publish_فرستاده = 27,
	mosq_counter_mqtt_puback_received = 28,
	mosq_counter_mqtt_puback_فرستاده = 29,
	mosq_counter_mqtt_pubrec_received = 30,
	mosq_counter_mqtt_pubrec_فرستاده = 31,
	mosq_counter_mqtt_pubrel_received = 32,
	mosq_counter_mqtt_pubrel_فرستاده = 33,
	mosq_counter_mqtt_pubcomp_received = 34,
	mosq_counter_mqtt_pubcomp_فرستاده = 35,
	mosq_counter_mqtt_subscribe_received = 36,
	mosq_counter_mqtt_subscribe_فرستاده = 37,
	mosq_counter_mqtt_suback_received = 38,
	mosq_counter_mqtt_suback_فرستاده = 39,
	mosq_counter_mqtt_unsubscribe_received = 40,
	mosq_counter_mqtt_unsubscribe_فرستاده = 41,
	mosq_counter_mqtt_unsuback_received = 42,
	mosq_counter_mqtt_unsuback_فرستاده = 43,
	mosq_counter_mqtt_pingreq_received = 44,
	mosq_counter_mqtt_pingreq_فرستاده = 45,
	mosq_counter_mqtt_pingresp_received = 46,
	mosq_counter_mqtt_pingresp_فرستاده = 47,
	mosq_counter_mqtt_disconnect_received = 48,
	mosq_counter_mqtt_disconnect_فرستاده = 49,
	mosq_counter_mqtt_auth_received = 50,
	mosq_counter_mqtt_auth_فرستاده = 51,

	mosq_metric_max,
};

/* This ordering *must* match the metrics_load array in sys_tree.c. */
تعداد mosq_metric_بارگیری_type {
	mosq_بارگیری_messages_received_1min = 0,
	mosq_بارگیری_messages_received_5min = 1,
	mosq_بارگیری_messages_received_15min = 2,
	mosq_بارگیری_messages_فرستاده_1min = 3,
	mosq_بارگیری_messages_فرستاده_5min = 4,
	mosq_بارگیری_messages_فرستاده_15min = 5,
	mosq_بارگیری_pub_messages_dropped_1min = 6,
	mosq_بارگیری_pub_messages_dropped_5min = 7,
	mosq_بارگیری_pub_messages_dropped_15min = 8,
	mosq_بارگیری_pub_messages_received_1min = 9,
	mosq_بارگیری_pub_messages_received_5min = 10,
	mosq_بارگیری_pub_messages_received_15min = 11,
	mosq_بارگیری_pub_messages_فرستاده_1min = 12,
	mosq_بارگیری_pub_messages_فرستاده_5min = 13,
	mosq_بارگیری_pub_messages_فرستاده_15min = 14,
	mosq_بارگیری_بایتها_received_1min = 15,
	mosq_بارگیری_بایتها_received_5min = 16,
	mosq_بارگیری_بایتها_received_15min = 17,
	mosq_بارگیری_بایتها_فرستاده_1min = 18,
	mosq_بارگیری_بایتها_فرستاده_5min = 19,
	mosq_بارگیری_بایتها_فرستاده_15min = 20,
	mosq_بارگیری_sockets_1min = 21,
	mosq_بارگیری_sockets_5min = 22,
	mosq_بارگیری_sockets_15min = 23,
	mosq_بارگیری_connections_1min = 24,
	mosq_بارگیری_connections_5min = 25,
	mosq_بارگیری_connections_15min = 26,

	mosq_metric_بارگیری_max,
};

فراغ metrics__int_inc(تعداد mosq_metric_type m, int64_t مقدار);
فراغ metrics__int_dec(تعداد mosq_metric_type m, int64_t مقدار);

#وإلا
#  define metrics__int_inc(A, B)
#  define metrics__int_dec(A, B)

#پایان_اگر

#پایان_اگر
