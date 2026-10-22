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
#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <inttypes.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdarg.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <inttypes.h>
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
#ਅਨ੍ਤਰ੍ਭਾਵ <syslog.h>
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <time.h>

#ਜੇ defined(__APPLE__)
#  include <sys/time.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_DLT
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/stat.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <dlt/dlt.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "logging_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util_mosq.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WIN32
HANDLE syslog_h;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ANDROID
#ਅਨ੍ਤਰ੍ਭਾਵ <android/log.h>
ਸਥਿਰ ਅਚਲ ਅੱਖਰ *LOG_ਟੈਗ = "mosquitto";
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਅੱਖਰ log_fptr_ਬਫਰ[BUFSIZ];
ਸਥਿਰ ਖਾਲੀ libcommon__vprintf(ਅਚਲ ਅੱਖਰ *fmt, va_list va);

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
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ log_destinations = MQTT3_LOG_STDERR;
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ log_priorities = MOSQ_LOG_ERR | MOSQ_LOG_WARNING | MOSQ_LOG_NOTICE | MOSQ_LOG_INFO;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_DLT
ਸਥਿਰ DltContext dltContext;
ਸਥਿਰ bool dlt_allowed = false;


ਖਾਲੀ dlt_fifo_check(ਖਾਲੀ)
{
	ਬਣਤਰ stat statbuf;

	/* If we start DLT but the /tmp/dlt fifo doesn't exist, or isn't available
	 * for writing then there is a big delay when we try and close the log
	 * later, so check for it first. This has the side effect of not letting
	 * people using DLT create the fifo after Mosquitto has started, but at the
	 * benefit of not having a massive delay for everybody else. */
	memset(&statbuf, 0, ਆਕਾਰ(statbuf));
	ਜੇ(stat("/tmp/dlt", &statbuf) == 0){
		ਜੇ(S_ISFIFO(statbuf.st_ਬਹੁਲਕ)){
			ਪੂਰਨ fd = open("/tmp/dlt", O_NONBLOCK | O_WRONLY);
			ਜੇ(fd != -1){
				dlt_allowed = true;
				close(fd);
			}
		}
	}
}
#ਜੇ_ਅੰਤ


ਸਥਿਰ ਪੂਰਨ get_ਸਮਾਂ(ਬਣਤਰ tm **ti)
{
	time_t s;

	s = db.now_real_s;

	*ti = localtime(&s);
	ਜੇ(!(*ti)){
		fprintf(stderr, "Error obtaining system time.\n");
		ਵਾਪਸ 1;
	}

	ਵਾਪਸ 0;
}


ਪੂਰਨ log__init(ਬਣਤਰ mosquitto__config *config)
{
	ਪੂਰਨ rc = 0;

	libcommon_vprintf = libcommon__vprintf;

	log_priorities = config->log_type;
	log_destinations = config->log_dest;

	ਜੇ(log_destinations & MQTT3_LOG_SYSLOG){
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
		openlog("mosquitto", LOG_PID|LOG_CONS, config->log_facility);
#ਨਹੀਂਤਾਂ
		syslog_h = OpenEventLog(ਨਿਰਾ, "mosquitto");
#ਜੇ_ਅੰਤ
	}

	ਜੇ(log_destinations & MQTT3_LOG_FILE){
		config->log_fptr = mosquitto_fopen(config->log_file, "at", true);
		ਜੇ(config->log_fptr){
			setvbuf(config->log_fptr, log_fptr_ਬਫਰ, _IOLBF, ਆਕਾਰ(log_fptr_ਬਫਰ));
		}ਨਹੀਂਤਾਂ{
			log_destinations = MQTT3_LOG_STDERR;
			log_priorities = MOSQ_LOG_ERR;
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Unable to open log file %s for writing.", config->log_file);
		}
	}
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
	ਜੇ(log_destinations & MQTT3_LOG_STDOUT){
		setvbuf(stdout, ਨਿਰਾ, _IOLBF, 0);
	}
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_DLT
	ਜੇ(log_destinations & MQTT3_LOG_DLT){
		dlt_fifo_check();
		ਜੇ(dlt_allowed){
			DLT_ਰਜਿਸਟਰ_APP("MQTT", "mosquitto log");
			dlt_ਰਜਿਸਟਰ_context(&dltContext, "MQTT", "mosquitto DLT context");
		}
	}
#ਜੇ_ਅੰਤ
	ਵਾਪਸ rc;
}


ਪੂਰਨ log__close(ਬਣਤਰ mosquitto__config *config)
{
	ਜੇ(log_destinations & MQTT3_LOG_SYSLOG){
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
		closelog();
#ਨਹੀਂਤਾਂ
		CloseEventLog(syslog_h);
#ਜੇ_ਅੰਤ
	}
	ਜੇ(log_destinations & MQTT3_LOG_FILE){
		ਜੇ(config->log_fptr){
			fclose(config->log_fptr);
			config->log_fptr = ਨਿਰਾ;
		}
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_DLT
	ਜੇ(dlt_allowed){
		dlt_unregister_context(&dltContext);
		DLT_UNREGISTER_APP();
	}
#ਜੇ_ਅੰਤ
	/* FIXME - do something for all destinations! */
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_DLT


DltLogLevelType get_dlt_level(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਤਰਜੀਹ)
{
	ਚੋਣ(ਤਰਜੀਹ){
		ਹਾਲਤ MOSQ_LOG_ERR:
			ਵਾਪਸ DLT_LOG_ERROR;
		ਹਾਲਤ MOSQ_LOG_WARNING:
			ਵਾਪਸ DLT_LOG_WARN;
		ਹਾਲਤ MOSQ_LOG_INFO:
			ਵਾਪਸ DLT_LOG_INFO;
		ਹਾਲਤ MOSQ_LOG_DEBUG:
			ਵਾਪਸ DLT_LOG_DEBUG;
		ਹਾਲਤ MOSQ_LOG_NOTICE:
		ਹਾਲਤ MOSQ_LOG_SUBSCRIBE:
		ਹਾਲਤ MOSQ_LOG_UNSUBSCRIBE:
			ਵਾਪਸ DLT_LOG_VERBOSE;
		ਮੂਲ:
			ਵਾਪਸ DLT_LOG_DEFAULT;
	}
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ANDROID


android_Log_ਤਰਜੀਹ get_android_level(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਤਰਜੀਹ)
{
	ਚੋਣ(ਤਰਜੀਹ){
		ਹਾਲਤ MOSQ_LOG_ERR:
			ਵਾਪਸ ANDROID_LOG_ERROR;
		ਹਾਲਤ MOSQ_LOG_WARNING:
			ਵਾਪਸ ANDROID_LOG_WARN;
		ਹਾਲਤ MOSQ_LOG_INFO:
			ਵਾਪਸ ANDROID_LOG_INFO;
		ਹਾਲਤ MOSQ_LOG_DEBUG:
			ਵਾਪਸ ANDROID_LOG_DEBUG;
		ਹਾਲਤ MOSQ_LOG_NOTICE:
		ਹਾਲਤ MOSQ_LOG_SUBSCRIBE:
		ਹਾਲਤ MOSQ_LOG_UNSUBSCRIBE:
			ਵਾਪਸ ANDROID_LOG_VERBOSE;
		ਮੂਲ:
			ਵਾਪਸ ANDROID_LOG_DEBUG;
	}
}
#ਜੇ_ਅੰਤ


ਸਥਿਰ ਪੂਰਨ log__vprintf(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਤਰਜੀਹ, ਅਚਲ ਅੱਖਰ *fmt, va_list va)
{
	ਅਚਲ ਅੱਖਰ *topic;
	ਪੂਰਨ syslog_ਤਰਜੀਹ;
	ਅੱਖਰ log_ਸਤਰ[1000];
	size_t log_ਸਤਰ_pos;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WIN32
	ਅੱਖਰ *sp;
#ਜੇ_ਅੰਤ
	bool log_timestamp = true;
	ਅੱਖਰ *log_timestamp_format = ਨਿਰਾ;
	FILE *log_fptr = ਨਿਰਾ;

	ਜੇ(db.config){
		log_timestamp = db.config->log_timestamp;
		log_timestamp_format = db.config->log_timestamp_format;
		log_fptr = db.config->log_fptr;
	}

	ਜੇ((log_priorities & ਤਰਜੀਹ) && log_destinations != MQTT3_LOG_NONE){
		ਚੋਣ(ਤਰਜੀਹ){
			ਹਾਲਤ MOSQ_LOG_SUBSCRIBE:
				topic = "$SYS/broker/log/M/subscribe";
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
				syslog_ਤਰਜੀਹ = LOG_NOTICE;
#ਨਹੀਂਤਾਂ
				syslog_ਤਰਜੀਹ = EVENTLOG_INFORMATION_TYPE;
#ਜੇ_ਅੰਤ
				ਰੋਕੋ;
			ਹਾਲਤ MOSQ_LOG_UNSUBSCRIBE:
				topic = "$SYS/broker/log/M/unsubscribe";
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
				syslog_ਤਰਜੀਹ = LOG_NOTICE;
#ਨਹੀਂਤਾਂ
				syslog_ਤਰਜੀਹ = EVENTLOG_INFORMATION_TYPE;
#ਜੇ_ਅੰਤ
				ਰੋਕੋ;
			ਹਾਲਤ MOSQ_LOG_DEBUG:
				topic = "$SYS/broker/log/D";
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
				syslog_ਤਰਜੀਹ = LOG_DEBUG;
#ਨਹੀਂਤਾਂ
				syslog_ਤਰਜੀਹ = EVENTLOG_INFORMATION_TYPE;
#ਜੇ_ਅੰਤ
				ਰੋਕੋ;
			ਹਾਲਤ MOSQ_LOG_ERR:
				topic = "$SYS/broker/log/E";
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
				syslog_ਤਰਜੀਹ = LOG_ERR;
#ਨਹੀਂਤਾਂ
				syslog_ਤਰਜੀਹ = EVENTLOG_ERROR_TYPE;
#ਜੇ_ਅੰਤ
				ਰੋਕੋ;
			ਹਾਲਤ MOSQ_LOG_WARNING:
				topic = "$SYS/broker/log/W";
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
				syslog_ਤਰਜੀਹ = LOG_WARNING;
#ਨਹੀਂਤਾਂ
				syslog_ਤਰਜੀਹ = EVENTLOG_WARNING_TYPE;
#ਜੇ_ਅੰਤ
				ਰੋਕੋ;
			ਹਾਲਤ MOSQ_LOG_NOTICE:
				topic = "$SYS/broker/log/N";
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
				syslog_ਤਰਜੀਹ = LOG_NOTICE;
#ਨਹੀਂਤਾਂ
				syslog_ਤਰਜੀਹ = EVENTLOG_INFORMATION_TYPE;
#ਜੇ_ਅੰਤ
				ਰੋਕੋ;
			ਹਾਲਤ MOSQ_LOG_INFO:
				topic = "$SYS/broker/log/I";
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
				syslog_ਤਰਜੀਹ = LOG_INFO;
#ਨਹੀਂਤਾਂ
				syslog_ਤਰਜੀਹ = EVENTLOG_INFORMATION_TYPE;
#ਜੇ_ਅੰਤ
				ਰੋਕੋ;
#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			ਹਾਲਤ MOSQ_LOG_WEBSOCKETS:
				topic = "$SYS/broker/log/WS";
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
				syslog_ਤਰਜੀਹ = LOG_DEBUG;
#ਨਹੀਂਤਾਂ
				syslog_ਤਰਜੀਹ = EVENTLOG_INFORMATION_TYPE;
#ਜੇ_ਅੰਤ
				ਰੋਕੋ;
#ਜੇ_ਅੰਤ
			ਮੂਲ:
				topic = "$SYS/broker/log/E";
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
				syslog_ਤਰਜੀਹ = LOG_ERR;
#ਨਹੀਂਤਾਂ
				syslog_ਤਰਜੀਹ = EVENTLOG_ERROR_TYPE;
#ਜੇ_ਅੰਤ
		}
		ਜੇ(log_timestamp){
			ਜੇ(log_timestamp_format){
				ਬਣਤਰ tm *ti = ਨਿਰਾ;
				get_ਸਮਾਂ(&ti);
				log_ਸਤਰ_pos = strftime(log_ਸਤਰ, ਆਕਾਰ(log_ਸਤਰ), log_timestamp_format, ti);
				ਜੇ(log_ਸਤਰ_pos == 0){
					log_ਸਤਰ_pos = (size_t)snprintf(log_ਸਤਰ, ਆਕਾਰ(log_ਸਤਰ), "Time error");
				}
			}ਨਹੀਂਤਾਂ{
				log_ਸਤਰ_pos = (size_t)snprintf(log_ਸਤਰ, ਆਕਾਰ(log_ਸਤਰ), "%" PRIu64, (uint64_t)db.now_real_s);
			}
			ਜੇ(log_ਸਤਰ_pos < ਆਕਾਰ(log_ਸਤਰ)-3){
				log_ਸਤਰ[log_ਸਤਰ_pos] = ':';
				log_ਸਤਰ[log_ਸਤਰ_pos+1] = ' ';
				log_ਸਤਰ[log_ਸਤਰ_pos+2] = '\0';
				log_ਸਤਰ_pos += 2;
			}
		}ਨਹੀਂਤਾਂ{
			log_ਸਤਰ_pos = 0;
		}
		vsnprintf(&log_ਸਤਰ[log_ਸਤਰ_pos], ਆਕਾਰ(log_ਸਤਰ)-log_ਸਤਰ_pos, fmt, va);
		log_ਸਤਰ[ਆਕਾਰ(log_ਸਤਰ)-1] = '\0'; /* Ensure string is null terminated. */

		ਜੇ(log_destinations & MQTT3_LOG_STDOUT){
			fprintf(stdout, "%s\n", log_ਸਤਰ);
		}
		ਜੇ(log_destinations & MQTT3_LOG_STDERR){
			fprintf(stderr, "%s\n", log_ਸਤਰ);
		}
		ਜੇ(log_destinations & MQTT3_LOG_FILE && log_fptr){
			fprintf(log_fptr, "%s\n", log_ਸਤਰ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WIN32
			/* Windows doesn't support line buffering, so flush. */
			fflush(log_fptr);
#ਜੇ_ਅੰਤ
		}
		ਜੇ(log_destinations & MQTT3_LOG_SYSLOG){
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
			syslog(syslog_ਤਰਜੀਹ, "%s", log_ਸਤਰ);
#ਨਹੀਂਤਾਂ
			sp = (ਅੱਖਰ *)log_ਸਤਰ;
			ReportEvent(syslog_h, syslog_ਤਰਜੀਹ, 0, 0, ਨਿਰਾ, 1, 0, (ਅਚਲ ਅੱਖਰ **)&sp, ਨਿਰਾ);
#ਜੇ_ਅੰਤ
		}
		ਜੇ(log_destinations & MQTT3_LOG_TOPIC && ਤਰਜੀਹ != MOSQ_LOG_DEBUG && ਤਰਜੀਹ != MOSQ_LOG_ਅੰਦਰੂਨੀ){
			db_messages_easy_ਕਤਾਰ(ਨਿਰਾ, topic, 2, (uint32_t)ਲੜੀ_ਲੰਬਾਈ(log_ਸਤਰ), log_ਸਤਰ, 0, 20, ਨਿਰਾ);
		}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_DLT
		ਜੇ(log_destinations & MQTT3_LOG_DLT && ਤਰਜੀਹ != MOSQ_LOG_ਅੰਦਰੂਨੀ){
			DLT_LOG_STRING(dltContext, get_dlt_level(ਤਰਜੀਹ), log_ਸਤਰ);
		}
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ANDROID
		ਜੇ(log_destinations & MQTT3_LOG_ANDROID && ਤਰਜੀਹ != MOSQ_LOG_ਅੰਦਰੂਨੀ){
			__android_log_write(get_android_level(ਤਰਜੀਹ), LOG_ਟੈਗ, log_ਸਤਰ);
		}
#ਜੇ_ਅੰਤ
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਪੂਰਨ log__printf(ਬਣਤਰ mosquitto *mosq, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਤਰਜੀਹ, ਅਚਲ ਅੱਖਰ *fmt, ...)
{
	va_list va;
	ਪੂਰਨ rc;

	ਅਣਵਰਤਿਆ(mosq);

	va_ਸ਼ੁਰੂ(va, fmt);
	rc = log__vprintf(ਤਰਜੀਹ, fmt, va);
	va_ਸਿਰਾ(va);

	ਵਾਪਸ rc;
}


ਖਾਲੀ log_ਅੰਦਰੂਨੀ(ਅਚਲ ਅੱਖਰ *fmt, ...)
{
	va_list va;
	ਅੱਖਰ buf[200];
	ਪੂਰਨ len;

	va_ਸ਼ੁਰੂ(va, fmt);
	len = vsnprintf(buf, 200, fmt, va);
	va_ਸਿਰਾ(va);

	ਜੇ(len >= 200){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ਅੰਦਰੂਨੀ, "Internal log buffer too short (%d)", len);
		ਵਾਪਸ;
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WIN32
	log__printf(ਨਿਰਾ, MOSQ_LOG_ਅੰਦਰੂਨੀ, "%s", buf);
#ਨਹੀਂਤਾਂ
	log__printf(ਨਿਰਾ, MOSQ_LOG_ਅੰਦਰੂਨੀ, "%s%s%s", "\e[32m", buf, "\e[0m");
#ਜੇ_ਅੰਤ
}


BROKER_EXPORT ਪੂਰਨ mosquitto_log_vprintf(ਪੂਰਨ level, ਅਚਲ ਅੱਖਰ *fmt, va_list va)
{
	ਵਾਪਸ log__vprintf((ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)level, fmt, va);
}


BROKER_EXPORT ਖਾਲੀ mosquitto_log_printf(ਪੂਰਨ level, ਅਚਲ ਅੱਖਰ *fmt, ...)
{
	va_list va;

	va_ਸ਼ੁਰੂ(va, fmt);
	log__vprintf((ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)level, fmt, va);
	va_ਸਿਰਾ(va);
}


ਸਥਿਰ ਖਾਲੀ libcommon__vprintf(ਅਚਲ ਅੱਖਰ *fmt, va_list va)
{
	log__vprintf(MOSQ_LOG_INFO, fmt, va);
}
