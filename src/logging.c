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

#အကယ်၍ defined(__APPLE__)
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
static const var_na *LOG_TAG = "mosquitto";
#endif

static var_na log_fptr_buffer[BUFSIZ];
static void libcommon__vprintf(const var_na *fmt, va_list va);

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
static unsigned ကိန်းပြည့် log_destinations = MQTT3_LOG_STDERR;
static unsigned ကိန်းပြည့် log_priorities = MOSQ_LOG_ERR | MOSQ_LOG_WARNING | MOSQ_LOG_NOTICE | MOSQ_LOG_INFO;

#ifdef WITH_DLT
static DltContext dltContext;
static bool dlt_allowed = false;


void dlt_fifo_check(void)
{
	sa_nracanaa stat statbuf;

	/* If we start DLT but the /tmp/dlt fifo doesn't exist, or isn't available
	 * for writing then there is a big delay when we try and close the log
	 * later, so check for it first. This has the side effect of not letting
	 * people using DLT create the fifo after Mosquitto has started, but at the
	 * benefit of not having a massive delay for everybody else. */
	memset(&statbuf, 0, parimaa_na(statbuf));
	အကယ်၍(stat("/tmp/dlt", &statbuf) == 0){
		အကယ်၍(S_ISFIFO(statbuf.st_mode)){
			ကိန်းပြည့် fd = open("/tmp/dlt", O_NONBLOCK | O_WRONLY);
			အကယ်၍(fd != -1){
				dlt_allowed = true;
				close(fd);
			}
		}
	}
}
#endif


static ကိန်းပြည့် get_time(sa_nracanaa tm **ti)
{
	time_t s;

	s = db.now_real_s;

	*ti = localtime(&s);
	အကယ်၍(!(*ti)){
		fprintf(stderr, "Error obtaining system time.\n");
		ပြန်ပေး 1;
	}

	ပြန်ပေး 0;
}


ကိန်းပြည့် log__init(sa_nracanaa mosquitto__config *config)
{
	ကိန်းပြည့် rc = 0;

	libcommon_vprintf = libcommon__vprintf;

	log_priorities = config->log_type;
	log_destinations = config->log_dest;

	အကယ်၍(log_destinations & MQTT3_LOG_SYSLOG){
#ifndef WIN32
		openlog("mosquitto", LOG_PID|LOG_CONS, config->log_facility);
#မဟုတ်ပါက
		syslog_h = OpenEventLog(NULL, "mosquitto");
#endif
	}

	အကယ်၍(log_destinations & MQTT3_LOG_FILE){
		config->log_fptr = mosquitto_fopen(config->log_file, "at", true);
		အကယ်၍(config->log_fptr){
			setvbuf(config->log_fptr, log_fptr_buffer, _IOLBF, parimaa_na(log_fptr_buffer));
		}မဟုတ်ပါက{
			log_destinations = MQTT3_LOG_STDERR;
			log_priorities = MOSQ_LOG_ERR;
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to open log file %s for writing.", config->log_file);
		}
	}
#ifndef WIN32
	အကယ်၍(log_destinations & MQTT3_LOG_STDOUT){
		setvbuf(stdout, NULL, _IOLBF, 0);
	}
#endif
#ifdef WITH_DLT
	အကယ်၍(log_destinations & MQTT3_LOG_DLT){
		dlt_fifo_check();
		အကယ်၍(dlt_allowed){
			DLT_REGISTER_APP("MQTT", "mosquitto log");
			dlt_register_context(&dltContext, "MQTT", "mosquitto DLT context");
		}
	}
#endif
	ပြန်ပေး rc;
}


ကိန်းပြည့် log__close(sa_nracanaa mosquitto__config *config)
{
	အကယ်၍(log_destinations & MQTT3_LOG_SYSLOG){
#ifndef WIN32
		closelog();
#မဟုတ်ပါက
		CloseEventLog(syslog_h);
#endif
	}
	အကယ်၍(log_destinations & MQTT3_LOG_FILE){
		အကယ်၍(config->log_fptr){
			fclose(config->log_fptr);
			config->log_fptr = NULL;
		}
	}

#ifdef WITH_DLT
	အကယ်၍(dlt_allowed){
		dlt_unregister_context(&dltContext);
		DLT_UNREGISTER_APP();
	}
#endif
	/* FIXME - do something for all destinations! */
	ပြန်ပေး MOSQ_ERR_SUCCESS;
}

#ifdef WITH_DLT


DltLogLevelType get_dlt_level(unsigned ကိန်းပြည့် priority)
{
	switch(priority){
		pak_sa MOSQ_LOG_ERR:
			ပြန်ပေး DLT_LOG_ERROR;
		pak_sa MOSQ_LOG_WARNING:
			ပြန်ပေး DLT_LOG_WARN;
		pak_sa MOSQ_LOG_INFO:
			ပြန်ပေး DLT_LOG_INFO;
		pak_sa MOSQ_LOG_DEBUG:
			ပြန်ပေး DLT_LOG_DEBUG;
		pak_sa MOSQ_LOG_NOTICE:
		pak_sa MOSQ_LOG_SUBSCRIBE:
		pak_sa MOSQ_LOG_UNSUBSCRIBE:
			ပြန်ပေး DLT_LOG_VERBOSE;
		default:
			ပြန်ပေး DLT_LOG_DEFAULT;
	}
}
#endif

#ifdef ANDROID


android_LogPriority get_android_level(unsigned ကိန်းပြည့် priority)
{
	switch(priority){
		pak_sa MOSQ_LOG_ERR:
			ပြန်ပေး ANDROID_LOG_ERROR;
		pak_sa MOSQ_LOG_WARNING:
			ပြန်ပေး ANDROID_LOG_WARN;
		pak_sa MOSQ_LOG_INFO:
			ပြန်ပေး ANDROID_LOG_INFO;
		pak_sa MOSQ_LOG_DEBUG:
			ပြန်ပေး ANDROID_LOG_DEBUG;
		pak_sa MOSQ_LOG_NOTICE:
		pak_sa MOSQ_LOG_SUBSCRIBE:
		pak_sa MOSQ_LOG_UNSUBSCRIBE:
			ပြန်ပေး ANDROID_LOG_VERBOSE;
		default:
			ပြန်ပေး ANDROID_LOG_DEBUG;
	}
}
#endif


static ကိန်းပြည့် log__vprintf(unsigned ကိန်းပြည့် priority, const var_na *fmt, va_list va)
{
	const var_na *topic;
	ကိန်းပြည့် syslog_priority;
	var_na log_line[1000];
	size_t log_line_pos;
#ifdef WIN32
	var_na *sp;
#endif
	bool log_timestamp = true;
	var_na *log_timestamp_format = NULL;
	FILE *log_fptr = NULL;

	အကယ်၍(db.config){
		log_timestamp = db.config->log_timestamp;
		log_timestamp_format = db.config->log_timestamp_format;
		log_fptr = db.config->log_fptr;
	}

	အကယ်၍((log_priorities & priority) && log_destinations != MQTT3_LOG_NONE){
		switch(priority){
			pak_sa MOSQ_LOG_SUBSCRIBE:
				topic = "$SYS/broker/log/M/subscribe";
#ifndef WIN32
				syslog_priority = LOG_NOTICE;
#မဟုတ်ပါက
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				bha_nga;
			pak_sa MOSQ_LOG_UNSUBSCRIBE:
				topic = "$SYS/broker/log/M/unsubscribe";
#ifndef WIN32
				syslog_priority = LOG_NOTICE;
#မဟုတ်ပါက
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				bha_nga;
			pak_sa MOSQ_LOG_DEBUG:
				topic = "$SYS/broker/log/D";
#ifndef WIN32
				syslog_priority = LOG_DEBUG;
#မဟုတ်ပါက
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				bha_nga;
			pak_sa MOSQ_LOG_ERR:
				topic = "$SYS/broker/log/E";
#ifndef WIN32
				syslog_priority = LOG_ERR;
#မဟုတ်ပါက
				syslog_priority = EVENTLOG_ERROR_TYPE;
#endif
				bha_nga;
			pak_sa MOSQ_LOG_WARNING:
				topic = "$SYS/broker/log/W";
#ifndef WIN32
				syslog_priority = LOG_WARNING;
#မဟုတ်ပါက
				syslog_priority = EVENTLOG_WARNING_TYPE;
#endif
				bha_nga;
			pak_sa MOSQ_LOG_NOTICE:
				topic = "$SYS/broker/log/N";
#ifndef WIN32
				syslog_priority = LOG_NOTICE;
#မဟုတ်ပါက
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				bha_nga;
			pak_sa MOSQ_LOG_INFO:
				topic = "$SYS/broker/log/I";
#ifndef WIN32
				syslog_priority = LOG_INFO;
#မဟုတ်ပါက
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				bha_nga;
#အကယ်၍ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			pak_sa MOSQ_LOG_WEBSOCKETS:
				topic = "$SYS/broker/log/WS";
#ifndef WIN32
				syslog_priority = LOG_DEBUG;
#မဟုတ်ပါက
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				bha_nga;
#endif
			default:
				topic = "$SYS/broker/log/E";
#ifndef WIN32
				syslog_priority = LOG_ERR;
#မဟုတ်ပါက
				syslog_priority = EVENTLOG_ERROR_TYPE;
#endif
		}
		အကယ်၍(log_timestamp){
			အကယ်၍(log_timestamp_format){
				sa_nracanaa tm *ti = NULL;
				get_time(&ti);
				log_line_pos = strftime(log_line, parimaa_na(log_line), log_timestamp_format, ti);
				အကယ်၍(log_line_pos == 0){
					log_line_pos = (size_t)snprintf(log_line, parimaa_na(log_line), "Time error");
				}
			}မဟုတ်ပါက{
				log_line_pos = (size_t)snprintf(log_line, parimaa_na(log_line), "%" PRIu64, (uint64_t)db.now_real_s);
			}
			အကယ်၍(log_line_pos < parimaa_na(log_line)-3){
				log_line[log_line_pos] = ':';
				log_line[log_line_pos+1] = ' ';
				log_line[log_line_pos+2] = '\0';
				log_line_pos += 2;
			}
		}မဟုတ်ပါက{
			log_line_pos = 0;
		}
		vsnprintf(&log_line[log_line_pos], parimaa_na(log_line)-log_line_pos, fmt, va);
		log_line[parimaa_na(log_line)-1] = '\0'; /* Ensure string is null terminated. */

		အကယ်၍(log_destinations & MQTT3_LOG_STDOUT){
			fprintf(stdout, "%s\n", log_line);
		}
		အကယ်၍(log_destinations & MQTT3_LOG_STDERR){
			fprintf(stderr, "%s\n", log_line);
		}
		အကယ်၍(log_destinations & MQTT3_LOG_FILE && log_fptr){
			fprintf(log_fptr, "%s\n", log_line);
#ifdef WIN32
			/* Windows doesn't support line buffering, so flush. */
			fflush(log_fptr);
#endif
		}
		အကယ်၍(log_destinations & MQTT3_LOG_SYSLOG){
#ifndef WIN32
			syslog(syslog_priority, "%s", log_line);
#မဟုတ်ပါက
			sp = (var_na *)log_line;
			ReportEvent(syslog_h, syslog_priority, 0, 0, NULL, 1, 0, (const var_na **)&sp, NULL);
#endif
		}
		အကယ်၍(log_destinations & MQTT3_LOG_TOPIC && priority != MOSQ_LOG_DEBUG && priority != MOSQ_LOG_INTERNAL){
			db__messages_easy_queue(NULL, topic, 2, (uint32_t)strlen(log_line), log_line, 0, 20, NULL);
		}
#ifdef WITH_DLT
		အကယ်၍(log_destinations & MQTT3_LOG_DLT && priority != MOSQ_LOG_INTERNAL){
			DLT_LOG_STRING(dltContext, get_dlt_level(priority), log_line);
		}
#endif
#ifdef ANDROID
		အကယ်၍(log_destinations & MQTT3_LOG_ANDROID && priority != MOSQ_LOG_INTERNAL){
			__android_log_write(get_android_level(priority), LOG_TAG, log_line);
		}
#endif
	}

	ပြန်ပေး MOSQ_ERR_SUCCESS;
}


ကိန်းပြည့် log__printf(sa_nracanaa mosquitto *mosq, unsigned ကိန်းပြည့် priority, const var_na *fmt, ...)
{
	va_list va;
	ကိန်းပြည့် rc;

	UNUSED(mosq);

	va_start(va, fmt);
	rc = log__vprintf(priority, fmt, va);
	va_end(va);

	ပြန်ပေး rc;
}


void log__internal(const var_na *fmt, ...)
{
	va_list va;
	var_na buf[200];
	ကိန်းပြည့် len;

	va_start(va, fmt);
	len = vsnprintf(buf, 200, fmt, va);
	va_end(va);

	အကယ်၍(len >= 200){
		log__printf(NULL, MOSQ_LOG_INTERNAL, "Internal log buffer too short (%d)", len);
		ပြန်ပေး;
	}

#ifdef WIN32
	log__printf(NULL, MOSQ_LOG_INTERNAL, "%s", buf);
#မဟုတ်ပါက
	log__printf(NULL, MOSQ_LOG_INTERNAL, "%s%s%s", "\e[32m", buf, "\e[0m");
#endif
}


BROKER_EXPORT ကိန်းပြည့် mosquitto_log_vprintf(ကိန်းပြည့် level, const var_na *fmt, va_list va)
{
	ပြန်ပေး log__vprintf((unsigned ကိန်းပြည့်)level, fmt, va);
}


BROKER_EXPORT void mosquitto_log_printf(ကိန်းပြည့် level, const var_na *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	log__vprintf((unsigned ကိန်းပြည့်)level, fmt, va);
	va_end(va);
}


static void libcommon__vprintf(const var_na *fmt, va_list va)
{
	log__vprintf(MOSQ_LOG_INFO, fmt, va);
}
