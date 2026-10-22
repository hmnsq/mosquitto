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
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
#ಅನ್ತರ್ಭಾವ <syslog.h>
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <time.h>

#ಆದರೆ defined(__APPLE__)
#  include <sys/time.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_DLT
#ಅನ್ತರ್ಭಾವ <sys/stat.h>
#ಅನ್ತರ್ಭಾವ <dlt/dlt.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "logging_mosq.h"
#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WIN32
HANDLE syslog_h;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ANDROID
#ಅನ್ತರ್ಭಾವ <android/log.h>
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *LOG_ಟ್ಯಾಗ್ = "mosquitto";
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಅಕ್ಷರ log_fptr_ಬಫರ್[BUFSIZ];
ಸ್ಥಿರ ಶೂನ್ಯ libcommon__vprintf(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, va_list va);

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
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ log_destinations = MQTT3_LOG_STDERR;
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ log_priorities = MOSQ_LOG_ERR | MOSQ_LOG_WARNING | MOSQ_LOG_NOTICE | MOSQ_LOG_INFO;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_DLT
ಸ್ಥಿರ DltContext dltContext;
ಸ್ಥಿರ bool dlt_allowed = false;


ಶೂನ್ಯ dlt_fifo_check(ಶೂನ್ಯ)
{
	ರಚನೆ stat statbuf;

	/* If we start DLT but the /tmp/dlt fifo doesn't exist, or isn't available
	 * for writing then there is a big delay when we try and close the log
	 * later, so check for it first. This has the side effect of not letting
	 * people using DLT create the fifo after Mosquitto has started, but at the
	 * benefit of not having a massive delay for everybody else. */
	memset(&statbuf, 0, ಗಾತ್ರ(statbuf));
	ಆದರೆ(stat("/tmp/dlt", &statbuf) == 0){
		ಆದರೆ(S_ISFIFO(statbuf.st_ಬಹುಲಕ)){
			ಪೂರ್ಣಾಂಕ fd = open("/tmp/dlt", O_NONBLOCK | O_WRONLY);
			ಆದರೆ(fd != -1){
				dlt_allowed = true;
				close(fd);
			}
		}
	}
}
#ಆದರೆ_ಅಂತ್ಯ


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ get_ಸಮಯ(ರಚನೆ tm **ti)
{
	time_t s;

	s = db.now_real_s;

	*ti = localtime(&s);
	ಆದರೆ(!(*ti)){
		fprintf(stderr, "Error obtaining system time.\n");
		ಹಿಂತಿರುಗಿ 1;
	}

	ಹಿಂತಿರುಗಿ 0;
}


ಪೂರ್ಣಾಂಕ log__init(ರಚನೆ mosquitto__config *config)
{
	ಪೂರ್ಣಾಂಕ rc = 0;

	libcommon_vprintf = libcommon__vprintf;

	log_priorities = config->log_type;
	log_destinations = config->log_dest;

	ಆದರೆ(log_destinations & MQTT3_LOG_SYSLOG){
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
		openlog("mosquitto", LOG_PID|LOG_CONS, config->log_facility);
#ಇಲ್ಲದಿದ್ದರೆ
		syslog_h = OpenEventLog(ಖಾಲಿ, "mosquitto");
#ಆದರೆ_ಅಂತ್ಯ
	}

	ಆದರೆ(log_destinations & MQTT3_LOG_FILE){
		config->log_fptr = mosquitto_fopen(config->log_file, "at", true);
		ಆದರೆ(config->log_fptr){
			setvbuf(config->log_fptr, log_fptr_ಬಫರ್, _IOLBF, ಗಾತ್ರ(log_fptr_ಬಫರ್));
		}ಇಲ್ಲದಿದ್ದರೆ{
			log_destinations = MQTT3_LOG_STDERR;
			log_priorities = MOSQ_LOG_ERR;
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Unable to open log file %s for writing.", config->log_file);
		}
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
	ಆದರೆ(log_destinations & MQTT3_LOG_STDOUT){
		setvbuf(stdout, ಖಾಲಿ, _IOLBF, 0);
	}
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_DLT
	ಆದರೆ(log_destinations & MQTT3_LOG_DLT){
		dlt_fifo_check();
		ಆದರೆ(dlt_allowed){
			DLT_ರಿಜಿಸ್ಟರ್_APP("MQTT", "mosquitto log");
			dlt_ರಿಜಿಸ್ಟರ್_context(&dltContext, "MQTT", "mosquitto DLT context");
		}
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ rc;
}


ಪೂರ್ಣಾಂಕ log__close(ರಚನೆ mosquitto__config *config)
{
	ಆದರೆ(log_destinations & MQTT3_LOG_SYSLOG){
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
		closelog();
#ಇಲ್ಲದಿದ್ದರೆ
		CloseEventLog(syslog_h);
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಆದರೆ(log_destinations & MQTT3_LOG_FILE){
		ಆದರೆ(config->log_fptr){
			fclose(config->log_fptr);
			config->log_fptr = ಖಾಲಿ;
		}
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_DLT
	ಆದರೆ(dlt_allowed){
		dlt_unregister_context(&dltContext);
		DLT_UNREGISTER_APP();
	}
#ಆದರೆ_ಅಂತ್ಯ
	/* FIXME - do something for all destinations! */
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_DLT


DltLogLevelType get_dlt_level(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಆದ್ಯತೆ)
{
	ಆಯ್ಕೆ(ಆದ್ಯತೆ){
		ಸ್ಥಿತಿ MOSQ_LOG_ERR:
			ಹಿಂತಿರುಗಿ DLT_LOG_ERROR;
		ಸ್ಥಿತಿ MOSQ_LOG_WARNING:
			ಹಿಂತಿರುಗಿ DLT_LOG_WARN;
		ಸ್ಥಿತಿ MOSQ_LOG_INFO:
			ಹಿಂತಿರುಗಿ DLT_LOG_INFO;
		ಸ್ಥಿತಿ MOSQ_LOG_DEBUG:
			ಹಿಂತಿರುಗಿ DLT_LOG_DEBUG;
		ಸ್ಥಿತಿ MOSQ_LOG_NOTICE:
		ಸ್ಥಿತಿ MOSQ_LOG_SUBSCRIBE:
		ಸ್ಥಿತಿ MOSQ_LOG_UNSUBSCRIBE:
			ಹಿಂತಿರುಗಿ DLT_LOG_VERBOSE;
		ಪೂರ್ವನಿಯೋಜಿತ:
			ಹಿಂತಿರುಗಿ DLT_LOG_DEFAULT;
	}
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ANDROID


android_Log_ಆದ್ಯತೆ get_android_level(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಆದ್ಯತೆ)
{
	ಆಯ್ಕೆ(ಆದ್ಯತೆ){
		ಸ್ಥಿತಿ MOSQ_LOG_ERR:
			ಹಿಂತಿರುಗಿ ANDROID_LOG_ERROR;
		ಸ್ಥಿತಿ MOSQ_LOG_WARNING:
			ಹಿಂತಿರುಗಿ ANDROID_LOG_WARN;
		ಸ್ಥಿತಿ MOSQ_LOG_INFO:
			ಹಿಂತಿರುಗಿ ANDROID_LOG_INFO;
		ಸ್ಥಿತಿ MOSQ_LOG_DEBUG:
			ಹಿಂತಿರುಗಿ ANDROID_LOG_DEBUG;
		ಸ್ಥಿತಿ MOSQ_LOG_NOTICE:
		ಸ್ಥಿತಿ MOSQ_LOG_SUBSCRIBE:
		ಸ್ಥಿತಿ MOSQ_LOG_UNSUBSCRIBE:
			ಹಿಂತಿರುಗಿ ANDROID_LOG_VERBOSE;
		ಪೂರ್ವನಿಯೋಜಿತ:
			ಹಿಂತಿರುಗಿ ANDROID_LOG_DEBUG;
	}
}
#ಆದರೆ_ಅಂತ್ಯ


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ log__vprintf(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಆದ್ಯತೆ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, va_list va)
{
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *topic;
	ಪೂರ್ಣಾಂಕ syslog_ಆದ್ಯತೆ;
	ಅಕ್ಷರ log_ಸಾಲು[1000];
	size_t log_ಸಾಲು_pos;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WIN32
	ಅಕ್ಷರ *sp;
#ಆದರೆ_ಅಂತ್ಯ
	bool log_timestamp = true;
	ಅಕ್ಷರ *log_timestamp_format = ಖಾಲಿ;
	FILE *log_fptr = ಖಾಲಿ;

	ಆದರೆ(db.config){
		log_timestamp = db.config->log_timestamp;
		log_timestamp_format = db.config->log_timestamp_format;
		log_fptr = db.config->log_fptr;
	}

	ಆದರೆ((log_priorities & ಆದ್ಯತೆ) && log_destinations != MQTT3_LOG_NONE){
		ಆಯ್ಕೆ(ಆದ್ಯತೆ){
			ಸ್ಥಿತಿ MOSQ_LOG_SUBSCRIBE:
				topic = "$SYS/broker/log/M/subscribe";
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
				syslog_ಆದ್ಯತೆ = LOG_NOTICE;
#ಇಲ್ಲದಿದ್ದರೆ
				syslog_ಆದ್ಯತೆ = EVENTLOG_INFORMATION_TYPE;
#ಆದರೆ_ಅಂತ್ಯ
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ MOSQ_LOG_UNSUBSCRIBE:
				topic = "$SYS/broker/log/M/unsubscribe";
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
				syslog_ಆದ್ಯತೆ = LOG_NOTICE;
#ಇಲ್ಲದಿದ್ದರೆ
				syslog_ಆದ್ಯತೆ = EVENTLOG_INFORMATION_TYPE;
#ಆದರೆ_ಅಂತ್ಯ
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ MOSQ_LOG_DEBUG:
				topic = "$SYS/broker/log/D";
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
				syslog_ಆದ್ಯತೆ = LOG_DEBUG;
#ಇಲ್ಲದಿದ್ದರೆ
				syslog_ಆದ್ಯತೆ = EVENTLOG_INFORMATION_TYPE;
#ಆದರೆ_ಅಂತ್ಯ
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ MOSQ_LOG_ERR:
				topic = "$SYS/broker/log/E";
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
				syslog_ಆದ್ಯತೆ = LOG_ERR;
#ಇಲ್ಲದಿದ್ದರೆ
				syslog_ಆದ್ಯತೆ = EVENTLOG_ERROR_TYPE;
#ಆದರೆ_ಅಂತ್ಯ
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ MOSQ_LOG_WARNING:
				topic = "$SYS/broker/log/W";
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
				syslog_ಆದ್ಯತೆ = LOG_WARNING;
#ಇಲ್ಲದಿದ್ದರೆ
				syslog_ಆದ್ಯತೆ = EVENTLOG_WARNING_TYPE;
#ಆದರೆ_ಅಂತ್ಯ
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ MOSQ_LOG_NOTICE:
				topic = "$SYS/broker/log/N";
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
				syslog_ಆದ್ಯತೆ = LOG_NOTICE;
#ಇಲ್ಲದಿದ್ದರೆ
				syslog_ಆದ್ಯತೆ = EVENTLOG_INFORMATION_TYPE;
#ಆದರೆ_ಅಂತ್ಯ
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ MOSQ_LOG_INFO:
				topic = "$SYS/broker/log/I";
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
				syslog_ಆದ್ಯತೆ = LOG_INFO;
#ಇಲ್ಲದಿದ್ದರೆ
				syslog_ಆದ್ಯತೆ = EVENTLOG_INFORMATION_TYPE;
#ಆದರೆ_ಅಂತ್ಯ
				ನಿಲ್ಲಿಸು;
#ಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			ಸ್ಥಿತಿ MOSQ_LOG_WEBSOCKETS:
				topic = "$SYS/broker/log/WS";
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
				syslog_ಆದ್ಯತೆ = LOG_DEBUG;
#ಇಲ್ಲದಿದ್ದರೆ
				syslog_ಆದ್ಯತೆ = EVENTLOG_INFORMATION_TYPE;
#ಆದರೆ_ಅಂತ್ಯ
				ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
			ಪೂರ್ವನಿಯೋಜಿತ:
				topic = "$SYS/broker/log/E";
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
				syslog_ಆದ್ಯತೆ = LOG_ERR;
#ಇಲ್ಲದಿದ್ದರೆ
				syslog_ಆದ್ಯತೆ = EVENTLOG_ERROR_TYPE;
#ಆದರೆ_ಅಂತ್ಯ
		}
		ಆದರೆ(log_timestamp){
			ಆದರೆ(log_timestamp_format){
				ರಚನೆ tm *ti = ಖಾಲಿ;
				get_ಸಮಯ(&ti);
				log_ಸಾಲು_pos = strftime(log_ಸಾಲು, ಗಾತ್ರ(log_ಸಾಲು), log_timestamp_format, ti);
				ಆದರೆ(log_ಸಾಲು_pos == 0){
					log_ಸಾಲು_pos = (size_t)snprintf(log_ಸಾಲು, ಗಾತ್ರ(log_ಸಾಲು), "Time error");
				}
			}ಇಲ್ಲದಿದ್ದರೆ{
				log_ಸಾಲು_pos = (size_t)snprintf(log_ಸಾಲು, ಗಾತ್ರ(log_ಸಾಲು), "%" PRIu64, (uint64_t)db.now_real_s);
			}
			ಆದರೆ(log_ಸಾಲು_pos < ಗಾತ್ರ(log_ಸಾಲು)-3){
				log_ಸಾಲು[log_ಸಾಲು_pos] = ':';
				log_ಸಾಲು[log_ಸಾಲು_pos+1] = ' ';
				log_ಸಾಲು[log_ಸಾಲು_pos+2] = '\0';
				log_ಸಾಲು_pos += 2;
			}
		}ಇಲ್ಲದಿದ್ದರೆ{
			log_ಸಾಲು_pos = 0;
		}
		vsnprintf(&log_ಸಾಲು[log_ಸಾಲು_pos], ಗಾತ್ರ(log_ಸಾಲು)-log_ಸಾಲು_pos, fmt, va);
		log_ಸಾಲು[ಗಾತ್ರ(log_ಸಾಲು)-1] = '\0'; /* Ensure string is null terminated. */

		ಆದರೆ(log_destinations & MQTT3_LOG_STDOUT){
			fprintf(stdout, "%s\n", log_ಸಾಲು);
		}
		ಆದರೆ(log_destinations & MQTT3_LOG_STDERR){
			fprintf(stderr, "%s\n", log_ಸಾಲು);
		}
		ಆದರೆ(log_destinations & MQTT3_LOG_FILE && log_fptr){
			fprintf(log_fptr, "%s\n", log_ಸಾಲು);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WIN32
			/* Windows doesn't support line buffering, so flush. */
			fflush(log_fptr);
#ಆದರೆ_ಅಂತ್ಯ
		}
		ಆದರೆ(log_destinations & MQTT3_LOG_SYSLOG){
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
			syslog(syslog_ಆದ್ಯತೆ, "%s", log_ಸಾಲು);
#ಇಲ್ಲದಿದ್ದರೆ
			sp = (ಅಕ್ಷರ *)log_ಸಾಲು;
			ReportEvent(syslog_h, syslog_ಆದ್ಯತೆ, 0, 0, ಖಾಲಿ, 1, 0, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **)&sp, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
		}
		ಆದರೆ(log_destinations & MQTT3_LOG_TOPIC && ಆದ್ಯತೆ != MOSQ_LOG_DEBUG && ಆದ್ಯತೆ != MOSQ_LOG_ಆಂತರಿಕ){
			db_messages_easy_ಸರತಿ(ಖಾಲಿ, topic, 2, (uint32_t)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(log_ಸಾಲು), log_ಸಾಲು, 0, 20, ಖಾಲಿ);
		}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_DLT
		ಆದರೆ(log_destinations & MQTT3_LOG_DLT && ಆದ್ಯತೆ != MOSQ_LOG_ಆಂತರಿಕ){
			DLT_LOG_STRING(dltContext, get_dlt_level(ಆದ್ಯತೆ), log_ಸಾಲು);
		}
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ANDROID
		ಆದರೆ(log_destinations & MQTT3_LOG_ANDROID && ಆದ್ಯತೆ != MOSQ_LOG_ಆಂತರಿಕ){
			__android_log_write(get_android_level(ಆದ್ಯತೆ), LOG_ಟ್ಯಾಗ್, log_ಸಾಲು);
		}
#ಆದರೆ_ಅಂತ್ಯ
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಪೂರ್ಣಾಂಕ log__printf(ರಚನೆ mosquitto *mosq, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಆದ್ಯತೆ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
	va_list va;
	ಪೂರ್ಣಾಂಕ rc;

	ಬಳಸದ(mosq);

	va_ಆರಂಭ(va, fmt);
	rc = log__vprintf(ಆದ್ಯತೆ, fmt, va);
	va_ಕೊನೆ(va);

	ಹಿಂತಿರುಗಿ rc;
}


ಶೂನ್ಯ log_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
	va_list va;
	ಅಕ್ಷರ buf[200];
	ಪೂರ್ಣಾಂಕ len;

	va_ಆರಂಭ(va, fmt);
	len = vsnprintf(buf, 200, fmt, va);
	va_ಕೊನೆ(va);

	ಆದರೆ(len >= 200){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ಆಂತರಿಕ, "Internal log buffer too short (%d)", len);
		ಹಿಂತಿರುಗಿ;
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WIN32
	log__printf(ಖಾಲಿ, MOSQ_LOG_ಆಂತರಿಕ, "%s", buf);
#ಇಲ್ಲದಿದ್ದರೆ
	log__printf(ಖಾಲಿ, MOSQ_LOG_ಆಂತರಿಕ, "%s%s%s", "\e[32m", buf, "\e[0m");
#ಆದರೆ_ಅಂತ್ಯ
}


BROKER_EXPORT ಪೂರ್ಣಾಂಕ mosquitto_log_vprintf(ಪೂರ್ಣಾಂಕ level, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, va_list va)
{
	ಹಿಂತಿರುಗಿ log__vprintf((ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)level, fmt, va);
}


BROKER_EXPORT ಶೂನ್ಯ mosquitto_log_printf(ಪೂರ್ಣಾಂಕ level, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
	va_list va;

	va_ಆರಂಭ(va, fmt);
	log__vprintf((ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)level, fmt, va);
	va_ಕೊನೆ(va);
}


ಸ್ಥಿರ ಶೂನ್ಯ libcommon__vprintf(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, va_list va)
{
	log__vprintf(MOSQ_LOG_INFO, fmt, va);
}
