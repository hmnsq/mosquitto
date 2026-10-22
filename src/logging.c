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
#ifndef WIN32
#شامل <syslog.h>
#endif
#شامل <time.h>

#اگر defined(__APPLE__)
#  include <sys/time.h>
#endif

#ifdef WITH_DLT
#شامل <sys/stat.h>
#شامل <dlt/dlt.h>
#endif

#شامل "logging_mosq.h"
#شامل "mosquitto_broker_internal.h"
#شامل "util_mosq.h"

#ifdef WIN32
HANDLE syslog_h;
#endif

#ifdef ANDROID
#شامل <android/log.h>
ایستا ثابت نویسه *LOG_TAG = "mosquitto";
#endif

ایستا نویسه log_fptr_buffer[BUFSIZ];
ایستا تهی libcommon__vprintf(ثابت نویسه *fmt, va_list va);

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
ایستا unsigned صحیح log_destinations = MQTT3_LOG_STDERR;
ایستا unsigned صحیح log_priorities = MOSQ_LOG_ERR | MOSQ_LOG_WARNING | MOSQ_LOG_NOTICE | MOSQ_LOG_INFO;

#ifdef WITH_DLT
ایستا DltContext dltContext;
ایستا bool dlt_allowed = false;


تهی dlt_fifo_check(تهی)
{
	ساختار stat statbuf;

	/* If we start DLT but the /tmp/dlt fifo doesn't exist, or isn't available
	 * for writing then there is a big delay when we try and close the log
	 * later, so check for it first. This has the side effect of not letting
	 * people using DLT create the fifo after Mosquitto has started, but at the
	 * benefit of not having a massive delay for everybody else. */
	memset(&statbuf, 0, اندازه(statbuf));
	اگر(stat("/tmp/dlt", &statbuf) == 0){
		اگر(S_ISFIFO(statbuf.st_mode)){
			صحیح fd = open("/tmp/dlt", O_NONBLOCK | O_WRONLY);
			اگر(fd != -1){
				dlt_allowed = true;
				close(fd);
			}
		}
	}
}
#endif


ایستا صحیح get_time(ساختار tm **ti)
{
	time_t s;

	s = db.now_real_s;

	*ti = localtime(&s);
	اگر(!(*ti)){
		fprintf(stderr, "Error obtaining system time.\n");
		بازگشت 1;
	}

	بازگشت 0;
}


صحیح log__init(ساختار mosquitto__config *config)
{
	صحیح rc = 0;

	libcommon_vprintf = libcommon__vprintf;

	log_priorities = config->log_type;
	log_destinations = config->log_dest;

	اگر(log_destinations & MQTT3_LOG_SYSLOG){
#ifndef WIN32
		openlog("mosquitto", LOG_PID|LOG_CONS, config->log_facility);
#وگرنه
		syslog_h = OpenEventLog(پوچ, "mosquitto");
#endif
	}

	اگر(log_destinations & MQTT3_LOG_FILE){
		config->log_fptr = mosquitto_fopen(config->log_file, "at", true);
		اگر(config->log_fptr){
			setvbuf(config->log_fptr, log_fptr_buffer, _IOLBF, اندازه(log_fptr_buffer));
		}وگرنه{
			log_destinations = MQTT3_LOG_STDERR;
			log_priorities = MOSQ_LOG_ERR;
			log__printf(پوچ, MOSQ_LOG_ERR, "Error: Unable to open log file %s for writing.", config->log_file);
		}
	}
#ifndef WIN32
	اگر(log_destinations & MQTT3_LOG_STDOUT){
		setvbuf(stdout, پوچ, _IOLBF, 0);
	}
#endif
#ifdef WITH_DLT
	اگر(log_destinations & MQTT3_LOG_DLT){
		dlt_fifo_check();
		اگر(dlt_allowed){
			DLT_REGISTER_APP("MQTT", "mosquitto log");
			dlt_register_context(&dltContext, "MQTT", "mosquitto DLT context");
		}
	}
#endif
	بازگشت rc;
}


صحیح log__close(ساختار mosquitto__config *config)
{
	اگر(log_destinations & MQTT3_LOG_SYSLOG){
#ifndef WIN32
		closelog();
#وگرنه
		CloseEventLog(syslog_h);
#endif
	}
	اگر(log_destinations & MQTT3_LOG_FILE){
		اگر(config->log_fptr){
			fclose(config->log_fptr);
			config->log_fptr = پوچ;
		}
	}

#ifdef WITH_DLT
	اگر(dlt_allowed){
		dlt_unregister_context(&dltContext);
		DLT_UNREGISTER_APP();
	}
#endif
	/* FIXME - do something for all destinations! */
	بازگشت MOSQ_ERR_SUCCESS;
}

#ifdef WITH_DLT


DltLogLevelType get_dlt_level(unsigned صحیح priority)
{
	انتخاب(priority){
		حالت MOSQ_LOG_ERR:
			بازگشت DLT_LOG_ERROR;
		حالت MOSQ_LOG_WARNING:
			بازگشت DLT_LOG_WARN;
		حالت MOSQ_LOG_INFO:
			بازگشت DLT_LOG_INFO;
		حالت MOSQ_LOG_DEBUG:
			بازگشت DLT_LOG_DEBUG;
		حالت MOSQ_LOG_NOTICE:
		حالت MOSQ_LOG_SUBSCRIBE:
		حالت MOSQ_LOG_UNSUBSCRIBE:
			بازگشت DLT_LOG_VERBOSE;
		پیشفرض:
			بازگشت DLT_LOG_DEFAULT;
	}
}
#endif

#ifdef ANDROID


android_LogPriority get_android_level(unsigned صحیح priority)
{
	انتخاب(priority){
		حالت MOSQ_LOG_ERR:
			بازگشت ANDROID_LOG_ERROR;
		حالت MOSQ_LOG_WARNING:
			بازگشت ANDROID_LOG_WARN;
		حالت MOSQ_LOG_INFO:
			بازگشت ANDROID_LOG_INFO;
		حالت MOSQ_LOG_DEBUG:
			بازگشت ANDROID_LOG_DEBUG;
		حالت MOSQ_LOG_NOTICE:
		حالت MOSQ_LOG_SUBSCRIBE:
		حالت MOSQ_LOG_UNSUBSCRIBE:
			بازگشت ANDROID_LOG_VERBOSE;
		پیشفرض:
			بازگشت ANDROID_LOG_DEBUG;
	}
}
#endif


ایستا صحیح log__vprintf(unsigned صحیح priority, ثابت نویسه *fmt, va_list va)
{
	ثابت نویسه *topic;
	صحیح syslog_priority;
	نویسه log_سطر[1000];
	size_t log_سطر_pos;
#ifdef WIN32
	نویسه *sp;
#endif
	bool log_timestamp = true;
	نویسه *log_timestamp_format = پوچ;
	FILE *log_fptr = پوچ;

	اگر(db.config){
		log_timestamp = db.config->log_timestamp;
		log_timestamp_format = db.config->log_timestamp_format;
		log_fptr = db.config->log_fptr;
	}

	اگر((log_priorities & priority) && log_destinations != MQTT3_LOG_NONE){
		انتخاب(priority){
			حالت MOSQ_LOG_SUBSCRIBE:
				topic = "$SYS/broker/log/M/subscribe";
#ifndef WIN32
				syslog_priority = LOG_NOTICE;
#وگرنه
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				بشکن;
			حالت MOSQ_LOG_UNSUBSCRIBE:
				topic = "$SYS/broker/log/M/unsubscribe";
#ifndef WIN32
				syslog_priority = LOG_NOTICE;
#وگرنه
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				بشکن;
			حالت MOSQ_LOG_DEBUG:
				topic = "$SYS/broker/log/D";
#ifndef WIN32
				syslog_priority = LOG_DEBUG;
#وگرنه
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				بشکن;
			حالت MOSQ_LOG_ERR:
				topic = "$SYS/broker/log/E";
#ifndef WIN32
				syslog_priority = LOG_ERR;
#وگرنه
				syslog_priority = EVENTLOG_ERROR_TYPE;
#endif
				بشکن;
			حالت MOSQ_LOG_WARNING:
				topic = "$SYS/broker/log/W";
#ifndef WIN32
				syslog_priority = LOG_WARNING;
#وگرنه
				syslog_priority = EVENTLOG_WARNING_TYPE;
#endif
				بشکن;
			حالت MOSQ_LOG_NOTICE:
				topic = "$SYS/broker/log/N";
#ifndef WIN32
				syslog_priority = LOG_NOTICE;
#وگرنه
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				بشکن;
			حالت MOSQ_LOG_INFO:
				topic = "$SYS/broker/log/I";
#ifndef WIN32
				syslog_priority = LOG_INFO;
#وگرنه
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				بشکن;
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			حالت MOSQ_LOG_WEBSOCKETS:
				topic = "$SYS/broker/log/WS";
#ifndef WIN32
				syslog_priority = LOG_DEBUG;
#وگرنه
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				بشکن;
#endif
			پیشفرض:
				topic = "$SYS/broker/log/E";
#ifndef WIN32
				syslog_priority = LOG_ERR;
#وگرنه
				syslog_priority = EVENTLOG_ERROR_TYPE;
#endif
		}
		اگر(log_timestamp){
			اگر(log_timestamp_format){
				ساختار tm *ti = پوچ;
				get_time(&ti);
				log_سطر_pos = strftime(log_سطر, اندازه(log_سطر), log_timestamp_format, ti);
				اگر(log_سطر_pos == 0){
					log_سطر_pos = (size_t)snprintf(log_سطر, اندازه(log_سطر), "Time error");
				}
			}وگرنه{
				log_سطر_pos = (size_t)snprintf(log_سطر, اندازه(log_سطر), "%" PRIu64, (uint64_t)db.now_real_s);
			}
			اگر(log_سطر_pos < اندازه(log_سطر)-3){
				log_سطر[log_سطر_pos] = ':';
				log_سطر[log_سطر_pos+1] = ' ';
				log_سطر[log_سطر_pos+2] = '\0';
				log_سطر_pos += 2;
			}
		}وگرنه{
			log_سطر_pos = 0;
		}
		vsnprintf(&log_سطر[log_سطر_pos], اندازه(log_سطر)-log_سطر_pos, fmt, va);
		log_سطر[اندازه(log_سطر)-1] = '\0'; /* Ensure string is null terminated. */

		اگر(log_destinations & MQTT3_LOG_STDOUT){
			fprintf(stdout, "%s\n", log_سطر);
		}
		اگر(log_destinations & MQTT3_LOG_STDERR){
			fprintf(stderr, "%s\n", log_سطر);
		}
		اگر(log_destinations & MQTT3_LOG_FILE && log_fptr){
			fprintf(log_fptr, "%s\n", log_سطر);
#ifdef WIN32
			/* Windows doesn't support line buffering, so flush. */
			fflush(log_fptr);
#endif
		}
		اگر(log_destinations & MQTT3_LOG_SYSLOG){
#ifndef WIN32
			syslog(syslog_priority, "%s", log_سطر);
#وگرنه
			sp = (نویسه *)log_سطر;
			ReportEvent(syslog_h, syslog_priority, 0, 0, پوچ, 1, 0, (ثابت نویسه **)&sp, پوچ);
#endif
		}
		اگر(log_destinations & MQTT3_LOG_TOPIC && priority != MOSQ_LOG_DEBUG && priority != MOSQ_LOG_INTERNAL){
			db__messages_easy_queue(پوچ, topic, 2, (uint32_t)strlen(log_سطر), log_سطر, 0, 20, پوچ);
		}
#ifdef WITH_DLT
		اگر(log_destinations & MQTT3_LOG_DLT && priority != MOSQ_LOG_INTERNAL){
			DLT_LOG_STRING(dltContext, get_dlt_level(priority), log_سطر);
		}
#endif
#ifdef ANDROID
		اگر(log_destinations & MQTT3_LOG_ANDROID && priority != MOSQ_LOG_INTERNAL){
			__android_log_write(get_android_level(priority), LOG_TAG, log_سطر);
		}
#endif
	}

	بازگشت MOSQ_ERR_SUCCESS;
}


صحیح log__printf(ساختار mosquitto *mosq, unsigned صحیح priority, ثابت نویسه *fmt, ...)
{
	va_list va;
	صحیح rc;

	UNUSED(mosq);

	va_start(va, fmt);
	rc = log__vprintf(priority, fmt, va);
	va_end(va);

	بازگشت rc;
}


تهی log__internal(ثابت نویسه *fmt, ...)
{
	va_list va;
	نویسه buf[200];
	صحیح len;

	va_start(va, fmt);
	len = vsnprintf(buf, 200, fmt, va);
	va_end(va);

	اگر(len >= 200){
		log__printf(پوچ, MOSQ_LOG_INTERNAL, "Internal log buffer too short (%d)", len);
		بازگشت;
	}

#ifdef WIN32
	log__printf(پوچ, MOSQ_LOG_INTERNAL, "%s", buf);
#وگرنه
	log__printf(پوچ, MOSQ_LOG_INTERNAL, "%s%s%s", "\e[32m", buf, "\e[0m");
#endif
}


BROKER_EXPORT صحیح mosquitto_log_vprintf(صحیح level, ثابت نویسه *fmt, va_list va)
{
	بازگشت log__vprintf((unsigned صحیح)level, fmt, va);
}


BROKER_EXPORT تهی mosquitto_log_printf(صحیح level, ثابت نویسه *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	log__vprintf((unsigned صحیح)level, fmt, va);
	va_end(va);
}


ایستا تهی libcommon__vprintf(ثابت نویسه *fmt, va_list va)
{
	log__vprintf(MOSQ_LOG_INFO, fmt, va);
}
