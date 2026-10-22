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

#నిర్వచితమైతే WITH_SYS_వృక్షం

#అన్తర్భావ "config.h"

#అన్తర్భావ <math.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <inttypes.h>
#అన్తర్భావ <limits.h>
#అన్తర్భావ <inttypes.h>

#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "sys_tree.h"

#నిర్వచించు BUFLEN 100

#నిర్వచించు SYS_వృక్షం_QOS 2

#నిర్వచించు METRIC_ఎక్కించు_1_MIN 1
#నిర్వచించు METRIC_ఎక్కించు_5_MIN 2
#నిర్వచించు METRIC_ఎక్కించు_15_MIN 3

నిర్మాణం metric {
	int64_t current;
	int64_t తదుపరి;
	స్థిరరాశి అక్షరం *topic, *topic_alias;
	bool is_max;
};

నిర్మాణం metric_ఎక్కించు {
	ద్విగుణ current;
	స్థిరరాశి అక్షరం *topic;
	పూర్ణాంకం ఎక్కించు_ref;
	uint8_t interval;
};

నిర్మాణం metric metrics[mosq_metric_max] = {
	{ 1, 0, "$SYS/broker/clients/total", ఖాళీ, false }, /* mosq_gauge_clients_total */
	{ 1, 0, "$SYS/broker/clients/maximum", ఖాళీ, true }, /* metric_clients_maximum */
	{ 1, 0, "$SYS/broker/clients/disconnected", "$SYS/broker/clients/inactive", false }, /* mosq_gauge_clients_disconnected */
	{ 1, 0, "$SYS/broker/clients/connected", "$SYS/broker/clients/active", false }, /* mosq_gauge_clients_connected */
	{ 1, 0, "$SYS/broker/clients/expired", ఖాళీ, false }, /* mosq_counter_clients_expired */
	{ 1, 0, "$SYS/broker/messages/stored", "$SYS/broker/store/messages/count", false }, /* mosq_gauge_message_store_count */
	{ 1, 0, "$SYS/broker/store/messages/bytes", ఖాళీ, false }, /* mosq_gauge_message_store_bytes */
	{ 1, 0, "$SYS/broker/subscriptions/count", ఖాళీ, false }, /* mosq_gauge_subscription_count */
	{ 1, 0, "$SYS/broker/shared_subscriptions/count", ఖాళీ, false }, /* mosq_gauge_shared_subscription_count */
	{ 1, 0, "$SYS/broker/retained messages/count", ఖాళీ, false }, /* mosq_gauge_retained_message_count */
#నిర్వచితమైతే WITH_జ్ఞాపకం_TRACKING
	{ 1, 0, "$SYS/broker/heap/current", ఖాళీ, false }, /* mosq_gauge_heap_current */
	{ 1, 0, "$SYS/broker/heap/maximum", ఖాళీ, true }, /* mosq_gauge_heap_maximum */
#లేకపోతే
	{ 1, 0, ఖాళీ, ఖాళీ, 0 }, /* mosq_gauge_heap_current */
	{ 1, 0, ఖాళీ, ఖాళీ, 0 }, /* mosq_gauge_heap_maximum */
#అయితే_ముగింపు
	{ 1, 0, "$SYS/broker/messages/received", ఖాళీ, false }, /* mosq_counter_messages_received */
	{ 1, 0, "$SYS/broker/messages/sent", ఖాళీ, false }, /* mosq_counter_messages_sent */
	{ 1, 0, "$SYS/broker/bytes/received", ఖాళీ, false }, /* mosq_counter_bytes_received */
	{ 1, 0, "$SYS/broker/bytes/sent", ఖాళీ, false }, /* mosq_counter_bytes_sent */
	{ 1, 0, "$SYS/broker/publish/bytes/received", ఖాళీ, false }, /* mosq_counter_pub_bytes_received */
	{ 1, 0, "$SYS/broker/publish/bytes/sent", ఖాళీ, false }, /* mosq_counter_pub_bytes_sent */
	{ 1, 0, "$SYS/broker/packet/out/count", ఖాళీ, false }, /* mosq_gauge_out_packet_count */
	{ 1, 0, "$SYS/broker/packet/out/bytes", ఖాళీ, false }, /* mosq_gauge_out_packet_bytes */
	{ 1, 0, "$SYS/broker/connections/socket/count", ఖాళీ, false }, /* mosq_counter_socket_connections */
	{ 1, 0, ఖాళీ, ఖాళీ, false }, /* mosq_counter_mqtt_connect_received */
	{ 1, 0, ఖాళీ, ఖాళీ, false }, /* mosq_counter_mqtt_connect_sent */
	{ 1, 0, ఖాళీ, ఖాళీ, false }, /* mosq_counter_mqtt_connack_received */
	{ 1, 0, ఖాళీ, ఖాళీ, false }, /* mosq_counter_mqtt_connack_sent */
	{ 1, 0, "$SYS/broker/publish/messages/dropped", ఖాళీ, false }, /* mosq_counter_mqtt_publish_dropped */
	{ 1, 0, "$SYS/broker/publish/messages/received", ఖాళీ, false }, /* mosq_counter_mqtt_publish_received */
	{ 1, 0, "$SYS/broker/publish/messages/sent", ఖాళీ, false }, /* mosq_counter_mqtt_publish_sent */
	{ 1, 0, ఖాళీ, ఖాళీ, false }, /* mosq_counter_mqtt_puback_received */
	{ 1, 0, ఖాళీ, ఖాళీ, false }, /* mosq_counter_mqtt_puback_sent */
	{ 1, 0, ఖాళీ, ఖాళీ, false }, /* mosq_counter_mqtt_pubrec_received */
	{ 1, 0, ఖాళీ, ఖాళీ, false }, /* mosq_counter_mqtt_pubrec_sent */
	{ 1, 0, ఖాళీ, ఖాళీ, false }, /* mosq_counter_mqtt_pubrel_received */
	{ 1, 0, ఖాళీ, ఖాళీ, false }, /* mosq_counter_mqtt_pubrel_sent */
	{ 1, 0, ఖాళీ, ఖాళీ, false }, /* mosq_counter_mqtt_pubcomp_received */
	{ 1, 0, ఖాళీ, ఖాళీ, false }, /* mosq_counter_mqtt_pubcomp_sent */
	{ 1, 0, ఖాళీ, ఖాళీ, false }, /* mosq_counter_mqtt_subscribe_received */
	{ 1, 0, ఖాళీ, ఖాళీ, false }, /* mosq_counter_mqtt_subscribe_sent */
	{ 1, 0, ఖాళీ, ఖాళీ, false }, /* mosq_counter_mqtt_suback_received */
	{ 1, 0, ఖాళీ, ఖాళీ, false }, /* mosq_counter_mqtt_suback_sent */
	{ 1, 0, ఖాళీ, ఖాళీ, false }, /* mosq_counter_mqtt_unsubscribe_received */
	{ 1, 0, ఖాళీ, ఖాళీ, false }, /* mosq_counter_mqtt_unsubscribe_sent */
	{ 1, 0, ఖాళీ, ఖాళీ, false }, /* mosq_counter_mqtt_unsuback_received */
	{ 1, 0, ఖాళీ, ఖాళీ, false }, /* mosq_counter_mqtt_unsuback_sent */
	{ 1, 0, ఖాళీ, ఖాళీ, false }, /* mosq_counter_mqtt_pingreq_received */
	{ 1, 0, ఖాళీ, ఖాళీ, false }, /* mosq_counter_mqtt_pingreq_sent */
	{ 1, 0, ఖాళీ, ఖాళీ, false }, /* mosq_counter_mqtt_pingresp_received */
	{ 1, 0, ఖాళీ, ఖాళీ, false }, /* mosq_counter_mqtt_pingresp_sent */
	{ 1, 0, ఖాళీ, ఖాళీ, false }, /* mosq_counter_mqtt_disconnect_received */
	{ 1, 0, ఖాళీ, ఖాళీ, false }, /* mosq_counter_mqtt_disconnect_sent */
	{ 1, 0, ఖాళీ, ఖాళీ, false }, /* mosq_counter_mqtt_auth_received */
	{ 1, 0, ఖాళీ, ఖాళీ, false }, /* mosq_counter_mqtt_auth_sent */
};

నిర్మాణం metric_ఎక్కించు metric_loads[mosq_metric_ఎక్కించు_max] = {
	{ 0.0, "$SYS/broker/load/messages/received/1min", mosq_counter_messages_received, METRIC_ఎక్కించు_1_MIN }, /* metric_load_messages_received_1min */
	{ 0.0, "$SYS/broker/load/messages/received/5min", mosq_counter_messages_received, METRIC_ఎక్కించు_5_MIN }, /* metric_load_messages_received_5min */
	{ 0.0, "$SYS/broker/load/messages/received/15min", mosq_counter_messages_received, METRIC_ఎక్కించు_15_MIN }, /* metric_load_messages_received_15min */
	{ 0.0, "$SYS/broker/load/messages/sent/1min", mosq_counter_messages_పంపిన, METRIC_ఎక్కించు_1_MIN }, /* metric_load_messages_sent_1min */
	{ 0.0, "$SYS/broker/load/messages/sent/5min", mosq_counter_messages_పంపిన, METRIC_ఎక్కించు_5_MIN }, /* metric_load_messages_sent_5min */
	{ 0.0, "$SYS/broker/load/messages/sent/15min", mosq_counter_messages_పంపిన, METRIC_ఎక్కించు_15_MIN }, /* metric_load_messages_sent_15min */
	{ 0.0, "$SYS/broker/load/publish/dropped/1min", mosq_counter_mqtt_publish_dropped, METRIC_ఎక్కించు_1_MIN }, /* metric_load_pub_messages_dropped_1min */
	{ 0.0, "$SYS/broker/load/publish/dropped/5min", mosq_counter_mqtt_publish_dropped, METRIC_ఎక్కించు_5_MIN }, /* metric_load_pub_messages_dropped_5min */
	{ 0.0, "$SYS/broker/load/publish/dropped/15min", mosq_counter_mqtt_publish_dropped, METRIC_ఎక్కించు_15_MIN }, /* metric_load_pub_messages_dropped_15min */
	{ 0.0, "$SYS/broker/load/publish/received/1min", mosq_counter_mqtt_publish_received, METRIC_ఎక్కించు_1_MIN }, /* metric_load_pub_messages_received_1min */
	{ 0.0, "$SYS/broker/load/publish/received/5min", mosq_counter_mqtt_publish_received, METRIC_ఎక్కించు_5_MIN }, /* metric_load_pub_messages_received_5min */
	{ 0.0, "$SYS/broker/load/publish/received/15min", mosq_counter_mqtt_publish_received, METRIC_ఎక్కించు_15_MIN }, /* metric_load_pub_messages_received_15min */
	{ 0.0, "$SYS/broker/load/publish/sent/1min", mosq_counter_mqtt_publish_పంపిన, METRIC_ఎక్కించు_1_MIN }, /* metric_load_pub_messages_sent_1min */
	{ 0.0, "$SYS/broker/load/publish/sent/5min", mosq_counter_mqtt_publish_పంపిన, METRIC_ఎక్కించు_5_MIN }, /* metric_load_pub_messages_sent_5min */
	{ 0.0, "$SYS/broker/load/publish/sent/15min", mosq_counter_mqtt_publish_పంపిన, METRIC_ఎక్కించు_15_MIN }, /* metric_load_pub_messages_sent_15min */
	{ 0.0, "$SYS/broker/load/bytes/received/1min", mosq_counter_బైట్లు_received, METRIC_ఎక్కించు_1_MIN }, /* metric_load_bytes_received_1min */
	{ 0.0, "$SYS/broker/load/bytes/received/5min", mosq_counter_బైట్లు_received, METRIC_ఎక్కించు_5_MIN }, /* metric_load_bytes_received_5min */
	{ 0.0, "$SYS/broker/load/bytes/received/15min", mosq_counter_బైట్లు_received, METRIC_ఎక్కించు_15_MIN }, /* metric_load_bytes_received_15min */
	{ 0.0, "$SYS/broker/load/bytes/sent/1min", mosq_counter_బైట్లు_పంపిన, METRIC_ఎక్కించు_1_MIN }, /* metric_load_bytes_sent_1min */
	{ 0.0, "$SYS/broker/load/bytes/sent/5min", mosq_counter_బైట్లు_పంపిన, METRIC_ఎక్కించు_5_MIN }, /* metric_load_bytes_sent_5min */
	{ 0.0, "$SYS/broker/load/bytes/sent/15min", mosq_counter_బైట్లు_పంపిన, METRIC_ఎక్కించు_15_MIN }, /* metric_load_bytes_sent_15min */
	{ 0.0, "$SYS/broker/load/sockets/1min", mosq_counter_socket_connections, METRIC_ఎక్కించు_1_MIN }, /* metric_load_socket_connections_1min */
	{ 0.0, "$SYS/broker/load/sockets/5min", mosq_counter_socket_connections, METRIC_ఎక్కించు_5_MIN }, /* metric_load_socket_connections_5min */
	{ 0.0, "$SYS/broker/load/sockets/15min", mosq_counter_socket_connections, METRIC_ఎక్కించు_15_MIN }, /* metric_load_socket_connections_15min */
	{ 0.0, "$SYS/broker/load/connections/1min", mosq_counter_mqtt_connect_received, METRIC_ఎక్కించు_1_MIN }, /* metric_load_connections_1min */
	{ 0.0, "$SYS/broker/load/connections/5min", mosq_counter_mqtt_connect_received, METRIC_ఎక్కించు_5_MIN }, /* metric_load_connections_5min */
	{ 0.0, "$SYS/broker/load/connections/15min", mosq_counter_mqtt_connect_received, METRIC_ఎక్కించు_15_MIN }, /* metric_load_connections_15min */
};

స్థిర time_t ప్రారంభం_సమయం = 0;
స్థిర time_t చివరి_update = 0;


time_t broker_uptime(శూన్య)
{
	తిరిగి db.now_s - ప్రారంభం_సమయం;
}


స్థిర శూన్య calc_ఎక్కించు(అక్షరం *buf, ద్విగుణ exponent, ద్విగుణ i_mult, నిర్మాణం metric_ఎక్కించు *m, bool force)
{
	ద్విగుణ new_విలువ;
	uint32_t len;
	ద్విగుణ interval;

	interval = (ద్విగుణ)(metrics[m->ఎక్కించు_ref].తదుపరి - metrics[m->ఎక్కించు_ref].current)*i_mult;
	new_విలువ = interval + exponent*(m->current - interval);
	ఒకవేళ(fabs(new_విలువ - (m->current)) >= 0.01 || force){
		len = (uint32_t)snprintf(buf, BUFLEN, "%.2f", new_విలువ);
		db_messages_easy_వరుస(ఖాళీ, m->topic, SYS_వృక్షం_QOS, len, buf, 1, సందేశం_EXPIRY_INFINITE, ఖాళీ);
	}
	m->current = new_విలువ;
}


శూన్య sys_వృక్షం_init(శూన్య)
{
	అక్షరం buf[BUFLEN];
	uint32_t len;

	ఒకవేళ(db.config->sys_interval == 0){
		తిరిగి;
	}

	/* Set static $SYS messages */
	len = (uint32_t)snprintf(buf, 64, "mosquitto version %s", సంస్కరణ);
	db_messages_easy_వరుస(ఖాళీ, "$SYS/broker/version", SYS_వృక్షం_QOS, len, buf, 1, సందేశం_EXPIRY_INFINITE, ఖాళీ);

	ప్రారంభం_సమయం = mosquitto_సమయం();
	చివరి_update = ప్రారంభం_సమయం;

	sys_వృక్షం_update(true);

	/* Force published load values to 0 */
	కొరకు(పూర్ణాంకం i=0; i<mosq_metric_ఎక్కించు_max; i++){
		ఒకవేళ(metric_loads[i].interval == METRIC_ఎక్కించు_1_MIN){
			calc_ఎక్కించు(buf, 0.0, 0.0, &metric_loads[i], true);
		}లేకపోతే ఒకవేళ(metric_loads[i].interval == METRIC_ఎక్కించు_5_MIN){
			calc_ఎక్కించు(buf, 0.0, 0.0, &metric_loads[i], true);
		}లేకపోతే{
			calc_ఎక్కించు(buf, 0.0, 0.0, &metric_loads[i], true);
		}
	}
}


శూన్య metrics__int_inc(గణన mosq_metric_type m, int64_t విలువ)
{
	ఒకవేళ(m < mosq_metric_max){
		metrics[m].తదుపరి += విలువ;
	}
}


శూన్య metrics__int_dec(గణన mosq_metric_type m, int64_t విలువ)
{
	ఒకవేళ(m < mosq_metric_max){
		metrics[m].తదుపరి -= విలువ;
	}
}


/* Send messages for the $SYS hierarchy if the last update is longer than
 * 'interval' seconds ago.
 * 'interval' is the amount of seconds between updates. If 0, then no periodic
 * messages are sent for the $SYS hierarchy.
 * 'start_time' is the result of time() that the broker was started at.
 */
శూన్య sys_వృక్షం_update(bool force)
{
	time_t uptime;
	అక్షరం buf[BUFLEN];
	uint32_t len;
	time_t తదుపరి_event;
	స్థిర time_t చివరి_update_real = 0;

	ఒకవేళ(db.config->sys_interval){
		తదుపరి_event = db.config->sys_interval - db.now_real_s % db.config->sys_interval - 1;
		ఒకవేళ(తదుపరి_event <= 0){
			తదుపరి_event = db.config->sys_interval;
		}
		loop_update_తదుపరి_event(తదుపరి_event*1000);
	}

	ఒకవేళ(db.config->sys_interval
			&& ((db.now_real_s % db.config->sys_interval == 0 && చివరి_update_real != db.now_real_s) || force)){

		uptime = db.now_s - ప్రారంభం_సమయం;
		len = (uint32_t)snprintf(buf, BUFLEN, "%" PRIu64 " seconds", (uint64_t)uptime);
		db_messages_easy_వరుస(ఖాళీ, "$SYS/broker/uptime", SYS_వృక్షం_QOS, len, buf, 1, సందేశం_EXPIRY_INFINITE, ఖాళీ);

		/*  Update metrics values where not otherwise updated */
		metrics[mosq_gauge_సందేశం_store_లెక్క].తదుపరి = db.సందేశం_store_లెక్క;
		metrics[mosq_gauge_సందేశం_store_బైట్లు].తదుపరి = (int64_t)db.సందేశం_store_బైట్లు;
		metrics[mosq_gauge_subscriptions].తదుపరి = db.subscription_లెక్క;
		metrics[mosq_gauge_shared_subscriptions].తదుపరి = db.shared_subscription_లెక్క;
		metrics[mosq_gauge_retained_messages].తదుపరి = db.retained_లెక్క;
#నిర్వచితమైతే WITH_జ్ఞాపకం_TRACKING
		metrics[mosq_gauge_కుప్ప_current].తదుపరి = (int64_t)mosquitto_జ్ఞాపకం_వాడిన();
		metrics[mosq_counter_కుప్ప_గరిష్ఠ].తదుపరి = (int64_t)mosquitto_max_జ్ఞాపకం_వాడిన();
#అయితే_ముగింపు
		metrics[mosq_gauge_clients_మొత్తం].తదుపరి = HASH_CNT(hh_id, db.contexts_by_id);
		metrics[mosq_counter_clients_గరిష్ఠ].తదుపరి = HASH_CNT(hh_id, db.contexts_by_id);
		metrics[mosq_gauge_clients_connected].తదుపరి = HASH_CNT(hh_sock, db.contexts_by_sock);
		metrics[mosq_gauge_clients_disconnected].తదుపరి = HASH_CNT(hh_id, db.contexts_by_id) - HASH_CNT(hh_sock, db.contexts_by_sock);

		/* Handle loads first, because they reference other metrics and need next != current */
		ఒకవేళ(db.now_s > చివరి_update){
			ద్విగుణ i_mult = 60.0/(ద్విగుణ)(db.now_s-చివరి_update);

			ద్విగుణ exponent_1min = ఘాతాంకం(-1.0*(ద్విగుణ)(db.now_s-చివరి_update)/60.0);
			ద్విగుణ exponent_5min = ఘాతాంకం(-1.0*(ద్విగుణ)(db.now_s-చివరి_update)/300.0);
			ద్విగుణ exponent_15min = ఘాతాంకం(-1.0*(ద్విగుణ)(db.now_s-చివరి_update)/900.0);

			కొరకు(పూర్ణాంకం i=0; i<mosq_metric_ఎక్కించు_max; i++){
				ఒకవేళ(metric_loads[i].interval == METRIC_ఎక్కించు_1_MIN){
					calc_ఎక్కించు(buf, exponent_1min, i_mult, &metric_loads[i], false);
				}లేకపోతే ఒకవేళ(metric_loads[i].interval == METRIC_ఎక్కించు_5_MIN){
					calc_ఎక్కించు(buf, exponent_5min, i_mult, &metric_loads[i], false);
				}లేకపోతే{
					calc_ఎక్కించు(buf, exponent_15min, i_mult, &metric_loads[i], false);
				}
			}
		}

		కొరకు(పూర్ణాంకం i=0; i<mosq_metric_max; i++){
			ఒకవేళ((metrics[i].is_max && metrics[i].తదుపరి > metrics[i].current) ||
					(!metrics[i].is_max && metrics[i].తదుపరి != metrics[i].current)){

				metrics[i].current = metrics[i].తదుపరి;
				len = (uint32_t)snprintf(buf, BUFLEN, "%lu", metrics[i].current);
				ఒకవేళ(metrics[i].topic){
					db_messages_easy_వరుస(ఖాళీ, metrics[i].topic, SYS_వృక్షం_QOS, len, buf, 1, సందేశం_EXPIRY_INFINITE, ఖాళీ);
				}
				ఒకవేళ(metrics[i].topic_alias){
					db_messages_easy_వరుస(ఖాళీ, metrics[i].topic_alias, SYS_వృక్షం_QOS, len, buf, 1, సందేశం_EXPIRY_INFINITE, ఖాళీ);
				}
			}
		}

		చివరి_update = db.now_s;
		చివరి_update_real = db.now_real_s;
	}
}

#అయితే_ముగింపు
