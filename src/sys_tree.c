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

#که_تعریف_شوی WITH_SYS_ونه

#ضمن "config.h"

#ضمن <math.h>
#ضمن <stdio.h>
#ضمن <inttypes.h>
#ضمن <limits.h>
#ضمن <inttypes.h>

#ضمن "mosquitto_broker_internal.h"
#ضمن "sys_tree.h"

#تعریف_کړه BUFLEN 100

#تعریف_کړه SYS_ونه_QOS 2

#تعریف_کړه METRIC_بارول_1_MIN 1
#تعریف_کړه METRIC_بارول_5_MIN 2
#تعریف_کړه METRIC_بارول_15_MIN 3

بنية metric {
	int64_t current;
	int64_t بل;
	ثابت حرف *topic, *topic_alias;
	bool is_max;
};

بنية metric_بارول {
	مزدوج current;
	ثابت حرف *topic;
	صحيح بارول_ref;
	uint8_t interval;
};

بنية metric metrics[mosq_metric_max] = {
	{ 1, 0, "$SYS/broker/clients/total", عدم, false }, /* mosq_gauge_clients_total */
	{ 1, 0, "$SYS/broker/clients/maximum", عدم, true }, /* metric_clients_maximum */
	{ 1, 0, "$SYS/broker/clients/disconnected", "$SYS/broker/clients/inactive", false }, /* mosq_gauge_clients_disconnected */
	{ 1, 0, "$SYS/broker/clients/connected", "$SYS/broker/clients/active", false }, /* mosq_gauge_clients_connected */
	{ 1, 0, "$SYS/broker/clients/expired", عدم, false }, /* mosq_counter_clients_expired */
	{ 1, 0, "$SYS/broker/messages/stored", "$SYS/broker/store/messages/count", false }, /* mosq_gauge_message_store_count */
	{ 1, 0, "$SYS/broker/store/messages/bytes", عدم, false }, /* mosq_gauge_message_store_bytes */
	{ 1, 0, "$SYS/broker/subscriptions/count", عدم, false }, /* mosq_gauge_subscription_count */
	{ 1, 0, "$SYS/broker/shared_subscriptions/count", عدم, false }, /* mosq_gauge_shared_subscription_count */
	{ 1, 0, "$SYS/broker/retained messages/count", عدم, false }, /* mosq_gauge_retained_message_count */
#که_تعریف_شوی WITH_حافظه_TRACKING
	{ 1, 0, "$SYS/broker/heap/current", عدم, false }, /* mosq_gauge_heap_current */
	{ 1, 0, "$SYS/broker/heap/maximum", عدم, true }, /* mosq_gauge_heap_maximum */
#وإلا
	{ 1, 0, عدم, عدم, 0 }, /* mosq_gauge_heap_current */
	{ 1, 0, عدم, عدم, 0 }, /* mosq_gauge_heap_maximum */
#د_که_پای
	{ 1, 0, "$SYS/broker/messages/received", عدم, false }, /* mosq_counter_messages_received */
	{ 1, 0, "$SYS/broker/messages/sent", عدم, false }, /* mosq_counter_messages_sent */
	{ 1, 0, "$SYS/broker/bytes/received", عدم, false }, /* mosq_counter_bytes_received */
	{ 1, 0, "$SYS/broker/bytes/sent", عدم, false }, /* mosq_counter_bytes_sent */
	{ 1, 0, "$SYS/broker/publish/bytes/received", عدم, false }, /* mosq_counter_pub_bytes_received */
	{ 1, 0, "$SYS/broker/publish/bytes/sent", عدم, false }, /* mosq_counter_pub_bytes_sent */
	{ 1, 0, "$SYS/broker/packet/out/count", عدم, false }, /* mosq_gauge_out_packet_count */
	{ 1, 0, "$SYS/broker/packet/out/bytes", عدم, false }, /* mosq_gauge_out_packet_bytes */
	{ 1, 0, "$SYS/broker/connections/socket/count", عدم, false }, /* mosq_counter_socket_connections */
	{ 1, 0, عدم, عدم, false }, /* mosq_counter_mqtt_connect_received */
	{ 1, 0, عدم, عدم, false }, /* mosq_counter_mqtt_connect_sent */
	{ 1, 0, عدم, عدم, false }, /* mosq_counter_mqtt_connack_received */
	{ 1, 0, عدم, عدم, false }, /* mosq_counter_mqtt_connack_sent */
	{ 1, 0, "$SYS/broker/publish/messages/dropped", عدم, false }, /* mosq_counter_mqtt_publish_dropped */
	{ 1, 0, "$SYS/broker/publish/messages/received", عدم, false }, /* mosq_counter_mqtt_publish_received */
	{ 1, 0, "$SYS/broker/publish/messages/sent", عدم, false }, /* mosq_counter_mqtt_publish_sent */
	{ 1, 0, عدم, عدم, false }, /* mosq_counter_mqtt_puback_received */
	{ 1, 0, عدم, عدم, false }, /* mosq_counter_mqtt_puback_sent */
	{ 1, 0, عدم, عدم, false }, /* mosq_counter_mqtt_pubrec_received */
	{ 1, 0, عدم, عدم, false }, /* mosq_counter_mqtt_pubrec_sent */
	{ 1, 0, عدم, عدم, false }, /* mosq_counter_mqtt_pubrel_received */
	{ 1, 0, عدم, عدم, false }, /* mosq_counter_mqtt_pubrel_sent */
	{ 1, 0, عدم, عدم, false }, /* mosq_counter_mqtt_pubcomp_received */
	{ 1, 0, عدم, عدم, false }, /* mosq_counter_mqtt_pubcomp_sent */
	{ 1, 0, عدم, عدم, false }, /* mosq_counter_mqtt_subscribe_received */
	{ 1, 0, عدم, عدم, false }, /* mosq_counter_mqtt_subscribe_sent */
	{ 1, 0, عدم, عدم, false }, /* mosq_counter_mqtt_suback_received */
	{ 1, 0, عدم, عدم, false }, /* mosq_counter_mqtt_suback_sent */
	{ 1, 0, عدم, عدم, false }, /* mosq_counter_mqtt_unsubscribe_received */
	{ 1, 0, عدم, عدم, false }, /* mosq_counter_mqtt_unsubscribe_sent */
	{ 1, 0, عدم, عدم, false }, /* mosq_counter_mqtt_unsuback_received */
	{ 1, 0, عدم, عدم, false }, /* mosq_counter_mqtt_unsuback_sent */
	{ 1, 0, عدم, عدم, false }, /* mosq_counter_mqtt_pingreq_received */
	{ 1, 0, عدم, عدم, false }, /* mosq_counter_mqtt_pingreq_sent */
	{ 1, 0, عدم, عدم, false }, /* mosq_counter_mqtt_pingresp_received */
	{ 1, 0, عدم, عدم, false }, /* mosq_counter_mqtt_pingresp_sent */
	{ 1, 0, عدم, عدم, false }, /* mosq_counter_mqtt_disconnect_received */
	{ 1, 0, عدم, عدم, false }, /* mosq_counter_mqtt_disconnect_sent */
	{ 1, 0, عدم, عدم, false }, /* mosq_counter_mqtt_auth_received */
	{ 1, 0, عدم, عدم, false }, /* mosq_counter_mqtt_auth_sent */
};

بنية metric_بارول metric_loads[mosq_metric_بارول_max] = {
	{ 0.0, "$SYS/broker/load/messages/received/1min", mosq_counter_messages_received, METRIC_بارول_1_MIN }, /* metric_load_messages_received_1min */
	{ 0.0, "$SYS/broker/load/messages/received/5min", mosq_counter_messages_received, METRIC_بارول_5_MIN }, /* metric_load_messages_received_5min */
	{ 0.0, "$SYS/broker/load/messages/received/15min", mosq_counter_messages_received, METRIC_بارول_15_MIN }, /* metric_load_messages_received_15min */
	{ 0.0, "$SYS/broker/load/messages/sent/1min", mosq_counter_messages_لېږل_شوی, METRIC_بارول_1_MIN }, /* metric_load_messages_sent_1min */
	{ 0.0, "$SYS/broker/load/messages/sent/5min", mosq_counter_messages_لېږل_شوی, METRIC_بارول_5_MIN }, /* metric_load_messages_sent_5min */
	{ 0.0, "$SYS/broker/load/messages/sent/15min", mosq_counter_messages_لېږل_شوی, METRIC_بارول_15_MIN }, /* metric_load_messages_sent_15min */
	{ 0.0, "$SYS/broker/load/publish/dropped/1min", mosq_counter_mqtt_publish_dropped, METRIC_بارول_1_MIN }, /* metric_load_pub_messages_dropped_1min */
	{ 0.0, "$SYS/broker/load/publish/dropped/5min", mosq_counter_mqtt_publish_dropped, METRIC_بارول_5_MIN }, /* metric_load_pub_messages_dropped_5min */
	{ 0.0, "$SYS/broker/load/publish/dropped/15min", mosq_counter_mqtt_publish_dropped, METRIC_بارول_15_MIN }, /* metric_load_pub_messages_dropped_15min */
	{ 0.0, "$SYS/broker/load/publish/received/1min", mosq_counter_mqtt_publish_received, METRIC_بارول_1_MIN }, /* metric_load_pub_messages_received_1min */
	{ 0.0, "$SYS/broker/load/publish/received/5min", mosq_counter_mqtt_publish_received, METRIC_بارول_5_MIN }, /* metric_load_pub_messages_received_5min */
	{ 0.0, "$SYS/broker/load/publish/received/15min", mosq_counter_mqtt_publish_received, METRIC_بارول_15_MIN }, /* metric_load_pub_messages_received_15min */
	{ 0.0, "$SYS/broker/load/publish/sent/1min", mosq_counter_mqtt_publish_لېږل_شوی, METRIC_بارول_1_MIN }, /* metric_load_pub_messages_sent_1min */
	{ 0.0, "$SYS/broker/load/publish/sent/5min", mosq_counter_mqtt_publish_لېږل_شوی, METRIC_بارول_5_MIN }, /* metric_load_pub_messages_sent_5min */
	{ 0.0, "$SYS/broker/load/publish/sent/15min", mosq_counter_mqtt_publish_لېږل_شوی, METRIC_بارول_15_MIN }, /* metric_load_pub_messages_sent_15min */
	{ 0.0, "$SYS/broker/load/bytes/received/1min", mosq_counter_بایټونه_received, METRIC_بارول_1_MIN }, /* metric_load_bytes_received_1min */
	{ 0.0, "$SYS/broker/load/bytes/received/5min", mosq_counter_بایټونه_received, METRIC_بارول_5_MIN }, /* metric_load_bytes_received_5min */
	{ 0.0, "$SYS/broker/load/bytes/received/15min", mosq_counter_بایټونه_received, METRIC_بارول_15_MIN }, /* metric_load_bytes_received_15min */
	{ 0.0, "$SYS/broker/load/bytes/sent/1min", mosq_counter_بایټونه_لېږل_شوی, METRIC_بارول_1_MIN }, /* metric_load_bytes_sent_1min */
	{ 0.0, "$SYS/broker/load/bytes/sent/5min", mosq_counter_بایټونه_لېږل_شوی, METRIC_بارول_5_MIN }, /* metric_load_bytes_sent_5min */
	{ 0.0, "$SYS/broker/load/bytes/sent/15min", mosq_counter_بایټونه_لېږل_شوی, METRIC_بارول_15_MIN }, /* metric_load_bytes_sent_15min */
	{ 0.0, "$SYS/broker/load/sockets/1min", mosq_counter_socket_connections, METRIC_بارول_1_MIN }, /* metric_load_socket_connections_1min */
	{ 0.0, "$SYS/broker/load/sockets/5min", mosq_counter_socket_connections, METRIC_بارول_5_MIN }, /* metric_load_socket_connections_5min */
	{ 0.0, "$SYS/broker/load/sockets/15min", mosq_counter_socket_connections, METRIC_بارول_15_MIN }, /* metric_load_socket_connections_15min */
	{ 0.0, "$SYS/broker/load/connections/1min", mosq_counter_mqtt_connect_received, METRIC_بارول_1_MIN }, /* metric_load_connections_1min */
	{ 0.0, "$SYS/broker/load/connections/5min", mosq_counter_mqtt_connect_received, METRIC_بارول_5_MIN }, /* metric_load_connections_5min */
	{ 0.0, "$SYS/broker/load/connections/15min", mosq_counter_mqtt_connect_received, METRIC_بارول_15_MIN }, /* metric_load_connections_15min */
};

ساكن time_t پیل_وخت = 0;
ساكن time_t وروستی_update = 0;


time_t broker_uptime(فراغ)
{
	أرجع db.now_s - پیل_وخت;
}


ساكن فراغ calc_بارول(حرف *buf, مزدوج exponent, مزدوج i_mult, بنية metric_بارول *m, bool force)
{
	مزدوج new_ارزښت;
	uint32_t len;
	مزدوج interval;

	interval = (مزدوج)(metrics[m->بارول_ref].بل - metrics[m->بارول_ref].current)*i_mult;
	new_ارزښت = interval + exponent*(m->current - interval);
	إذا(fabs(new_ارزښت - (m->current)) >= 0.01 || force){
		len = (uint32_t)snprintf(buf, BUFLEN, "%.2f", new_ارزښت);
		db_messages_easy_کتار(عدم, m->topic, SYS_ونه_QOS, len, buf, 1, پیغام_EXPIRY_INFINITE, عدم);
	}
	m->current = new_ارزښت;
}


فراغ sys_ونه_init(فراغ)
{
	حرف buf[BUFLEN];
	uint32_t len;

	إذا(db.config->sys_interval == 0){
		أرجع;
	}

	/* Set static $SYS messages */
	len = (uint32_t)snprintf(buf, 64, "mosquitto version %s", نسخه);
	db_messages_easy_کتار(عدم, "$SYS/broker/version", SYS_ونه_QOS, len, buf, 1, پیغام_EXPIRY_INFINITE, عدم);

	پیل_وخت = mosquitto_وخت();
	وروستی_update = پیل_وخت;

	sys_ونه_update(true);

	/* Force published load values to 0 */
	لكل(صحيح i=0; i<mosq_metric_بارول_max; i++){
		إذا(metric_loads[i].interval == METRIC_بارول_1_MIN){
			calc_بارول(buf, 0.0, 0.0, &metric_loads[i], true);
		}وإلا إذا(metric_loads[i].interval == METRIC_بارول_5_MIN){
			calc_بارول(buf, 0.0, 0.0, &metric_loads[i], true);
		}وإلا{
			calc_بارول(buf, 0.0, 0.0, &metric_loads[i], true);
		}
	}
}


فراغ metrics__int_inc(تعداد mosq_metric_type m, int64_t ارزښت)
{
	إذا(m < mosq_metric_max){
		metrics[m].بل += ارزښت;
	}
}


فراغ metrics__int_dec(تعداد mosq_metric_type m, int64_t ارزښت)
{
	إذا(m < mosq_metric_max){
		metrics[m].بل -= ارزښت;
	}
}


/* Send messages for the $SYS hierarchy if the last update is longer than
 * 'interval' seconds ago.
 * 'interval' is the amount of seconds between updates. If 0, then no periodic
 * messages are sent for the $SYS hierarchy.
 * 'start_time' is the result of time() that the broker was started at.
 */
فراغ sys_ونه_update(bool force)
{
	time_t uptime;
	حرف buf[BUFLEN];
	uint32_t len;
	time_t بل_event;
	ساكن time_t وروستی_update_real = 0;

	إذا(db.config->sys_interval){
		بل_event = db.config->sys_interval - db.now_real_s % db.config->sys_interval - 1;
		إذا(بل_event <= 0){
			بل_event = db.config->sys_interval;
		}
		loop_update_بل_event(بل_event*1000);
	}

	إذا(db.config->sys_interval
			&& ((db.now_real_s % db.config->sys_interval == 0 && وروستی_update_real != db.now_real_s) || force)){

		uptime = db.now_s - پیل_وخت;
		len = (uint32_t)snprintf(buf, BUFLEN, "%" PRIu64 " seconds", (uint64_t)uptime);
		db_messages_easy_کتار(عدم, "$SYS/broker/uptime", SYS_ونه_QOS, len, buf, 1, پیغام_EXPIRY_INFINITE, عدم);

		/*  Update metrics values where not otherwise updated */
		metrics[mosq_gauge_پیغام_store_شمېر].بل = db.پیغام_store_شمېر;
		metrics[mosq_gauge_پیغام_store_بایټونه].بل = (int64_t)db.پیغام_store_بایټونه;
		metrics[mosq_gauge_subscriptions].بل = db.subscription_شمېر;
		metrics[mosq_gauge_shared_subscriptions].بل = db.shared_subscription_شمېر;
		metrics[mosq_gauge_retained_messages].بل = db.retained_شمېر;
#که_تعریف_شوی WITH_حافظه_TRACKING
		metrics[mosq_gauge_ډېرۍ_current].بل = (int64_t)mosquitto_حافظه_کارول_شوی();
		metrics[mosq_counter_ډېرۍ_اعظمي].بل = (int64_t)mosquitto_max_حافظه_کارول_شوی();
#د_که_پای
		metrics[mosq_gauge_clients_ټول].بل = HASH_CNT(hh_id, db.contexts_by_id);
		metrics[mosq_counter_clients_اعظمي].بل = HASH_CNT(hh_id, db.contexts_by_id);
		metrics[mosq_gauge_clients_connected].بل = HASH_CNT(hh_sock, db.contexts_by_sock);
		metrics[mosq_gauge_clients_disconnected].بل = HASH_CNT(hh_id, db.contexts_by_id) - HASH_CNT(hh_sock, db.contexts_by_sock);

		/* Handle loads first, because they reference other metrics and need next != current */
		إذا(db.now_s > وروستی_update){
			مزدوج i_mult = 60.0/(مزدوج)(db.now_s-وروستی_update);

			مزدوج exponent_1min = توانیز_تابع(-1.0*(مزدوج)(db.now_s-وروستی_update)/60.0);
			مزدوج exponent_5min = توانیز_تابع(-1.0*(مزدوج)(db.now_s-وروستی_update)/300.0);
			مزدوج exponent_15min = توانیز_تابع(-1.0*(مزدوج)(db.now_s-وروستی_update)/900.0);

			لكل(صحيح i=0; i<mosq_metric_بارول_max; i++){
				إذا(metric_loads[i].interval == METRIC_بارول_1_MIN){
					calc_بارول(buf, exponent_1min, i_mult, &metric_loads[i], false);
				}وإلا إذا(metric_loads[i].interval == METRIC_بارول_5_MIN){
					calc_بارول(buf, exponent_5min, i_mult, &metric_loads[i], false);
				}وإلا{
					calc_بارول(buf, exponent_15min, i_mult, &metric_loads[i], false);
				}
			}
		}

		لكل(صحيح i=0; i<mosq_metric_max; i++){
			إذا((metrics[i].is_max && metrics[i].بل > metrics[i].current) ||
					(!metrics[i].is_max && metrics[i].بل != metrics[i].current)){

				metrics[i].current = metrics[i].بل;
				len = (uint32_t)snprintf(buf, BUFLEN, "%lu", metrics[i].current);
				إذا(metrics[i].topic){
					db_messages_easy_کتار(عدم, metrics[i].topic, SYS_ونه_QOS, len, buf, 1, پیغام_EXPIRY_INFINITE, عدم);
				}
				إذا(metrics[i].topic_alias){
					db_messages_easy_کتار(عدم, metrics[i].topic_alias, SYS_ونه_QOS, len, buf, 1, پیغام_EXPIRY_INFINITE, عدم);
				}
			}
		}

		وروستی_update = db.now_s;
		وروستی_update_real = db.now_real_s;
	}
}

#د_که_پای
