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

#اگر_تعریف_شدہ WITH_SYS_درخت

#شامل "config.h"

#شامل <math.h>
#شامل <stdio.h>
#شامل <inttypes.h>
#شامل <limits.h>
#شامل <inttypes.h>

#شامل "mosquitto_broker_internal.h"
#شامل "sys_tree.h"

#تعریف_کرو BUFLEN 100

#تعریف_کرو SYS_درخت_QOS 2

#تعریف_کرو METRIC_بھرو_1_MIN 1
#تعریف_کرو METRIC_بھرو_5_MIN 2
#تعریف_کرو METRIC_بھرو_15_MIN 3

ساخت metric {
	int64_t current;
	int64_t اگلا;
	ثابت حرف *topic, *topic_alias;
	bool is_max;
};

ساخت metric_بھرو {
	دوہرا current;
	ثابت حرف *topic;
	صحیح بھرو_ref;
	uint8_t interval;
};

ساخت metric metrics[mosq_metric_max] = {
	{ 1, 0, "$SYS/broker/clients/total", تہی, false }, /* mosq_gauge_clients_total */
	{ 1, 0, "$SYS/broker/clients/maximum", تہی, true }, /* metric_clients_maximum */
	{ 1, 0, "$SYS/broker/clients/disconnected", "$SYS/broker/clients/inactive", false }, /* mosq_gauge_clients_disconnected */
	{ 1, 0, "$SYS/broker/clients/connected", "$SYS/broker/clients/active", false }, /* mosq_gauge_clients_connected */
	{ 1, 0, "$SYS/broker/clients/expired", تہی, false }, /* mosq_counter_clients_expired */
	{ 1, 0, "$SYS/broker/messages/stored", "$SYS/broker/store/messages/count", false }, /* mosq_gauge_message_store_count */
	{ 1, 0, "$SYS/broker/store/messages/bytes", تہی, false }, /* mosq_gauge_message_store_bytes */
	{ 1, 0, "$SYS/broker/subscriptions/count", تہی, false }, /* mosq_gauge_subscription_count */
	{ 1, 0, "$SYS/broker/shared_subscriptions/count", تہی, false }, /* mosq_gauge_shared_subscription_count */
	{ 1, 0, "$SYS/broker/retained messages/count", تہی, false }, /* mosq_gauge_retained_message_count */
#اگر_تعریف_شدہ WITH_حافظہ_TRACKING
	{ 1, 0, "$SYS/broker/heap/current", تہی, false }, /* mosq_gauge_heap_current */
	{ 1, 0, "$SYS/broker/heap/maximum", تہی, true }, /* mosq_gauge_heap_maximum */
#ورنہ
	{ 1, 0, تہی, تہی, 0 }, /* mosq_gauge_heap_current */
	{ 1, 0, تہی, تہی, 0 }, /* mosq_gauge_heap_maximum */
#اگر_ختم
	{ 1, 0, "$SYS/broker/messages/received", تہی, false }, /* mosq_counter_messages_received */
	{ 1, 0, "$SYS/broker/messages/sent", تہی, false }, /* mosq_counter_messages_sent */
	{ 1, 0, "$SYS/broker/bytes/received", تہی, false }, /* mosq_counter_bytes_received */
	{ 1, 0, "$SYS/broker/bytes/sent", تہی, false }, /* mosq_counter_bytes_sent */
	{ 1, 0, "$SYS/broker/publish/bytes/received", تہی, false }, /* mosq_counter_pub_bytes_received */
	{ 1, 0, "$SYS/broker/publish/bytes/sent", تہی, false }, /* mosq_counter_pub_bytes_sent */
	{ 1, 0, "$SYS/broker/packet/out/count", تہی, false }, /* mosq_gauge_out_packet_count */
	{ 1, 0, "$SYS/broker/packet/out/bytes", تہی, false }, /* mosq_gauge_out_packet_bytes */
	{ 1, 0, "$SYS/broker/connections/socket/count", تہی, false }, /* mosq_counter_socket_connections */
	{ 1, 0, تہی, تہی, false }, /* mosq_counter_mqtt_connect_received */
	{ 1, 0, تہی, تہی, false }, /* mosq_counter_mqtt_connect_sent */
	{ 1, 0, تہی, تہی, false }, /* mosq_counter_mqtt_connack_received */
	{ 1, 0, تہی, تہی, false }, /* mosq_counter_mqtt_connack_sent */
	{ 1, 0, "$SYS/broker/publish/messages/dropped", تہی, false }, /* mosq_counter_mqtt_publish_dropped */
	{ 1, 0, "$SYS/broker/publish/messages/received", تہی, false }, /* mosq_counter_mqtt_publish_received */
	{ 1, 0, "$SYS/broker/publish/messages/sent", تہی, false }, /* mosq_counter_mqtt_publish_sent */
	{ 1, 0, تہی, تہی, false }, /* mosq_counter_mqtt_puback_received */
	{ 1, 0, تہی, تہی, false }, /* mosq_counter_mqtt_puback_sent */
	{ 1, 0, تہی, تہی, false }, /* mosq_counter_mqtt_pubrec_received */
	{ 1, 0, تہی, تہی, false }, /* mosq_counter_mqtt_pubrec_sent */
	{ 1, 0, تہی, تہی, false }, /* mosq_counter_mqtt_pubrel_received */
	{ 1, 0, تہی, تہی, false }, /* mosq_counter_mqtt_pubrel_sent */
	{ 1, 0, تہی, تہی, false }, /* mosq_counter_mqtt_pubcomp_received */
	{ 1, 0, تہی, تہی, false }, /* mosq_counter_mqtt_pubcomp_sent */
	{ 1, 0, تہی, تہی, false }, /* mosq_counter_mqtt_subscribe_received */
	{ 1, 0, تہی, تہی, false }, /* mosq_counter_mqtt_subscribe_sent */
	{ 1, 0, تہی, تہی, false }, /* mosq_counter_mqtt_suback_received */
	{ 1, 0, تہی, تہی, false }, /* mosq_counter_mqtt_suback_sent */
	{ 1, 0, تہی, تہی, false }, /* mosq_counter_mqtt_unsubscribe_received */
	{ 1, 0, تہی, تہی, false }, /* mosq_counter_mqtt_unsubscribe_sent */
	{ 1, 0, تہی, تہی, false }, /* mosq_counter_mqtt_unsuback_received */
	{ 1, 0, تہی, تہی, false }, /* mosq_counter_mqtt_unsuback_sent */
	{ 1, 0, تہی, تہی, false }, /* mosq_counter_mqtt_pingreq_received */
	{ 1, 0, تہی, تہی, false }, /* mosq_counter_mqtt_pingreq_sent */
	{ 1, 0, تہی, تہی, false }, /* mosq_counter_mqtt_pingresp_received */
	{ 1, 0, تہی, تہی, false }, /* mosq_counter_mqtt_pingresp_sent */
	{ 1, 0, تہی, تہی, false }, /* mosq_counter_mqtt_disconnect_received */
	{ 1, 0, تہی, تہی, false }, /* mosq_counter_mqtt_disconnect_sent */
	{ 1, 0, تہی, تہی, false }, /* mosq_counter_mqtt_auth_received */
	{ 1, 0, تہی, تہی, false }, /* mosq_counter_mqtt_auth_sent */
};

ساخت metric_بھرو metric_loads[mosq_metric_بھرو_max] = {
	{ 0.0, "$SYS/broker/load/messages/received/1min", mosq_شمار_messages_received, METRIC_بھرو_1_MIN }, /* metric_load_messages_received_1min */
	{ 0.0, "$SYS/broker/load/messages/received/5min", mosq_شمار_messages_received, METRIC_بھرو_5_MIN }, /* metric_load_messages_received_5min */
	{ 0.0, "$SYS/broker/load/messages/received/15min", mosq_شمار_messages_received, METRIC_بھرو_15_MIN }, /* metric_load_messages_received_15min */
	{ 0.0, "$SYS/broker/load/messages/sent/1min", mosq_شمار_messages_بھیجا, METRIC_بھرو_1_MIN }, /* metric_load_messages_sent_1min */
	{ 0.0, "$SYS/broker/load/messages/sent/5min", mosq_شمار_messages_بھیجا, METRIC_بھرو_5_MIN }, /* metric_load_messages_sent_5min */
	{ 0.0, "$SYS/broker/load/messages/sent/15min", mosq_شمار_messages_بھیجا, METRIC_بھرو_15_MIN }, /* metric_load_messages_sent_15min */
	{ 0.0, "$SYS/broker/load/publish/dropped/1min", mosq_شمار_mqtt_publish_dropped, METRIC_بھرو_1_MIN }, /* metric_load_pub_messages_dropped_1min */
	{ 0.0, "$SYS/broker/load/publish/dropped/5min", mosq_شمار_mqtt_publish_dropped, METRIC_بھرو_5_MIN }, /* metric_load_pub_messages_dropped_5min */
	{ 0.0, "$SYS/broker/load/publish/dropped/15min", mosq_شمار_mqtt_publish_dropped, METRIC_بھرو_15_MIN }, /* metric_load_pub_messages_dropped_15min */
	{ 0.0, "$SYS/broker/load/publish/received/1min", mosq_شمار_mqtt_publish_received, METRIC_بھرو_1_MIN }, /* metric_load_pub_messages_received_1min */
	{ 0.0, "$SYS/broker/load/publish/received/5min", mosq_شمار_mqtt_publish_received, METRIC_بھرو_5_MIN }, /* metric_load_pub_messages_received_5min */
	{ 0.0, "$SYS/broker/load/publish/received/15min", mosq_شمار_mqtt_publish_received, METRIC_بھرو_15_MIN }, /* metric_load_pub_messages_received_15min */
	{ 0.0, "$SYS/broker/load/publish/sent/1min", mosq_شمار_mqtt_publish_بھیجا, METRIC_بھرو_1_MIN }, /* metric_load_pub_messages_sent_1min */
	{ 0.0, "$SYS/broker/load/publish/sent/5min", mosq_شمار_mqtt_publish_بھیجا, METRIC_بھرو_5_MIN }, /* metric_load_pub_messages_sent_5min */
	{ 0.0, "$SYS/broker/load/publish/sent/15min", mosq_شمار_mqtt_publish_بھیجا, METRIC_بھرو_15_MIN }, /* metric_load_pub_messages_sent_15min */
	{ 0.0, "$SYS/broker/load/bytes/received/1min", mosq_شمار_بائٹیں_received, METRIC_بھرو_1_MIN }, /* metric_load_bytes_received_1min */
	{ 0.0, "$SYS/broker/load/bytes/received/5min", mosq_شمار_بائٹیں_received, METRIC_بھرو_5_MIN }, /* metric_load_bytes_received_5min */
	{ 0.0, "$SYS/broker/load/bytes/received/15min", mosq_شمار_بائٹیں_received, METRIC_بھرو_15_MIN }, /* metric_load_bytes_received_15min */
	{ 0.0, "$SYS/broker/load/bytes/sent/1min", mosq_شمار_بائٹیں_بھیجا, METRIC_بھرو_1_MIN }, /* metric_load_bytes_sent_1min */
	{ 0.0, "$SYS/broker/load/bytes/sent/5min", mosq_شمار_بائٹیں_بھیجا, METRIC_بھرو_5_MIN }, /* metric_load_bytes_sent_5min */
	{ 0.0, "$SYS/broker/load/bytes/sent/15min", mosq_شمار_بائٹیں_بھیجا, METRIC_بھرو_15_MIN }, /* metric_load_bytes_sent_15min */
	{ 0.0, "$SYS/broker/load/sockets/1min", mosq_شمار_socket_connections, METRIC_بھرو_1_MIN }, /* metric_load_socket_connections_1min */
	{ 0.0, "$SYS/broker/load/sockets/5min", mosq_شمار_socket_connections, METRIC_بھرو_5_MIN }, /* metric_load_socket_connections_5min */
	{ 0.0, "$SYS/broker/load/sockets/15min", mosq_شمار_socket_connections, METRIC_بھرو_15_MIN }, /* metric_load_socket_connections_15min */
	{ 0.0, "$SYS/broker/load/connections/1min", mosq_شمار_mqtt_connect_received, METRIC_بھرو_1_MIN }, /* metric_load_connections_1min */
	{ 0.0, "$SYS/broker/load/connections/5min", mosq_شمار_mqtt_connect_received, METRIC_بھرو_5_MIN }, /* metric_load_connections_5min */
	{ 0.0, "$SYS/broker/load/connections/15min", mosq_شمار_mqtt_connect_received, METRIC_بھرو_15_MIN }, /* metric_load_connections_15min */
};

ساکن time_t آغاز_وقت = 0;
ساکن time_t آخری_update = 0;


time_t broker_uptime(خالی)
{
	واپس db.now_s - آغاز_وقت;
}


ساکن خالی calc_بھرو(حرف *buf, دوہرا exponent, دوہرا i_mult, ساخت metric_بھرو *m, bool force)
{
	دوہرا new_قدر;
	uint32_t len;
	دوہرا interval;

	interval = (دوہرا)(metrics[m->بھرو_ref].اگلا - metrics[m->بھرو_ref].current)*i_mult;
	new_قدر = interval + exponent*(m->current - interval);
	اگر(fabs(new_قدر - (m->current)) >= 0.01 || force){
		len = (uint32_t)snprintf(buf, BUFLEN, "%.2f", new_قدر);
		db_messages_easy_قطار(تہی, m->topic, SYS_درخت_QOS, len, buf, 1, پیغام_EXPIRY_INFINITE, تہی);
	}
	m->current = new_قدر;
}


خالی sys_درخت_init(خالی)
{
	حرف buf[BUFLEN];
	uint32_t len;

	اگر(db.config->sys_interval == 0){
		واپس;
	}

	/* Set static $SYS messages */
	len = (uint32_t)snprintf(buf, 64, "mosquitto version %s", نسخہ);
	db_messages_easy_قطار(تہی, "$SYS/broker/version", SYS_درخت_QOS, len, buf, 1, پیغام_EXPIRY_INFINITE, تہی);

	آغاز_وقت = mosquitto_وقت();
	آخری_update = آغاز_وقت;

	sys_درخت_update(true);

	/* Force published load values to 0 */
	برائے(صحیح i=0; i<mosq_metric_بھرو_max; i++){
		اگر(metric_loads[i].interval == METRIC_بھرو_1_MIN){
			calc_بھرو(buf, 0.0, 0.0, &metric_loads[i], true);
		}ورنہ اگر(metric_loads[i].interval == METRIC_بھرو_5_MIN){
			calc_بھرو(buf, 0.0, 0.0, &metric_loads[i], true);
		}ورنہ{
			calc_بھرو(buf, 0.0, 0.0, &metric_loads[i], true);
		}
	}
}


خالی metrics__int_inc(شمار mosq_metric_type m, int64_t قدر)
{
	اگر(m < mosq_metric_max){
		metrics[m].اگلا += قدر;
	}
}


خالی metrics__int_dec(شمار mosq_metric_type m, int64_t قدر)
{
	اگر(m < mosq_metric_max){
		metrics[m].اگلا -= قدر;
	}
}


/* Send messages for the $SYS hierarchy if the last update is longer than
 * 'interval' seconds ago.
 * 'interval' is the amount of seconds between updates. If 0, then no periodic
 * messages are sent for the $SYS hierarchy.
 * 'start_time' is the result of time() that the broker was started at.
 */
خالی sys_درخت_update(bool force)
{
	time_t uptime;
	حرف buf[BUFLEN];
	uint32_t len;
	time_t اگلا_event;
	ساکن time_t آخری_update_real = 0;

	اگر(db.config->sys_interval){
		اگلا_event = db.config->sys_interval - db.now_real_s % db.config->sys_interval - 1;
		اگر(اگلا_event <= 0){
			اگلا_event = db.config->sys_interval;
		}
		loop_update_اگلا_event(اگلا_event*1000);
	}

	اگر(db.config->sys_interval
			&& ((db.now_real_s % db.config->sys_interval == 0 && آخری_update_real != db.now_real_s) || force)){

		uptime = db.now_s - آغاز_وقت;
		len = (uint32_t)snprintf(buf, BUFLEN, "%" PRIu64 " seconds", (uint64_t)uptime);
		db_messages_easy_قطار(تہی, "$SYS/broker/uptime", SYS_درخت_QOS, len, buf, 1, پیغام_EXPIRY_INFINITE, تہی);

		/*  Update metrics values where not otherwise updated */
		metrics[mosq_gauge_پیغام_store_گنتی].اگلا = db.پیغام_store_گنتی;
		metrics[mosq_gauge_پیغام_store_بائٹیں].اگلا = (int64_t)db.پیغام_store_بائٹیں;
		metrics[mosq_gauge_subscriptions].اگلا = db.subscription_گنتی;
		metrics[mosq_gauge_shared_subscriptions].اگلا = db.shared_subscription_گنتی;
		metrics[mosq_gauge_retained_messages].اگلا = db.retained_گنتی;
#اگر_تعریف_شدہ WITH_حافظہ_TRACKING
		metrics[mosq_gauge_ڈھیر_current].اگلا = (int64_t)mosquitto_حافظہ_مستعمل();
		metrics[mosq_شمار_ڈھیر_زیادہ_سے_زیادہ].اگلا = (int64_t)mosquitto_max_حافظہ_مستعمل();
#اگر_ختم
		metrics[mosq_gauge_clients_کل].اگلا = HASH_CNT(hh_id, db.contexts_by_id);
		metrics[mosq_شمار_clients_زیادہ_سے_زیادہ].اگلا = HASH_CNT(hh_id, db.contexts_by_id);
		metrics[mosq_gauge_clients_connected].اگلا = HASH_CNT(hh_sock, db.contexts_by_sock);
		metrics[mosq_gauge_clients_disconnected].اگلا = HASH_CNT(hh_id, db.contexts_by_id) - HASH_CNT(hh_sock, db.contexts_by_sock);

		/* Handle loads first, because they reference other metrics and need next != current */
		اگر(db.now_s > آخری_update){
			دوہرا i_mult = 60.0/(دوہرا)(db.now_s-آخری_update);

			دوہرا exponent_1min = قوت_نما(-1.0*(دوہرا)(db.now_s-آخری_update)/60.0);
			دوہرا exponent_5min = قوت_نما(-1.0*(دوہرا)(db.now_s-آخری_update)/300.0);
			دوہرا exponent_15min = قوت_نما(-1.0*(دوہرا)(db.now_s-آخری_update)/900.0);

			برائے(صحیح i=0; i<mosq_metric_بھرو_max; i++){
				اگر(metric_loads[i].interval == METRIC_بھرو_1_MIN){
					calc_بھرو(buf, exponent_1min, i_mult, &metric_loads[i], false);
				}ورنہ اگر(metric_loads[i].interval == METRIC_بھرو_5_MIN){
					calc_بھرو(buf, exponent_5min, i_mult, &metric_loads[i], false);
				}ورنہ{
					calc_بھرو(buf, exponent_15min, i_mult, &metric_loads[i], false);
				}
			}
		}

		برائے(صحیح i=0; i<mosq_metric_max; i++){
			اگر((metrics[i].is_max && metrics[i].اگلا > metrics[i].current) ||
					(!metrics[i].is_max && metrics[i].اگلا != metrics[i].current)){

				metrics[i].current = metrics[i].اگلا;
				len = (uint32_t)snprintf(buf, BUFLEN, "%lu", metrics[i].current);
				اگر(metrics[i].topic){
					db_messages_easy_قطار(تہی, metrics[i].topic, SYS_درخت_QOS, len, buf, 1, پیغام_EXPIRY_INFINITE, تہی);
				}
				اگر(metrics[i].topic_alias){
					db_messages_easy_قطار(تہی, metrics[i].topic_alias, SYS_درخت_QOS, len, buf, 1, پیغام_EXPIRY_INFINITE, تہی);
				}
			}
		}

		آخری_update = db.now_s;
		آخری_update_real = db.now_real_s;
	}
}

#اگر_ختم
