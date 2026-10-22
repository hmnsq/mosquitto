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

#यदि_अपरिभाषितम् SYS_वृक्षः_H
#परिभाषय SYS_वृक्षः_H

#यदि defined(WITH_SYS_वृक्षः) && defined(WITH_BROKER)

/* This ordering *must* match the metrics array in sys_tree.c. */
गणना mosq_metric_type {
	mosq_gauge_clients_योग = 0,
	mosq_गणक_clients_अधिकतमम् = 1,
	mosq_gauge_clients_disconnected = 2,
	mosq_gauge_clients_connected = 3,
	mosq_गणक_clients_expired = 4,
	mosq_gauge_सन्देशः_store_गणनम् = 5,
	mosq_gauge_सन्देशः_store_अष्टकानि = 6,
	mosq_gauge_subscriptions = 7,
	mosq_gauge_shared_subscriptions = 8,
	mosq_gauge_retained_messages = 9,
	mosq_gauge_heap_current = 10,
	mosq_गणक_heap_अधिकतमम् = 11,
	mosq_गणक_messages_received = 12,
	mosq_गणक_messages_प्रेषितम् = 13,
	mosq_गणक_अष्टकानि_received = 14,
	mosq_गणक_अष्टकानि_प्रेषितम् = 15,
	mosq_गणक_pub_अष्टकानि_received = 16,
	mosq_गणक_pub_अष्टकानि_प्रेषितम् = 17,
	mosq_gauge_बहिर्गमनम्_packets = 18,
	mosq_gauge_बहिर्गमनम्_packet_अष्टकानि = 19,
	mosq_गणक_socket_connections = 20,
	mosq_गणक_mqtt_connect_received = 21,
	mosq_गणक_mqtt_connect_प्रेषितम् = 22,
	mosq_गणक_mqtt_connack_received = 23,
	mosq_गणक_mqtt_connack_प्रेषितम् = 24,
	mosq_गणक_mqtt_publish_dropped = 25,
	mosq_गणक_mqtt_publish_received = 26,
	mosq_गणक_mqtt_publish_प्रेषितम् = 27,
	mosq_गणक_mqtt_puback_received = 28,
	mosq_गणक_mqtt_puback_प्रेषितम् = 29,
	mosq_गणक_mqtt_pubrec_received = 30,
	mosq_गणक_mqtt_pubrec_प्रेषितम् = 31,
	mosq_गणक_mqtt_pubrel_received = 32,
	mosq_गणक_mqtt_pubrel_प्रेषितम् = 33,
	mosq_गणक_mqtt_pubcomp_received = 34,
	mosq_गणक_mqtt_pubcomp_प्रेषितम् = 35,
	mosq_गणक_mqtt_subscribe_received = 36,
	mosq_गणक_mqtt_subscribe_प्रेषितम् = 37,
	mosq_गणक_mqtt_suback_received = 38,
	mosq_गणक_mqtt_suback_प्रेषितम् = 39,
	mosq_गणक_mqtt_unsubscribe_received = 40,
	mosq_गणक_mqtt_unsubscribe_प्रेषितम् = 41,
	mosq_गणक_mqtt_unsuback_received = 42,
	mosq_गणक_mqtt_unsuback_प्रेषितम् = 43,
	mosq_गणक_mqtt_pingreq_received = 44,
	mosq_गणक_mqtt_pingreq_प्रेषितम् = 45,
	mosq_गणक_mqtt_pingresp_received = 46,
	mosq_गणक_mqtt_pingresp_प्रेषितम् = 47,
	mosq_गणक_mqtt_disconnect_received = 48,
	mosq_गणक_mqtt_disconnect_प्रेषितम् = 49,
	mosq_गणक_mqtt_auth_received = 50,
	mosq_गणक_mqtt_auth_प्रेषितम् = 51,

	mosq_metric_max,
};

/* This ordering *must* match the metrics_load array in sys_tree.c. */
गणना mosq_metric_आरोपय_type {
	mosq_आरोपय_messages_received_1min = 0,
	mosq_आरोपय_messages_received_5min = 1,
	mosq_आरोपय_messages_received_15min = 2,
	mosq_आरोपय_messages_प्रेषितम्_1min = 3,
	mosq_आरोपय_messages_प्रेषितम्_5min = 4,
	mosq_आरोपय_messages_प्रेषितम्_15min = 5,
	mosq_आरोपय_pub_messages_dropped_1min = 6,
	mosq_आरोपय_pub_messages_dropped_5min = 7,
	mosq_आरोपय_pub_messages_dropped_15min = 8,
	mosq_आरोपय_pub_messages_received_1min = 9,
	mosq_आरोपय_pub_messages_received_5min = 10,
	mosq_आरोपय_pub_messages_received_15min = 11,
	mosq_आरोपय_pub_messages_प्रेषितम्_1min = 12,
	mosq_आरोपय_pub_messages_प्रेषितम्_5min = 13,
	mosq_आरोपय_pub_messages_प्रेषितम्_15min = 14,
	mosq_आरोपय_अष्टकानि_received_1min = 15,
	mosq_आरोपय_अष्टकानि_received_5min = 16,
	mosq_आरोपय_अष्टकानि_received_15min = 17,
	mosq_आरोपय_अष्टकानि_प्रेषितम्_1min = 18,
	mosq_आरोपय_अष्टकानि_प्रेषितम्_5min = 19,
	mosq_आरोपय_अष्टकानि_प्रेषितम्_15min = 20,
	mosq_आरोपय_sockets_1min = 21,
	mosq_आरोपय_sockets_5min = 22,
	mosq_आरोपय_sockets_15min = 23,
	mosq_आरोपय_connections_1min = 24,
	mosq_आरोपय_connections_5min = 25,
	mosq_आरोपय_connections_15min = 26,

	mosq_metric_आरोपय_max,
};

शून्य metrics__int_inc(गणना mosq_metric_type m, int64_t मानम्);
शून्य metrics__int_dec(गणना mosq_metric_type m, int64_t मानम्);

#अन्यथा
#  define metrics__int_inc(A, B)
#  define metrics__int_dec(A, B)

#यदि_अन्तः

#यदि_अन्तः
