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
#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ <inttypes.h>
#ಅನ್ತರ್ಭಾವ <stdarg.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <inttypes.h>
#ifndef WIN32
#ಅನ್ತರ್ಭಾವ <syslog.h>
#endif
#ಅನ್ತರ್ಭಾವ <time.h>

#ಯದಿ defined(__APPLE__)
#  include <sys/time.h>
#endif

#ifdef WITH_DLT
#ಅನ್ತರ್ಭಾವ <sys/stat.h>
#ಅನ್ತರ್ಭಾವ <dlt/dlt.h>
#endif

#ಅನ್ತರ್ಭಾವ "logging_mosq.h"
#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"

#ifdef WIN32
HANDLE syslog_h;
#endif

#ifdef ANDROID
#ಅನ್ತರ್ಭಾವ <android/log.h>
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *LOG_TAG = "mosquitto";
#endif

ಸ್ಥಿರ ವರ್ಣ log_fptr_buffer[BUFSIZ];
ಸ್ಥಿರ ಶೂನ್ಯ libcommon__vprintf(ಧ್ರುವ ವರ್ಣ *fmt, va_list va);

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
ಸ್ಥಿರ unsigned ಪೂರ್ಣಾಙ್ಕ log_destinations = MQTT3_LOG_STDERR;
ಸ್ಥಿರ unsigned ಪೂರ್ಣಾಙ್ಕ log_priorities = MOSQ_LOG_ERR | MOSQ_LOG_WARNING | MOSQ_LOG_NOTICE | MOSQ_LOG_INFO;

#ifdef WITH_DLT
ಸ್ಥಿರ DltContext dltContext;
ಸ್ಥಿರ bool dlt_allowed = false;


ಶೂನ್ಯ dlt_fifo_check(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ stat statbuf;

	/* If we start DLT but the /tmp/dlt fifo doesn't exist, or isn't available
	 * for writing then there is a big delay when we try and close the log
	 * later, so check for it first. This has the side effect of not letting
	 * people using DLT create the fifo after Mosquitto has started, but at the
	 * benefit of not having a massive delay for everybody else. */
	memset(&statbuf, 0, ಪರಿಮಾಣ(statbuf));
	ಯದಿ(stat("/tmp/dlt", &statbuf) == 0){
		ಯದಿ(S_ISFIFO(statbuf.st_mode)){
			ಪೂರ್ಣಾಙ್ಕ fd = open("/tmp/dlt", O_NONBLOCK | O_WRONLY);
			ಯದಿ(fd != -1){
				dlt_allowed = true;
				close(fd);
			}
		}
	}
}
#endif


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ get_time(ಸಂರಚನಾ tm **ti)
{
	time_t s;

	s = db.now_real_s;

	*ti = localtime(&s);
	ಯದಿ(!(*ti)){
		fprintf(stderr, "Error obtaining system time.\n");
		ಪ್ರತ್ಯಾಗಮ 1;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}


ಪೂರ್ಣಾಙ್ಕ log__init(ಸಂರಚನಾ mosquitto__config *config)
{
	ಪೂರ್ಣಾಙ್ಕ rc = 0;

	libcommon_vprintf = libcommon__vprintf;

	log_priorities = config->log_type;
	log_destinations = config->log_dest;

	ಯದಿ(log_destinations & MQTT3_LOG_SYSLOG){
#ifndef WIN32
		openlog("mosquitto", LOG_PID|LOG_CONS, config->log_facility);
#ಅನ್ಯಥಾ
		syslog_h = OpenEventLog(ರಿಕ್ತ, "mosquitto");
#endif
	}

	ಯದಿ(log_destinations & MQTT3_LOG_FILE){
		config->log_fptr = mosquitto_fopen(config->log_file, "at", true);
		ಯದಿ(config->log_fptr){
			setvbuf(config->log_fptr, log_fptr_buffer, _IOLBF, ಪರಿಮಾಣ(log_fptr_buffer));
		}ಅನ್ಯಥಾ{
			log_destinations = MQTT3_LOG_STDERR;
			log_priorities = MOSQ_LOG_ERR;
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Unable to open log file %s for writing.", config->log_file);
		}
	}
#ifndef WIN32
	ಯದಿ(log_destinations & MQTT3_LOG_STDOUT){
		setvbuf(stdout, ರಿಕ್ತ, _IOLBF, 0);
	}
#endif
#ifdef WITH_DLT
	ಯದಿ(log_destinations & MQTT3_LOG_DLT){
		dlt_fifo_check();
		ಯದಿ(dlt_allowed){
			DLT_REGISTER_APP("MQTT", "mosquitto log");
			dlt_register_context(&dltContext, "MQTT", "mosquitto DLT context");
		}
	}
#endif
	ಪ್ರತ್ಯಾಗಮ rc;
}


ಪೂರ್ಣಾಙ್ಕ log__close(ಸಂರಚನಾ mosquitto__config *config)
{
	ಯದಿ(log_destinations & MQTT3_LOG_SYSLOG){
#ifndef WIN32
		closelog();
#ಅನ್ಯಥಾ
		CloseEventLog(syslog_h);
#endif
	}
	ಯದಿ(log_destinations & MQTT3_LOG_FILE){
		ಯದಿ(config->log_fptr){
			fclose(config->log_fptr);
			config->log_fptr = ರಿಕ್ತ;
		}
	}

#ifdef WITH_DLT
	ಯದಿ(dlt_allowed){
		dlt_unregister_context(&dltContext);
		DLT_UNREGISTER_APP();
	}
#endif
	/* FIXME - do something for all destinations! */
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}

#ifdef WITH_DLT


DltLogLevelType get_dlt_level(unsigned ಪೂರ್ಣಾಙ್ಕ priority)
{
	ವಿಕಲ್ಪ(priority){
		ಪಕ್ಷ MOSQ_LOG_ERR:
			ಪ್ರತ್ಯಾಗಮ DLT_LOG_ERROR;
		ಪಕ್ಷ MOSQ_LOG_WARNING:
			ಪ್ರತ್ಯಾಗಮ DLT_LOG_WARN;
		ಪಕ್ಷ MOSQ_LOG_INFO:
			ಪ್ರತ್ಯಾಗಮ DLT_LOG_INFO;
		ಪಕ್ಷ MOSQ_LOG_DEBUG:
			ಪ್ರತ್ಯಾಗಮ DLT_LOG_DEBUG;
		ಪಕ್ಷ MOSQ_LOG_NOTICE:
		ಪಕ್ಷ MOSQ_LOG_SUBSCRIBE:
		ಪಕ್ಷ MOSQ_LOG_UNSUBSCRIBE:
			ಪ್ರತ್ಯಾಗಮ DLT_LOG_VERBOSE;
		ಸ್ವಭಾವ:
			ಪ್ರತ್ಯಾಗಮ DLT_LOG_DEFAULT;
	}
}
#endif

#ifdef ANDROID


android_LogPriority get_android_level(unsigned ಪೂರ್ಣಾಙ್ಕ priority)
{
	ವಿಕಲ್ಪ(priority){
		ಪಕ್ಷ MOSQ_LOG_ERR:
			ಪ್ರತ್ಯಾಗಮ ANDROID_LOG_ERROR;
		ಪಕ್ಷ MOSQ_LOG_WARNING:
			ಪ್ರತ್ಯಾಗಮ ANDROID_LOG_WARN;
		ಪಕ್ಷ MOSQ_LOG_INFO:
			ಪ್ರತ್ಯಾಗಮ ANDROID_LOG_INFO;
		ಪಕ್ಷ MOSQ_LOG_DEBUG:
			ಪ್ರತ್ಯಾಗಮ ANDROID_LOG_DEBUG;
		ಪಕ್ಷ MOSQ_LOG_NOTICE:
		ಪಕ್ಷ MOSQ_LOG_SUBSCRIBE:
		ಪಕ್ಷ MOSQ_LOG_UNSUBSCRIBE:
			ಪ್ರತ್ಯಾಗಮ ANDROID_LOG_VERBOSE;
		ಸ್ವಭಾವ:
			ಪ್ರತ್ಯಾಗಮ ANDROID_LOG_DEBUG;
	}
}
#endif


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ log__vprintf(unsigned ಪೂರ್ಣಾಙ್ಕ priority, ಧ್ರುವ ವರ್ಣ *fmt, va_list va)
{
	ಧ್ರುವ ವರ್ಣ *topic;
	ಪೂರ್ಣಾಙ್ಕ syslog_priority;
	ವರ್ಣ log_ಸಾಲು[1000];
	size_t log_ಸಾಲು_pos;
#ifdef WIN32
	ವರ್ಣ *sp;
#endif
	bool log_timestamp = true;
	ವರ್ಣ *log_timestamp_format = ರಿಕ್ತ;
	FILE *log_fptr = ರಿಕ್ತ;

	ಯದಿ(db.config){
		log_timestamp = db.config->log_timestamp;
		log_timestamp_format = db.config->log_timestamp_format;
		log_fptr = db.config->log_fptr;
	}

	ಯದಿ((log_priorities & priority) && log_destinations != MQTT3_LOG_NONE){
		ವಿಕಲ್ಪ(priority){
			ಪಕ್ಷ MOSQ_LOG_SUBSCRIBE:
				topic = "$SYS/broker/log/M/subscribe";
#ifndef WIN32
				syslog_priority = LOG_NOTICE;
#ಅನ್ಯಥಾ
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				ಭಙ್ಗ;
			ಪಕ್ಷ MOSQ_LOG_UNSUBSCRIBE:
				topic = "$SYS/broker/log/M/unsubscribe";
#ifndef WIN32
				syslog_priority = LOG_NOTICE;
#ಅನ್ಯಥಾ
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				ಭಙ್ಗ;
			ಪಕ್ಷ MOSQ_LOG_DEBUG:
				topic = "$SYS/broker/log/D";
#ifndef WIN32
				syslog_priority = LOG_DEBUG;
#ಅನ್ಯಥಾ
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				ಭಙ್ಗ;
			ಪಕ್ಷ MOSQ_LOG_ERR:
				topic = "$SYS/broker/log/E";
#ifndef WIN32
				syslog_priority = LOG_ERR;
#ಅನ್ಯಥಾ
				syslog_priority = EVENTLOG_ERROR_TYPE;
#endif
				ಭಙ್ಗ;
			ಪಕ್ಷ MOSQ_LOG_WARNING:
				topic = "$SYS/broker/log/W";
#ifndef WIN32
				syslog_priority = LOG_WARNING;
#ಅನ್ಯಥಾ
				syslog_priority = EVENTLOG_WARNING_TYPE;
#endif
				ಭಙ್ಗ;
			ಪಕ್ಷ MOSQ_LOG_NOTICE:
				topic = "$SYS/broker/log/N";
#ifndef WIN32
				syslog_priority = LOG_NOTICE;
#ಅನ್ಯಥಾ
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				ಭಙ್ಗ;
			ಪಕ್ಷ MOSQ_LOG_INFO:
				topic = "$SYS/broker/log/I";
#ifndef WIN32
				syslog_priority = LOG_INFO;
#ಅನ್ಯಥಾ
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				ಭಙ್ಗ;
#ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			ಪಕ್ಷ MOSQ_LOG_WEBSOCKETS:
				topic = "$SYS/broker/log/WS";
#ifndef WIN32
				syslog_priority = LOG_DEBUG;
#ಅನ್ಯಥಾ
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				ಭಙ್ಗ;
#endif
			ಸ್ವಭಾವ:
				topic = "$SYS/broker/log/E";
#ifndef WIN32
				syslog_priority = LOG_ERR;
#ಅನ್ಯಥಾ
				syslog_priority = EVENTLOG_ERROR_TYPE;
#endif
		}
		ಯದಿ(log_timestamp){
			ಯದಿ(log_timestamp_format){
				ಸಂರಚನಾ tm *ti = ರಿಕ್ತ;
				get_time(&ti);
				log_ಸಾಲು_pos = strftime(log_ಸಾಲು, ಪರಿಮಾಣ(log_ಸಾಲು), log_timestamp_format, ti);
				ಯದಿ(log_ಸಾಲು_pos == 0){
					log_ಸಾಲು_pos = (size_t)snprintf(log_ಸಾಲು, ಪರಿಮಾಣ(log_ಸಾಲು), "Time error");
				}
			}ಅನ್ಯಥಾ{
				log_ಸಾಲು_pos = (size_t)snprintf(log_ಸಾಲು, ಪರಿಮಾಣ(log_ಸಾಲು), "%" PRIu64, (uint64_t)db.now_real_s);
			}
			ಯದಿ(log_ಸಾಲು_pos < ಪರಿಮಾಣ(log_ಸಾಲು)-3){
				log_ಸಾಲು[log_ಸಾಲು_pos] = ':';
				log_ಸಾಲು[log_ಸಾಲು_pos+1] = ' ';
				log_ಸಾಲು[log_ಸಾಲು_pos+2] = '\0';
				log_ಸಾಲು_pos += 2;
			}
		}ಅನ್ಯಥಾ{
			log_ಸಾಲು_pos = 0;
		}
		vsnprintf(&log_ಸಾಲು[log_ಸಾಲು_pos], ಪರಿಮಾಣ(log_ಸಾಲು)-log_ಸಾಲು_pos, fmt, va);
		log_ಸಾಲು[ಪರಿಮಾಣ(log_ಸಾಲು)-1] = '\0'; /* Ensure string is null terminated. */

		ಯದಿ(log_destinations & MQTT3_LOG_STDOUT){
			fprintf(stdout, "%s\n", log_ಸಾಲು);
		}
		ಯದಿ(log_destinations & MQTT3_LOG_STDERR){
			fprintf(stderr, "%s\n", log_ಸಾಲು);
		}
		ಯದಿ(log_destinations & MQTT3_LOG_FILE && log_fptr){
			fprintf(log_fptr, "%s\n", log_ಸಾಲು);
#ifdef WIN32
			/* Windows doesn't support line buffering, so flush. */
			fflush(log_fptr);
#endif
		}
		ಯದಿ(log_destinations & MQTT3_LOG_SYSLOG){
#ifndef WIN32
			syslog(syslog_priority, "%s", log_ಸಾಲು);
#ಅನ್ಯಥಾ
			sp = (ವರ್ಣ *)log_ಸಾಲು;
			ReportEvent(syslog_h, syslog_priority, 0, 0, ರಿಕ್ತ, 1, 0, (ಧ್ರುವ ವರ್ಣ **)&sp, ರಿಕ್ತ);
#endif
		}
		ಯದಿ(log_destinations & MQTT3_LOG_TOPIC && priority != MOSQ_LOG_DEBUG && priority != MOSQ_LOG_INTERNAL){
			db__messages_easy_queue(ರಿಕ್ತ, topic, 2, (uint32_t)strlen(log_ಸಾಲು), log_ಸಾಲು, 0, 20, ರಿಕ್ತ);
		}
#ifdef WITH_DLT
		ಯದಿ(log_destinations & MQTT3_LOG_DLT && priority != MOSQ_LOG_INTERNAL){
			DLT_LOG_STRING(dltContext, get_dlt_level(priority), log_ಸಾಲು);
		}
#endif
#ifdef ANDROID
		ಯದಿ(log_destinations & MQTT3_LOG_ANDROID && priority != MOSQ_LOG_INTERNAL){
			__android_log_write(get_android_level(priority), LOG_TAG, log_ಸಾಲು);
		}
#endif
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಙ್ಕ log__printf(ಸಂರಚನಾ mosquitto *mosq, unsigned ಪೂರ್ಣಾಙ್ಕ priority, ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
	va_list va;
	ಪೂರ್ಣಾಙ್ಕ rc;

	UNUSED(mosq);

	va_start(va, fmt);
	rc = log__vprintf(priority, fmt, va);
	va_end(va);

	ಪ್ರತ್ಯಾಗಮ rc;
}


ಶೂನ್ಯ log__internal(ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
	va_list va;
	ವರ್ಣ buf[200];
	ಪೂರ್ಣಾಙ್ಕ len;

	va_start(va, fmt);
	len = vsnprintf(buf, 200, fmt, va);
	va_end(va);

	ಯದಿ(len >= 200){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_INTERNAL, "Internal log buffer too short (%d)", len);
		ಪ್ರತ್ಯಾಗಮ;
	}

#ifdef WIN32
	log__printf(ರಿಕ್ತ, MOSQ_LOG_INTERNAL, "%s", buf);
#ಅನ್ಯಥಾ
	log__printf(ರಿಕ್ತ, MOSQ_LOG_INTERNAL, "%s%s%s", "\e[32m", buf, "\e[0m");
#endif
}


BROKER_EXPORT ಪೂರ್ಣಾಙ್ಕ mosquitto_log_vprintf(ಪೂರ್ಣಾಙ್ಕ level, ಧ್ರುವ ವರ್ಣ *fmt, va_list va)
{
	ಪ್ರತ್ಯಾಗಮ log__vprintf((unsigned ಪೂರ್ಣಾಙ್ಕ)level, fmt, va);
}


BROKER_EXPORT ಶೂನ್ಯ mosquitto_log_printf(ಪೂರ್ಣಾಙ್ಕ level, ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	log__vprintf((unsigned ಪೂರ್ಣಾಙ್ಕ)level, fmt, va);
	va_end(va);
}


ಸ್ಥಿರ ಶೂನ್ಯ libcommon__vprintf(ಧ್ರುವ ವರ್ಣ *fmt, va_list va)
{
	log__vprintf(MOSQ_LOG_INFO, fmt, va);
}
