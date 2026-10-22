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

#यदि_परिभाषितम् WITH_SYS_वृक्षः

#अन्तर्भाव "config.h"

#अन्तर्भाव <math.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <inttypes.h>
#अन्तर्भाव <limits.h>
#अन्तर्भाव <inttypes.h>

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "sys_tree.h"

#परिभाषय BUFLEN 100

#परिभाषय SYS_वृक्षः_QOS 2

#परिभाषय METRIC_आरोपय_1_MIN 1
#परिभाषय METRIC_आरोपय_5_MIN 2
#परिभाषय METRIC_आरोपय_15_MIN 3

संरचना metric {
	int64_t current;
	int64_t अग्र;
	ध्रुव वर्ण *topic, *topic_alias;
	bool is_max;
};

संरचना metric_आरोपय {
	द्विगुण current;
	ध्रुव वर्ण *topic;
	पूर्णाङ्क आरोपय_ref;
	uint8_t interval;
};

संरचना metric metrics[mosq_metric_max] = {
	{ 1, 0, "$SYS/broker/clients/total", रिक्त, false }, /* mosq_gauge_clients_total */
	{ 1, 0, "$SYS/broker/clients/maximum", रिक्त, true }, /* metric_clients_maximum */
	{ 1, 0, "$SYS/broker/clients/disconnected", "$SYS/broker/clients/inactive", false }, /* mosq_gauge_clients_disconnected */
	{ 1, 0, "$SYS/broker/clients/connected", "$SYS/broker/clients/active", false }, /* mosq_gauge_clients_connected */
	{ 1, 0, "$SYS/broker/clients/expired", रिक्त, false }, /* mosq_counter_clients_expired */
	{ 1, 0, "$SYS/broker/messages/stored", "$SYS/broker/store/messages/count", false }, /* mosq_gauge_message_store_count */
	{ 1, 0, "$SYS/broker/store/messages/bytes", रिक्त, false }, /* mosq_gauge_message_store_bytes */
	{ 1, 0, "$SYS/broker/subscriptions/count", रिक्त, false }, /* mosq_gauge_subscription_count */
	{ 1, 0, "$SYS/broker/shared_subscriptions/count", रिक्त, false }, /* mosq_gauge_shared_subscription_count */
	{ 1, 0, "$SYS/broker/retained messages/count", रिक्त, false }, /* mosq_gauge_retained_message_count */
#यदि_परिभाषितम् WITH_स्मृतिः_TRACKING
	{ 1, 0, "$SYS/broker/heap/current", रिक्त, false }, /* mosq_gauge_heap_current */
	{ 1, 0, "$SYS/broker/heap/maximum", रिक्त, true }, /* mosq_gauge_heap_maximum */
#अन्यथा
	{ 1, 0, रिक्त, रिक्त, 0 }, /* mosq_gauge_heap_current */
	{ 1, 0, रिक्त, रिक्त, 0 }, /* mosq_gauge_heap_maximum */
#यदि_अन्तः
	{ 1, 0, "$SYS/broker/messages/received", रिक्त, false }, /* mosq_counter_messages_received */
	{ 1, 0, "$SYS/broker/messages/sent", रिक्त, false }, /* mosq_counter_messages_sent */
	{ 1, 0, "$SYS/broker/bytes/received", रिक्त, false }, /* mosq_counter_bytes_received */
	{ 1, 0, "$SYS/broker/bytes/sent", रिक्त, false }, /* mosq_counter_bytes_sent */
	{ 1, 0, "$SYS/broker/publish/bytes/received", रिक्त, false }, /* mosq_counter_pub_bytes_received */
	{ 1, 0, "$SYS/broker/publish/bytes/sent", रिक्त, false }, /* mosq_counter_pub_bytes_sent */
	{ 1, 0, "$SYS/broker/packet/out/count", रिक्त, false }, /* mosq_gauge_out_packet_count */
	{ 1, 0, "$SYS/broker/packet/out/bytes", रिक्त, false }, /* mosq_gauge_out_packet_bytes */
	{ 1, 0, "$SYS/broker/connections/socket/count", रिक्त, false }, /* mosq_counter_socket_connections */
	{ 1, 0, रिक्त, रिक्त, false }, /* mosq_counter_mqtt_connect_received */
	{ 1, 0, रिक्त, रिक्त, false }, /* mosq_counter_mqtt_connect_sent */
	{ 1, 0, रिक्त, रिक्त, false }, /* mosq_counter_mqtt_connack_received */
	{ 1, 0, रिक्त, रिक्त, false }, /* mosq_counter_mqtt_connack_sent */
	{ 1, 0, "$SYS/broker/publish/messages/dropped", रिक्त, false }, /* mosq_counter_mqtt_publish_dropped */
	{ 1, 0, "$SYS/broker/publish/messages/received", रिक्त, false }, /* mosq_counter_mqtt_publish_received */
	{ 1, 0, "$SYS/broker/publish/messages/sent", रिक्त, false }, /* mosq_counter_mqtt_publish_sent */
	{ 1, 0, रिक्त, रिक्त, false }, /* mosq_counter_mqtt_puback_received */
	{ 1, 0, रिक्त, रिक्त, false }, /* mosq_counter_mqtt_puback_sent */
	{ 1, 0, रिक्त, रिक्त, false }, /* mosq_counter_mqtt_pubrec_received */
	{ 1, 0, रिक्त, रिक्त, false }, /* mosq_counter_mqtt_pubrec_sent */
	{ 1, 0, रिक्त, रिक्त, false }, /* mosq_counter_mqtt_pubrel_received */
	{ 1, 0, रिक्त, रिक्त, false }, /* mosq_counter_mqtt_pubrel_sent */
	{ 1, 0, रिक्त, रिक्त, false }, /* mosq_counter_mqtt_pubcomp_received */
	{ 1, 0, रिक्त, रिक्त, false }, /* mosq_counter_mqtt_pubcomp_sent */
	{ 1, 0, रिक्त, रिक्त, false }, /* mosq_counter_mqtt_subscribe_received */
	{ 1, 0, रिक्त, रिक्त, false }, /* mosq_counter_mqtt_subscribe_sent */
	{ 1, 0, रिक्त, रिक्त, false }, /* mosq_counter_mqtt_suback_received */
	{ 1, 0, रिक्त, रिक्त, false }, /* mosq_counter_mqtt_suback_sent */
	{ 1, 0, रिक्त, रिक्त, false }, /* mosq_counter_mqtt_unsubscribe_received */
	{ 1, 0, रिक्त, रिक्त, false }, /* mosq_counter_mqtt_unsubscribe_sent */
	{ 1, 0, रिक्त, रिक्त, false }, /* mosq_counter_mqtt_unsuback_received */
	{ 1, 0, रिक्त, रिक्त, false }, /* mosq_counter_mqtt_unsuback_sent */
	{ 1, 0, रिक्त, रिक्त, false }, /* mosq_counter_mqtt_pingreq_received */
	{ 1, 0, रिक्त, रिक्त, false }, /* mosq_counter_mqtt_pingreq_sent */
	{ 1, 0, रिक्त, रिक्त, false }, /* mosq_counter_mqtt_pingresp_received */
	{ 1, 0, रिक्त, रिक्त, false }, /* mosq_counter_mqtt_pingresp_sent */
	{ 1, 0, रिक्त, रिक्त, false }, /* mosq_counter_mqtt_disconnect_received */
	{ 1, 0, रिक्त, रिक्त, false }, /* mosq_counter_mqtt_disconnect_sent */
	{ 1, 0, रिक्त, रिक्त, false }, /* mosq_counter_mqtt_auth_received */
	{ 1, 0, रिक्त, रिक्त, false }, /* mosq_counter_mqtt_auth_sent */
};

संरचना metric_आरोपय metric_loads[mosq_metric_आरोपय_max] = {
	{ 0.0, "$SYS/broker/load/messages/received/1min", mosq_गणक_messages_received, METRIC_आरोपय_1_MIN }, /* metric_load_messages_received_1min */
	{ 0.0, "$SYS/broker/load/messages/received/5min", mosq_गणक_messages_received, METRIC_आरोपय_5_MIN }, /* metric_load_messages_received_5min */
	{ 0.0, "$SYS/broker/load/messages/received/15min", mosq_गणक_messages_received, METRIC_आरोपय_15_MIN }, /* metric_load_messages_received_15min */
	{ 0.0, "$SYS/broker/load/messages/sent/1min", mosq_गणक_messages_प्रेषितम्, METRIC_आरोपय_1_MIN }, /* metric_load_messages_sent_1min */
	{ 0.0, "$SYS/broker/load/messages/sent/5min", mosq_गणक_messages_प्रेषितम्, METRIC_आरोपय_5_MIN }, /* metric_load_messages_sent_5min */
	{ 0.0, "$SYS/broker/load/messages/sent/15min", mosq_गणक_messages_प्रेषितम्, METRIC_आरोपय_15_MIN }, /* metric_load_messages_sent_15min */
	{ 0.0, "$SYS/broker/load/publish/dropped/1min", mosq_गणक_mqtt_publish_dropped, METRIC_आरोपय_1_MIN }, /* metric_load_pub_messages_dropped_1min */
	{ 0.0, "$SYS/broker/load/publish/dropped/5min", mosq_गणक_mqtt_publish_dropped, METRIC_आरोपय_5_MIN }, /* metric_load_pub_messages_dropped_5min */
	{ 0.0, "$SYS/broker/load/publish/dropped/15min", mosq_गणक_mqtt_publish_dropped, METRIC_आरोपय_15_MIN }, /* metric_load_pub_messages_dropped_15min */
	{ 0.0, "$SYS/broker/load/publish/received/1min", mosq_गणक_mqtt_publish_received, METRIC_आरोपय_1_MIN }, /* metric_load_pub_messages_received_1min */
	{ 0.0, "$SYS/broker/load/publish/received/5min", mosq_गणक_mqtt_publish_received, METRIC_आरोपय_5_MIN }, /* metric_load_pub_messages_received_5min */
	{ 0.0, "$SYS/broker/load/publish/received/15min", mosq_गणक_mqtt_publish_received, METRIC_आरोपय_15_MIN }, /* metric_load_pub_messages_received_15min */
	{ 0.0, "$SYS/broker/load/publish/sent/1min", mosq_गणक_mqtt_publish_प्रेषितम्, METRIC_आरोपय_1_MIN }, /* metric_load_pub_messages_sent_1min */
	{ 0.0, "$SYS/broker/load/publish/sent/5min", mosq_गणक_mqtt_publish_प्रेषितम्, METRIC_आरोपय_5_MIN }, /* metric_load_pub_messages_sent_5min */
	{ 0.0, "$SYS/broker/load/publish/sent/15min", mosq_गणक_mqtt_publish_प्रेषितम्, METRIC_आरोपय_15_MIN }, /* metric_load_pub_messages_sent_15min */
	{ 0.0, "$SYS/broker/load/bytes/received/1min", mosq_गणक_अष्टकानि_received, METRIC_आरोपय_1_MIN }, /* metric_load_bytes_received_1min */
	{ 0.0, "$SYS/broker/load/bytes/received/5min", mosq_गणक_अष्टकानि_received, METRIC_आरोपय_5_MIN }, /* metric_load_bytes_received_5min */
	{ 0.0, "$SYS/broker/load/bytes/received/15min", mosq_गणक_अष्टकानि_received, METRIC_आरोपय_15_MIN }, /* metric_load_bytes_received_15min */
	{ 0.0, "$SYS/broker/load/bytes/sent/1min", mosq_गणक_अष्टकानि_प्रेषितम्, METRIC_आरोपय_1_MIN }, /* metric_load_bytes_sent_1min */
	{ 0.0, "$SYS/broker/load/bytes/sent/5min", mosq_गणक_अष्टकानि_प्रेषितम्, METRIC_आरोपय_5_MIN }, /* metric_load_bytes_sent_5min */
	{ 0.0, "$SYS/broker/load/bytes/sent/15min", mosq_गणक_अष्टकानि_प्रेषितम्, METRIC_आरोपय_15_MIN }, /* metric_load_bytes_sent_15min */
	{ 0.0, "$SYS/broker/load/sockets/1min", mosq_गणक_socket_connections, METRIC_आरोपय_1_MIN }, /* metric_load_socket_connections_1min */
	{ 0.0, "$SYS/broker/load/sockets/5min", mosq_गणक_socket_connections, METRIC_आरोपय_5_MIN }, /* metric_load_socket_connections_5min */
	{ 0.0, "$SYS/broker/load/sockets/15min", mosq_गणक_socket_connections, METRIC_आरोपय_15_MIN }, /* metric_load_socket_connections_15min */
	{ 0.0, "$SYS/broker/load/connections/1min", mosq_गणक_mqtt_connect_received, METRIC_आरोपय_1_MIN }, /* metric_load_connections_1min */
	{ 0.0, "$SYS/broker/load/connections/5min", mosq_गणक_mqtt_connect_received, METRIC_आरोपय_5_MIN }, /* metric_load_connections_5min */
	{ 0.0, "$SYS/broker/load/connections/15min", mosq_गणक_mqtt_connect_received, METRIC_आरोपय_15_MIN }, /* metric_load_connections_15min */
};

स्थिर time_t start_time = 0;
स्थिर time_t चरमः_update = 0;


time_t broker_uptime(शून्य)
{
	प्रत्यागम db.now_s - start_time;
}


स्थिर शून्य calc_आरोपय(वर्ण *buf, द्विगुण exponent, द्विगुण i_mult, संरचना metric_आरोपय *m, bool force)
{
	द्विगुण new_मानम्;
	uint32_t len;
	द्विगुण interval;

	interval = (द्विगुण)(metrics[m->आरोपय_ref].अग्र - metrics[m->आरोपय_ref].current)*i_mult;
	new_मानम् = interval + exponent*(m->current - interval);
	यदि(fabs(new_मानम् - (m->current)) >= 0.01 || force){
		len = (uint32_t)snprintf(buf, BUFLEN, "%.2f", new_मानम्);
		db__messages_easy_queue(रिक्त, m->topic, SYS_वृक्षः_QOS, len, buf, 1, सन्देशः_EXPIRY_INFINITE, रिक्त);
	}
	m->current = new_मानम्;
}


शून्य sys_वृक्षः_init(शून्य)
{
	वर्ण buf[BUFLEN];
	uint32_t len;

	यदि(db.config->sys_interval == 0){
		प्रत्यागम;
	}

	/* Set static $SYS messages */
	len = (uint32_t)snprintf(buf, 64, "mosquitto version %s", संस्करणम्);
	db__messages_easy_queue(रिक्त, "$SYS/broker/version", SYS_वृक्षः_QOS, len, buf, 1, सन्देशः_EXPIRY_INFINITE, रिक्त);

	start_time = mosquitto_time();
	चरमः_update = start_time;

	sys_वृक्षः_update(true);

	/* Force published load values to 0 */
	प्रति(पूर्णाङ्क i=0; i<mosq_metric_आरोपय_max; i++){
		यदि(metric_loads[i].interval == METRIC_आरोपय_1_MIN){
			calc_आरोपय(buf, 0.0, 0.0, &metric_loads[i], true);
		}अन्यथा यदि(metric_loads[i].interval == METRIC_आरोपय_5_MIN){
			calc_आरोपय(buf, 0.0, 0.0, &metric_loads[i], true);
		}अन्यथा{
			calc_आरोपय(buf, 0.0, 0.0, &metric_loads[i], true);
		}
	}
}


शून्य metrics__int_inc(गणना mosq_metric_type m, int64_t मानम्)
{
	यदि(m < mosq_metric_max){
		metrics[m].अग्र += मानम्;
	}
}


शून्य metrics__int_dec(गणना mosq_metric_type m, int64_t मानम्)
{
	यदि(m < mosq_metric_max){
		metrics[m].अग्र -= मानम्;
	}
}


/* Send messages for the $SYS hierarchy if the last update is longer than
 * 'interval' seconds ago.
 * 'interval' is the amount of seconds between updates. If 0, then no periodic
 * messages are sent for the $SYS hierarchy.
 * 'start_time' is the result of time() that the broker was started at.
 */
शून्य sys_वृक्षः_update(bool force)
{
	time_t uptime;
	वर्ण buf[BUFLEN];
	uint32_t len;
	time_t अग्र_event;
	स्थिर time_t चरमः_update_real = 0;

	यदि(db.config->sys_interval){
		अग्र_event = db.config->sys_interval - db.now_real_s % db.config->sys_interval - 1;
		यदि(अग्र_event <= 0){
			अग्र_event = db.config->sys_interval;
		}
		loop_update_अग्र_event(अग्र_event*1000);
	}

	यदि(db.config->sys_interval
			&& ((db.now_real_s % db.config->sys_interval == 0 && चरमः_update_real != db.now_real_s) || force)){

		uptime = db.now_s - start_time;
		len = (uint32_t)snprintf(buf, BUFLEN, "%" PRIu64 " seconds", (uint64_t)uptime);
		db__messages_easy_queue(रिक्त, "$SYS/broker/uptime", SYS_वृक्षः_QOS, len, buf, 1, सन्देशः_EXPIRY_INFINITE, रिक्त);

		/*  Update metrics values where not otherwise updated */
		metrics[mosq_gauge_सन्देशः_store_गणनम्].अग्र = db.सन्देशः_store_गणनम्;
		metrics[mosq_gauge_सन्देशः_store_अष्टकानि].अग्र = (int64_t)db.सन्देशः_store_अष्टकानि;
		metrics[mosq_gauge_subscriptions].अग्र = db.subscription_गणनम्;
		metrics[mosq_gauge_shared_subscriptions].अग्र = db.shared_subscription_गणनम्;
		metrics[mosq_gauge_retained_messages].अग्र = db.retained_गणनम्;
#यदि_परिभाषितम् WITH_स्मृतिः_TRACKING
		metrics[mosq_gauge_heap_current].अग्र = (int64_t)mosquitto_स्मृतिः_प्रयुक्तम्();
		metrics[mosq_गणक_heap_अधिकतमम्].अग्र = (int64_t)mosquitto_max_स्मृतिः_प्रयुक्तम्();
#यदि_अन्तः
		metrics[mosq_gauge_clients_योग].अग्र = HASH_CNT(hh_id, db.contexts_by_id);
		metrics[mosq_गणक_clients_अधिकतमम्].अग्र = HASH_CNT(hh_id, db.contexts_by_id);
		metrics[mosq_gauge_clients_connected].अग्र = HASH_CNT(hh_sock, db.contexts_by_sock);
		metrics[mosq_gauge_clients_disconnected].अग्र = HASH_CNT(hh_id, db.contexts_by_id) - HASH_CNT(hh_sock, db.contexts_by_sock);

		/* Handle loads first, because they reference other metrics and need next != current */
		यदि(db.now_s > चरमः_update){
			द्विगुण i_mult = 60.0/(द्विगुण)(db.now_s-चरमः_update);

			द्विगुण exponent_1min = घातांकः(-1.0*(द्विगुण)(db.now_s-चरमः_update)/60.0);
			द्विगुण exponent_5min = घातांकः(-1.0*(द्विगुण)(db.now_s-चरमः_update)/300.0);
			द्विगुण exponent_15min = घातांकः(-1.0*(द्विगुण)(db.now_s-चरमः_update)/900.0);

			प्रति(पूर्णाङ्क i=0; i<mosq_metric_आरोपय_max; i++){
				यदि(metric_loads[i].interval == METRIC_आरोपय_1_MIN){
					calc_आरोपय(buf, exponent_1min, i_mult, &metric_loads[i], false);
				}अन्यथा यदि(metric_loads[i].interval == METRIC_आरोपय_5_MIN){
					calc_आरोपय(buf, exponent_5min, i_mult, &metric_loads[i], false);
				}अन्यथा{
					calc_आरोपय(buf, exponent_15min, i_mult, &metric_loads[i], false);
				}
			}
		}

		प्रति(पूर्णाङ्क i=0; i<mosq_metric_max; i++){
			यदि((metrics[i].is_max && metrics[i].अग्र > metrics[i].current) ||
					(!metrics[i].is_max && metrics[i].अग्र != metrics[i].current)){

				metrics[i].current = metrics[i].अग्र;
				len = (uint32_t)snprintf(buf, BUFLEN, "%lu", metrics[i].current);
				यदि(metrics[i].topic){
					db__messages_easy_queue(रिक्त, metrics[i].topic, SYS_वृक्षः_QOS, len, buf, 1, सन्देशः_EXPIRY_INFINITE, रिक्त);
				}
				यदि(metrics[i].topic_alias){
					db__messages_easy_queue(रिक्त, metrics[i].topic_alias, SYS_वृक्षः_QOS, len, buf, 1, सन्देशः_EXPIRY_INFINITE, रिक्त);
				}
			}
		}

		चरमः_update = db.now_s;
		चरमः_update_real = db.now_real_s;
	}
}

#यदि_अन्तः
