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
#ଅନ୍ତର୍ଭାଵ "config.h"

#ଅନ୍ତର୍ଭାଵ <inttypes.h>
#ଅନ୍ତର୍ଭାଵ <stdarg.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <inttypes.h>
#ifndef WIN32
#ଅନ୍ତର୍ଭାଵ <syslog.h>
#endif
#ଅନ୍ତର୍ଭାଵ <time.h>

#ଯଦି defined(__APPLE__)
#  include <sys/time.h>
#endif

#ifdef WITH_DLT
#ଅନ୍ତର୍ଭାଵ <sys/stat.h>
#ଅନ୍ତର୍ଭାଵ <dlt/dlt.h>
#endif

#ଅନ୍ତର୍ଭାଵ "logging_mosq.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "util_mosq.h"

#ifdef WIN32
HANDLE syslog_h;
#endif

#ifdef ANDROID
#ଅନ୍ତର୍ଭାଵ <android/log.h>
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *LOG_TAG = "mosquitto";
#endif

ସ୍ଥିର ଅକ୍ଷର log_fptr_buffer[BUFSIZ];
ସ୍ଥିର ଶୂନ୍ୟ libcommon__vprintf(ଅଚଳ ଅକ୍ଷର *fmt, va_list va);

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
ସ୍ଥିର unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_destinations = MQTT3_LOG_STDERR;
ସ୍ଥିର unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_priorities = MOSQ_LOG_ERR | MOSQ_LOG_WARNING | MOSQ_LOG_NOTICE | MOSQ_LOG_INFO;

#ifdef WITH_DLT
ସ୍ଥିର DltContext dltContext;
ସ୍ଥିର bool dlt_allowed = false;


ଶୂନ୍ୟ dlt_fifo_check(ଶୂନ୍ୟ)
{
	ସଂରଚନା stat statbuf;

	/* If we start DLT but the /tmp/dlt fifo doesn't exist, or isn't available
	 * for writing then there is a big delay when we try and close the log
	 * later, so check for it first. This has the side effect of not letting
	 * people using DLT create the fifo after Mosquitto has started, but at the
	 * benefit of not having a massive delay for everybody else. */
	memset(&statbuf, 0, ଆକାର(statbuf));
	ଯଦି(stat("/tmp/dlt", &statbuf) == 0){
		ଯଦି(S_ISFIFO(statbuf.st_mode)){
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = open("/tmp/dlt", O_NONBLOCK | O_WRONLY);
			ଯଦି(fd != -1){
				dlt_allowed = true;
				close(fd);
			}
		}
	}
}
#endif


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_time(ସଂରଚନା tm **ti)
{
	time_t s;

	s = db.now_real_s;

	*ti = localtime(&s);
	ଯଦି(!(*ti)){
		fprintf(stderr, "Error obtaining system time.\n");
		ଫେରନ୍ତୁ 1;
	}

	ଫେରନ୍ତୁ 0;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା log__init(ସଂରଚନା mosquitto__config *config)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = 0;

	libcommon_vprintf = libcommon__vprintf;

	log_priorities = config->log_type;
	log_destinations = config->log_dest;

	ଯଦି(log_destinations & MQTT3_LOG_SYSLOG){
#ifndef WIN32
		openlog("mosquitto", LOG_PID|LOG_CONS, config->log_facility);
#ଅନ୍ୟଥା
		syslog_h = OpenEventLog(ଖାଲି, "mosquitto");
#endif
	}

	ଯଦି(log_destinations & MQTT3_LOG_FILE){
		config->log_fptr = mosquitto_fopen(config->log_file, "at", true);
		ଯଦି(config->log_fptr){
			setvbuf(config->log_fptr, log_fptr_buffer, _IOLBF, ଆକାର(log_fptr_buffer));
		}ଅନ୍ୟଥା{
			log_destinations = MQTT3_LOG_STDERR;
			log_priorities = MOSQ_LOG_ERR;
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Unable to open log file %s for writing.", config->log_file);
		}
	}
#ifndef WIN32
	ଯଦି(log_destinations & MQTT3_LOG_STDOUT){
		setvbuf(stdout, ଖାଲି, _IOLBF, 0);
	}
#endif
#ifdef WITH_DLT
	ଯଦି(log_destinations & MQTT3_LOG_DLT){
		dlt_fifo_check();
		ଯଦି(dlt_allowed){
			DLT_REGISTER_APP("MQTT", "mosquitto log");
			dlt_register_context(&dltContext, "MQTT", "mosquitto DLT context");
		}
	}
#endif
	ଫେରନ୍ତୁ rc;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା log__close(ସଂରଚନା mosquitto__config *config)
{
	ଯଦି(log_destinations & MQTT3_LOG_SYSLOG){
#ifndef WIN32
		closelog();
#ଅନ୍ୟଥା
		CloseEventLog(syslog_h);
#endif
	}
	ଯଦି(log_destinations & MQTT3_LOG_FILE){
		ଯଦି(config->log_fptr){
			fclose(config->log_fptr);
			config->log_fptr = ଖାଲି;
		}
	}

#ifdef WITH_DLT
	ଯଦି(dlt_allowed){
		dlt_unregister_context(&dltContext);
		DLT_UNREGISTER_APP();
	}
#endif
	/* FIXME - do something for all destinations! */
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}

#ifdef WITH_DLT


DltLogLevelType get_dlt_level(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା priority)
{
	ବାଛ(priority){
		ସ୍ଥିତି MOSQ_LOG_ERR:
			ଫେରନ୍ତୁ DLT_LOG_ERROR;
		ସ୍ଥିତି MOSQ_LOG_WARNING:
			ଫେରନ୍ତୁ DLT_LOG_WARN;
		ସ୍ଥିତି MOSQ_LOG_INFO:
			ଫେରନ୍ତୁ DLT_LOG_INFO;
		ସ୍ଥିତି MOSQ_LOG_DEBUG:
			ଫେରନ୍ତୁ DLT_LOG_DEBUG;
		ସ୍ଥିତି MOSQ_LOG_NOTICE:
		ସ୍ଥିତି MOSQ_LOG_SUBSCRIBE:
		ସ୍ଥିତି MOSQ_LOG_UNSUBSCRIBE:
			ଫେରନ୍ତୁ DLT_LOG_VERBOSE;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			ଫେରନ୍ତୁ DLT_LOG_DEFAULT;
	}
}
#endif

#ifdef ANDROID


android_LogPriority get_android_level(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା priority)
{
	ବାଛ(priority){
		ସ୍ଥିତି MOSQ_LOG_ERR:
			ଫେରନ୍ତୁ ANDROID_LOG_ERROR;
		ସ୍ଥିତି MOSQ_LOG_WARNING:
			ଫେରନ୍ତୁ ANDROID_LOG_WARN;
		ସ୍ଥିତି MOSQ_LOG_INFO:
			ଫେରନ୍ତୁ ANDROID_LOG_INFO;
		ସ୍ଥିତି MOSQ_LOG_DEBUG:
			ଫେରନ୍ତୁ ANDROID_LOG_DEBUG;
		ସ୍ଥିତି MOSQ_LOG_NOTICE:
		ସ୍ଥିତି MOSQ_LOG_SUBSCRIBE:
		ସ୍ଥିତି MOSQ_LOG_UNSUBSCRIBE:
			ଫେରନ୍ତୁ ANDROID_LOG_VERBOSE;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			ଫେରନ୍ତୁ ANDROID_LOG_DEBUG;
	}
}
#endif


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା log__vprintf(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା priority, ଅଚଳ ଅକ୍ଷର *fmt, va_list va)
{
	ଅଚଳ ଅକ୍ଷର *topic;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା syslog_priority;
	ଅକ୍ଷର log_ଧାଡ଼ି[1000];
	size_t log_ଧାଡ଼ି_pos;
#ifdef WIN32
	ଅକ୍ଷର *sp;
#endif
	bool log_timestamp = true;
	ଅକ୍ଷର *log_timestamp_format = ଖାଲି;
	FILE *log_fptr = ଖାଲି;

	ଯଦି(db.config){
		log_timestamp = db.config->log_timestamp;
		log_timestamp_format = db.config->log_timestamp_format;
		log_fptr = db.config->log_fptr;
	}

	ଯଦି((log_priorities & priority) && log_destinations != MQTT3_LOG_NONE){
		ବାଛ(priority){
			ସ୍ଥିତି MOSQ_LOG_SUBSCRIBE:
				topic = "$SYS/broker/log/M/subscribe";
#ifndef WIN32
				syslog_priority = LOG_NOTICE;
#ଅନ୍ୟଥା
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				ଭାଙ୍ଗ;
			ସ୍ଥିତି MOSQ_LOG_UNSUBSCRIBE:
				topic = "$SYS/broker/log/M/unsubscribe";
#ifndef WIN32
				syslog_priority = LOG_NOTICE;
#ଅନ୍ୟଥା
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				ଭାଙ୍ଗ;
			ସ୍ଥିତି MOSQ_LOG_DEBUG:
				topic = "$SYS/broker/log/D";
#ifndef WIN32
				syslog_priority = LOG_DEBUG;
#ଅନ୍ୟଥା
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				ଭାଙ୍ଗ;
			ସ୍ଥିତି MOSQ_LOG_ERR:
				topic = "$SYS/broker/log/E";
#ifndef WIN32
				syslog_priority = LOG_ERR;
#ଅନ୍ୟଥା
				syslog_priority = EVENTLOG_ERROR_TYPE;
#endif
				ଭାଙ୍ଗ;
			ସ୍ଥିତି MOSQ_LOG_WARNING:
				topic = "$SYS/broker/log/W";
#ifndef WIN32
				syslog_priority = LOG_WARNING;
#ଅନ୍ୟଥା
				syslog_priority = EVENTLOG_WARNING_TYPE;
#endif
				ଭାଙ୍ଗ;
			ସ୍ଥିତି MOSQ_LOG_NOTICE:
				topic = "$SYS/broker/log/N";
#ifndef WIN32
				syslog_priority = LOG_NOTICE;
#ଅନ୍ୟଥା
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				ଭାଙ୍ଗ;
			ସ୍ଥିତି MOSQ_LOG_INFO:
				topic = "$SYS/broker/log/I";
#ifndef WIN32
				syslog_priority = LOG_INFO;
#ଅନ୍ୟଥା
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				ଭାଙ୍ଗ;
#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			ସ୍ଥିତି MOSQ_LOG_WEBSOCKETS:
				topic = "$SYS/broker/log/WS";
#ifndef WIN32
				syslog_priority = LOG_DEBUG;
#ଅନ୍ୟଥା
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				ଭାଙ୍ଗ;
#endif
			ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
				topic = "$SYS/broker/log/E";
#ifndef WIN32
				syslog_priority = LOG_ERR;
#ଅନ୍ୟଥା
				syslog_priority = EVENTLOG_ERROR_TYPE;
#endif
		}
		ଯଦି(log_timestamp){
			ଯଦି(log_timestamp_format){
				ସଂରଚନା tm *ti = ଖାଲି;
				get_time(&ti);
				log_ଧାଡ଼ି_pos = strftime(log_ଧାଡ଼ି, ଆକାର(log_ଧାଡ଼ି), log_timestamp_format, ti);
				ଯଦି(log_ଧାଡ଼ି_pos == 0){
					log_ଧାଡ଼ି_pos = (size_t)snprintf(log_ଧାଡ଼ି, ଆକାର(log_ଧାଡ଼ି), "Time error");
				}
			}ଅନ୍ୟଥା{
				log_ଧାଡ଼ି_pos = (size_t)snprintf(log_ଧାଡ଼ି, ଆକାର(log_ଧାଡ଼ି), "%" PRIu64, (uint64_t)db.now_real_s);
			}
			ଯଦି(log_ଧାଡ଼ି_pos < ଆକାର(log_ଧାଡ଼ି)-3){
				log_ଧାଡ଼ି[log_ଧାଡ଼ି_pos] = ':';
				log_ଧାଡ଼ି[log_ଧାଡ଼ି_pos+1] = ' ';
				log_ଧାଡ଼ି[log_ଧାଡ଼ି_pos+2] = '\0';
				log_ଧାଡ଼ି_pos += 2;
			}
		}ଅନ୍ୟଥା{
			log_ଧାଡ଼ି_pos = 0;
		}
		vsnprintf(&log_ଧାଡ଼ି[log_ଧାଡ଼ି_pos], ଆକାର(log_ଧାଡ଼ି)-log_ଧାଡ଼ି_pos, fmt, va);
		log_ଧାଡ଼ି[ଆକାର(log_ଧାଡ଼ି)-1] = '\0'; /* Ensure string is null terminated. */

		ଯଦି(log_destinations & MQTT3_LOG_STDOUT){
			fprintf(stdout, "%s\n", log_ଧାଡ଼ି);
		}
		ଯଦି(log_destinations & MQTT3_LOG_STDERR){
			fprintf(stderr, "%s\n", log_ଧାଡ଼ି);
		}
		ଯଦି(log_destinations & MQTT3_LOG_FILE && log_fptr){
			fprintf(log_fptr, "%s\n", log_ଧାଡ଼ି);
#ifdef WIN32
			/* Windows doesn't support line buffering, so flush. */
			fflush(log_fptr);
#endif
		}
		ଯଦି(log_destinations & MQTT3_LOG_SYSLOG){
#ifndef WIN32
			syslog(syslog_priority, "%s", log_ଧାଡ଼ି);
#ଅନ୍ୟଥା
			sp = (ଅକ୍ଷର *)log_ଧାଡ଼ି;
			ReportEvent(syslog_h, syslog_priority, 0, 0, ଖାଲି, 1, 0, (ଅଚଳ ଅକ୍ଷର **)&sp, ଖାଲି);
#endif
		}
		ଯଦି(log_destinations & MQTT3_LOG_TOPIC && priority != MOSQ_LOG_DEBUG && priority != MOSQ_LOG_INTERNAL){
			db__messages_easy_queue(ଖାଲି, topic, 2, (uint32_t)strlen(log_ଧାଡ଼ି), log_ଧାଡ଼ି, 0, 20, ଖାଲି);
		}
#ifdef WITH_DLT
		ଯଦି(log_destinations & MQTT3_LOG_DLT && priority != MOSQ_LOG_INTERNAL){
			DLT_LOG_STRING(dltContext, get_dlt_level(priority), log_ଧାଡ଼ି);
		}
#endif
#ifdef ANDROID
		ଯଦି(log_destinations & MQTT3_LOG_ANDROID && priority != MOSQ_LOG_INTERNAL){
			__android_log_write(get_android_level(priority), LOG_TAG, log_ଧାଡ଼ି);
		}
#endif
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା log__printf(ସଂରଚନା mosquitto *mosq, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା priority, ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
	va_list va;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	UNUSED(mosq);

	va_start(va, fmt);
	rc = log__vprintf(priority, fmt, va);
	va_end(va);

	ଫେରନ୍ତୁ rc;
}


ଶୂନ୍ୟ log__internal(ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
	va_list va;
	ଅକ୍ଷର buf[200];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;

	va_start(va, fmt);
	len = vsnprintf(buf, 200, fmt, va);
	va_end(va);

	ଯଦି(len >= 200){
		log__printf(ଖାଲି, MOSQ_LOG_INTERNAL, "Internal log buffer too short (%d)", len);
		ଫେରନ୍ତୁ;
	}

#ifdef WIN32
	log__printf(ଖାଲି, MOSQ_LOG_INTERNAL, "%s", buf);
#ଅନ୍ୟଥା
	log__printf(ଖାଲି, MOSQ_LOG_INTERNAL, "%s%s%s", "\e[32m", buf, "\e[0m");
#endif
}


BROKER_EXPORT ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_log_vprintf(ପୂର୍ଣ୍ଣସଂଖ୍ୟା level, ଅଚଳ ଅକ୍ଷର *fmt, va_list va)
{
	ଫେରନ୍ତୁ log__vprintf((unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)level, fmt, va);
}


BROKER_EXPORT ଶୂନ୍ୟ mosquitto_log_printf(ପୂର୍ଣ୍ଣସଂଖ୍ୟା level, ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	log__vprintf((unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)level, fmt, va);
	va_end(va);
}


ସ୍ଥିର ଶୂନ୍ୟ libcommon__vprintf(ଅଚଳ ଅକ୍ଷର *fmt, va_list va)
{
	log__vprintf(MOSQ_LOG_INFO, fmt, va);
}
