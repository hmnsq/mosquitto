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

#अगर_घोषित WITH_SYS_वृक्ष

#समावेश "config.h"

#समावेश <math.h>
#समावेश <stdio.h>
#समावेश <inttypes.h>
#समावेश <limits.h>
#समावेश <inttypes.h>

#समावेश "mosquitto_broker_internal.h"
#समावेश "sys_tree.h"

#घोषणा BUFLEN 100

#घोषणा SYS_वृक्ष_QOS 2

#घोषणा METRIC_LOAD_1MIN 1
#घोषणा METRIC_LOAD_5MIN 2
#घोषणा METRIC_LOAD_15MIN 3

काष्ठा metric {
	int64_t current;
	int64_t अग्र;
	स्थिर अक्षर *topic, *topic_alias;
	bool is_max;
};

काष्ठा metric_load {
	द्विगुन current;
	स्थिर अक्षर *topic;
	पूर्णांक load_ref;
	uint8_t interval;
};

काष्ठा metric metrics[mosq_metric_max] = {
	{ 1, 0, "$SYS/broker/clients/total", शून्य, false }, /* mosq_gauge_clients_total */
	{ 1, 0, "$SYS/broker/clients/maximum", शून्य, true }, /* metric_clients_maximum */
	{ 1, 0, "$SYS/broker/clients/disconnected", "$SYS/broker/clients/inactive", false }, /* mosq_gauge_clients_disconnected */
	{ 1, 0, "$SYS/broker/clients/connected", "$SYS/broker/clients/active", false }, /* mosq_gauge_clients_connected */
	{ 1, 0, "$SYS/broker/clients/expired", शून्य, false }, /* mosq_counter_clients_expired */
	{ 1, 0, "$SYS/broker/messages/stored", "$SYS/broker/store/messages/count", false }, /* mosq_gauge_message_store_count */
	{ 1, 0, "$SYS/broker/store/messages/bytes", शून्य, false }, /* mosq_gauge_message_store_bytes */
	{ 1, 0, "$SYS/broker/subscriptions/count", शून्य, false }, /* mosq_gauge_subscription_count */
	{ 1, 0, "$SYS/broker/shared_subscriptions/count", शून्य, false }, /* mosq_gauge_shared_subscription_count */
	{ 1, 0, "$SYS/broker/retained messages/count", शून्य, false }, /* mosq_gauge_retained_message_count */
#अगर_घोषित WITH_MEMORY_TRACKING
	{ 1, 0, "$SYS/broker/heap/current", शून्य, false }, /* mosq_gauge_heap_current */
	{ 1, 0, "$SYS/broker/heap/maximum", शून्य, true }, /* mosq_gauge_heap_maximum */
#अन्यथा
	{ 1, 0, शून्य, शून्य, 0 }, /* mosq_gauge_heap_current */
	{ 1, 0, शून्य, शून्य, 0 }, /* mosq_gauge_heap_maximum */
#पूर्ण_अगर
	{ 1, 0, "$SYS/broker/messages/received", शून्य, false }, /* mosq_counter_messages_received */
	{ 1, 0, "$SYS/broker/messages/sent", शून्य, false }, /* mosq_counter_messages_sent */
	{ 1, 0, "$SYS/broker/bytes/received", शून्य, false }, /* mosq_counter_bytes_received */
	{ 1, 0, "$SYS/broker/bytes/sent", शून्य, false }, /* mosq_counter_bytes_sent */
	{ 1, 0, "$SYS/broker/publish/bytes/received", शून्य, false }, /* mosq_counter_pub_bytes_received */
	{ 1, 0, "$SYS/broker/publish/bytes/sent", शून्य, false }, /* mosq_counter_pub_bytes_sent */
	{ 1, 0, "$SYS/broker/packet/out/count", शून्य, false }, /* mosq_gauge_out_packet_count */
	{ 1, 0, "$SYS/broker/packet/out/bytes", शून्य, false }, /* mosq_gauge_out_packet_bytes */
	{ 1, 0, "$SYS/broker/connections/socket/count", शून्य, false }, /* mosq_counter_socket_connections */
	{ 1, 0, शून्य, शून्य, false }, /* mosq_counter_mqtt_connect_received */
	{ 1, 0, शून्य, शून्य, false }, /* mosq_counter_mqtt_connect_sent */
	{ 1, 0, शून्य, शून्य, false }, /* mosq_counter_mqtt_connack_received */
	{ 1, 0, शून्य, शून्य, false }, /* mosq_counter_mqtt_connack_sent */
	{ 1, 0, "$SYS/broker/publish/messages/dropped", शून्य, false }, /* mosq_counter_mqtt_publish_dropped */
	{ 1, 0, "$SYS/broker/publish/messages/received", शून्य, false }, /* mosq_counter_mqtt_publish_received */
	{ 1, 0, "$SYS/broker/publish/messages/sent", शून्य, false }, /* mosq_counter_mqtt_publish_sent */
	{ 1, 0, शून्य, शून्य, false }, /* mosq_counter_mqtt_puback_received */
	{ 1, 0, शून्य, शून्य, false }, /* mosq_counter_mqtt_puback_sent */
	{ 1, 0, शून्य, शून्य, false }, /* mosq_counter_mqtt_pubrec_received */
	{ 1, 0, शून्य, शून्य, false }, /* mosq_counter_mqtt_pubrec_sent */
	{ 1, 0, शून्य, शून्य, false }, /* mosq_counter_mqtt_pubrel_received */
	{ 1, 0, शून्य, शून्य, false }, /* mosq_counter_mqtt_pubrel_sent */
	{ 1, 0, शून्य, शून्य, false }, /* mosq_counter_mqtt_pubcomp_received */
	{ 1, 0, शून्य, शून्य, false }, /* mosq_counter_mqtt_pubcomp_sent */
	{ 1, 0, शून्य, शून्य, false }, /* mosq_counter_mqtt_subscribe_received */
	{ 1, 0, शून्य, शून्य, false }, /* mosq_counter_mqtt_subscribe_sent */
	{ 1, 0, शून्य, शून्य, false }, /* mosq_counter_mqtt_suback_received */
	{ 1, 0, शून्य, शून्य, false }, /* mosq_counter_mqtt_suback_sent */
	{ 1, 0, शून्य, शून्य, false }, /* mosq_counter_mqtt_unsubscribe_received */
	{ 1, 0, शून्य, शून्य, false }, /* mosq_counter_mqtt_unsubscribe_sent */
	{ 1, 0, शून्य, शून्य, false }, /* mosq_counter_mqtt_unsuback_received */
	{ 1, 0, शून्य, शून्य, false }, /* mosq_counter_mqtt_unsuback_sent */
	{ 1, 0, शून्य, शून्य, false }, /* mosq_counter_mqtt_pingreq_received */
	{ 1, 0, शून्य, शून्य, false }, /* mosq_counter_mqtt_pingreq_sent */
	{ 1, 0, शून्य, शून्य, false }, /* mosq_counter_mqtt_pingresp_received */
	{ 1, 0, शून्य, शून्य, false }, /* mosq_counter_mqtt_pingresp_sent */
	{ 1, 0, शून्य, शून्य, false }, /* mosq_counter_mqtt_disconnect_received */
	{ 1, 0, शून्य, शून्य, false }, /* mosq_counter_mqtt_disconnect_sent */
	{ 1, 0, शून्य, शून्य, false }, /* mosq_counter_mqtt_auth_received */
	{ 1, 0, शून्य, शून्य, false }, /* mosq_counter_mqtt_auth_sent */
};

काष्ठा metric_load metric_loads[mosq_metric_load_max] = {
	{ 0.0, "$SYS/broker/load/messages/received/1min", mosq_गणक_messages_received, METRIC_LOAD_1MIN }, /* metric_load_messages_received_1min */
	{ 0.0, "$SYS/broker/load/messages/received/5min", mosq_गणक_messages_received, METRIC_LOAD_5MIN }, /* metric_load_messages_received_5min */
	{ 0.0, "$SYS/broker/load/messages/received/15min", mosq_गणक_messages_received, METRIC_LOAD_15MIN }, /* metric_load_messages_received_15min */
	{ 0.0, "$SYS/broker/load/messages/sent/1min", mosq_गणक_messages_sent, METRIC_LOAD_1MIN }, /* metric_load_messages_sent_1min */
	{ 0.0, "$SYS/broker/load/messages/sent/5min", mosq_गणक_messages_sent, METRIC_LOAD_5MIN }, /* metric_load_messages_sent_5min */
	{ 0.0, "$SYS/broker/load/messages/sent/15min", mosq_गणक_messages_sent, METRIC_LOAD_15MIN }, /* metric_load_messages_sent_15min */
	{ 0.0, "$SYS/broker/load/publish/dropped/1min", mosq_गणक_mqtt_publish_dropped, METRIC_LOAD_1MIN }, /* metric_load_pub_messages_dropped_1min */
	{ 0.0, "$SYS/broker/load/publish/dropped/5min", mosq_गणक_mqtt_publish_dropped, METRIC_LOAD_5MIN }, /* metric_load_pub_messages_dropped_5min */
	{ 0.0, "$SYS/broker/load/publish/dropped/15min", mosq_गणक_mqtt_publish_dropped, METRIC_LOAD_15MIN }, /* metric_load_pub_messages_dropped_15min */
	{ 0.0, "$SYS/broker/load/publish/received/1min", mosq_गणक_mqtt_publish_received, METRIC_LOAD_1MIN }, /* metric_load_pub_messages_received_1min */
	{ 0.0, "$SYS/broker/load/publish/received/5min", mosq_गणक_mqtt_publish_received, METRIC_LOAD_5MIN }, /* metric_load_pub_messages_received_5min */
	{ 0.0, "$SYS/broker/load/publish/received/15min", mosq_गणक_mqtt_publish_received, METRIC_LOAD_15MIN }, /* metric_load_pub_messages_received_15min */
	{ 0.0, "$SYS/broker/load/publish/sent/1min", mosq_गणक_mqtt_publish_sent, METRIC_LOAD_1MIN }, /* metric_load_pub_messages_sent_1min */
	{ 0.0, "$SYS/broker/load/publish/sent/5min", mosq_गणक_mqtt_publish_sent, METRIC_LOAD_5MIN }, /* metric_load_pub_messages_sent_5min */
	{ 0.0, "$SYS/broker/load/publish/sent/15min", mosq_गणक_mqtt_publish_sent, METRIC_LOAD_15MIN }, /* metric_load_pub_messages_sent_15min */
	{ 0.0, "$SYS/broker/load/bytes/received/1min", mosq_गणक_bytes_received, METRIC_LOAD_1MIN }, /* metric_load_bytes_received_1min */
	{ 0.0, "$SYS/broker/load/bytes/received/5min", mosq_गणक_bytes_received, METRIC_LOAD_5MIN }, /* metric_load_bytes_received_5min */
	{ 0.0, "$SYS/broker/load/bytes/received/15min", mosq_गणक_bytes_received, METRIC_LOAD_15MIN }, /* metric_load_bytes_received_15min */
	{ 0.0, "$SYS/broker/load/bytes/sent/1min", mosq_गणक_bytes_sent, METRIC_LOAD_1MIN }, /* metric_load_bytes_sent_1min */
	{ 0.0, "$SYS/broker/load/bytes/sent/5min", mosq_गणक_bytes_sent, METRIC_LOAD_5MIN }, /* metric_load_bytes_sent_5min */
	{ 0.0, "$SYS/broker/load/bytes/sent/15min", mosq_गणक_bytes_sent, METRIC_LOAD_15MIN }, /* metric_load_bytes_sent_15min */
	{ 0.0, "$SYS/broker/load/sockets/1min", mosq_गणक_socket_connections, METRIC_LOAD_1MIN }, /* metric_load_socket_connections_1min */
	{ 0.0, "$SYS/broker/load/sockets/5min", mosq_गणक_socket_connections, METRIC_LOAD_5MIN }, /* metric_load_socket_connections_5min */
	{ 0.0, "$SYS/broker/load/sockets/15min", mosq_गणक_socket_connections, METRIC_LOAD_15MIN }, /* metric_load_socket_connections_15min */
	{ 0.0, "$SYS/broker/load/connections/1min", mosq_गणक_mqtt_connect_received, METRIC_LOAD_1MIN }, /* metric_load_connections_1min */
	{ 0.0, "$SYS/broker/load/connections/5min", mosq_गणक_mqtt_connect_received, METRIC_LOAD_5MIN }, /* metric_load_connections_5min */
	{ 0.0, "$SYS/broker/load/connections/15min", mosq_गणक_mqtt_connect_received, METRIC_LOAD_15MIN }, /* metric_load_connections_15min */
};

अटल समय_प्रकार start_time = 0;
अटल समय_प्रकार last_update = 0;


समय_प्रकार broker_uptime(व्योम)
{
	वापस db.now_s - start_time;
}


अटल व्योम calc_load(अक्षर *buf, द्विगुन exponent, द्विगुन i_mult, काष्ठा metric_load *m, bool force)
{
	द्विगुन new_मूल्य;
	uint32_t len;
	द्विगुन interval;

	interval = (द्विगुन)(metrics[m->load_ref].अग्र - metrics[m->load_ref].current)*i_mult;
	new_मूल्य = interval + exponent*(m->current - interval);
	अगर(भ_असल(new_मूल्य - (m->current)) >= 0.01 || force){
		len = (uint32_t)snprintf(buf, BUFLEN, "%.2f", new_मूल्य);
		db__messages_easy_queue(शून्य, m->topic, SYS_वृक्ष_QOS, len, buf, 1, संदेश_EXPIRY_INFINITE, शून्य);
	}
	m->current = new_मूल्य;
}


व्योम sys_वृक्ष_init(व्योम)
{
	अक्षर buf[BUFLEN];
	uint32_t len;

	अगर(db.config->sys_interval == 0){
		वापस;
	}

	/* Set static $SYS messages */
	len = (uint32_t)snprintf(buf, 64, "mosquitto version %s", VERSION);
	db__messages_easy_queue(शून्य, "$SYS/broker/version", SYS_वृक्ष_QOS, len, buf, 1, संदेश_EXPIRY_INFINITE, शून्य);

	start_time = mosquitto_time();
	last_update = start_time;

	sys_वृक्ष_update(true);

	/* Force published load values to 0 */
	क्रम(पूर्णांक i=0; i<mosq_metric_load_max; i++){
		अगर(metric_loads[i].interval == METRIC_LOAD_1MIN){
			calc_load(buf, 0.0, 0.0, &metric_loads[i], true);
		}अन्यथा अगर(metric_loads[i].interval == METRIC_LOAD_5MIN){
			calc_load(buf, 0.0, 0.0, &metric_loads[i], true);
		}अन्यथा{
			calc_load(buf, 0.0, 0.0, &metric_loads[i], true);
		}
	}
}


व्योम metrics__int_inc(क्रमागत mosq_metric_type m, int64_t मूल्य)
{
	अगर(m < mosq_metric_max){
		metrics[m].अग्र += मूल्य;
	}
}


व्योम metrics__int_dec(क्रमागत mosq_metric_type m, int64_t मूल्य)
{
	अगर(m < mosq_metric_max){
		metrics[m].अग्र -= मूल्य;
	}
}


/* Send messages for the $SYS hierarchy if the last update is longer than
 * 'interval' seconds ago.
 * 'interval' is the amount of seconds between updates. If 0, then no periodic
 * messages are sent for the $SYS hierarchy.
 * 'start_time' is the result of time() that the broker was started at.
 */
व्योम sys_वृक्ष_update(bool force)
{
	समय_प्रकार uptime;
	अक्षर buf[BUFLEN];
	uint32_t len;
	समय_प्रकार अग्र_event;
	अटल समय_प्रकार last_update_real = 0;

	अगर(db.config->sys_interval){
		अग्र_event = db.config->sys_interval - db.now_real_s % db.config->sys_interval - 1;
		अगर(अग्र_event <= 0){
			अग्र_event = db.config->sys_interval;
		}
		loop_update_अग्र_event(अग्र_event*1000);
	}

	अगर(db.config->sys_interval
			&& ((db.now_real_s % db.config->sys_interval == 0 && last_update_real != db.now_real_s) || force)){

		uptime = db.now_s - start_time;
		len = (uint32_t)snprintf(buf, BUFLEN, "%" PRIu64 " seconds", (uint64_t)uptime);
		db__messages_easy_queue(शून्य, "$SYS/broker/uptime", SYS_वृक्ष_QOS, len, buf, 1, संदेश_EXPIRY_INFINITE, शून्य);

		/*  Update metrics values where not otherwise updated */
		metrics[mosq_gauge_message_store_गिनती].अग्र = db.संदेश_store_गिनती;
		metrics[mosq_gauge_message_store_bytes].अग्र = (int64_t)db.संदेश_store_bytes;
		metrics[mosq_gauge_subscriptions].अग्र = db.subscription_गिनती;
		metrics[mosq_gauge_shared_subscriptions].अग्र = db.shared_subscription_गिनती;
		metrics[mosq_gauge_retained_messages].अग्र = db.retained_गिनती;
#अगर_घोषित WITH_MEMORY_TRACKING
		metrics[mosq_gauge_heap_current].अग्र = (int64_t)mosquitto_memory_used();
		metrics[mosq_गणक_heap_maximum].अग्र = (int64_t)mosquitto_max_memory_used();
#पूर्ण_अगर
		metrics[mosq_gauge_clients_योग].अग्र = HASH_CNT(hh_id, db.contexts_by_id);
		metrics[mosq_गणक_clients_maximum].अग्र = HASH_CNT(hh_id, db.contexts_by_id);
		metrics[mosq_gauge_clients_connected].अग्र = HASH_CNT(hh_sock, db.contexts_by_sock);
		metrics[mosq_gauge_clients_disconnected].अग्र = HASH_CNT(hh_id, db.contexts_by_id) - HASH_CNT(hh_sock, db.contexts_by_sock);

		/* Handle loads first, because they reference other metrics and need next != current */
		अगर(db.now_s > last_update){
			द्विगुन i_mult = 60.0/(द्विगुन)(db.now_s-last_update);

			द्विगुन exponent_1min = exp(-1.0*(द्विगुन)(db.now_s-last_update)/60.0);
			द्विगुन exponent_5min = exp(-1.0*(द्विगुन)(db.now_s-last_update)/300.0);
			द्विगुन exponent_15min = exp(-1.0*(द्विगुन)(db.now_s-last_update)/900.0);

			क्रम(पूर्णांक i=0; i<mosq_metric_load_max; i++){
				अगर(metric_loads[i].interval == METRIC_LOAD_1MIN){
					calc_load(buf, exponent_1min, i_mult, &metric_loads[i], false);
				}अन्यथा अगर(metric_loads[i].interval == METRIC_LOAD_5MIN){
					calc_load(buf, exponent_5min, i_mult, &metric_loads[i], false);
				}अन्यथा{
					calc_load(buf, exponent_15min, i_mult, &metric_loads[i], false);
				}
			}
		}

		क्रम(पूर्णांक i=0; i<mosq_metric_max; i++){
			अगर((metrics[i].is_max && metrics[i].अग्र > metrics[i].current) ||
					(!metrics[i].is_max && metrics[i].अग्र != metrics[i].current)){

				metrics[i].current = metrics[i].अग्र;
				len = (uint32_t)snprintf(buf, BUFLEN, "%lu", metrics[i].current);
				अगर(metrics[i].topic){
					db__messages_easy_queue(शून्य, metrics[i].topic, SYS_वृक्ष_QOS, len, buf, 1, संदेश_EXPIRY_INFINITE, शून्य);
				}
				अगर(metrics[i].topic_alias){
					db__messages_easy_queue(शून्य, metrics[i].topic_alias, SYS_वृक्ष_QOS, len, buf, 1, संदेश_EXPIRY_INFINITE, शून्य);
				}
			}
		}

		last_update = db.now_s;
		last_update_real = db.now_real_s;
	}
}

#पूर्ण_अगर
