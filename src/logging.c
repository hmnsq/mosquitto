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
#അന്തര്ഭാവ "config.h"

#അന്തര്ഭാവ <inttypes.h>
#അന്തര്ഭാവ <stdarg.h>
#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <inttypes.h>
#ifndef WIN32
#അന്തര്ഭാവ <syslog.h>
#endif
#അന്തര്ഭാവ <time.h>

#എങ്കിൽ defined(__APPLE__)
#  include <sys/time.h>
#endif

#ifdef WITH_DLT
#അന്തര്ഭാവ <sys/stat.h>
#അന്തര്ഭാവ <dlt/dlt.h>
#endif

#അന്തര്ഭാവ "logging_mosq.h"
#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "util_mosq.h"

#ifdef WIN32
HANDLE syslog_h;
#endif

#ifdef ANDROID
#അന്തര്ഭാവ <android/log.h>
സ്ഥിതം സ്ഥിരം അക്ഷരം *LOG_TAG = "mosquitto";
#endif

സ്ഥിതം അക്ഷരം log_fptr_buffer[BUFSIZ];
സ്ഥിതം ശൂന്യം libcommon__vprintf(സ്ഥിരം അക്ഷരം *fmt, va_list va);

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
സ്ഥിതം unsigned പൂർണ്ണസംഖ്യ log_destinations = MQTT3_LOG_STDERR;
സ്ഥിതം unsigned പൂർണ്ണസംഖ്യ log_priorities = MOSQ_LOG_ERR | MOSQ_LOG_WARNING | MOSQ_LOG_NOTICE | MOSQ_LOG_INFO;

#ifdef WITH_DLT
സ്ഥിതം DltContext dltContext;
സ്ഥിതം bool dlt_allowed = false;


ശൂന്യം dlt_fifo_check(ശൂന്യം)
{
	ഘടന stat statbuf;

	/* If we start DLT but the /tmp/dlt fifo doesn't exist, or isn't available
	 * for writing then there is a big delay when we try and close the log
	 * later, so check for it first. This has the side effect of not letting
	 * people using DLT create the fifo after Mosquitto has started, but at the
	 * benefit of not having a massive delay for everybody else. */
	memset(&statbuf, 0, വലുപ്പം(statbuf));
	എങ്കിൽ(stat("/tmp/dlt", &statbuf) == 0){
		എങ്കിൽ(S_ISFIFO(statbuf.st_mode)){
			പൂർണ്ണസംഖ്യ fd = open("/tmp/dlt", O_NONBLOCK | O_WRONLY);
			എങ്കിൽ(fd != -1){
				dlt_allowed = true;
				close(fd);
			}
		}
	}
}
#endif


സ്ഥിതം പൂർണ്ണസംഖ്യ get_time(ഘടന tm **ti)
{
	time_t s;

	s = db.now_real_s;

	*ti = localtime(&s);
	എങ്കിൽ(!(*ti)){
		fprintf(stderr, "Error obtaining system time.\n");
		തിരികെ 1;
	}

	തിരികെ 0;
}


പൂർണ്ണസംഖ്യ log__init(ഘടന mosquitto__config *config)
{
	പൂർണ്ണസംഖ്യ rc = 0;

	libcommon_vprintf = libcommon__vprintf;

	log_priorities = config->log_type;
	log_destinations = config->log_dest;

	എങ്കിൽ(log_destinations & MQTT3_LOG_SYSLOG){
#ifndef WIN32
		openlog("mosquitto", LOG_PID|LOG_CONS, config->log_facility);
#അല്ലെങ്കിൽ
		syslog_h = OpenEventLog(ശൂന്യ, "mosquitto");
#endif
	}

	എങ്കിൽ(log_destinations & MQTT3_LOG_FILE){
		config->log_fptr = mosquitto_fopen(config->log_file, "at", true);
		എങ്കിൽ(config->log_fptr){
			setvbuf(config->log_fptr, log_fptr_buffer, _IOLBF, വലുപ്പം(log_fptr_buffer));
		}അല്ലെങ്കിൽ{
			log_destinations = MQTT3_LOG_STDERR;
			log_priorities = MOSQ_LOG_ERR;
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Unable to open log file %s for writing.", config->log_file);
		}
	}
#ifndef WIN32
	എങ്കിൽ(log_destinations & MQTT3_LOG_STDOUT){
		setvbuf(stdout, ശൂന്യ, _IOLBF, 0);
	}
#endif
#ifdef WITH_DLT
	എങ്കിൽ(log_destinations & MQTT3_LOG_DLT){
		dlt_fifo_check();
		എങ്കിൽ(dlt_allowed){
			DLT_REGISTER_APP("MQTT", "mosquitto log");
			dlt_register_context(&dltContext, "MQTT", "mosquitto DLT context");
		}
	}
#endif
	തിരികെ rc;
}


പൂർണ്ണസംഖ്യ log__close(ഘടന mosquitto__config *config)
{
	എങ്കിൽ(log_destinations & MQTT3_LOG_SYSLOG){
#ifndef WIN32
		closelog();
#അല്ലെങ്കിൽ
		CloseEventLog(syslog_h);
#endif
	}
	എങ്കിൽ(log_destinations & MQTT3_LOG_FILE){
		എങ്കിൽ(config->log_fptr){
			fclose(config->log_fptr);
			config->log_fptr = ശൂന്യ;
		}
	}

#ifdef WITH_DLT
	എങ്കിൽ(dlt_allowed){
		dlt_unregister_context(&dltContext);
		DLT_UNREGISTER_APP();
	}
#endif
	/* FIXME - do something for all destinations! */
	തിരികെ MOSQ_ERR_SUCCESS;
}

#ifdef WITH_DLT


DltLogLevelType get_dlt_level(unsigned പൂർണ്ണസംഖ്യ priority)
{
	തിരഞ്ഞെടുപ്പ്(priority){
		അവസ്ഥ MOSQ_LOG_ERR:
			തിരികെ DLT_LOG_ERROR;
		അവസ്ഥ MOSQ_LOG_WARNING:
			തിരികെ DLT_LOG_WARN;
		അവസ്ഥ MOSQ_LOG_INFO:
			തിരികെ DLT_LOG_INFO;
		അവസ്ഥ MOSQ_LOG_DEBUG:
			തിരികെ DLT_LOG_DEBUG;
		അവസ്ഥ MOSQ_LOG_NOTICE:
		അവസ്ഥ MOSQ_LOG_SUBSCRIBE:
		അവസ്ഥ MOSQ_LOG_UNSUBSCRIBE:
			തിരികെ DLT_LOG_VERBOSE;
		സ്ഥിരസ്ഥിതി:
			തിരികെ DLT_LOG_DEFAULT;
	}
}
#endif

#ifdef ANDROID


android_LogPriority get_android_level(unsigned പൂർണ്ണസംഖ്യ priority)
{
	തിരഞ്ഞെടുപ്പ്(priority){
		അവസ്ഥ MOSQ_LOG_ERR:
			തിരികെ ANDROID_LOG_ERROR;
		അവസ്ഥ MOSQ_LOG_WARNING:
			തിരികെ ANDROID_LOG_WARN;
		അവസ്ഥ MOSQ_LOG_INFO:
			തിരികെ ANDROID_LOG_INFO;
		അവസ്ഥ MOSQ_LOG_DEBUG:
			തിരികെ ANDROID_LOG_DEBUG;
		അവസ്ഥ MOSQ_LOG_NOTICE:
		അവസ്ഥ MOSQ_LOG_SUBSCRIBE:
		അവസ്ഥ MOSQ_LOG_UNSUBSCRIBE:
			തിരികെ ANDROID_LOG_VERBOSE;
		സ്ഥിരസ്ഥിതി:
			തിരികെ ANDROID_LOG_DEBUG;
	}
}
#endif


സ്ഥിതം പൂർണ്ണസംഖ്യ log__vprintf(unsigned പൂർണ്ണസംഖ്യ priority, സ്ഥിരം അക്ഷരം *fmt, va_list va)
{
	സ്ഥിരം അക്ഷരം *topic;
	പൂർണ്ണസംഖ്യ syslog_priority;
	അക്ഷരം log_വരി[1000];
	size_t log_വരി_pos;
#ifdef WIN32
	അക്ഷരം *sp;
#endif
	bool log_timestamp = true;
	അക്ഷരം *log_timestamp_format = ശൂന്യ;
	FILE *log_fptr = ശൂന്യ;

	എങ്കിൽ(db.config){
		log_timestamp = db.config->log_timestamp;
		log_timestamp_format = db.config->log_timestamp_format;
		log_fptr = db.config->log_fptr;
	}

	എങ്കിൽ((log_priorities & priority) && log_destinations != MQTT3_LOG_NONE){
		തിരഞ്ഞെടുപ്പ്(priority){
			അവസ്ഥ MOSQ_LOG_SUBSCRIBE:
				topic = "$SYS/broker/log/M/subscribe";
#ifndef WIN32
				syslog_priority = LOG_NOTICE;
#അല്ലെങ്കിൽ
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				നിർത്തുക;
			അവസ്ഥ MOSQ_LOG_UNSUBSCRIBE:
				topic = "$SYS/broker/log/M/unsubscribe";
#ifndef WIN32
				syslog_priority = LOG_NOTICE;
#അല്ലെങ്കിൽ
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				നിർത്തുക;
			അവസ്ഥ MOSQ_LOG_DEBUG:
				topic = "$SYS/broker/log/D";
#ifndef WIN32
				syslog_priority = LOG_DEBUG;
#അല്ലെങ്കിൽ
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				നിർത്തുക;
			അവസ്ഥ MOSQ_LOG_ERR:
				topic = "$SYS/broker/log/E";
#ifndef WIN32
				syslog_priority = LOG_ERR;
#അല്ലെങ്കിൽ
				syslog_priority = EVENTLOG_ERROR_TYPE;
#endif
				നിർത്തുക;
			അവസ്ഥ MOSQ_LOG_WARNING:
				topic = "$SYS/broker/log/W";
#ifndef WIN32
				syslog_priority = LOG_WARNING;
#അല്ലെങ്കിൽ
				syslog_priority = EVENTLOG_WARNING_TYPE;
#endif
				നിർത്തുക;
			അവസ്ഥ MOSQ_LOG_NOTICE:
				topic = "$SYS/broker/log/N";
#ifndef WIN32
				syslog_priority = LOG_NOTICE;
#അല്ലെങ്കിൽ
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				നിർത്തുക;
			അവസ്ഥ MOSQ_LOG_INFO:
				topic = "$SYS/broker/log/I";
#ifndef WIN32
				syslog_priority = LOG_INFO;
#അല്ലെങ്കിൽ
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				നിർത്തുക;
#എങ്കിൽ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			അവസ്ഥ MOSQ_LOG_WEBSOCKETS:
				topic = "$SYS/broker/log/WS";
#ifndef WIN32
				syslog_priority = LOG_DEBUG;
#അല്ലെങ്കിൽ
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				നിർത്തുക;
#endif
			സ്ഥിരസ്ഥിതി:
				topic = "$SYS/broker/log/E";
#ifndef WIN32
				syslog_priority = LOG_ERR;
#അല്ലെങ്കിൽ
				syslog_priority = EVENTLOG_ERROR_TYPE;
#endif
		}
		എങ്കിൽ(log_timestamp){
			എങ്കിൽ(log_timestamp_format){
				ഘടന tm *ti = ശൂന്യ;
				get_time(&ti);
				log_വരി_pos = strftime(log_വരി, വലുപ്പം(log_വരി), log_timestamp_format, ti);
				എങ്കിൽ(log_വരി_pos == 0){
					log_വരി_pos = (size_t)snprintf(log_വരി, വലുപ്പം(log_വരി), "Time error");
				}
			}അല്ലെങ്കിൽ{
				log_വരി_pos = (size_t)snprintf(log_വരി, വലുപ്പം(log_വരി), "%" PRIu64, (uint64_t)db.now_real_s);
			}
			എങ്കിൽ(log_വരി_pos < വലുപ്പം(log_വരി)-3){
				log_വരി[log_വരി_pos] = ':';
				log_വരി[log_വരി_pos+1] = ' ';
				log_വരി[log_വരി_pos+2] = '\0';
				log_വരി_pos += 2;
			}
		}അല്ലെങ്കിൽ{
			log_വരി_pos = 0;
		}
		vsnprintf(&log_വരി[log_വരി_pos], വലുപ്പം(log_വരി)-log_വരി_pos, fmt, va);
		log_വരി[വലുപ്പം(log_വരി)-1] = '\0'; /* Ensure string is null terminated. */

		എങ്കിൽ(log_destinations & MQTT3_LOG_STDOUT){
			fprintf(stdout, "%s\n", log_വരി);
		}
		എങ്കിൽ(log_destinations & MQTT3_LOG_STDERR){
			fprintf(stderr, "%s\n", log_വരി);
		}
		എങ്കിൽ(log_destinations & MQTT3_LOG_FILE && log_fptr){
			fprintf(log_fptr, "%s\n", log_വരി);
#ifdef WIN32
			/* Windows doesn't support line buffering, so flush. */
			fflush(log_fptr);
#endif
		}
		എങ്കിൽ(log_destinations & MQTT3_LOG_SYSLOG){
#ifndef WIN32
			syslog(syslog_priority, "%s", log_വരി);
#അല്ലെങ്കിൽ
			sp = (അക്ഷരം *)log_വരി;
			ReportEvent(syslog_h, syslog_priority, 0, 0, ശൂന്യ, 1, 0, (സ്ഥിരം അക്ഷരം **)&sp, ശൂന്യ);
#endif
		}
		എങ്കിൽ(log_destinations & MQTT3_LOG_TOPIC && priority != MOSQ_LOG_DEBUG && priority != MOSQ_LOG_INTERNAL){
			db__messages_easy_queue(ശൂന്യ, topic, 2, (uint32_t)strlen(log_വരി), log_വരി, 0, 20, ശൂന്യ);
		}
#ifdef WITH_DLT
		എങ്കിൽ(log_destinations & MQTT3_LOG_DLT && priority != MOSQ_LOG_INTERNAL){
			DLT_LOG_STRING(dltContext, get_dlt_level(priority), log_വരി);
		}
#endif
#ifdef ANDROID
		എങ്കിൽ(log_destinations & MQTT3_LOG_ANDROID && priority != MOSQ_LOG_INTERNAL){
			__android_log_write(get_android_level(priority), LOG_TAG, log_വരി);
		}
#endif
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


പൂർണ്ണസംഖ്യ log__printf(ഘടന mosquitto *mosq, unsigned പൂർണ്ണസംഖ്യ priority, സ്ഥിരം അക്ഷരം *fmt, ...)
{
	va_list va;
	പൂർണ്ണസംഖ്യ rc;

	UNUSED(mosq);

	va_start(va, fmt);
	rc = log__vprintf(priority, fmt, va);
	va_end(va);

	തിരികെ rc;
}


ശൂന്യം log__internal(സ്ഥിരം അക്ഷരം *fmt, ...)
{
	va_list va;
	അക്ഷരം buf[200];
	പൂർണ്ണസംഖ്യ len;

	va_start(va, fmt);
	len = vsnprintf(buf, 200, fmt, va);
	va_end(va);

	എങ്കിൽ(len >= 200){
		log__printf(ശൂന്യ, MOSQ_LOG_INTERNAL, "Internal log buffer too short (%d)", len);
		തിരികെ;
	}

#ifdef WIN32
	log__printf(ശൂന്യ, MOSQ_LOG_INTERNAL, "%s", buf);
#അല്ലെങ്കിൽ
	log__printf(ശൂന്യ, MOSQ_LOG_INTERNAL, "%s%s%s", "\e[32m", buf, "\e[0m");
#endif
}


BROKER_EXPORT പൂർണ്ണസംഖ്യ mosquitto_log_vprintf(പൂർണ്ണസംഖ്യ level, സ്ഥിരം അക്ഷരം *fmt, va_list va)
{
	തിരികെ log__vprintf((unsigned പൂർണ്ണസംഖ്യ)level, fmt, va);
}


BROKER_EXPORT ശൂന്യം mosquitto_log_printf(പൂർണ്ണസംഖ്യ level, സ്ഥിരം അക്ഷരം *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	log__vprintf((unsigned പൂർണ്ണസംഖ്യ)level, fmt, va);
	va_end(va);
}


സ്ഥിതം ശൂന്യം libcommon__vprintf(സ്ഥിരം അക്ഷരം *fmt, va_list va)
{
	log__vprintf(MOSQ_LOG_INFO, fmt, va);
}
