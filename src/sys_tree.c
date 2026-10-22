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

#اگر_تعریف_شده WITH_SYS_درخت

#ضمن "config.h"

#ضمن <math.h>
#ضمن <stdio.h>
#ضمن <inttypes.h>
#ضمن <limits.h>
#ضمن <inttypes.h>

#ضمن "mosquitto_broker_internal.h"
#ضمن "sys_tree.h"

#تعریف_کن BUFLEN 100

#تعریف_کن SYS_درخت_QOS 2

#تعریف_کن METRIC_بارگیری_1_MIN 1
#تعریف_کن METRIC_بارگیری_5_MIN 2
#تعریف_کن METRIC_بارگیری_15_MIN 3

بنية metric {
	int64_t current;
	int64_t بعدی;
	ثابت حرف *topic, *topic_alias;
	bool is_max;
};

بنية metric_بارگیری {
	مزدوج current;
	ثابت حرف *topic;
	صحيح بارگیری_ref;
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
#اگر_تعریف_شده WITH_حافظه_TRACKING
	{ 1, 0, "$SYS/broker/heap/current", عدم, false }, /* mosq_gauge_heap_current */
	{ 1, 0, "$SYS/broker/heap/maximum", عدم, true }, /* mosq_gauge_heap_maximum */
#وإلا
	{ 1, 0, عدم, عدم, 0 }, /* mosq_gauge_heap_current */
	{ 1, 0, عدم, عدم, 0 }, /* mosq_gauge_heap_maximum */
#پایان_اگر
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

بنية metric_بارگیری metric_loads[mosq_metric_بارگیری_max] = {
	{ 0.0, "$SYS/broker/load/messages/received/1min", mosq_counter_messages_received, METRIC_بارگیری_1_MIN }, /* metric_load_messages_received_1min */
	{ 0.0, "$SYS/broker/load/messages/received/5min", mosq_counter_messages_received, METRIC_بارگیری_5_MIN }, /* metric_load_messages_received_5min */
	{ 0.0, "$SYS/broker/load/messages/received/15min", mosq_counter_messages_received, METRIC_بارگیری_15_MIN }, /* metric_load_messages_received_15min */
	{ 0.0, "$SYS/broker/load/messages/sent/1min", mosq_counter_messages_فرستاده, METRIC_بارگیری_1_MIN }, /* metric_load_messages_sent_1min */
	{ 0.0, "$SYS/broker/load/messages/sent/5min", mosq_counter_messages_فرستاده, METRIC_بارگیری_5_MIN }, /* metric_load_messages_sent_5min */
	{ 0.0, "$SYS/broker/load/messages/sent/15min", mosq_counter_messages_فرستاده, METRIC_بارگیری_15_MIN }, /* metric_load_messages_sent_15min */
	{ 0.0, "$SYS/broker/load/publish/dropped/1min", mosq_counter_mqtt_publish_dropped, METRIC_بارگیری_1_MIN }, /* metric_load_pub_messages_dropped_1min */
	{ 0.0, "$SYS/broker/load/publish/dropped/5min", mosq_counter_mqtt_publish_dropped, METRIC_بارگیری_5_MIN }, /* metric_load_pub_messages_dropped_5min */
	{ 0.0, "$SYS/broker/load/publish/dropped/15min", mosq_counter_mqtt_publish_dropped, METRIC_بارگیری_15_MIN }, /* metric_load_pub_messages_dropped_15min */
	{ 0.0, "$SYS/broker/load/publish/received/1min", mosq_counter_mqtt_publish_received, METRIC_بارگیری_1_MIN }, /* metric_load_pub_messages_received_1min */
	{ 0.0, "$SYS/broker/load/publish/received/5min", mosq_counter_mqtt_publish_received, METRIC_بارگیری_5_MIN }, /* metric_load_pub_messages_received_5min */
	{ 0.0, "$SYS/broker/load/publish/received/15min", mosq_counter_mqtt_publish_received, METRIC_بارگیری_15_MIN }, /* metric_load_pub_messages_received_15min */
	{ 0.0, "$SYS/broker/load/publish/sent/1min", mosq_counter_mqtt_publish_فرستاده, METRIC_بارگیری_1_MIN }, /* metric_load_pub_messages_sent_1min */
	{ 0.0, "$SYS/broker/load/publish/sent/5min", mosq_counter_mqtt_publish_فرستاده, METRIC_بارگیری_5_MIN }, /* metric_load_pub_messages_sent_5min */
	{ 0.0, "$SYS/broker/load/publish/sent/15min", mosq_counter_mqtt_publish_فرستاده, METRIC_بارگیری_15_MIN }, /* metric_load_pub_messages_sent_15min */
	{ 0.0, "$SYS/broker/load/bytes/received/1min", mosq_counter_بایتها_received, METRIC_بارگیری_1_MIN }, /* metric_load_bytes_received_1min */
	{ 0.0, "$SYS/broker/load/bytes/received/5min", mosq_counter_بایتها_received, METRIC_بارگیری_5_MIN }, /* metric_load_bytes_received_5min */
	{ 0.0, "$SYS/broker/load/bytes/received/15min", mosq_counter_بایتها_received, METRIC_بارگیری_15_MIN }, /* metric_load_bytes_received_15min */
	{ 0.0, "$SYS/broker/load/bytes/sent/1min", mosq_counter_بایتها_فرستاده, METRIC_بارگیری_1_MIN }, /* metric_load_bytes_sent_1min */
	{ 0.0, "$SYS/broker/load/bytes/sent/5min", mosq_counter_بایتها_فرستاده, METRIC_بارگیری_5_MIN }, /* metric_load_bytes_sent_5min */
	{ 0.0, "$SYS/broker/load/bytes/sent/15min", mosq_counter_بایتها_فرستاده, METRIC_بارگیری_15_MIN }, /* metric_load_bytes_sent_15min */
	{ 0.0, "$SYS/broker/load/sockets/1min", mosq_counter_socket_connections, METRIC_بارگیری_1_MIN }, /* metric_load_socket_connections_1min */
	{ 0.0, "$SYS/broker/load/sockets/5min", mosq_counter_socket_connections, METRIC_بارگیری_5_MIN }, /* metric_load_socket_connections_5min */
	{ 0.0, "$SYS/broker/load/sockets/15min", mosq_counter_socket_connections, METRIC_بارگیری_15_MIN }, /* metric_load_socket_connections_15min */
	{ 0.0, "$SYS/broker/load/connections/1min", mosq_counter_mqtt_connect_received, METRIC_بارگیری_1_MIN }, /* metric_load_connections_1min */
	{ 0.0, "$SYS/broker/load/connections/5min", mosq_counter_mqtt_connect_received, METRIC_بارگیری_5_MIN }, /* metric_load_connections_5min */
	{ 0.0, "$SYS/broker/load/connections/15min", mosq_counter_mqtt_connect_received, METRIC_بارگیری_15_MIN }, /* metric_load_connections_15min */
};

ساكن time_t شروع_زمان = 0;
ساكن time_t آخرین_update = 0;


time_t broker_uptime(فراغ)
{
	أرجع db.now_s - شروع_زمان;
}


ساكن فراغ calc_بارگیری(حرف *buf, مزدوج exponent, مزدوج i_mult, بنية metric_بارگیری *m, bool force)
{
	مزدوج new_مقدار;
	uint32_t len;
	مزدوج interval;

	interval = (مزدوج)(metrics[m->بارگیری_ref].بعدی - metrics[m->بارگیری_ref].current)*i_mult;
	new_مقدار = interval + exponent*(m->current - interval);
	إذا(fabs(new_مقدار - (m->current)) >= 0.01 || force){
		len = (uint32_t)snprintf(buf, BUFLEN, "%.2f", new_مقدار);
		db_messages_easy_صف(عدم, m->topic, SYS_درخت_QOS, len, buf, 1, پیام_EXPIRY_INFINITE, عدم);
	}
	m->current = new_مقدار;
}


فراغ sys_درخت_init(فراغ)
{
	حرف buf[BUFLEN];
	uint32_t len;

	إذا(db.config->sys_interval == 0){
		أرجع;
	}

	/* Set static $SYS messages */
	len = (uint32_t)snprintf(buf, 64, "mosquitto version %s", نسخه);
	db_messages_easy_صف(عدم, "$SYS/broker/version", SYS_درخت_QOS, len, buf, 1, پیام_EXPIRY_INFINITE, عدم);

	شروع_زمان = mosquitto_زمان();
	آخرین_update = شروع_زمان;

	sys_درخت_update(true);

	/* Force published load values to 0 */
	لكل(صحيح i=0; i<mosq_metric_بارگیری_max; i++){
		إذا(metric_loads[i].interval == METRIC_بارگیری_1_MIN){
			calc_بارگیری(buf, 0.0, 0.0, &metric_loads[i], true);
		}وإلا إذا(metric_loads[i].interval == METRIC_بارگیری_5_MIN){
			calc_بارگیری(buf, 0.0, 0.0, &metric_loads[i], true);
		}وإلا{
			calc_بارگیری(buf, 0.0, 0.0, &metric_loads[i], true);
		}
	}
}


فراغ metrics__int_inc(تعداد mosq_metric_type m, int64_t مقدار)
{
	إذا(m < mosq_metric_max){
		metrics[m].بعدی += مقدار;
	}
}


فراغ metrics__int_dec(تعداد mosq_metric_type m, int64_t مقدار)
{
	إذا(m < mosq_metric_max){
		metrics[m].بعدی -= مقدار;
	}
}


/* Send messages for the $SYS hierarchy if the last update is longer than
 * 'interval' seconds ago.
 * 'interval' is the amount of seconds between updates. If 0, then no periodic
 * messages are sent for the $SYS hierarchy.
 * 'start_time' is the result of time() that the broker was started at.
 */
فراغ sys_درخت_update(bool force)
{
	time_t uptime;
	حرف buf[BUFLEN];
	uint32_t len;
	time_t بعدی_event;
	ساكن time_t آخرین_update_real = 0;

	إذا(db.config->sys_interval){
		بعدی_event = db.config->sys_interval - db.now_real_s % db.config->sys_interval - 1;
		إذا(بعدی_event <= 0){
			بعدی_event = db.config->sys_interval;
		}
		loop_update_بعدی_event(بعدی_event*1000);
	}

	إذا(db.config->sys_interval
			&& ((db.now_real_s % db.config->sys_interval == 0 && آخرین_update_real != db.now_real_s) || force)){

		uptime = db.now_s - شروع_زمان;
		len = (uint32_t)snprintf(buf, BUFLEN, "%" PRIu64 " seconds", (uint64_t)uptime);
		db_messages_easy_صف(عدم, "$SYS/broker/uptime", SYS_درخت_QOS, len, buf, 1, پیام_EXPIRY_INFINITE, عدم);

		/*  Update metrics values where not otherwise updated */
		metrics[mosq_gauge_پیام_store_شمار].بعدی = db.پیام_store_شمار;
		metrics[mosq_gauge_پیام_store_بایتها].بعدی = (int64_t)db.پیام_store_بایتها;
		metrics[mosq_gauge_subscriptions].بعدی = db.subscription_شمار;
		metrics[mosq_gauge_shared_subscriptions].بعدی = db.shared_subscription_شمار;
		metrics[mosq_gauge_retained_messages].بعدی = db.retained_شمار;
#اگر_تعریف_شده WITH_حافظه_TRACKING
		metrics[mosq_gauge_انباشته_current].بعدی = (int64_t)mosquitto_حافظه_استفاده_شده();
		metrics[mosq_counter_انباشته_بیشینه].بعدی = (int64_t)mosquitto_max_حافظه_استفاده_شده();
#پایان_اگر
		metrics[mosq_gauge_clients_مجموع].بعدی = HASH_CNT(hh_id, db.contexts_by_id);
		metrics[mosq_counter_clients_بیشینه].بعدی = HASH_CNT(hh_id, db.contexts_by_id);
		metrics[mosq_gauge_clients_connected].بعدی = HASH_CNT(hh_sock, db.contexts_by_sock);
		metrics[mosq_gauge_clients_disconnected].بعدی = HASH_CNT(hh_id, db.contexts_by_id) - HASH_CNT(hh_sock, db.contexts_by_sock);

		/* Handle loads first, because they reference other metrics and need next != current */
		إذا(db.now_s > آخرین_update){
			مزدوج i_mult = 60.0/(مزدوج)(db.now_s-آخرین_update);

			مزدوج exponent_1min = نمایی_تابع(-1.0*(مزدوج)(db.now_s-آخرین_update)/60.0);
			مزدوج exponent_5min = نمایی_تابع(-1.0*(مزدوج)(db.now_s-آخرین_update)/300.0);
			مزدوج exponent_15min = نمایی_تابع(-1.0*(مزدوج)(db.now_s-آخرین_update)/900.0);

			لكل(صحيح i=0; i<mosq_metric_بارگیری_max; i++){
				إذا(metric_loads[i].interval == METRIC_بارگیری_1_MIN){
					calc_بارگیری(buf, exponent_1min, i_mult, &metric_loads[i], false);
				}وإلا إذا(metric_loads[i].interval == METRIC_بارگیری_5_MIN){
					calc_بارگیری(buf, exponent_5min, i_mult, &metric_loads[i], false);
				}وإلا{
					calc_بارگیری(buf, exponent_15min, i_mult, &metric_loads[i], false);
				}
			}
		}

		لكل(صحيح i=0; i<mosq_metric_max; i++){
			إذا((metrics[i].is_max && metrics[i].بعدی > metrics[i].current) ||
					(!metrics[i].is_max && metrics[i].بعدی != metrics[i].current)){

				metrics[i].current = metrics[i].بعدی;
				len = (uint32_t)snprintf(buf, BUFLEN, "%lu", metrics[i].current);
				إذا(metrics[i].topic){
					db_messages_easy_صف(عدم, metrics[i].topic, SYS_درخت_QOS, len, buf, 1, پیام_EXPIRY_INFINITE, عدم);
				}
				إذا(metrics[i].topic_alias){
					db_messages_easy_صف(عدم, metrics[i].topic_alias, SYS_درخت_QOS, len, buf, 1, پیام_EXPIRY_INFINITE, عدم);
				}
			}
		}

		آخرین_update = db.now_s;
		آخرین_update_real = db.now_real_s;
	}
}

#پایان_اگر
