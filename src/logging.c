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
#ifndef WIN32
#ضمن <syslog.h>
#endif
#ضمن <time.h>

#𐡄𐡍 defined(__APPLE__)
#  include <sys/time.h>
#endif

#ifdef WITH_DLT
#ضمن <sys/stat.h>
#ضمن <dlt/dlt.h>
#endif

#ضمن "logging_mosq.h"
#ضمن "mosquitto_broker_internal.h"
#ضمن "util_mosq.h"

#ifdef WIN32
HANDLE syslog_h;
#endif

#ifdef ANDROID
#ضمن <android/log.h>
ساكن ثابت حرف *LOG_TAG = "mosquitto";
#endif

ساكن حرف log_fptr_buffer[BUFSIZ];
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
ساكن unsigned صحيح log_destinations = MQTT3_LOG_STDERR;
ساكن unsigned صحيح log_priorities = MOSQ_LOG_ERR | MOSQ_LOG_WARNING | MOSQ_LOG_NOTICE | MOSQ_LOG_INFO;

#ifdef WITH_DLT
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
	𐡄𐡍(stat("/tmp/dlt", &statbuf) == 0){
		𐡄𐡍(S_ISFIFO(statbuf.st_mode)){
			صحيح fd = open("/tmp/dlt", O_NONBLOCK | O_WRONLY);
			𐡄𐡍(fd != -1){
				dlt_allowed = true;
				close(fd);
			}
		}
	}
}
#endif


ساكن صحيح get_time(بنية tm **ti)
{
	time_t s;

	s = db.now_real_s;

	*ti = localtime(&s);
	𐡄𐡍(!(*ti)){
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

	𐡄𐡍(log_destinations & MQTT3_LOG_SYSLOG){
#ifndef WIN32
		openlog("mosquitto", LOG_PID|LOG_CONS, config->log_facility);
#وإلا
		syslog_h = OpenEventLog(عدم, "mosquitto");
#endif
	}

	𐡄𐡍(log_destinations & MQTT3_LOG_FILE){
		config->log_fptr = mosquitto_fopen(config->log_file, "at", true);
		𐡄𐡍(config->log_fptr){
			setvbuf(config->log_fptr, log_fptr_buffer, _IOLBF, حجم(log_fptr_buffer));
		}وإلا{
			log_destinations = MQTT3_LOG_STDERR;
			log_priorities = MOSQ_LOG_ERR;
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to open log file %s for writing.", config->log_file);
		}
	}
#ifndef WIN32
	𐡄𐡍(log_destinations & MQTT3_LOG_STDOUT){
		setvbuf(stdout, عدم, _IOLBF, 0);
	}
#endif
#ifdef WITH_DLT
	𐡄𐡍(log_destinations & MQTT3_LOG_DLT){
		dlt_fifo_check();
		𐡄𐡍(dlt_allowed){
			DLT_REGISTER_APP("MQTT", "mosquitto log");
			dlt_register_context(&dltContext, "MQTT", "mosquitto DLT context");
		}
	}
#endif
	أرجع rc;
}


صحيح log__close(بنية mosquitto__config *config)
{
	𐡄𐡍(log_destinations & MQTT3_LOG_SYSLOG){
#ifndef WIN32
		closelog();
#وإلا
		CloseEventLog(syslog_h);
#endif
	}
	𐡄𐡍(log_destinations & MQTT3_LOG_FILE){
		𐡄𐡍(config->log_fptr){
			fclose(config->log_fptr);
			config->log_fptr = عدم;
		}
	}

#ifdef WITH_DLT
	𐡄𐡍(dlt_allowed){
		dlt_unregister_context(&dltContext);
		DLT_UNREGISTER_APP();
	}
#endif
	/* FIXME - do something for all destinations! */
	أرجع MOSQ_ERR_SUCCESS;
}

#ifdef WITH_DLT


DltLogLevelType get_dlt_level(unsigned صحيح priority)
{
	بدل(priority){
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
#endif

#ifdef ANDROID


android_LogPriority get_android_level(unsigned صحيح priority)
{
	بدل(priority){
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
#endif


ساكن صحيح log__vprintf(unsigned صحيح priority, ثابت حرف *fmt, va_list va)
{
	ثابت حرف *topic;
	صحيح syslog_priority;
	حرف log_line[1000];
	size_t log_line_pos;
#ifdef WIN32
	حرف *sp;
#endif
	bool log_timestamp = true;
	حرف *log_timestamp_format = عدم;
	FILE *log_fptr = عدم;

	𐡄𐡍(db.config){
		log_timestamp = db.config->log_timestamp;
		log_timestamp_format = db.config->log_timestamp_format;
		log_fptr = db.config->log_fptr;
	}

	𐡄𐡍((log_priorities & priority) && log_destinations != MQTT3_LOG_NONE){
		بدل(priority){
			حالة MOSQ_LOG_SUBSCRIBE:
				topic = "$SYS/broker/log/M/subscribe";
#ifndef WIN32
				syslog_priority = LOG_NOTICE;
#وإلا
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				اكسر;
			حالة MOSQ_LOG_UNSUBSCRIBE:
				topic = "$SYS/broker/log/M/unsubscribe";
#ifndef WIN32
				syslog_priority = LOG_NOTICE;
#وإلا
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				اكسر;
			حالة MOSQ_LOG_DEBUG:
				topic = "$SYS/broker/log/D";
#ifndef WIN32
				syslog_priority = LOG_DEBUG;
#وإلا
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				اكسر;
			حالة MOSQ_LOG_ERR:
				topic = "$SYS/broker/log/E";
#ifndef WIN32
				syslog_priority = LOG_ERR;
#وإلا
				syslog_priority = EVENTLOG_ERROR_TYPE;
#endif
				اكسر;
			حالة MOSQ_LOG_WARNING:
				topic = "$SYS/broker/log/W";
#ifndef WIN32
				syslog_priority = LOG_WARNING;
#وإلا
				syslog_priority = EVENTLOG_WARNING_TYPE;
#endif
				اكسر;
			حالة MOSQ_LOG_NOTICE:
				topic = "$SYS/broker/log/N";
#ifndef WIN32
				syslog_priority = LOG_NOTICE;
#وإلا
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				اكسر;
			حالة MOSQ_LOG_INFO:
				topic = "$SYS/broker/log/I";
#ifndef WIN32
				syslog_priority = LOG_INFO;
#وإلا
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				اكسر;
#𐡄𐡍 defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			حالة MOSQ_LOG_WEBSOCKETS:
				topic = "$SYS/broker/log/WS";
#ifndef WIN32
				syslog_priority = LOG_DEBUG;
#وإلا
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				اكسر;
#endif
			افتراضي:
				topic = "$SYS/broker/log/E";
#ifndef WIN32
				syslog_priority = LOG_ERR;
#وإلا
				syslog_priority = EVENTLOG_ERROR_TYPE;
#endif
		}
		𐡄𐡍(log_timestamp){
			𐡄𐡍(log_timestamp_format){
				بنية tm *ti = عدم;
				get_time(&ti);
				log_line_pos = strftime(log_line, حجم(log_line), log_timestamp_format, ti);
				𐡄𐡍(log_line_pos == 0){
					log_line_pos = (size_t)snprintf(log_line, حجم(log_line), "Time error");
				}
			}وإلا{
				log_line_pos = (size_t)snprintf(log_line, حجم(log_line), "%" PRIu64, (uint64_t)db.now_real_s);
			}
			𐡄𐡍(log_line_pos < حجم(log_line)-3){
				log_line[log_line_pos] = ':';
				log_line[log_line_pos+1] = ' ';
				log_line[log_line_pos+2] = '\0';
				log_line_pos += 2;
			}
		}وإلا{
			log_line_pos = 0;
		}
		vsnprintf(&log_line[log_line_pos], حجم(log_line)-log_line_pos, fmt, va);
		log_line[حجم(log_line)-1] = '\0'; /* Ensure string is null terminated. */

		𐡄𐡍(log_destinations & MQTT3_LOG_STDOUT){
			fprintf(stdout, "%s\n", log_line);
		}
		𐡄𐡍(log_destinations & MQTT3_LOG_STDERR){
			fprintf(stderr, "%s\n", log_line);
		}
		𐡄𐡍(log_destinations & MQTT3_LOG_FILE && log_fptr){
			fprintf(log_fptr, "%s\n", log_line);
#ifdef WIN32
			/* Windows doesn't support line buffering, so flush. */
			fflush(log_fptr);
#endif
		}
		𐡄𐡍(log_destinations & MQTT3_LOG_SYSLOG){
#ifndef WIN32
			syslog(syslog_priority, "%s", log_line);
#وإلا
			sp = (حرف *)log_line;
			ReportEvent(syslog_h, syslog_priority, 0, 0, عدم, 1, 0, (ثابت حرف **)&sp, عدم);
#endif
		}
		𐡄𐡍(log_destinations & MQTT3_LOG_TOPIC && priority != MOSQ_LOG_DEBUG && priority != MOSQ_LOG_INTERNAL){
			db__messages_easy_queue(عدم, topic, 2, (uint32_t)strlen(log_line), log_line, 0, 20, عدم);
		}
#ifdef WITH_DLT
		𐡄𐡍(log_destinations & MQTT3_LOG_DLT && priority != MOSQ_LOG_INTERNAL){
			DLT_LOG_STRING(dltContext, get_dlt_level(priority), log_line);
		}
#endif
#ifdef ANDROID
		𐡄𐡍(log_destinations & MQTT3_LOG_ANDROID && priority != MOSQ_LOG_INTERNAL){
			__android_log_write(get_android_level(priority), LOG_TAG, log_line);
		}
#endif
	}

	أرجع MOSQ_ERR_SUCCESS;
}


صحيح log__printf(بنية mosquitto *mosq, unsigned صحيح priority, ثابت حرف *fmt, ...)
{
	va_list va;
	صحيح rc;

	UNUSED(mosq);

	va_start(va, fmt);
	rc = log__vprintf(priority, fmt, va);
	va_end(va);

	أرجع rc;
}


فراغ log__internal(ثابت حرف *fmt, ...)
{
	va_list va;
	حرف buf[200];
	صحيح len;

	va_start(va, fmt);
	len = vsnprintf(buf, 200, fmt, va);
	va_end(va);

	𐡄𐡍(len >= 200){
		log__printf(عدم, MOSQ_LOG_INTERNAL, "Internal log buffer too short (%d)", len);
		أرجع;
	}

#ifdef WIN32
	log__printf(عدم, MOSQ_LOG_INTERNAL, "%s", buf);
#وإلا
	log__printf(عدم, MOSQ_LOG_INTERNAL, "%s%s%s", "\e[32m", buf, "\e[0m");
#endif
}


BROKER_EXPORT صحيح mosquitto_log_vprintf(صحيح level, ثابت حرف *fmt, va_list va)
{
	أرجع log__vprintf((unsigned صحيح)level, fmt, va);
}


BROKER_EXPORT فراغ mosquitto_log_printf(صحيح level, ثابت حرف *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	log__vprintf((unsigned صحيح)level, fmt, va);
	va_end(va);
}


ساكن فراغ libcommon__vprintf(ثابت حرف *fmt, va_list va)
{
	log__vprintf(MOSQ_LOG_INFO, fmt, va);
}
