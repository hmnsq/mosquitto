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
#অন্তর্ভাব "config.h"

#অন্তর্ভাব <inttypes.h>
#অন্তর্ভাব <stdarg.h>
#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <inttypes.h>
#ifndef WIN32
#অন্তর্ভাব <syslog.h>
#endif
#অন্তর্ভাব <time.h>

#যদি defined(__APPLE__)
#  include <sys/time.h>
#endif

#ifdef WITH_DLT
#অন্তর্ভাব <sys/stat.h>
#অন্তর্ভাব <dlt/dlt.h>
#endif

#অন্তর্ভাব "logging_mosq.h"
#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "util_mosq.h"

#ifdef WIN32
HANDLE syslog_h;
#endif

#ifdef ANDROID
#অন্তর্ভাব <android/log.h>
স্থির ধ্রুব বর্ণ *LOG_TAG = "mosquitto";
#endif

স্থির বর্ণ log_fptr_buffer[BUFSIZ];
স্থির শূন্য libcommon__vprintf(ধ্রুব বর্ণ *fmt, va_list va);

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
স্থির unsigned পূর্ণাঙ্ক log_destinations = MQTT3_LOG_STDERR;
স্থির unsigned পূর্ণাঙ্ক log_priorities = MOSQ_LOG_ERR | MOSQ_LOG_WARNING | MOSQ_LOG_NOTICE | MOSQ_LOG_INFO;

#ifdef WITH_DLT
স্থির DltContext dltContext;
স্থির bool dlt_allowed = false;


শূন্য dlt_fifo_check(শূন্য)
{
	সংরচনা stat statbuf;

	/* If we start DLT but the /tmp/dlt fifo doesn't exist, or isn't available
	 * for writing then there is a big delay when we try and close the log
	 * later, so check for it first. This has the side effect of not letting
	 * people using DLT create the fifo after Mosquitto has started, but at the
	 * benefit of not having a massive delay for everybody else. */
	memset(&statbuf, 0, পরিমাণ(statbuf));
	যদি(stat("/tmp/dlt", &statbuf) == 0){
		যদি(S_ISFIFO(statbuf.st_mode)){
			পূর্ণাঙ্ক fd = open("/tmp/dlt", O_NONBLOCK | O_WRONLY);
			যদি(fd != -1){
				dlt_allowed = true;
				close(fd);
			}
		}
	}
}
#endif


স্থির পূর্ণাঙ্ক get_time(সংরচনা tm **ti)
{
	time_t s;

	s = db.now_real_s;

	*ti = localtime(&s);
	যদি(!(*ti)){
		fprintf(stderr, "Error obtaining system time.\n");
		প্রত্যাগম 1;
	}

	প্রত্যাগম 0;
}


পূর্ণাঙ্ক log__init(সংরচনা mosquitto__config *config)
{
	পূর্ণাঙ্ক rc = 0;

	libcommon_vprintf = libcommon__vprintf;

	log_priorities = config->log_type;
	log_destinations = config->log_dest;

	যদি(log_destinations & MQTT3_LOG_SYSLOG){
#ifndef WIN32
		openlog("mosquitto", LOG_PID|LOG_CONS, config->log_facility);
#অন্যথা
		syslog_h = OpenEventLog(রিক্ত, "mosquitto");
#endif
	}

	যদি(log_destinations & MQTT3_LOG_FILE){
		config->log_fptr = mosquitto_fopen(config->log_file, "at", true);
		যদি(config->log_fptr){
			setvbuf(config->log_fptr, log_fptr_buffer, _IOLBF, পরিমাণ(log_fptr_buffer));
		}অন্যথা{
			log_destinations = MQTT3_LOG_STDERR;
			log_priorities = MOSQ_LOG_ERR;
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Unable to open log file %s for writing.", config->log_file);
		}
	}
#ifndef WIN32
	যদি(log_destinations & MQTT3_LOG_STDOUT){
		setvbuf(stdout, রিক্ত, _IOLBF, 0);
	}
#endif
#ifdef WITH_DLT
	যদি(log_destinations & MQTT3_LOG_DLT){
		dlt_fifo_check();
		যদি(dlt_allowed){
			DLT_REGISTER_APP("MQTT", "mosquitto log");
			dlt_register_context(&dltContext, "MQTT", "mosquitto DLT context");
		}
	}
#endif
	প্রত্যাগম rc;
}


পূর্ণাঙ্ক log__close(সংরচনা mosquitto__config *config)
{
	যদি(log_destinations & MQTT3_LOG_SYSLOG){
#ifndef WIN32
		closelog();
#অন্যথা
		CloseEventLog(syslog_h);
#endif
	}
	যদি(log_destinations & MQTT3_LOG_FILE){
		যদি(config->log_fptr){
			fclose(config->log_fptr);
			config->log_fptr = রিক্ত;
		}
	}

#ifdef WITH_DLT
	যদি(dlt_allowed){
		dlt_unregister_context(&dltContext);
		DLT_UNREGISTER_APP();
	}
#endif
	/* FIXME - do something for all destinations! */
	প্রত্যাগম MOSQ_ERR_SUCCESS;
}

#ifdef WITH_DLT


DltLogLevelType get_dlt_level(unsigned পূর্ণাঙ্ক priority)
{
	বিকল্প(priority){
		পক্ষ MOSQ_LOG_ERR:
			প্রত্যাগম DLT_LOG_ERROR;
		পক্ষ MOSQ_LOG_WARNING:
			প্রত্যাগম DLT_LOG_WARN;
		পক্ষ MOSQ_LOG_INFO:
			প্রত্যাগম DLT_LOG_INFO;
		পক্ষ MOSQ_LOG_DEBUG:
			প্রত্যাগম DLT_LOG_DEBUG;
		পক্ষ MOSQ_LOG_NOTICE:
		পক্ষ MOSQ_LOG_SUBSCRIBE:
		পক্ষ MOSQ_LOG_UNSUBSCRIBE:
			প্রত্যাগম DLT_LOG_VERBOSE;
		স্বভাব:
			প্রত্যাগম DLT_LOG_DEFAULT;
	}
}
#endif

#ifdef ANDROID


android_LogPriority get_android_level(unsigned পূর্ণাঙ্ক priority)
{
	বিকল্প(priority){
		পক্ষ MOSQ_LOG_ERR:
			প্রত্যাগম ANDROID_LOG_ERROR;
		পক্ষ MOSQ_LOG_WARNING:
			প্রত্যাগম ANDROID_LOG_WARN;
		পক্ষ MOSQ_LOG_INFO:
			প্রত্যাগম ANDROID_LOG_INFO;
		পক্ষ MOSQ_LOG_DEBUG:
			প্রত্যাগম ANDROID_LOG_DEBUG;
		পক্ষ MOSQ_LOG_NOTICE:
		পক্ষ MOSQ_LOG_SUBSCRIBE:
		পক্ষ MOSQ_LOG_UNSUBSCRIBE:
			প্রত্যাগম ANDROID_LOG_VERBOSE;
		স্বভাব:
			প্রত্যাগম ANDROID_LOG_DEBUG;
	}
}
#endif


স্থির পূর্ণাঙ্ক log__vprintf(unsigned পূর্ণাঙ্ক priority, ধ্রুব বর্ণ *fmt, va_list va)
{
	ধ্রুব বর্ণ *topic;
	পূর্ণাঙ্ক syslog_priority;
	বর্ণ log_সারি[1000];
	size_t log_সারি_pos;
#ifdef WIN32
	বর্ণ *sp;
#endif
	bool log_timestamp = true;
	বর্ণ *log_timestamp_format = রিক্ত;
	FILE *log_fptr = রিক্ত;

	যদি(db.config){
		log_timestamp = db.config->log_timestamp;
		log_timestamp_format = db.config->log_timestamp_format;
		log_fptr = db.config->log_fptr;
	}

	যদি((log_priorities & priority) && log_destinations != MQTT3_LOG_NONE){
		বিকল্প(priority){
			পক্ষ MOSQ_LOG_SUBSCRIBE:
				topic = "$SYS/broker/log/M/subscribe";
#ifndef WIN32
				syslog_priority = LOG_NOTICE;
#অন্যথা
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				ভঙ্গ;
			পক্ষ MOSQ_LOG_UNSUBSCRIBE:
				topic = "$SYS/broker/log/M/unsubscribe";
#ifndef WIN32
				syslog_priority = LOG_NOTICE;
#অন্যথা
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				ভঙ্গ;
			পক্ষ MOSQ_LOG_DEBUG:
				topic = "$SYS/broker/log/D";
#ifndef WIN32
				syslog_priority = LOG_DEBUG;
#অন্যথা
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				ভঙ্গ;
			পক্ষ MOSQ_LOG_ERR:
				topic = "$SYS/broker/log/E";
#ifndef WIN32
				syslog_priority = LOG_ERR;
#অন্যথা
				syslog_priority = EVENTLOG_ERROR_TYPE;
#endif
				ভঙ্গ;
			পক্ষ MOSQ_LOG_WARNING:
				topic = "$SYS/broker/log/W";
#ifndef WIN32
				syslog_priority = LOG_WARNING;
#অন্যথা
				syslog_priority = EVENTLOG_WARNING_TYPE;
#endif
				ভঙ্গ;
			পক্ষ MOSQ_LOG_NOTICE:
				topic = "$SYS/broker/log/N";
#ifndef WIN32
				syslog_priority = LOG_NOTICE;
#অন্যথা
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				ভঙ্গ;
			পক্ষ MOSQ_LOG_INFO:
				topic = "$SYS/broker/log/I";
#ifndef WIN32
				syslog_priority = LOG_INFO;
#অন্যথা
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				ভঙ্গ;
#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			পক্ষ MOSQ_LOG_WEBSOCKETS:
				topic = "$SYS/broker/log/WS";
#ifndef WIN32
				syslog_priority = LOG_DEBUG;
#অন্যথা
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				ভঙ্গ;
#endif
			স্বভাব:
				topic = "$SYS/broker/log/E";
#ifndef WIN32
				syslog_priority = LOG_ERR;
#অন্যথা
				syslog_priority = EVENTLOG_ERROR_TYPE;
#endif
		}
		যদি(log_timestamp){
			যদি(log_timestamp_format){
				সংরচনা tm *ti = রিক্ত;
				get_time(&ti);
				log_সারি_pos = strftime(log_সারি, পরিমাণ(log_সারি), log_timestamp_format, ti);
				যদি(log_সারি_pos == 0){
					log_সারি_pos = (size_t)snprintf(log_সারি, পরিমাণ(log_সারি), "Time error");
				}
			}অন্যথা{
				log_সারি_pos = (size_t)snprintf(log_সারি, পরিমাণ(log_সারি), "%" PRIu64, (uint64_t)db.now_real_s);
			}
			যদি(log_সারি_pos < পরিমাণ(log_সারি)-3){
				log_সারি[log_সারি_pos] = ':';
				log_সারি[log_সারি_pos+1] = ' ';
				log_সারি[log_সারি_pos+2] = '\0';
				log_সারি_pos += 2;
			}
		}অন্যথা{
			log_সারি_pos = 0;
		}
		vsnprintf(&log_সারি[log_সারি_pos], পরিমাণ(log_সারি)-log_সারি_pos, fmt, va);
		log_সারি[পরিমাণ(log_সারি)-1] = '\0'; /* Ensure string is null terminated. */

		যদি(log_destinations & MQTT3_LOG_STDOUT){
			fprintf(stdout, "%s\n", log_সারি);
		}
		যদি(log_destinations & MQTT3_LOG_STDERR){
			fprintf(stderr, "%s\n", log_সারি);
		}
		যদি(log_destinations & MQTT3_LOG_FILE && log_fptr){
			fprintf(log_fptr, "%s\n", log_সারি);
#ifdef WIN32
			/* Windows doesn't support line buffering, so flush. */
			fflush(log_fptr);
#endif
		}
		যদি(log_destinations & MQTT3_LOG_SYSLOG){
#ifndef WIN32
			syslog(syslog_priority, "%s", log_সারি);
#অন্যথা
			sp = (বর্ণ *)log_সারি;
			ReportEvent(syslog_h, syslog_priority, 0, 0, রিক্ত, 1, 0, (ধ্রুব বর্ণ **)&sp, রিক্ত);
#endif
		}
		যদি(log_destinations & MQTT3_LOG_TOPIC && priority != MOSQ_LOG_DEBUG && priority != MOSQ_LOG_INTERNAL){
			db__messages_easy_queue(রিক্ত, topic, 2, (uint32_t)strlen(log_সারি), log_সারি, 0, 20, রিক্ত);
		}
#ifdef WITH_DLT
		যদি(log_destinations & MQTT3_LOG_DLT && priority != MOSQ_LOG_INTERNAL){
			DLT_LOG_STRING(dltContext, get_dlt_level(priority), log_সারি);
		}
#endif
#ifdef ANDROID
		যদি(log_destinations & MQTT3_LOG_ANDROID && priority != MOSQ_LOG_INTERNAL){
			__android_log_write(get_android_level(priority), LOG_TAG, log_সারি);
		}
#endif
	}

	প্রত্যাগম MOSQ_ERR_SUCCESS;
}


পূর্ণাঙ্ক log__printf(সংরচনা mosquitto *mosq, unsigned পূর্ণাঙ্ক priority, ধ্রুব বর্ণ *fmt, ...)
{
	va_list va;
	পূর্ণাঙ্ক rc;

	UNUSED(mosq);

	va_start(va, fmt);
	rc = log__vprintf(priority, fmt, va);
	va_end(va);

	প্রত্যাগম rc;
}


শূন্য log__internal(ধ্রুব বর্ণ *fmt, ...)
{
	va_list va;
	বর্ণ buf[200];
	পূর্ণাঙ্ক len;

	va_start(va, fmt);
	len = vsnprintf(buf, 200, fmt, va);
	va_end(va);

	যদি(len >= 200){
		log__printf(রিক্ত, MOSQ_LOG_INTERNAL, "Internal log buffer too short (%d)", len);
		প্রত্যাগম;
	}

#ifdef WIN32
	log__printf(রিক্ত, MOSQ_LOG_INTERNAL, "%s", buf);
#অন্যথা
	log__printf(রিক্ত, MOSQ_LOG_INTERNAL, "%s%s%s", "\e[32m", buf, "\e[0m");
#endif
}


BROKER_EXPORT পূর্ণাঙ্ক mosquitto_log_vprintf(পূর্ণাঙ্ক level, ধ্রুব বর্ণ *fmt, va_list va)
{
	প্রত্যাগম log__vprintf((unsigned পূর্ণাঙ্ক)level, fmt, va);
}


BROKER_EXPORT শূন্য mosquitto_log_printf(পূর্ণাঙ্ক level, ধ্রুব বর্ণ *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	log__vprintf((unsigned পূর্ণাঙ্ক)level, fmt, va);
	va_end(va);
}


স্থির শূন্য libcommon__vprintf(ধ্রুব বর্ণ *fmt, va_list va)
{
	log__vprintf(MOSQ_LOG_INFO, fmt, va);
}
