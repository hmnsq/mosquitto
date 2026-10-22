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
#ضمن "config.h"

#ضمن <inttypes.h>
#ضمن <stdarg.h>
#ضمن <stdio.h>
#ضمن <string.h>
#ضمن <inttypes.h>
#جے_بنا_تعریف WIN32
#ضمن <syslog.h>
#جے_انت
#ضمن <time.h>

#إذا defined(__APPLE__)
#  include <sys/time.h>
#جے_انت

#جے_تعریف_شدہ WITH_DLT
#ضمن <sys/stat.h>
#ضمن <dlt/dlt.h>
#جے_انت

#ضمن "logging_mosq.h"
#ضمن "mosquitto_broker_internal.h"
#ضمن "util_mosq.h"

#جے_تعریف_شدہ WIN32
HANDLE syslog_h;
#جے_انت

#جے_تعریف_شدہ ANDROID
#ضمن <android/log.h>
ساكن ثابت حرف *LOG_ٹیگ = "mosquitto";
#جے_انت

ساكن حرف log_fptr_بفر[BUFSIZ];
ساكن فراغ libcommon__vprintf(ثابت حرف *fmt, va_list va);

/* Options for logging should be:
 *
 * A combination of:
 * Via syslog
 * To a file
 * To stdout/stderr
 * To topics
 */

/* Give option of logging timestamp.
 * Logging pid.
 */
ساكن بنانشانی صحيح log_destinations = MQTT3_LOG_STDERR;
ساكن بنانشانی صحيح log_priorities = MOSQ_LOG_ERR | MOSQ_LOG_WARNING | MOSQ_LOG_NOTICE | MOSQ_LOG_INFO;

#جے_تعریف_شدہ WITH_DLT
ساكن DltContext dltContext;
ساكن bool dlt_allowed = false;


فراغ dlt_fifo_check(فراغ)
{
	بنية stat statbuf;

	/* If we start DLT but the /tmp/dlt fifo doesn't exist, or isn't available
	 * for writing then there is a big delay when we try and close the log
	 * later, so check for it first. This has the side effect of not letting
	 * people using DLT create the fifo after Mosquitto has started, but at the
	 * benefit of not having a massive delay for everybody else. */
	memset(&statbuf, 0, حجم(statbuf));
	إذا(stat("/tmp/dlt", &statbuf) == 0){
		إذا(S_ISFIFO(statbuf.st_منوال)){
			صحيح fd = open("/tmp/dlt", O_NONBLOCK | O_WRONLY);
			إذا(fd != -1){
				dlt_allowed = true;
				close(fd);
			}
		}
	}
}
#جے_انت


ساكن صحيح get_ویلا(بنية tm **ti)
{
	time_t s;

	s = db.now_real_s;

	*ti = localtime(&s);
	إذا(!(*ti)){
		fprintf(stderr, "Error obtaining system time.\n");
		أرجع 1;
	}

	أرجع 0;
}


صحيح log__init(بنية mosquitto__config *config)
{
	صحيح rc = 0;

	libcommon_vprintf = libcommon__vprintf;

	log_priorities = config->log_type;
	log_destinations = config->log_dest;

	إذا(log_destinations & MQTT3_LOG_SYSLOG){
#جے_بنا_تعریف WIN32
		openlog("mosquitto", LOG_PID|LOG_CONS, config->log_facility);
#وإلا
		syslog_h = OpenEventLog(عدم, "mosquitto");
#جے_انت
	}

	إذا(log_destinations & MQTT3_LOG_FILE){
		config->log_fptr = mosquitto_fopen(config->log_file, "at", true);
		إذا(config->log_fptr){
			setvbuf(config->log_fptr, log_fptr_بفر, _IOLBF, حجم(log_fptr_بفر));
		}وإلا{
			log_destinations = MQTT3_LOG_STDERR;
			log_priorities = MOSQ_LOG_ERR;
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to open log file %s for writing.", config->log_file);
		}
	}
#جے_بنا_تعریف WIN32
	إذا(log_destinations & MQTT3_LOG_STDOUT){
		setvbuf(stdout, عدم, _IOLBF, 0);
	}
#جے_انت
#جے_تعریف_شدہ WITH_DLT
	إذا(log_destinations & MQTT3_LOG_DLT){
		dlt_fifo_check();
		إذا(dlt_allowed){
			DLT_رجسٹر_APP("MQTT", "mosquitto log");
			dlt_رجسٹر_context(&dltContext, "MQTT", "mosquitto DLT context");
		}
	}
#جے_انت
	أرجع rc;
}


صحيح log__close(بنية mosquitto__config *config)
{
	إذا(log_destinations & MQTT3_LOG_SYSLOG){
#جے_بنا_تعریف WIN32
		closelog();
#وإلا
		CloseEventLog(syslog_h);
#جے_انت
	}
	إذا(log_destinations & MQTT3_LOG_FILE){
		إذا(config->log_fptr){
			fclose(config->log_fptr);
			config->log_fptr = عدم;
		}
	}

#جے_تعریف_شدہ WITH_DLT
	إذا(dlt_allowed){
		dlt_unregister_context(&dltContext);
		DLT_UNREGISTER_APP();
	}
#جے_انت
	/* FIXME - do something for all destinations! */
	أرجع MOSQ_ERR_SUCCESS;
}

#جے_تعریف_شدہ WITH_DLT


DltLogLevelType get_dlt_level(بنانشانی صحيح ترجیح)
{
	بدل(ترجیح){
		حالة MOSQ_LOG_ERR:
			أرجع DLT_LOG_ERROR;
		حالة MOSQ_LOG_WARNING:
			أرجع DLT_LOG_WARN;
		حالة MOSQ_LOG_INFO:
			أرجع DLT_LOG_INFO;
		حالة MOSQ_LOG_DEBUG:
			أرجع DLT_LOG_DEBUG;
		حالة MOSQ_LOG_NOTICE:
		حالة MOSQ_LOG_SUBSCRIBE:
		حالة MOSQ_LOG_UNSUBSCRIBE:
			أرجع DLT_LOG_VERBOSE;
		افتراضي:
			أرجع DLT_LOG_DEFAULT;
	}
}
#جے_انت

#جے_تعریف_شدہ ANDROID


android_Log_ترجیح get_android_level(بنانشانی صحيح ترجیح)
{
	بدل(ترجیح){
		حالة MOSQ_LOG_ERR:
			أرجع ANDROID_LOG_ERROR;
		حالة MOSQ_LOG_WARNING:
			أرجع ANDROID_LOG_WARN;
		حالة MOSQ_LOG_INFO:
			أرجع ANDROID_LOG_INFO;
		حالة MOSQ_LOG_DEBUG:
			أرجع ANDROID_LOG_DEBUG;
		حالة MOSQ_LOG_NOTICE:
		حالة MOSQ_LOG_SUBSCRIBE:
		حالة MOSQ_LOG_UNSUBSCRIBE:
			أرجع ANDROID_LOG_VERBOSE;
		افتراضي:
			أرجع ANDROID_LOG_DEBUG;
	}
}
#جے_انت


ساكن صحيح log__vprintf(بنانشانی صحيح ترجیح, ثابت حرف *fmt, va_list va)
{
	ثابت حرف *topic;
	صحيح syslog_ترجیح;
	حرف log_سطر[1000];
	size_t log_سطر_pos;
#جے_تعریف_شدہ WIN32
	حرف *sp;
#جے_انت
	bool log_timestamp = true;
	حرف *log_timestamp_format = عدم;
	FILE *log_fptr = عدم;

	إذا(db.config){
		log_timestamp = db.config->log_timestamp;
		log_timestamp_format = db.config->log_timestamp_format;
		log_fptr = db.config->log_fptr;
	}

	إذا((log_priorities & ترجیح) && log_destinations != MQTT3_LOG_NONE){
		بدل(ترجیح){
			حالة MOSQ_LOG_SUBSCRIBE:
				topic = "$SYS/broker/log/M/subscribe";
#جے_بنا_تعریف WIN32
				syslog_ترجیح = LOG_NOTICE;
#وإلا
				syslog_ترجیح = EVENTLOG_INFORMATION_TYPE;
#جے_انت
				اكسر;
			حالة MOSQ_LOG_UNSUBSCRIBE:
				topic = "$SYS/broker/log/M/unsubscribe";
#جے_بنا_تعریف WIN32
				syslog_ترجیح = LOG_NOTICE;
#وإلا
				syslog_ترجیح = EVENTLOG_INFORMATION_TYPE;
#جے_انت
				اكسر;
			حالة MOSQ_LOG_DEBUG:
				topic = "$SYS/broker/log/D";
#جے_بنا_تعریف WIN32
				syslog_ترجیح = LOG_DEBUG;
#وإلا
				syslog_ترجیح = EVENTLOG_INFORMATION_TYPE;
#جے_انت
				اكسر;
			حالة MOSQ_LOG_ERR:
				topic = "$SYS/broker/log/E";
#جے_بنا_تعریف WIN32
				syslog_ترجیح = LOG_ERR;
#وإلا
				syslog_ترجیح = EVENTLOG_ERROR_TYPE;
#جے_انت
				اكسر;
			حالة MOSQ_LOG_WARNING:
				topic = "$SYS/broker/log/W";
#جے_بنا_تعریف WIN32
				syslog_ترجیح = LOG_WARNING;
#وإلا
				syslog_ترجیح = EVENTLOG_WARNING_TYPE;
#جے_انت
				اكسر;
			حالة MOSQ_LOG_NOTICE:
				topic = "$SYS/broker/log/N";
#جے_بنا_تعریف WIN32
				syslog_ترجیح = LOG_NOTICE;
#وإلا
				syslog_ترجیح = EVENTLOG_INFORMATION_TYPE;
#جے_انت
				اكسر;
			حالة MOSQ_LOG_INFO:
				topic = "$SYS/broker/log/I";
#جے_بنا_تعریف WIN32
				syslog_ترجیح = LOG_INFO;
#وإلا
				syslog_ترجیح = EVENTLOG_INFORMATION_TYPE;
#جے_انت
				اكسر;
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			حالة MOSQ_LOG_WEBSOCKETS:
				topic = "$SYS/broker/log/WS";
#جے_بنا_تعریف WIN32
				syslog_ترجیح = LOG_DEBUG;
#وإلا
				syslog_ترجیح = EVENTLOG_INFORMATION_TYPE;
#جے_انت
				اكسر;
#جے_انت
			افتراضي:
				topic = "$SYS/broker/log/E";
#جے_بنا_تعریف WIN32
				syslog_ترجیح = LOG_ERR;
#وإلا
				syslog_ترجیح = EVENTLOG_ERROR_TYPE;
#جے_انت
		}
		إذا(log_timestamp){
			إذا(log_timestamp_format){
				بنية tm *ti = عدم;
				get_ویلا(&ti);
				log_سطر_pos = strftime(log_سطر, حجم(log_سطر), log_timestamp_format, ti);
				إذا(log_سطر_pos == 0){
					log_سطر_pos = (size_t)snprintf(log_سطر, حجم(log_سطر), "Time error");
				}
			}وإلا{
				log_سطر_pos = (size_t)snprintf(log_سطر, حجم(log_سطر), "%" PRIu64, (uint64_t)db.now_real_s);
			}
			إذا(log_سطر_pos < حجم(log_سطر)-3){
				log_سطر[log_سطر_pos] = ':';
				log_سطر[log_سطر_pos+1] = ' ';
				log_سطر[log_سطر_pos+2] = '\0';
				log_سطر_pos += 2;
			}
		}وإلا{
			log_سطر_pos = 0;
		}
		vsnprintf(&log_سطر[log_سطر_pos], حجم(log_سطر)-log_سطر_pos, fmt, va);
		log_سطر[حجم(log_سطر)-1] = '\0'; /* Ensure string is null terminated. */

		إذا(log_destinations & MQTT3_LOG_STDOUT){
			fprintf(stdout, "%s\n", log_سطر);
		}
		إذا(log_destinations & MQTT3_LOG_STDERR){
			fprintf(stderr, "%s\n", log_سطر);
		}
		إذا(log_destinations & MQTT3_LOG_FILE && log_fptr){
			fprintf(log_fptr, "%s\n", log_سطر);
#جے_تعریف_شدہ WIN32
			/* Windows doesn't support line buffering, so flush. */
			fflush(log_fptr);
#جے_انت
		}
		إذا(log_destinations & MQTT3_LOG_SYSLOG){
#جے_بنا_تعریف WIN32
			syslog(syslog_ترجیح, "%s", log_سطر);
#وإلا
			sp = (حرف *)log_سطر;
			ReportEvent(syslog_h, syslog_ترجیح, 0, 0, عدم, 1, 0, (ثابت حرف **)&sp, عدم);
#جے_انت
		}
		إذا(log_destinations & MQTT3_LOG_TOPIC && ترجیح != MOSQ_LOG_DEBUG && ترجیح != MOSQ_LOG_اندرونی){
			db_messages_easy_قطار(عدم, topic, 2, (uint32_t)لڑی_لمبائی(log_سطر), log_سطر, 0, 20, عدم);
		}
#جے_تعریف_شدہ WITH_DLT
		إذا(log_destinations & MQTT3_LOG_DLT && ترجیح != MOSQ_LOG_اندرونی){
			DLT_LOG_STRING(dltContext, get_dlt_level(ترجیح), log_سطر);
		}
#جے_انت
#جے_تعریف_شدہ ANDROID
		إذا(log_destinations & MQTT3_LOG_ANDROID && ترجیح != MOSQ_LOG_اندرونی){
			__android_log_write(get_android_level(ترجیح), LOG_ٹیگ, log_سطر);
		}
#جے_انت
	}

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح log__printf(بنية mosquitto *mosq, بنانشانی صحيح ترجیح, ثابت حرف *fmt, ...)
{
	va_list va;
	صحيح rc;

	بن_ورتیا(mosq);

	va_شروع(va, fmt);
	rc = log__vprintf(ترجیح, fmt, va);
	va_سرا(va);

	أرجع rc;
}


فراغ log_اندرونی(ثابت حرف *fmt, ...)
{
	va_list va;
	حرف buf[200];
	صحيح len;

	va_شروع(va, fmt);
	len = vsnprintf(buf, 200, fmt, va);
	va_سرا(va);

	إذا(len >= 200){
		log__printf(عدم, MOSQ_LOG_اندرونی, "Internal log buffer too short (%d)", len);
		أرجع;
	}

#جے_تعریف_شدہ WIN32
	log__printf(عدم, MOSQ_LOG_اندرونی, "%s", buf);
#وإلا
	log__printf(عدم, MOSQ_LOG_اندرونی, "%s%s%s", "\e[32m", buf, "\e[0m");
#جے_انت
}


BROKER_EXPORT صحيح mosquitto_log_vprintf(صحيح level, ثابت حرف *fmt, va_list va)
{
	أرجع log__vprintf((بنانشانی صحيح)level, fmt, va);
}


BROKER_EXPORT فراغ mosquitto_log_printf(صحيح level, ثابت حرف *fmt, ...)
{
	va_list va;

	va_شروع(va, fmt);
	log__vprintf((بنانشانی صحيح)level, fmt, va);
	va_سرا(va);
}


ساكن فراغ libcommon__vprintf(ثابت حرف *fmt, va_list va)
{
	log__vprintf(MOSQ_LOG_INFO, fmt, va);
}
