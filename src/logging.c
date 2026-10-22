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
#அந்தர்பாவ "config.h"

#அந்தர்பாவ <inttypes.h>
#அந்தர்பாவ <stdarg.h>
#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <inttypes.h>
#ifndef WIN32
#அந்தர்பாவ <syslog.h>
#endif
#அந்தர்பாவ <time.h>

#எனில் defined(__APPLE__)
#  include <sys/time.h>
#endif

#ifdef WITH_DLT
#அந்தர்பாவ <sys/stat.h>
#அந்தர்பாவ <dlt/dlt.h>
#endif

#அந்தர்பாவ "logging_mosq.h"
#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "util_mosq.h"

#ifdef WIN32
HANDLE syslog_h;
#endif

#ifdef ANDROID
#அந்தர்பாவ <android/log.h>
நிலையான மாறிலி எழுத்து *LOG_TAG = "mosquitto";
#endif

நிலையான எழுத்து log_fptr_buffer[BUFSIZ];
நிலையான வெறுமை libcommon__vprintf(மாறிலி எழுத்து *fmt, va_list va);

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
நிலையான unsigned முழு log_destinations = MQTT3_LOG_STDERR;
நிலையான unsigned முழு log_priorities = MOSQ_LOG_ERR | MOSQ_LOG_WARNING | MOSQ_LOG_NOTICE | MOSQ_LOG_INFO;

#ifdef WITH_DLT
நிலையான DltContext dltContext;
நிலையான bool dlt_allowed = false;


வெறுமை dlt_fifo_check(வெறுமை)
{
	கட்டமைப்பு stat statbuf;

	/* If we start DLT but the /tmp/dlt fifo doesn't exist, or isn't available
	 * for writing then there is a big delay when we try and close the log
	 * later, so check for it first. This has the side effect of not letting
	 * people using DLT create the fifo after Mosquitto has started, but at the
	 * benefit of not having a massive delay for everybody else. */
	memset(&statbuf, 0, அளவு(statbuf));
	எனில்(stat("/tmp/dlt", &statbuf) == 0){
		எனில்(S_ISFIFO(statbuf.st_mode)){
			முழு fd = open("/tmp/dlt", O_NONBLOCK | O_WRONLY);
			எனில்(fd != -1){
				dlt_allowed = true;
				close(fd);
			}
		}
	}
}
#endif


நிலையான முழு get_time(கட்டமைப்பு tm **ti)
{
	time_t s;

	s = db.now_real_s;

	*ti = localtime(&s);
	எனில்(!(*ti)){
		fprintf(stderr, "Error obtaining system time.\n");
		திரும்பு 1;
	}

	திரும்பு 0;
}


முழு log__init(கட்டமைப்பு mosquitto__config *config)
{
	முழு rc = 0;

	libcommon_vprintf = libcommon__vprintf;

	log_priorities = config->log_type;
	log_destinations = config->log_dest;

	எனில்(log_destinations & MQTT3_LOG_SYSLOG){
#ifndef WIN32
		openlog("mosquitto", LOG_PID|LOG_CONS, config->log_facility);
#இல்லையெனில்
		syslog_h = OpenEventLog(வெற்று, "mosquitto");
#endif
	}

	எனில்(log_destinations & MQTT3_LOG_FILE){
		config->log_fptr = mosquitto_fopen(config->log_file, "at", true);
		எனில்(config->log_fptr){
			setvbuf(config->log_fptr, log_fptr_buffer, _IOLBF, அளவு(log_fptr_buffer));
		}இல்லையெனில்{
			log_destinations = MQTT3_LOG_STDERR;
			log_priorities = MOSQ_LOG_ERR;
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Unable to open log file %s for writing.", config->log_file);
		}
	}
#ifndef WIN32
	எனில்(log_destinations & MQTT3_LOG_STDOUT){
		setvbuf(stdout, வெற்று, _IOLBF, 0);
	}
#endif
#ifdef WITH_DLT
	எனில்(log_destinations & MQTT3_LOG_DLT){
		dlt_fifo_check();
		எனில்(dlt_allowed){
			DLT_REGISTER_APP("MQTT", "mosquitto log");
			dlt_register_context(&dltContext, "MQTT", "mosquitto DLT context");
		}
	}
#endif
	திரும்பு rc;
}


முழு log__close(கட்டமைப்பு mosquitto__config *config)
{
	எனில்(log_destinations & MQTT3_LOG_SYSLOG){
#ifndef WIN32
		closelog();
#இல்லையெனில்
		CloseEventLog(syslog_h);
#endif
	}
	எனில்(log_destinations & MQTT3_LOG_FILE){
		எனில்(config->log_fptr){
			fclose(config->log_fptr);
			config->log_fptr = வெற்று;
		}
	}

#ifdef WITH_DLT
	எனில்(dlt_allowed){
		dlt_unregister_context(&dltContext);
		DLT_UNREGISTER_APP();
	}
#endif
	/* FIXME - do something for all destinations! */
	திரும்பு MOSQ_ERR_SUCCESS;
}

#ifdef WITH_DLT


DltLogLevelType get_dlt_level(unsigned முழு priority)
{
	தேர்வு(priority){
		நிலை MOSQ_LOG_ERR:
			திரும்பு DLT_LOG_ERROR;
		நிலை MOSQ_LOG_WARNING:
			திரும்பு DLT_LOG_WARN;
		நிலை MOSQ_LOG_INFO:
			திரும்பு DLT_LOG_INFO;
		நிலை MOSQ_LOG_DEBUG:
			திரும்பு DLT_LOG_DEBUG;
		நிலை MOSQ_LOG_NOTICE:
		நிலை MOSQ_LOG_SUBSCRIBE:
		நிலை MOSQ_LOG_UNSUBSCRIBE:
			திரும்பு DLT_LOG_VERBOSE;
		இயல்பு:
			திரும்பு DLT_LOG_DEFAULT;
	}
}
#endif

#ifdef ANDROID


android_LogPriority get_android_level(unsigned முழு priority)
{
	தேர்வு(priority){
		நிலை MOSQ_LOG_ERR:
			திரும்பு ANDROID_LOG_ERROR;
		நிலை MOSQ_LOG_WARNING:
			திரும்பு ANDROID_LOG_WARN;
		நிலை MOSQ_LOG_INFO:
			திரும்பு ANDROID_LOG_INFO;
		நிலை MOSQ_LOG_DEBUG:
			திரும்பு ANDROID_LOG_DEBUG;
		நிலை MOSQ_LOG_NOTICE:
		நிலை MOSQ_LOG_SUBSCRIBE:
		நிலை MOSQ_LOG_UNSUBSCRIBE:
			திரும்பு ANDROID_LOG_VERBOSE;
		இயல்பு:
			திரும்பு ANDROID_LOG_DEBUG;
	}
}
#endif


நிலையான முழு log__vprintf(unsigned முழு priority, மாறிலி எழுத்து *fmt, va_list va)
{
	மாறிலி எழுத்து *topic;
	முழு syslog_priority;
	எழுத்து log_வரி[1000];
	size_t log_வரி_pos;
#ifdef WIN32
	எழுத்து *sp;
#endif
	bool log_timestamp = true;
	எழுத்து *log_timestamp_format = வெற்று;
	FILE *log_fptr = வெற்று;

	எனில்(db.config){
		log_timestamp = db.config->log_timestamp;
		log_timestamp_format = db.config->log_timestamp_format;
		log_fptr = db.config->log_fptr;
	}

	எனில்((log_priorities & priority) && log_destinations != MQTT3_LOG_NONE){
		தேர்வு(priority){
			நிலை MOSQ_LOG_SUBSCRIBE:
				topic = "$SYS/broker/log/M/subscribe";
#ifndef WIN32
				syslog_priority = LOG_NOTICE;
#இல்லையெனில்
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				நிறுத்து;
			நிலை MOSQ_LOG_UNSUBSCRIBE:
				topic = "$SYS/broker/log/M/unsubscribe";
#ifndef WIN32
				syslog_priority = LOG_NOTICE;
#இல்லையெனில்
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				நிறுத்து;
			நிலை MOSQ_LOG_DEBUG:
				topic = "$SYS/broker/log/D";
#ifndef WIN32
				syslog_priority = LOG_DEBUG;
#இல்லையெனில்
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				நிறுத்து;
			நிலை MOSQ_LOG_ERR:
				topic = "$SYS/broker/log/E";
#ifndef WIN32
				syslog_priority = LOG_ERR;
#இல்லையெனில்
				syslog_priority = EVENTLOG_ERROR_TYPE;
#endif
				நிறுத்து;
			நிலை MOSQ_LOG_WARNING:
				topic = "$SYS/broker/log/W";
#ifndef WIN32
				syslog_priority = LOG_WARNING;
#இல்லையெனில்
				syslog_priority = EVENTLOG_WARNING_TYPE;
#endif
				நிறுத்து;
			நிலை MOSQ_LOG_NOTICE:
				topic = "$SYS/broker/log/N";
#ifndef WIN32
				syslog_priority = LOG_NOTICE;
#இல்லையெனில்
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				நிறுத்து;
			நிலை MOSQ_LOG_INFO:
				topic = "$SYS/broker/log/I";
#ifndef WIN32
				syslog_priority = LOG_INFO;
#இல்லையெனில்
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				நிறுத்து;
#எனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			நிலை MOSQ_LOG_WEBSOCKETS:
				topic = "$SYS/broker/log/WS";
#ifndef WIN32
				syslog_priority = LOG_DEBUG;
#இல்லையெனில்
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				நிறுத்து;
#endif
			இயல்பு:
				topic = "$SYS/broker/log/E";
#ifndef WIN32
				syslog_priority = LOG_ERR;
#இல்லையெனில்
				syslog_priority = EVENTLOG_ERROR_TYPE;
#endif
		}
		எனில்(log_timestamp){
			எனில்(log_timestamp_format){
				கட்டமைப்பு tm *ti = வெற்று;
				get_time(&ti);
				log_வரி_pos = strftime(log_வரி, அளவு(log_வரி), log_timestamp_format, ti);
				எனில்(log_வரி_pos == 0){
					log_வரி_pos = (size_t)snprintf(log_வரி, அளவு(log_வரி), "Time error");
				}
			}இல்லையெனில்{
				log_வரி_pos = (size_t)snprintf(log_வரி, அளவு(log_வரி), "%" PRIu64, (uint64_t)db.now_real_s);
			}
			எனில்(log_வரி_pos < அளவு(log_வரி)-3){
				log_வரி[log_வரி_pos] = ':';
				log_வரி[log_வரி_pos+1] = ' ';
				log_வரி[log_வரி_pos+2] = '\0';
				log_வரி_pos += 2;
			}
		}இல்லையெனில்{
			log_வரி_pos = 0;
		}
		vsnprintf(&log_வரி[log_வரி_pos], அளவு(log_வரி)-log_வரி_pos, fmt, va);
		log_வரி[அளவு(log_வரி)-1] = '\0'; /* Ensure string is null terminated. */

		எனில்(log_destinations & MQTT3_LOG_STDOUT){
			fprintf(stdout, "%s\n", log_வரி);
		}
		எனில்(log_destinations & MQTT3_LOG_STDERR){
			fprintf(stderr, "%s\n", log_வரி);
		}
		எனில்(log_destinations & MQTT3_LOG_FILE && log_fptr){
			fprintf(log_fptr, "%s\n", log_வரி);
#ifdef WIN32
			/* Windows doesn't support line buffering, so flush. */
			fflush(log_fptr);
#endif
		}
		எனில்(log_destinations & MQTT3_LOG_SYSLOG){
#ifndef WIN32
			syslog(syslog_priority, "%s", log_வரி);
#இல்லையெனில்
			sp = (எழுத்து *)log_வரி;
			ReportEvent(syslog_h, syslog_priority, 0, 0, வெற்று, 1, 0, (மாறிலி எழுத்து **)&sp, வெற்று);
#endif
		}
		எனில்(log_destinations & MQTT3_LOG_TOPIC && priority != MOSQ_LOG_DEBUG && priority != MOSQ_LOG_INTERNAL){
			db__messages_easy_queue(வெற்று, topic, 2, (uint32_t)strlen(log_வரி), log_வரி, 0, 20, வெற்று);
		}
#ifdef WITH_DLT
		எனில்(log_destinations & MQTT3_LOG_DLT && priority != MOSQ_LOG_INTERNAL){
			DLT_LOG_STRING(dltContext, get_dlt_level(priority), log_வரி);
		}
#endif
#ifdef ANDROID
		எனில்(log_destinations & MQTT3_LOG_ANDROID && priority != MOSQ_LOG_INTERNAL){
			__android_log_write(get_android_level(priority), LOG_TAG, log_வரி);
		}
#endif
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


முழு log__printf(கட்டமைப்பு mosquitto *mosq, unsigned முழு priority, மாறிலி எழுத்து *fmt, ...)
{
	va_list va;
	முழு rc;

	UNUSED(mosq);

	va_start(va, fmt);
	rc = log__vprintf(priority, fmt, va);
	va_end(va);

	திரும்பு rc;
}


வெறுமை log__internal(மாறிலி எழுத்து *fmt, ...)
{
	va_list va;
	எழுத்து buf[200];
	முழு len;

	va_start(va, fmt);
	len = vsnprintf(buf, 200, fmt, va);
	va_end(va);

	எனில்(len >= 200){
		log__printf(வெற்று, MOSQ_LOG_INTERNAL, "Internal log buffer too short (%d)", len);
		திரும்பு;
	}

#ifdef WIN32
	log__printf(வெற்று, MOSQ_LOG_INTERNAL, "%s", buf);
#இல்லையெனில்
	log__printf(வெற்று, MOSQ_LOG_INTERNAL, "%s%s%s", "\e[32m", buf, "\e[0m");
#endif
}


BROKER_EXPORT முழு mosquitto_log_vprintf(முழு level, மாறிலி எழுத்து *fmt, va_list va)
{
	திரும்பு log__vprintf((unsigned முழு)level, fmt, va);
}


BROKER_EXPORT வெறுமை mosquitto_log_printf(முழு level, மாறிலி எழுத்து *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	log__vprintf((unsigned முழு)level, fmt, va);
	va_end(va);
}


நிலையான வெறுமை libcommon__vprintf(மாறிலி எழுத்து *fmt, va_list va)
{
	log__vprintf(MOSQ_LOG_INFO, fmt, va);
}
