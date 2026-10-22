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
#که_نه_تعریف_شوی WIN32
#ضمن <syslog.h>
#د_که_پای
#ضمن <time.h>

#إذا defined(__APPLE__)
#  include <sys/time.h>
#د_که_پای

#که_تعریف_شوی WITH_DLT
#ضمن <sys/stat.h>
#ضمن <dlt/dlt.h>
#د_که_پای

#ضمن "logging_mosq.h"
#ضمن "mosquitto_broker_internal.h"
#ضمن "util_mosq.h"

#که_تعریف_شوی WIN32
HANDLE syslog_h;
#د_که_پای

#که_تعریف_شوی ANDROID
#ضمن <android/log.h>
ساكن ثابت حرف *LOG_نښان = "mosquitto";
#د_که_پای

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
ساكن بینښه صحيح log_destinations = MQTT3_LOG_STDERR;
ساكن بینښه صحيح log_priorities = MOSQ_LOG_ERR | MOSQ_LOG_WARNING | MOSQ_LOG_NOTICE | MOSQ_LOG_INFO;

#که_تعریف_شوی WITH_DLT
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
		إذا(S_ISFIFO(statbuf.st_ډېرپېښ)){
			صحيح fd = open("/tmp/dlt", O_NONBLOCK | O_WRONLY);
			إذا(fd != -1){
				dlt_allowed = true;
				close(fd);
			}
		}
	}
}
#د_که_پای


ساكن صحيح get_وخت(بنية tm **ti)
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
#که_نه_تعریف_شوی WIN32
		openlog("mosquitto", LOG_PID|LOG_CONS, config->log_facility);
#وإلا
		syslog_h = OpenEventLog(عدم, "mosquitto");
#د_که_پای
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
#که_نه_تعریف_شوی WIN32
	إذا(log_destinations & MQTT3_LOG_STDOUT){
		setvbuf(stdout, عدم, _IOLBF, 0);
	}
#د_که_پای
#که_تعریف_شوی WITH_DLT
	إذا(log_destinations & MQTT3_LOG_DLT){
		dlt_fifo_check();
		إذا(dlt_allowed){
			DLT_ثبتوونکی_APP("MQTT", "mosquitto log");
			dlt_ثبتوونکی_context(&dltContext, "MQTT", "mosquitto DLT context");
		}
	}
#د_که_پای
	أرجع rc;
}


صحيح log__close(بنية mosquitto__config *config)
{
	إذا(log_destinations & MQTT3_LOG_SYSLOG){
#که_نه_تعریف_شوی WIN32
		closelog();
#وإلا
		CloseEventLog(syslog_h);
#د_که_پای
	}
	إذا(log_destinations & MQTT3_LOG_FILE){
		إذا(config->log_fptr){
			fclose(config->log_fptr);
			config->log_fptr = عدم;
		}
	}

#که_تعریف_شوی WITH_DLT
	إذا(dlt_allowed){
		dlt_unregister_context(&dltContext);
		DLT_UNREGISTER_APP();
	}
#د_که_پای
	/* FIXME - do something for all destinations! */
	أرجع MOSQ_ERR_SUCCESS;
}

#که_تعریف_شوی WITH_DLT


DltLogLevelType get_dlt_level(بینښه صحيح لومړیتوب)
{
	بدل(لومړیتوب){
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
#د_که_پای

#که_تعریف_شوی ANDROID


android_Log_لومړیتوب get_android_level(بینښه صحيح لومړیتوب)
{
	بدل(لومړیتوب){
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
#د_که_پای


ساكن صحيح log__vprintf(بینښه صحيح لومړیتوب, ثابت حرف *fmt, va_list va)
{
	ثابت حرف *topic;
	صحيح syslog_لومړیتوب;
	حرف log_کرښه[1000];
	size_t log_کرښه_pos;
#که_تعریف_شوی WIN32
	حرف *sp;
#د_که_پای
	bool log_timestamp = true;
	حرف *log_timestamp_format = عدم;
	FILE *log_fptr = عدم;

	إذا(db.config){
		log_timestamp = db.config->log_timestamp;
		log_timestamp_format = db.config->log_timestamp_format;
		log_fptr = db.config->log_fptr;
	}

	إذا((log_priorities & لومړیتوب) && log_destinations != MQTT3_LOG_NONE){
		بدل(لومړیتوب){
			حالة MOSQ_LOG_SUBSCRIBE:
				topic = "$SYS/broker/log/M/subscribe";
#که_نه_تعریف_شوی WIN32
				syslog_لومړیتوب = LOG_NOTICE;
#وإلا
				syslog_لومړیتوب = EVENTLOG_INFORMATION_TYPE;
#د_که_پای
				اكسر;
			حالة MOSQ_LOG_UNSUBSCRIBE:
				topic = "$SYS/broker/log/M/unsubscribe";
#که_نه_تعریف_شوی WIN32
				syslog_لومړیتوب = LOG_NOTICE;
#وإلا
				syslog_لومړیتوب = EVENTLOG_INFORMATION_TYPE;
#د_که_پای
				اكسر;
			حالة MOSQ_LOG_DEBUG:
				topic = "$SYS/broker/log/D";
#که_نه_تعریف_شوی WIN32
				syslog_لومړیتوب = LOG_DEBUG;
#وإلا
				syslog_لومړیتوب = EVENTLOG_INFORMATION_TYPE;
#د_که_پای
				اكسر;
			حالة MOSQ_LOG_ERR:
				topic = "$SYS/broker/log/E";
#که_نه_تعریف_شوی WIN32
				syslog_لومړیتوب = LOG_ERR;
#وإلا
				syslog_لومړیتوب = EVENTLOG_ERROR_TYPE;
#د_که_پای
				اكسر;
			حالة MOSQ_LOG_WARNING:
				topic = "$SYS/broker/log/W";
#که_نه_تعریف_شوی WIN32
				syslog_لومړیتوب = LOG_WARNING;
#وإلا
				syslog_لومړیتوب = EVENTLOG_WARNING_TYPE;
#د_که_پای
				اكسر;
			حالة MOSQ_LOG_NOTICE:
				topic = "$SYS/broker/log/N";
#که_نه_تعریف_شوی WIN32
				syslog_لومړیتوب = LOG_NOTICE;
#وإلا
				syslog_لومړیتوب = EVENTLOG_INFORMATION_TYPE;
#د_که_پای
				اكسر;
			حالة MOSQ_LOG_INFO:
				topic = "$SYS/broker/log/I";
#که_نه_تعریف_شوی WIN32
				syslog_لومړیتوب = LOG_INFO;
#وإلا
				syslog_لومړیتوب = EVENTLOG_INFORMATION_TYPE;
#د_که_پای
				اكسر;
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			حالة MOSQ_LOG_WEBSOCKETS:
				topic = "$SYS/broker/log/WS";
#که_نه_تعریف_شوی WIN32
				syslog_لومړیتوب = LOG_DEBUG;
#وإلا
				syslog_لومړیتوب = EVENTLOG_INFORMATION_TYPE;
#د_که_پای
				اكسر;
#د_که_پای
			افتراضي:
				topic = "$SYS/broker/log/E";
#که_نه_تعریف_شوی WIN32
				syslog_لومړیتوب = LOG_ERR;
#وإلا
				syslog_لومړیتوب = EVENTLOG_ERROR_TYPE;
#د_که_پای
		}
		إذا(log_timestamp){
			إذا(log_timestamp_format){
				بنية tm *ti = عدم;
				get_وخت(&ti);
				log_کرښه_pos = strftime(log_کرښه, حجم(log_کرښه), log_timestamp_format, ti);
				إذا(log_کرښه_pos == 0){
					log_کرښه_pos = (size_t)snprintf(log_کرښه, حجم(log_کرښه), "Time error");
				}
			}وإلا{
				log_کرښه_pos = (size_t)snprintf(log_کرښه, حجم(log_کرښه), "%" PRIu64, (uint64_t)db.now_real_s);
			}
			إذا(log_کرښه_pos < حجم(log_کرښه)-3){
				log_کرښه[log_کرښه_pos] = ':';
				log_کرښه[log_کرښه_pos+1] = ' ';
				log_کرښه[log_کرښه_pos+2] = '\0';
				log_کرښه_pos += 2;
			}
		}وإلا{
			log_کرښه_pos = 0;
		}
		vsnprintf(&log_کرښه[log_کرښه_pos], حجم(log_کرښه)-log_کرښه_pos, fmt, va);
		log_کرښه[حجم(log_کرښه)-1] = '\0'; /* Ensure string is null terminated. */

		إذا(log_destinations & MQTT3_LOG_STDOUT){
			fprintf(stdout, "%s\n", log_کرښه);
		}
		إذا(log_destinations & MQTT3_LOG_STDERR){
			fprintf(stderr, "%s\n", log_کرښه);
		}
		إذا(log_destinations & MQTT3_LOG_FILE && log_fptr){
			fprintf(log_fptr, "%s\n", log_کرښه);
#که_تعریف_شوی WIN32
			/* Windows doesn't support line buffering, so flush. */
			fflush(log_fptr);
#د_که_پای
		}
		إذا(log_destinations & MQTT3_LOG_SYSLOG){
#که_نه_تعریف_شوی WIN32
			syslog(syslog_لومړیتوب, "%s", log_کرښه);
#وإلا
			sp = (حرف *)log_کرښه;
			ReportEvent(syslog_h, syslog_لومړیتوب, 0, 0, عدم, 1, 0, (ثابت حرف **)&sp, عدم);
#د_که_پای
		}
		إذا(log_destinations & MQTT3_LOG_TOPIC && لومړیتوب != MOSQ_LOG_DEBUG && لومړیتوب != MOSQ_LOG_دننی){
			db_messages_easy_کتار(عدم, topic, 2, (uint32_t)مزي_اوږدوالی(log_کرښه), log_کرښه, 0, 20, عدم);
		}
#که_تعریف_شوی WITH_DLT
		إذا(log_destinations & MQTT3_LOG_DLT && لومړیتوب != MOSQ_LOG_دننی){
			DLT_LOG_STRING(dltContext, get_dlt_level(لومړیتوب), log_کرښه);
		}
#د_که_پای
#که_تعریف_شوی ANDROID
		إذا(log_destinations & MQTT3_LOG_ANDROID && لومړیتوب != MOSQ_LOG_دننی){
			__android_log_write(get_android_level(لومړیتوب), LOG_نښان, log_کرښه);
		}
#د_که_پای
	}

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح log__printf(بنية mosquitto *mosq, بینښه صحيح لومړیتوب, ثابت حرف *fmt, ...)
{
	va_list va;
	صحيح rc;

	ناکارول_شوی(mosq);

	va_پیل(va, fmt);
	rc = log__vprintf(لومړیتوب, fmt, va);
	va_پای_ټکی(va);

	أرجع rc;
}


فراغ log_دننی(ثابت حرف *fmt, ...)
{
	va_list va;
	حرف buf[200];
	صحيح len;

	va_پیل(va, fmt);
	len = vsnprintf(buf, 200, fmt, va);
	va_پای_ټکی(va);

	إذا(len >= 200){
		log__printf(عدم, MOSQ_LOG_دننی, "Internal log buffer too short (%d)", len);
		أرجع;
	}

#که_تعریف_شوی WIN32
	log__printf(عدم, MOSQ_LOG_دننی, "%s", buf);
#وإلا
	log__printf(عدم, MOSQ_LOG_دننی, "%s%s%s", "\e[32m", buf, "\e[0m");
#د_که_پای
}


BROKER_EXPORT صحيح mosquitto_log_vprintf(صحيح level, ثابت حرف *fmt, va_list va)
{
	أرجع log__vprintf((بینښه صحيح)level, fmt, va);
}


BROKER_EXPORT فراغ mosquitto_log_printf(صحيح level, ثابت حرف *fmt, ...)
{
	va_list va;

	va_پیل(va, fmt);
	log__vprintf((بینښه صحيح)level, fmt, va);
	va_پای_ټکی(va);
}


ساكن فراغ libcommon__vprintf(ثابت حرف *fmt, va_list va)
{
	log__vprintf(MOSQ_LOG_INFO, fmt, va);
}
