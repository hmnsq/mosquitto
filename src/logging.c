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
#અન્તર્ભાવ "config.h"

#અન્તર્ભાવ <inttypes.h>
#અન્તર્ભાવ <stdarg.h>
#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <inttypes.h>
#ifndef WIN32
#અન્તર્ભાવ <syslog.h>
#endif
#અન્તર્ભાવ <time.h>

#જો defined(__APPLE__)
#  include <sys/time.h>
#endif

#ifdef WITH_DLT
#અન્તર્ભાવ <sys/stat.h>
#અન્તર્ભાવ <dlt/dlt.h>
#endif

#અન્તર્ભાવ "logging_mosq.h"
#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "util_mosq.h"

#ifdef WIN32
HANDLE syslog_h;
#endif

#ifdef ANDROID
#અન્તર્ભાવ <android/log.h>
સ્થિર અચલ અક્ષર *LOG_TAG = "mosquitto";
#endif

સ્થિર અક્ષર log_fptr_buffer[BUFSIZ];
સ્થિર શૂન્ય libcommon__vprintf(અચલ અક્ષર *fmt, va_list va);

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
સ્થિર unsigned પૂર્ણાંક log_destinations = MQTT3_LOG_STDERR;
સ્થિર unsigned પૂર્ણાંક log_priorities = MOSQ_LOG_ERR | MOSQ_LOG_WARNING | MOSQ_LOG_NOTICE | MOSQ_LOG_INFO;

#ifdef WITH_DLT
સ્થિર DltContext dltContext;
સ્થિર bool dlt_allowed = false;


શૂન્ય dlt_fifo_check(શૂન્ય)
{
	સંરચના stat statbuf;

	/* If we start DLT but the /tmp/dlt fifo doesn't exist, or isn't available
	 * for writing then there is a big delay when we try and close the log
	 * later, so check for it first. This has the side effect of not letting
	 * people using DLT create the fifo after Mosquitto has started, but at the
	 * benefit of not having a massive delay for everybody else. */
	memset(&statbuf, 0, કદ(statbuf));
	જો(stat("/tmp/dlt", &statbuf) == 0){
		જો(S_ISFIFO(statbuf.st_mode)){
			પૂર્ણાંક fd = open("/tmp/dlt", O_NONBLOCK | O_WRONLY);
			જો(fd != -1){
				dlt_allowed = true;
				close(fd);
			}
		}
	}
}
#endif


સ્થિર પૂર્ણાંક get_time(સંરચના tm **ti)
{
	time_t s;

	s = db.now_real_s;

	*ti = localtime(&s);
	જો(!(*ti)){
		fprintf(stderr, "Error obtaining system time.\n");
		પાછા 1;
	}

	પાછા 0;
}


પૂર્ણાંક log__init(સંરચના mosquitto__config *config)
{
	પૂર્ણાંક rc = 0;

	libcommon_vprintf = libcommon__vprintf;

	log_priorities = config->log_type;
	log_destinations = config->log_dest;

	જો(log_destinations & MQTT3_LOG_SYSLOG){
#ifndef WIN32
		openlog("mosquitto", LOG_PID|LOG_CONS, config->log_facility);
#નહીંતર
		syslog_h = OpenEventLog(ખાલી, "mosquitto");
#endif
	}

	જો(log_destinations & MQTT3_LOG_FILE){
		config->log_fptr = mosquitto_fopen(config->log_file, "at", true);
		જો(config->log_fptr){
			setvbuf(config->log_fptr, log_fptr_buffer, _IOLBF, કદ(log_fptr_buffer));
		}નહીંતર{
			log_destinations = MQTT3_LOG_STDERR;
			log_priorities = MOSQ_LOG_ERR;
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Unable to open log file %s for writing.", config->log_file);
		}
	}
#ifndef WIN32
	જો(log_destinations & MQTT3_LOG_STDOUT){
		setvbuf(stdout, ખાલી, _IOLBF, 0);
	}
#endif
#ifdef WITH_DLT
	જો(log_destinations & MQTT3_LOG_DLT){
		dlt_fifo_check();
		જો(dlt_allowed){
			DLT_REGISTER_APP("MQTT", "mosquitto log");
			dlt_register_context(&dltContext, "MQTT", "mosquitto DLT context");
		}
	}
#endif
	પાછા rc;
}


પૂર્ણાંક log__close(સંરચના mosquitto__config *config)
{
	જો(log_destinations & MQTT3_LOG_SYSLOG){
#ifndef WIN32
		closelog();
#નહીંતર
		CloseEventLog(syslog_h);
#endif
	}
	જો(log_destinations & MQTT3_LOG_FILE){
		જો(config->log_fptr){
			fclose(config->log_fptr);
			config->log_fptr = ખાલી;
		}
	}

#ifdef WITH_DLT
	જો(dlt_allowed){
		dlt_unregister_context(&dltContext);
		DLT_UNREGISTER_APP();
	}
#endif
	/* FIXME - do something for all destinations! */
	પાછા MOSQ_ERR_SUCCESS;
}

#ifdef WITH_DLT


DltLogLevelType get_dlt_level(unsigned પૂર્ણાંક priority)
{
	પસંદગી(priority){
		સ્થિતિ MOSQ_LOG_ERR:
			પાછા DLT_LOG_ERROR;
		સ્થિતિ MOSQ_LOG_WARNING:
			પાછા DLT_LOG_WARN;
		સ્થિતિ MOSQ_LOG_INFO:
			પાછા DLT_LOG_INFO;
		સ્થિતિ MOSQ_LOG_DEBUG:
			પાછા DLT_LOG_DEBUG;
		સ્થિતિ MOSQ_LOG_NOTICE:
		સ્થિતિ MOSQ_LOG_SUBSCRIBE:
		સ્થિતિ MOSQ_LOG_UNSUBSCRIBE:
			પાછા DLT_LOG_VERBOSE;
		મૂળભૂત:
			પાછા DLT_LOG_DEFAULT;
	}
}
#endif

#ifdef ANDROID


android_LogPriority get_android_level(unsigned પૂર્ણાંક priority)
{
	પસંદગી(priority){
		સ્થિતિ MOSQ_LOG_ERR:
			પાછા ANDROID_LOG_ERROR;
		સ્થિતિ MOSQ_LOG_WARNING:
			પાછા ANDROID_LOG_WARN;
		સ્થિતિ MOSQ_LOG_INFO:
			પાછા ANDROID_LOG_INFO;
		સ્થિતિ MOSQ_LOG_DEBUG:
			પાછા ANDROID_LOG_DEBUG;
		સ્થિતિ MOSQ_LOG_NOTICE:
		સ્થિતિ MOSQ_LOG_SUBSCRIBE:
		સ્થિતિ MOSQ_LOG_UNSUBSCRIBE:
			પાછા ANDROID_LOG_VERBOSE;
		મૂળભૂત:
			પાછા ANDROID_LOG_DEBUG;
	}
}
#endif


સ્થિર પૂર્ણાંક log__vprintf(unsigned પૂર્ણાંક priority, અચલ અક્ષર *fmt, va_list va)
{
	અચલ અક્ષર *topic;
	પૂર્ણાંક syslog_priority;
	અક્ષર log_લીટી[1000];
	size_t log_લીટી_pos;
#ifdef WIN32
	અક્ષર *sp;
#endif
	bool log_timestamp = true;
	અક્ષર *log_timestamp_format = ખાલી;
	FILE *log_fptr = ખાલી;

	જો(db.config){
		log_timestamp = db.config->log_timestamp;
		log_timestamp_format = db.config->log_timestamp_format;
		log_fptr = db.config->log_fptr;
	}

	જો((log_priorities & priority) && log_destinations != MQTT3_LOG_NONE){
		પસંદગી(priority){
			સ્થિતિ MOSQ_LOG_SUBSCRIBE:
				topic = "$SYS/broker/log/M/subscribe";
#ifndef WIN32
				syslog_priority = LOG_NOTICE;
#નહીંતર
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				તોડો;
			સ્થિતિ MOSQ_LOG_UNSUBSCRIBE:
				topic = "$SYS/broker/log/M/unsubscribe";
#ifndef WIN32
				syslog_priority = LOG_NOTICE;
#નહીંતર
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				તોડો;
			સ્થિતિ MOSQ_LOG_DEBUG:
				topic = "$SYS/broker/log/D";
#ifndef WIN32
				syslog_priority = LOG_DEBUG;
#નહીંતર
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				તોડો;
			સ્થિતિ MOSQ_LOG_ERR:
				topic = "$SYS/broker/log/E";
#ifndef WIN32
				syslog_priority = LOG_ERR;
#નહીંતર
				syslog_priority = EVENTLOG_ERROR_TYPE;
#endif
				તોડો;
			સ્થિતિ MOSQ_LOG_WARNING:
				topic = "$SYS/broker/log/W";
#ifndef WIN32
				syslog_priority = LOG_WARNING;
#નહીંતર
				syslog_priority = EVENTLOG_WARNING_TYPE;
#endif
				તોડો;
			સ્થિતિ MOSQ_LOG_NOTICE:
				topic = "$SYS/broker/log/N";
#ifndef WIN32
				syslog_priority = LOG_NOTICE;
#નહીંતર
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				તોડો;
			સ્થિતિ MOSQ_LOG_INFO:
				topic = "$SYS/broker/log/I";
#ifndef WIN32
				syslog_priority = LOG_INFO;
#નહીંતર
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				તોડો;
#જો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			સ્થિતિ MOSQ_LOG_WEBSOCKETS:
				topic = "$SYS/broker/log/WS";
#ifndef WIN32
				syslog_priority = LOG_DEBUG;
#નહીંતર
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				તોડો;
#endif
			મૂળભૂત:
				topic = "$SYS/broker/log/E";
#ifndef WIN32
				syslog_priority = LOG_ERR;
#નહીંતર
				syslog_priority = EVENTLOG_ERROR_TYPE;
#endif
		}
		જો(log_timestamp){
			જો(log_timestamp_format){
				સંરચના tm *ti = ખાલી;
				get_time(&ti);
				log_લીટી_pos = strftime(log_લીટી, કદ(log_લીટી), log_timestamp_format, ti);
				જો(log_લીટી_pos == 0){
					log_લીટી_pos = (size_t)snprintf(log_લીટી, કદ(log_લીટી), "Time error");
				}
			}નહીંતર{
				log_લીટી_pos = (size_t)snprintf(log_લીટી, કદ(log_લીટી), "%" PRIu64, (uint64_t)db.now_real_s);
			}
			જો(log_લીટી_pos < કદ(log_લીટી)-3){
				log_લીટી[log_લીટી_pos] = ':';
				log_લીટી[log_લીટી_pos+1] = ' ';
				log_લીટી[log_લીટી_pos+2] = '\0';
				log_લીટી_pos += 2;
			}
		}નહીંતર{
			log_લીટી_pos = 0;
		}
		vsnprintf(&log_લીટી[log_લીટી_pos], કદ(log_લીટી)-log_લીટી_pos, fmt, va);
		log_લીટી[કદ(log_લીટી)-1] = '\0'; /* Ensure string is null terminated. */

		જો(log_destinations & MQTT3_LOG_STDOUT){
			fprintf(stdout, "%s\n", log_લીટી);
		}
		જો(log_destinations & MQTT3_LOG_STDERR){
			fprintf(stderr, "%s\n", log_લીટી);
		}
		જો(log_destinations & MQTT3_LOG_FILE && log_fptr){
			fprintf(log_fptr, "%s\n", log_લીટી);
#ifdef WIN32
			/* Windows doesn't support line buffering, so flush. */
			fflush(log_fptr);
#endif
		}
		જો(log_destinations & MQTT3_LOG_SYSLOG){
#ifndef WIN32
			syslog(syslog_priority, "%s", log_લીટી);
#નહીંતર
			sp = (અક્ષર *)log_લીટી;
			ReportEvent(syslog_h, syslog_priority, 0, 0, ખાલી, 1, 0, (અચલ અક્ષર **)&sp, ખાલી);
#endif
		}
		જો(log_destinations & MQTT3_LOG_TOPIC && priority != MOSQ_LOG_DEBUG && priority != MOSQ_LOG_INTERNAL){
			db__messages_easy_queue(ખાલી, topic, 2, (uint32_t)strlen(log_લીટી), log_લીટી, 0, 20, ખાલી);
		}
#ifdef WITH_DLT
		જો(log_destinations & MQTT3_LOG_DLT && priority != MOSQ_LOG_INTERNAL){
			DLT_LOG_STRING(dltContext, get_dlt_level(priority), log_લીટી);
		}
#endif
#ifdef ANDROID
		જો(log_destinations & MQTT3_LOG_ANDROID && priority != MOSQ_LOG_INTERNAL){
			__android_log_write(get_android_level(priority), LOG_TAG, log_લીટી);
		}
#endif
	}

	પાછા MOSQ_ERR_SUCCESS;
}


પૂર્ણાંક log__printf(સંરચના mosquitto *mosq, unsigned પૂર્ણાંક priority, અચલ અક્ષર *fmt, ...)
{
	va_list va;
	પૂર્ણાંક rc;

	UNUSED(mosq);

	va_start(va, fmt);
	rc = log__vprintf(priority, fmt, va);
	va_end(va);

	પાછા rc;
}


શૂન્ય log__internal(અચલ અક્ષર *fmt, ...)
{
	va_list va;
	અક્ષર buf[200];
	પૂર્ણાંક len;

	va_start(va, fmt);
	len = vsnprintf(buf, 200, fmt, va);
	va_end(va);

	જો(len >= 200){
		log__printf(ખાલી, MOSQ_LOG_INTERNAL, "Internal log buffer too short (%d)", len);
		પાછા;
	}

#ifdef WIN32
	log__printf(ખાલી, MOSQ_LOG_INTERNAL, "%s", buf);
#નહીંતર
	log__printf(ખાલી, MOSQ_LOG_INTERNAL, "%s%s%s", "\e[32m", buf, "\e[0m");
#endif
}


BROKER_EXPORT પૂર્ણાંક mosquitto_log_vprintf(પૂર્ણાંક level, અચલ અક્ષર *fmt, va_list va)
{
	પાછા log__vprintf((unsigned પૂર્ણાંક)level, fmt, va);
}


BROKER_EXPORT શૂન્ય mosquitto_log_printf(પૂર્ણાંક level, અચલ અક્ષર *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	log__vprintf((unsigned પૂર્ણાંક)level, fmt, va);
	va_end(va);
}


સ્થિર શૂન્ય libcommon__vprintf(અચલ અક્ષર *fmt, va_list va)
{
	log__vprintf(MOSQ_LOG_INFO, fmt, va);
}
