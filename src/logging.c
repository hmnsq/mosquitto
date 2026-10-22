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
#شامل "config.h"

#شامل <inttypes.h>
#شامل <stdarg.h>
#شامل <stdio.h>
#شامل <string.h>
#شامل <inttypes.h>
#اگر_غیر_تعریف_شدہ WIN32
#شامل <syslog.h>
#اگر_ختم
#شامل <time.h>

#اگر defined(__APPLE__)
#  include <sys/time.h>
#اگر_ختم

#اگر_تعریف_شدہ WITH_DLT
#شامل <sys/stat.h>
#شامل <dlt/dlt.h>
#اگر_ختم

#شامل "logging_mosq.h"
#شامل "mosquitto_broker_internal.h"
#شامل "util_mosq.h"

#اگر_تعریف_شدہ WIN32
HANDLE syslog_h;
#اگر_ختم

#اگر_تعریف_شدہ ANDROID
#شامل <android/log.h>
ساکن ثابت حرف *LOG_لیبل = "mosquitto";
#اگر_ختم

ساکن حرف log_fptr_بفر[BUFSIZ];
ساکن خالی libcommon__vprintf(ثابت حرف *fmt, va_list va);

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
ساکن غیرعلامتی صحیح log_destinations = MQTT3_LOG_STDERR;
ساکن غیرعلامتی صحیح log_priorities = MOSQ_LOG_ERR | MOSQ_LOG_WARNING | MOSQ_LOG_NOTICE | MOSQ_LOG_INFO;

#اگر_تعریف_شدہ WITH_DLT
ساکن DltContext dltContext;
ساکن bool dlt_allowed = false;


خالی dlt_fifo_check(خالی)
{
	ساخت stat statbuf;

	/* If we start DLT but the /tmp/dlt fifo doesn't exist, or isn't available
	 * for writing then there is a big delay when we try and close the log
	 * later, so check for it first. This has the side effect of not letting
	 * people using DLT create the fifo after Mosquitto has started, but at the
	 * benefit of not having a massive delay for everybody else. */
	memset(&statbuf, 0, حجم(statbuf));
	اگر(stat("/tmp/dlt", &statbuf) == 0){
		اگر(S_ISFIFO(statbuf.st_منوال)){
			صحیح fd = open("/tmp/dlt", O_NONBLOCK | O_WRONLY);
			اگر(fd != -1){
				dlt_allowed = true;
				close(fd);
			}
		}
	}
}
#اگر_ختم


ساکن صحیح get_وقت(ساخت tm **ti)
{
	time_t s;

	s = db.now_real_s;

	*ti = localtime(&s);
	اگر(!(*ti)){
		fprintf(stderr, "Error obtaining system time.\n");
		واپس 1;
	}

	واپس 0;
}


صحیح log__init(ساخت mosquitto__config *config)
{
	صحیح rc = 0;

	libcommon_vprintf = libcommon__vprintf;

	log_priorities = config->log_type;
	log_destinations = config->log_dest;

	اگر(log_destinations & MQTT3_LOG_SYSLOG){
#اگر_غیر_تعریف_شدہ WIN32
		openlog("mosquitto", LOG_PID|LOG_CONS, config->log_facility);
#ورنہ
		syslog_h = OpenEventLog(تہی, "mosquitto");
#اگر_ختم
	}

	اگر(log_destinations & MQTT3_LOG_FILE){
		config->log_fptr = mosquitto_fopen(config->log_file, "at", true);
		اگر(config->log_fptr){
			setvbuf(config->log_fptr, log_fptr_بفر, _IOLBF, حجم(log_fptr_بفر));
		}ورنہ{
			log_destinations = MQTT3_LOG_STDERR;
			log_priorities = MOSQ_LOG_ERR;
			log__printf(تہی, MOSQ_LOG_ERR, "Error: Unable to open log file %s for writing.", config->log_file);
		}
	}
#اگر_غیر_تعریف_شدہ WIN32
	اگر(log_destinations & MQTT3_LOG_STDOUT){
		setvbuf(stdout, تہی, _IOLBF, 0);
	}
#اگر_ختم
#اگر_تعریف_شدہ WITH_DLT
	اگر(log_destinations & MQTT3_LOG_DLT){
		dlt_fifo_check();
		اگر(dlt_allowed){
			DLT_رجسٹر_APP("MQTT", "mosquitto log");
			dlt_رجسٹر_context(&dltContext, "MQTT", "mosquitto DLT context");
		}
	}
#اگر_ختم
	واپس rc;
}


صحیح log__close(ساخت mosquitto__config *config)
{
	اگر(log_destinations & MQTT3_LOG_SYSLOG){
#اگر_غیر_تعریف_شدہ WIN32
		closelog();
#ورنہ
		CloseEventLog(syslog_h);
#اگر_ختم
	}
	اگر(log_destinations & MQTT3_LOG_FILE){
		اگر(config->log_fptr){
			fclose(config->log_fptr);
			config->log_fptr = تہی;
		}
	}

#اگر_تعریف_شدہ WITH_DLT
	اگر(dlt_allowed){
		dlt_unregister_context(&dltContext);
		DLT_UNREGISTER_APP();
	}
#اگر_ختم
	/* FIXME - do something for all destinations! */
	واپس MOSQ_ERR_SUCCESS;
}

#اگر_تعریف_شدہ WITH_DLT


DltLogLevelType get_dlt_level(غیرعلامتی صحیح ترجیح)
{
	انتخاب(ترجیح){
		حالت MOSQ_LOG_ERR:
			واپس DLT_LOG_ERROR;
		حالت MOSQ_LOG_WARNING:
			واپس DLT_LOG_WARN;
		حالت MOSQ_LOG_INFO:
			واپس DLT_LOG_INFO;
		حالت MOSQ_LOG_DEBUG:
			واپس DLT_LOG_DEBUG;
		حالت MOSQ_LOG_NOTICE:
		حالت MOSQ_LOG_SUBSCRIBE:
		حالت MOSQ_LOG_UNSUBSCRIBE:
			واپس DLT_LOG_VERBOSE;
		طےشدہ:
			واپس DLT_LOG_DEFAULT;
	}
}
#اگر_ختم

#اگر_تعریف_شدہ ANDROID


android_Log_ترجیح get_android_level(غیرعلامتی صحیح ترجیح)
{
	انتخاب(ترجیح){
		حالت MOSQ_LOG_ERR:
			واپس ANDROID_LOG_ERROR;
		حالت MOSQ_LOG_WARNING:
			واپس ANDROID_LOG_WARN;
		حالت MOSQ_LOG_INFO:
			واپس ANDROID_LOG_INFO;
		حالت MOSQ_LOG_DEBUG:
			واپس ANDROID_LOG_DEBUG;
		حالت MOSQ_LOG_NOTICE:
		حالت MOSQ_LOG_SUBSCRIBE:
		حالت MOSQ_LOG_UNSUBSCRIBE:
			واپس ANDROID_LOG_VERBOSE;
		طےشدہ:
			واپس ANDROID_LOG_DEBUG;
	}
}
#اگر_ختم


ساکن صحیح log__vprintf(غیرعلامتی صحیح ترجیح, ثابت حرف *fmt, va_list va)
{
	ثابت حرف *topic;
	صحیح syslog_ترجیح;
	حرف log_سطر[1000];
	size_t log_سطر_pos;
#اگر_تعریف_شدہ WIN32
	حرف *sp;
#اگر_ختم
	bool log_timestamp = true;
	حرف *log_timestamp_format = تہی;
	FILE *log_fptr = تہی;

	اگر(db.config){
		log_timestamp = db.config->log_timestamp;
		log_timestamp_format = db.config->log_timestamp_format;
		log_fptr = db.config->log_fptr;
	}

	اگر((log_priorities & ترجیح) && log_destinations != MQTT3_LOG_NONE){
		انتخاب(ترجیح){
			حالت MOSQ_LOG_SUBSCRIBE:
				topic = "$SYS/broker/log/M/subscribe";
#اگر_غیر_تعریف_شدہ WIN32
				syslog_ترجیح = LOG_NOTICE;
#ورنہ
				syslog_ترجیح = EVENTLOG_INFORMATION_TYPE;
#اگر_ختم
				توڑو;
			حالت MOSQ_LOG_UNSUBSCRIBE:
				topic = "$SYS/broker/log/M/unsubscribe";
#اگر_غیر_تعریف_شدہ WIN32
				syslog_ترجیح = LOG_NOTICE;
#ورنہ
				syslog_ترجیح = EVENTLOG_INFORMATION_TYPE;
#اگر_ختم
				توڑو;
			حالت MOSQ_LOG_DEBUG:
				topic = "$SYS/broker/log/D";
#اگر_غیر_تعریف_شدہ WIN32
				syslog_ترجیح = LOG_DEBUG;
#ورنہ
				syslog_ترجیح = EVENTLOG_INFORMATION_TYPE;
#اگر_ختم
				توڑو;
			حالت MOSQ_LOG_ERR:
				topic = "$SYS/broker/log/E";
#اگر_غیر_تعریف_شدہ WIN32
				syslog_ترجیح = LOG_ERR;
#ورنہ
				syslog_ترجیح = EVENTLOG_ERROR_TYPE;
#اگر_ختم
				توڑو;
			حالت MOSQ_LOG_WARNING:
				topic = "$SYS/broker/log/W";
#اگر_غیر_تعریف_شدہ WIN32
				syslog_ترجیح = LOG_WARNING;
#ورنہ
				syslog_ترجیح = EVENTLOG_WARNING_TYPE;
#اگر_ختم
				توڑو;
			حالت MOSQ_LOG_NOTICE:
				topic = "$SYS/broker/log/N";
#اگر_غیر_تعریف_شدہ WIN32
				syslog_ترجیح = LOG_NOTICE;
#ورنہ
				syslog_ترجیح = EVENTLOG_INFORMATION_TYPE;
#اگر_ختم
				توڑو;
			حالت MOSQ_LOG_INFO:
				topic = "$SYS/broker/log/I";
#اگر_غیر_تعریف_شدہ WIN32
				syslog_ترجیح = LOG_INFO;
#ورنہ
				syslog_ترجیح = EVENTLOG_INFORMATION_TYPE;
#اگر_ختم
				توڑو;
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			حالت MOSQ_LOG_WEBSOCKETS:
				topic = "$SYS/broker/log/WS";
#اگر_غیر_تعریف_شدہ WIN32
				syslog_ترجیح = LOG_DEBUG;
#ورنہ
				syslog_ترجیح = EVENTLOG_INFORMATION_TYPE;
#اگر_ختم
				توڑو;
#اگر_ختم
			طےشدہ:
				topic = "$SYS/broker/log/E";
#اگر_غیر_تعریف_شدہ WIN32
				syslog_ترجیح = LOG_ERR;
#ورنہ
				syslog_ترجیح = EVENTLOG_ERROR_TYPE;
#اگر_ختم
		}
		اگر(log_timestamp){
			اگر(log_timestamp_format){
				ساخت tm *ti = تہی;
				get_وقت(&ti);
				log_سطر_pos = strftime(log_سطر, حجم(log_سطر), log_timestamp_format, ti);
				اگر(log_سطر_pos == 0){
					log_سطر_pos = (size_t)snprintf(log_سطر, حجم(log_سطر), "Time error");
				}
			}ورنہ{
				log_سطر_pos = (size_t)snprintf(log_سطر, حجم(log_سطر), "%" PRIu64, (uint64_t)db.now_real_s);
			}
			اگر(log_سطر_pos < حجم(log_سطر)-3){
				log_سطر[log_سطر_pos] = ':';
				log_سطر[log_سطر_pos+1] = ' ';
				log_سطر[log_سطر_pos+2] = '\0';
				log_سطر_pos += 2;
			}
		}ورنہ{
			log_سطر_pos = 0;
		}
		vsnprintf(&log_سطر[log_سطر_pos], حجم(log_سطر)-log_سطر_pos, fmt, va);
		log_سطر[حجم(log_سطر)-1] = '\0'; /* Ensure string is null terminated. */

		اگر(log_destinations & MQTT3_LOG_STDOUT){
			fprintf(stdout, "%s\n", log_سطر);
		}
		اگر(log_destinations & MQTT3_LOG_STDERR){
			fprintf(stderr, "%s\n", log_سطر);
		}
		اگر(log_destinations & MQTT3_LOG_FILE && log_fptr){
			fprintf(log_fptr, "%s\n", log_سطر);
#اگر_تعریف_شدہ WIN32
			/* Windows doesn't support line buffering, so flush. */
			fflush(log_fptr);
#اگر_ختم
		}
		اگر(log_destinations & MQTT3_LOG_SYSLOG){
#اگر_غیر_تعریف_شدہ WIN32
			syslog(syslog_ترجیح, "%s", log_سطر);
#ورنہ
			sp = (حرف *)log_سطر;
			ReportEvent(syslog_h, syslog_ترجیح, 0, 0, تہی, 1, 0, (ثابت حرف **)&sp, تہی);
#اگر_ختم
		}
		اگر(log_destinations & MQTT3_LOG_TOPIC && ترجیح != MOSQ_LOG_DEBUG && ترجیح != MOSQ_LOG_اندرونی){
			db_messages_easy_قطار(تہی, topic, 2, (uint32_t)لڑی_لمبائی(log_سطر), log_سطر, 0, 20, تہی);
		}
#اگر_تعریف_شدہ WITH_DLT
		اگر(log_destinations & MQTT3_LOG_DLT && ترجیح != MOSQ_LOG_اندرونی){
			DLT_LOG_STRING(dltContext, get_dlt_level(ترجیح), log_سطر);
		}
#اگر_ختم
#اگر_تعریف_شدہ ANDROID
		اگر(log_destinations & MQTT3_LOG_ANDROID && ترجیح != MOSQ_LOG_اندرونی){
			__android_log_write(get_android_level(ترجیح), LOG_لیبل, log_سطر);
		}
#اگر_ختم
	}

	واپس MOSQ_ERR_SUCCESS;
}


صحیح log__printf(ساخت mosquitto *mosq, غیرعلامتی صحیح ترجیح, ثابت حرف *fmt, ...)
{
	va_list va;
	صحیح rc;

	غیر_مستعمل(mosq);

	va_آغاز(va, fmt);
	rc = log__vprintf(ترجیح, fmt, va);
	va_انتہا(va);

	واپس rc;
}


خالی log_اندرونی(ثابت حرف *fmt, ...)
{
	va_list va;
	حرف buf[200];
	صحیح len;

	va_آغاز(va, fmt);
	len = vsnprintf(buf, 200, fmt, va);
	va_انتہا(va);

	اگر(len >= 200){
		log__printf(تہی, MOSQ_LOG_اندرونی, "Internal log buffer too short (%d)", len);
		واپس;
	}

#اگر_تعریف_شدہ WIN32
	log__printf(تہی, MOSQ_LOG_اندرونی, "%s", buf);
#ورنہ
	log__printf(تہی, MOSQ_LOG_اندرونی, "%s%s%s", "\e[32m", buf, "\e[0m");
#اگر_ختم
}


BROKER_EXPORT صحیح mosquitto_log_vprintf(صحیح level, ثابت حرف *fmt, va_list va)
{
	واپس log__vprintf((غیرعلامتی صحیح)level, fmt, va);
}


BROKER_EXPORT خالی mosquitto_log_printf(صحیح level, ثابت حرف *fmt, ...)
{
	va_list va;

	va_آغاز(va, fmt);
	log__vprintf((غیرعلامتی صحیح)level, fmt, va);
	va_انتہا(va);
}


ساکن خالی libcommon__vprintf(ثابت حرف *fmt, va_list va)
{
	log__vprintf(MOSQ_LOG_INFO, fmt, va);
}
