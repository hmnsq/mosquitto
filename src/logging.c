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

#ако defined(__APPLE__)
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
static unsigned цео log_destinations = MQTT3_LOG_STDERR;
static unsigned цео log_priorities = MOSQ_LOG_ERR | MOSQ_LOG_WARNING | MOSQ_LOG_NOTICE | MOSQ_LOG_INFO;

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
	ако(stat("/tmp/dlt", &statbuf) == 0){
		ако(S_ISFIFO(statbuf.st_mode)){
			цео fd = open("/tmp/dlt", O_NONBLOCK | O_WRONLY);
			ако(fd != -1){
				dlt_allowed = true;
				close(fd);
			}
		}
	}
}
#endif


static цео get_time(struct tm **ti)
{
	time_t s;

	s = db.now_real_s;

	*ti = localtime(&s);
	ако(!(*ti)){
		fprintf(stderr, "Error obtaining system time.\n");
		врати 1;
	}

	врати 0;
}


цео log__init(struct mosquitto__config *config)
{
	цео rc = 0;

	libcommon_vprintf = libcommon__vprintf;

	log_priorities = config->log_type;
	log_destinations = config->log_dest;

	ако(log_destinations & MQTT3_LOG_SYSLOG){
#ifndef WIN32
		openlog("mosquitto", LOG_PID|LOG_CONS, config->log_facility);
#инако
		syslog_h = OpenEventLog(NULL, "mosquitto");
#endif
	}

	ако(log_destinations & MQTT3_LOG_FILE){
		config->log_fptr = mosquitto_fopen(config->log_file, "at", true);
		ако(config->log_fptr){
			setvbuf(config->log_fptr, log_fptr_buffer, _IOLBF, sizeof(log_fptr_buffer));
		}инако{
			log_destinations = MQTT3_LOG_STDERR;
			log_priorities = MOSQ_LOG_ERR;
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to open log file %s for writing.", config->log_file);
		}
	}
#ifndef WIN32
	ако(log_destinations & MQTT3_LOG_STDOUT){
		setvbuf(stdout, NULL, _IOLBF, 0);
	}
#endif
#ifdef WITH_DLT
	ако(log_destinations & MQTT3_LOG_DLT){
		dlt_fifo_check();
		ако(dlt_allowed){
			DLT_REGISTER_APP("MQTT", "mosquitto log");
			dlt_register_context(&dltContext, "MQTT", "mosquitto DLT context");
		}
	}
#endif
	врати rc;
}


цео log__close(struct mosquitto__config *config)
{
	ако(log_destinations & MQTT3_LOG_SYSLOG){
#ifndef WIN32
		closelog();
#инако
		CloseEventLog(syslog_h);
#endif
	}
	ако(log_destinations & MQTT3_LOG_FILE){
		ако(config->log_fptr){
			fclose(config->log_fptr);
			config->log_fptr = NULL;
		}
	}

#ifdef WITH_DLT
	ако(dlt_allowed){
		dlt_unregister_context(&dltContext);
		DLT_UNREGISTER_APP();
	}
#endif
	/* FIXME - do something for all destinations! */
	врати MOSQ_ERR_SUCCESS;
}

#ifdef WITH_DLT


DltLogLevelType get_dlt_level(unsigned цео priority)
{
	switch(priority){
		case MOSQ_LOG_ERR:
			врати DLT_LOG_ERROR;
		case MOSQ_LOG_WARNING:
			врати DLT_LOG_WARN;
		case MOSQ_LOG_INFO:
			врати DLT_LOG_INFO;
		case MOSQ_LOG_DEBUG:
			врати DLT_LOG_DEBUG;
		case MOSQ_LOG_NOTICE:
		case MOSQ_LOG_SUBSCRIBE:
		case MOSQ_LOG_UNSUBSCRIBE:
			врати DLT_LOG_VERBOSE;
		default:
			врати DLT_LOG_DEFAULT;
	}
}
#endif

#ifdef ANDROID


android_LogPriority get_android_level(unsigned цео priority)
{
	switch(priority){
		case MOSQ_LOG_ERR:
			врати ANDROID_LOG_ERROR;
		case MOSQ_LOG_WARNING:
			врати ANDROID_LOG_WARN;
		case MOSQ_LOG_INFO:
			врати ANDROID_LOG_INFO;
		case MOSQ_LOG_DEBUG:
			врати ANDROID_LOG_DEBUG;
		case MOSQ_LOG_NOTICE:
		case MOSQ_LOG_SUBSCRIBE:
		case MOSQ_LOG_UNSUBSCRIBE:
			врати ANDROID_LOG_VERBOSE;
		default:
			врати ANDROID_LOG_DEBUG;
	}
}
#endif


static цео log__vprintf(unsigned цео priority, const char *fmt, va_list va)
{
	const char *topic;
	цео syslog_priority;
	char log_line[1000];
	size_t log_line_pos;
#ifdef WIN32
	char *sp;
#endif
	bool log_timestamp = true;
	char *log_timestamp_format = NULL;
	FILE *log_fptr = NULL;

	ако(db.config){
		log_timestamp = db.config->log_timestamp;
		log_timestamp_format = db.config->log_timestamp_format;
		log_fptr = db.config->log_fptr;
	}

	ако((log_priorities & priority) && log_destinations != MQTT3_LOG_NONE){
		switch(priority){
			case MOSQ_LOG_SUBSCRIBE:
				topic = "$SYS/broker/log/M/subscribe";
#ifndef WIN32
				syslog_priority = LOG_NOTICE;
#инако
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				break;
			case MOSQ_LOG_UNSUBSCRIBE:
				topic = "$SYS/broker/log/M/unsubscribe";
#ifndef WIN32
				syslog_priority = LOG_NOTICE;
#инако
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				break;
			case MOSQ_LOG_DEBUG:
				topic = "$SYS/broker/log/D";
#ifndef WIN32
				syslog_priority = LOG_DEBUG;
#инако
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				break;
			case MOSQ_LOG_ERR:
				topic = "$SYS/broker/log/E";
#ifndef WIN32
				syslog_priority = LOG_ERR;
#инако
				syslog_priority = EVENTLOG_ERROR_TYPE;
#endif
				break;
			case MOSQ_LOG_WARNING:
				topic = "$SYS/broker/log/W";
#ifndef WIN32
				syslog_priority = LOG_WARNING;
#инако
				syslog_priority = EVENTLOG_WARNING_TYPE;
#endif
				break;
			case MOSQ_LOG_NOTICE:
				topic = "$SYS/broker/log/N";
#ifndef WIN32
				syslog_priority = LOG_NOTICE;
#инако
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				break;
			case MOSQ_LOG_INFO:
				topic = "$SYS/broker/log/I";
#ifndef WIN32
				syslog_priority = LOG_INFO;
#инако
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				break;
#ако defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			case MOSQ_LOG_WEBSOCKETS:
				topic = "$SYS/broker/log/WS";
#ifndef WIN32
				syslog_priority = LOG_DEBUG;
#инако
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				break;
#endif
			default:
				topic = "$SYS/broker/log/E";
#ifndef WIN32
				syslog_priority = LOG_ERR;
#инако
				syslog_priority = EVENTLOG_ERROR_TYPE;
#endif
		}
		ако(log_timestamp){
			ако(log_timestamp_format){
				struct tm *ti = NULL;
				get_time(&ti);
				log_line_pos = strftime(log_line, sizeof(log_line), log_timestamp_format, ti);
				ако(log_line_pos == 0){
					log_line_pos = (size_t)snprintf(log_line, sizeof(log_line), "Time error");
				}
			}инако{
				log_line_pos = (size_t)snprintf(log_line, sizeof(log_line), "%" PRIu64, (uint64_t)db.now_real_s);
			}
			ако(log_line_pos < sizeof(log_line)-3){
				log_line[log_line_pos] = ':';
				log_line[log_line_pos+1] = ' ';
				log_line[log_line_pos+2] = '\0';
				log_line_pos += 2;
			}
		}инако{
			log_line_pos = 0;
		}
		vsnprintf(&log_line[log_line_pos], sizeof(log_line)-log_line_pos, fmt, va);
		log_line[sizeof(log_line)-1] = '\0'; /* Ensure string is null terminated. */

		ако(log_destinations & MQTT3_LOG_STDOUT){
			fprintf(stdout, "%s\n", log_line);
		}
		ако(log_destinations & MQTT3_LOG_STDERR){
			fprintf(stderr, "%s\n", log_line);
		}
		ако(log_destinations & MQTT3_LOG_FILE && log_fptr){
			fprintf(log_fptr, "%s\n", log_line);
#ifdef WIN32
			/* Windows doesn't support line buffering, so flush. */
			fflush(log_fptr);
#endif
		}
		ако(log_destinations & MQTT3_LOG_SYSLOG){
#ifndef WIN32
			syslog(syslog_priority, "%s", log_line);
#инако
			sp = (char *)log_line;
			ReportEvent(syslog_h, syslog_priority, 0, 0, NULL, 1, 0, (const char **)&sp, NULL);
#endif
		}
		ако(log_destinations & MQTT3_LOG_TOPIC && priority != MOSQ_LOG_DEBUG && priority != MOSQ_LOG_INTERNAL){
			db__messages_easy_queue(NULL, topic, 2, (uint32_t)strlen(log_line), log_line, 0, 20, NULL);
		}
#ifdef WITH_DLT
		ако(log_destinations & MQTT3_LOG_DLT && priority != MOSQ_LOG_INTERNAL){
			DLT_LOG_STRING(dltContext, get_dlt_level(priority), log_line);
		}
#endif
#ifdef ANDROID
		ако(log_destinations & MQTT3_LOG_ANDROID && priority != MOSQ_LOG_INTERNAL){
			__android_log_write(get_android_level(priority), LOG_TAG, log_line);
		}
#endif
	}

	врати MOSQ_ERR_SUCCESS;
}


цео log__printf(struct mosquitto *mosq, unsigned цео priority, const char *fmt, ...)
{
	va_list va;
	цео rc;

	UNUSED(mosq);

	va_start(va, fmt);
	rc = log__vprintf(priority, fmt, va);
	va_end(va);

	врати rc;
}


void log__internal(const char *fmt, ...)
{
	va_list va;
	char buf[200];
	цео len;

	va_start(va, fmt);
	len = vsnprintf(buf, 200, fmt, va);
	va_end(va);

	ако(len >= 200){
		log__printf(NULL, MOSQ_LOG_INTERNAL, "Internal log buffer too short (%d)", len);
		врати;
	}

#ifdef WIN32
	log__printf(NULL, MOSQ_LOG_INTERNAL, "%s", buf);
#инако
	log__printf(NULL, MOSQ_LOG_INTERNAL, "%s%s%s", "\e[32m", buf, "\e[0m");
#endif
}


BROKER_EXPORT цео mosquitto_log_vprintf(цео level, const char *fmt, va_list va)
{
	врати log__vprintf((unsigned цео)level, fmt, va);
}


BROKER_EXPORT void mosquitto_log_printf(цео level, const char *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	log__vprintf((unsigned цео)level, fmt, va);
	va_end(va);
}


static void libcommon__vprintf(const char *fmt, va_list va)
{
	log__vprintf(MOSQ_LOG_INFO, fmt, va);
}
