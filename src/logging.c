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
#include "config.h"

#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#ifndef WIN32
#include <syslog.h>
#endif
#include <time.h>

#如果 defined(__APPLE__)
#  include <sys/time.h>
#endif

#ifdef WITH_DLT
#include <sys/stat.h>
#include <dlt/dlt.h>
#endif

#include "logging_mosq.h"
#include "mosquitto_broker_internal.h"
#include "util_mosq.h"

#ifdef WIN32
HANDLE syslog_h;
#endif

#ifdef ANDROID
#include <android/log.h>
static const char *LOG_TAG = "mosquitto";
#endif

static char log_fptr_buffer[BUFSIZ];
static void libcommon__vprintf(const char *fmt, va_list va);

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
static unsigned 整数 log_destinations = MQTT3_LOG_STDERR;
static unsigned 整数 log_priorities = MOSQ_LOG_ERR | MOSQ_LOG_WARNING | MOSQ_LOG_NOTICE | MOSQ_LOG_INFO;

#ifdef WITH_DLT
static DltContext dltContext;
static bool dlt_allowed = false;


void dlt_fifo_check(void)
{
	struct stat statbuf;

	/* If we start DLT but the /tmp/dlt fifo doesn't exist, or isn't available
	 * for writing then there is a big delay when we try and close the log
	 * later, so check for it first. This has the side effect of not letting
	 * people using DLT create the fifo after Mosquitto has started, but at the
	 * benefit of not having a massive delay for everybody else. */
	memset(&statbuf, 0, sizeof(statbuf));
	如果(stat("/tmp/dlt", &statbuf) == 0){
		如果(S_ISFIFO(statbuf.st_mode)){
			整数 fd = open("/tmp/dlt", O_NONBLOCK | O_WRONLY);
			如果(fd != -1){
				dlt_allowed = true;
				close(fd);
			}
		}
	}
}
#endif


static 整数 get_time(struct tm **ti)
{
	time_t s;

	s = db.now_real_s;

	*ti = localtime(&s);
	如果(!(*ti)){
		fprintf(stderr, "Error obtaining system time.\n");
		返回 1;
	}

	返回 0;
}


整数 log__init(struct mosquitto__config *config)
{
	整数 rc = 0;

	libcommon_vprintf = libcommon__vprintf;

	log_priorities = config->log_type;
	log_destinations = config->log_dest;

	如果(log_destinations & MQTT3_LOG_SYSLOG){
#ifndef WIN32
		openlog("mosquitto", LOG_PID|LOG_CONS, config->log_facility);
#否则
		syslog_h = OpenEventLog(NULL, "mosquitto");
#endif
	}

	如果(log_destinations & MQTT3_LOG_FILE){
		config->log_fptr = mosquitto_fopen(config->log_file, "at", true);
		如果(config->log_fptr){
			setvbuf(config->log_fptr, log_fptr_buffer, _IOLBF, sizeof(log_fptr_buffer));
		}否则{
			log_destinations = MQTT3_LOG_STDERR;
			log_priorities = MOSQ_LOG_ERR;
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to open log file %s for writing.", config->log_file);
		}
	}
#ifndef WIN32
	如果(log_destinations & MQTT3_LOG_STDOUT){
		setvbuf(stdout, NULL, _IOLBF, 0);
	}
#endif
#ifdef WITH_DLT
	如果(log_destinations & MQTT3_LOG_DLT){
		dlt_fifo_check();
		如果(dlt_allowed){
			DLT_REGISTER_APP("MQTT", "mosquitto log");
			dlt_register_context(&dltContext, "MQTT", "mosquitto DLT context");
		}
	}
#endif
	返回 rc;
}


整数 log__close(struct mosquitto__config *config)
{
	如果(log_destinations & MQTT3_LOG_SYSLOG){
#ifndef WIN32
		closelog();
#否则
		CloseEventLog(syslog_h);
#endif
	}
	如果(log_destinations & MQTT3_LOG_FILE){
		如果(config->log_fptr){
			fclose(config->log_fptr);
			config->log_fptr = NULL;
		}
	}

#ifdef WITH_DLT
	如果(dlt_allowed){
		dlt_unregister_context(&dltContext);
		DLT_UNREGISTER_APP();
	}
#endif
	/* FIXME - do something for all destinations! */
	返回 MOSQ_ERR_SUCCESS;
}

#ifdef WITH_DLT


DltLogLevelType get_dlt_level(unsigned 整数 priority)
{
	switch(priority){
		case MOSQ_LOG_ERR:
			返回 DLT_LOG_ERROR;
		case MOSQ_LOG_WARNING:
			返回 DLT_LOG_WARN;
		case MOSQ_LOG_INFO:
			返回 DLT_LOG_INFO;
		case MOSQ_LOG_DEBUG:
			返回 DLT_LOG_DEBUG;
		case MOSQ_LOG_NOTICE:
		case MOSQ_LOG_SUBSCRIBE:
		case MOSQ_LOG_UNSUBSCRIBE:
			返回 DLT_LOG_VERBOSE;
		default:
			返回 DLT_LOG_DEFAULT;
	}
}
#endif

#ifdef ANDROID


android_LogPriority get_android_level(unsigned 整数 priority)
{
	switch(priority){
		case MOSQ_LOG_ERR:
			返回 ANDROID_LOG_ERROR;
		case MOSQ_LOG_WARNING:
			返回 ANDROID_LOG_WARN;
		case MOSQ_LOG_INFO:
			返回 ANDROID_LOG_INFO;
		case MOSQ_LOG_DEBUG:
			返回 ANDROID_LOG_DEBUG;
		case MOSQ_LOG_NOTICE:
		case MOSQ_LOG_SUBSCRIBE:
		case MOSQ_LOG_UNSUBSCRIBE:
			返回 ANDROID_LOG_VERBOSE;
		default:
			返回 ANDROID_LOG_DEBUG;
	}
}
#endif


static 整数 log__vprintf(unsigned 整数 priority, const char *fmt, va_list va)
{
	const char *topic;
	整数 syslog_priority;
	char log_line[1000];
	size_t log_line_pos;
#ifdef WIN32
	char *sp;
#endif
	bool log_timestamp = true;
	char *log_timestamp_format = NULL;
	FILE *log_fptr = NULL;

	如果(db.config){
		log_timestamp = db.config->log_timestamp;
		log_timestamp_format = db.config->log_timestamp_format;
		log_fptr = db.config->log_fptr;
	}

	如果((log_priorities & priority) && log_destinations != MQTT3_LOG_NONE){
		switch(priority){
			case MOSQ_LOG_SUBSCRIBE:
				topic = "$SYS/broker/log/M/subscribe";
#ifndef WIN32
				syslog_priority = LOG_NOTICE;
#否则
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				break;
			case MOSQ_LOG_UNSUBSCRIBE:
				topic = "$SYS/broker/log/M/unsubscribe";
#ifndef WIN32
				syslog_priority = LOG_NOTICE;
#否则
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				break;
			case MOSQ_LOG_DEBUG:
				topic = "$SYS/broker/log/D";
#ifndef WIN32
				syslog_priority = LOG_DEBUG;
#否则
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				break;
			case MOSQ_LOG_ERR:
				topic = "$SYS/broker/log/E";
#ifndef WIN32
				syslog_priority = LOG_ERR;
#否则
				syslog_priority = EVENTLOG_ERROR_TYPE;
#endif
				break;
			case MOSQ_LOG_WARNING:
				topic = "$SYS/broker/log/W";
#ifndef WIN32
				syslog_priority = LOG_WARNING;
#否则
				syslog_priority = EVENTLOG_WARNING_TYPE;
#endif
				break;
			case MOSQ_LOG_NOTICE:
				topic = "$SYS/broker/log/N";
#ifndef WIN32
				syslog_priority = LOG_NOTICE;
#否则
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				break;
			case MOSQ_LOG_INFO:
				topic = "$SYS/broker/log/I";
#ifndef WIN32
				syslog_priority = LOG_INFO;
#否则
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				break;
#如果 defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			case MOSQ_LOG_WEBSOCKETS:
				topic = "$SYS/broker/log/WS";
#ifndef WIN32
				syslog_priority = LOG_DEBUG;
#否则
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				break;
#endif
			default:
				topic = "$SYS/broker/log/E";
#ifndef WIN32
				syslog_priority = LOG_ERR;
#否则
				syslog_priority = EVENTLOG_ERROR_TYPE;
#endif
		}
		如果(log_timestamp){
			如果(log_timestamp_format){
				struct tm *ti = NULL;
				get_time(&ti);
				log_line_pos = strftime(log_line, sizeof(log_line), log_timestamp_format, ti);
				如果(log_line_pos == 0){
					log_line_pos = (size_t)snprintf(log_line, sizeof(log_line), "Time error");
				}
			}否则{
				log_line_pos = (size_t)snprintf(log_line, sizeof(log_line), "%" PRIu64, (uint64_t)db.now_real_s);
			}
			如果(log_line_pos < sizeof(log_line)-3){
				log_line[log_line_pos] = ':';
				log_line[log_line_pos+1] = ' ';
				log_line[log_line_pos+2] = '\0';
				log_line_pos += 2;
			}
		}否则{
			log_line_pos = 0;
		}
		vsnprintf(&log_line[log_line_pos], sizeof(log_line)-log_line_pos, fmt, va);
		log_line[sizeof(log_line)-1] = '\0'; /* Ensure string is null terminated. */

		如果(log_destinations & MQTT3_LOG_STDOUT){
			fprintf(stdout, "%s\n", log_line);
		}
		如果(log_destinations & MQTT3_LOG_STDERR){
			fprintf(stderr, "%s\n", log_line);
		}
		如果(log_destinations & MQTT3_LOG_FILE && log_fptr){
			fprintf(log_fptr, "%s\n", log_line);
#ifdef WIN32
			/* Windows doesn't support line buffering, so flush. */
			fflush(log_fptr);
#endif
		}
		如果(log_destinations & MQTT3_LOG_SYSLOG){
#ifndef WIN32
			syslog(syslog_priority, "%s", log_line);
#否则
			sp = (char *)log_line;
			ReportEvent(syslog_h, syslog_priority, 0, 0, NULL, 1, 0, (const char **)&sp, NULL);
#endif
		}
		如果(log_destinations & MQTT3_LOG_TOPIC && priority != MOSQ_LOG_DEBUG && priority != MOSQ_LOG_INTERNAL){
			db__messages_easy_queue(NULL, topic, 2, (uint32_t)strlen(log_line), log_line, 0, 20, NULL);
		}
#ifdef WITH_DLT
		如果(log_destinations & MQTT3_LOG_DLT && priority != MOSQ_LOG_INTERNAL){
			DLT_LOG_STRING(dltContext, get_dlt_level(priority), log_line);
		}
#endif
#ifdef ANDROID
		如果(log_destinations & MQTT3_LOG_ANDROID && priority != MOSQ_LOG_INTERNAL){
			__android_log_write(get_android_level(priority), LOG_TAG, log_line);
		}
#endif
	}

	返回 MOSQ_ERR_SUCCESS;
}


整数 log__printf(struct mosquitto *mosq, unsigned 整数 priority, const char *fmt, ...)
{
	va_list va;
	整数 rc;

	UNUSED(mosq);

	va_start(va, fmt);
	rc = log__vprintf(priority, fmt, va);
	va_end(va);

	返回 rc;
}


void log__internal(const char *fmt, ...)
{
	va_list va;
	char buf[200];
	整数 len;

	va_start(va, fmt);
	len = vsnprintf(buf, 200, fmt, va);
	va_end(va);

	如果(len >= 200){
		log__printf(NULL, MOSQ_LOG_INTERNAL, "Internal log buffer too short (%d)", len);
		返回;
	}

#ifdef WIN32
	log__printf(NULL, MOSQ_LOG_INTERNAL, "%s", buf);
#否则
	log__printf(NULL, MOSQ_LOG_INTERNAL, "%s%s%s", "\e[32m", buf, "\e[0m");
#endif
}


BROKER_EXPORT 整数 mosquitto_log_vprintf(整数 level, const char *fmt, va_list va)
{
	返回 log__vprintf((unsigned 整数)level, fmt, va);
}


BROKER_EXPORT void mosquitto_log_printf(整数 level, const char *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	log__vprintf((unsigned 整数)level, fmt, va);
	va_end(va);
}


static void libcommon__vprintf(const char *fmt, va_list va)
{
	log__vprintf(MOSQ_LOG_INFO, fmt, va);
}
