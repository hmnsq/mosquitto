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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SYS_ವೃಕ್ಷ_H
#ವ್ಯಾಖ್ಯಾನಿಸು SYS_ವೃಕ್ಷ_H

#ಆದರೆ defined(WITH_SYS_ವೃಕ್ಷ) && defined(WITH_BROKER)

/* This ordering *must* match the metrics array in sys_tree.c. */
ಎಣಿಕೆ mosq_metric_type {
	mosq_gauge_clients_ಒಟ್ಟು = 0,
	mosq_counter_clients_ಗರಿಷ್ಠ = 1,
	mosq_gauge_clients_disconnected = 2,
	mosq_gauge_clients_connected = 3,
	mosq_counter_clients_expired = 4,
	mosq_gauge_ಸಂದೇಶ_store_ಎಣಿಕೆ = 5,
	mosq_gauge_ಸಂದೇಶ_store_ಬೈಟುಗಳು = 6,
	mosq_gauge_subscriptions = 7,
	mosq_gauge_shared_subscriptions = 8,
	mosq_gauge_retained_messages = 9,
	mosq_gauge_ರಾಶಿ_current = 10,
	mosq_counter_ರಾಶಿ_ಗರಿಷ್ಠ = 11,
	mosq_counter_messages_received = 12,
	mosq_counter_messages_ಕಳುಹಿಸಿದ = 13,
	mosq_counter_ಬೈಟುಗಳು_received = 14,
	mosq_counter_ಬೈಟುಗಳು_ಕಳುಹಿಸಿದ = 15,
	mosq_counter_pub_ಬೈಟುಗಳು_received = 16,
	mosq_counter_pub_ಬೈಟುಗಳು_ಕಳುಹಿಸಿದ = 17,
	mosq_gauge_ಹೊರಹೋಗು_packets = 18,
	mosq_gauge_ಹೊರಹೋಗು_packet_ಬೈಟುಗಳು = 19,
	mosq_counter_socket_connections = 20,
	mosq_counter_mqtt_connect_received = 21,
	mosq_counter_mqtt_connect_ಕಳುಹಿಸಿದ = 22,
	mosq_counter_mqtt_connack_received = 23,
	mosq_counter_mqtt_connack_ಕಳುಹಿಸಿದ = 24,
	mosq_counter_mqtt_publish_dropped = 25,
	mosq_counter_mqtt_publish_received = 26,
	mosq_counter_mqtt_publish_ಕಳುಹಿಸಿದ = 27,
	mosq_counter_mqtt_puback_received = 28,
	mosq_counter_mqtt_puback_ಕಳುಹಿಸಿದ = 29,
	mosq_counter_mqtt_pubrec_received = 30,
	mosq_counter_mqtt_pubrec_ಕಳುಹಿಸಿದ = 31,
	mosq_counter_mqtt_pubrel_received = 32,
	mosq_counter_mqtt_pubrel_ಕಳುಹಿಸಿದ = 33,
	mosq_counter_mqtt_pubcomp_received = 34,
	mosq_counter_mqtt_pubcomp_ಕಳುಹಿಸಿದ = 35,
	mosq_counter_mqtt_subscribe_received = 36,
	mosq_counter_mqtt_subscribe_ಕಳುಹಿಸಿದ = 37,
	mosq_counter_mqtt_suback_received = 38,
	mosq_counter_mqtt_suback_ಕಳುಹಿಸಿದ = 39,
	mosq_counter_mqtt_unsubscribe_received = 40,
	mosq_counter_mqtt_unsubscribe_ಕಳುಹಿಸಿದ = 41,
	mosq_counter_mqtt_unsuback_received = 42,
	mosq_counter_mqtt_unsuback_ಕಳುಹಿಸಿದ = 43,
	mosq_counter_mqtt_pingreq_received = 44,
	mosq_counter_mqtt_pingreq_ಕಳುಹಿಸಿದ = 45,
	mosq_counter_mqtt_pingresp_received = 46,
	mosq_counter_mqtt_pingresp_ಕಳುಹಿಸಿದ = 47,
	mosq_counter_mqtt_disconnect_received = 48,
	mosq_counter_mqtt_disconnect_ಕಳುಹಿಸಿದ = 49,
	mosq_counter_mqtt_auth_received = 50,
	mosq_counter_mqtt_auth_ಕಳುಹಿಸಿದ = 51,

	mosq_metric_max,
};

/* This ordering *must* match the metrics_load array in sys_tree.c. */
ಎಣಿಕೆ mosq_metric_ತುಂಬು_type {
	mosq_ತುಂಬು_messages_received_1min = 0,
	mosq_ತುಂಬು_messages_received_5min = 1,
	mosq_ತುಂಬು_messages_received_15min = 2,
	mosq_ತುಂಬು_messages_ಕಳುಹಿಸಿದ_1min = 3,
	mosq_ತುಂಬು_messages_ಕಳುಹಿಸಿದ_5min = 4,
	mosq_ತುಂಬು_messages_ಕಳುಹಿಸಿದ_15min = 5,
	mosq_ತುಂಬು_pub_messages_dropped_1min = 6,
	mosq_ತುಂಬು_pub_messages_dropped_5min = 7,
	mosq_ತುಂಬು_pub_messages_dropped_15min = 8,
	mosq_ತುಂಬು_pub_messages_received_1min = 9,
	mosq_ತುಂಬು_pub_messages_received_5min = 10,
	mosq_ತುಂಬು_pub_messages_received_15min = 11,
	mosq_ತುಂಬು_pub_messages_ಕಳುಹಿಸಿದ_1min = 12,
	mosq_ತುಂಬು_pub_messages_ಕಳುಹಿಸಿದ_5min = 13,
	mosq_ತುಂಬು_pub_messages_ಕಳುಹಿಸಿದ_15min = 14,
	mosq_ತುಂಬು_ಬೈಟುಗಳು_received_1min = 15,
	mosq_ತುಂಬು_ಬೈಟುಗಳು_received_5min = 16,
	mosq_ತುಂಬು_ಬೈಟುಗಳು_received_15min = 17,
	mosq_ತುಂಬು_ಬೈಟುಗಳು_ಕಳುಹಿಸಿದ_1min = 18,
	mosq_ತುಂಬು_ಬೈಟುಗಳು_ಕಳುಹಿಸಿದ_5min = 19,
	mosq_ತುಂಬು_ಬೈಟುಗಳು_ಕಳುಹಿಸಿದ_15min = 20,
	mosq_ತುಂಬು_sockets_1min = 21,
	mosq_ತುಂಬು_sockets_5min = 22,
	mosq_ತುಂಬು_sockets_15min = 23,
	mosq_ತುಂಬು_connections_1min = 24,
	mosq_ತುಂಬು_connections_5min = 25,
	mosq_ತುಂಬು_connections_15min = 26,

	mosq_metric_ತುಂಬು_max,
};

ಶೂನ್ಯ metrics__int_inc(ಎಣಿಕೆ mosq_metric_type m, int64_t ಮೌಲ್ಯ);
ಶೂನ್ಯ metrics__int_dec(ಎಣಿಕೆ mosq_metric_type m, int64_t ಮೌಲ್ಯ);

#ಇಲ್ಲದಿದ್ದರೆ
#  define metrics__int_inc(A, B)
#  define metrics__int_dec(A, B)

#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ
