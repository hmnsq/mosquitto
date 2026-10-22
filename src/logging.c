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
#अन्तर्भाव "config.h"

#अन्तर्भाव <inttypes.h>
#अन्तर्भाव <stdarg.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <inttypes.h>
#ifndef WIN32
#अन्तर्भाव <syslog.h>
#endif
#अन्तर्भाव <time.h>

#यदि defined(__APPLE__)
#  include <sys/time.h>
#endif

#ifdef WITH_DLT
#अन्तर्भाव <sys/stat.h>
#अन्तर्भाव <dlt/dlt.h>
#endif

#अन्तर्भाव "logging_mosq.h"
#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "util_mosq.h"

#ifdef WIN32
HANDLE syslog_h;
#endif

#ifdef ANDROID
#अन्तर्भाव <android/log.h>
स्थिर अचर वर्ण *LOG_TAG = "mosquitto";
#endif

स्थिर वर्ण log_fptr_buffer[बफ_मान];
स्थिर शून्य libcommon__vprintf(अचर वर्ण *fmt, बहु_सूची va);

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
स्थिर अचिन्हित पूर्णाङ्क log_destinations = MQTT3_LOG_STDERR;
स्थिर अचिन्हित पूर्णाङ्क log_priorities = MOSQ_LOG_ERR | MOSQ_LOG_WARNING | MOSQ_LOG_NOTICE | MOSQ_LOG_INFO;

#ifdef WITH_DLT
स्थिर DltContext dltContext;
स्थिर bool dlt_allowed = false;


शून्य dlt_fifo_check(शून्य)
{
	संरचना stat statbuf;

	/* If we start DLT but the /tmp/dlt fifo doesn't exist, or isn't available
	 * for writing then there is a big delay when we try and close the log
	 * later, so check for it first. This has the side effect of not letting
	 * people using DLT create the fifo after Mosquitto has started, but at the
	 * benefit of not having a massive delay for everybody else. */
	स_रखो(&statbuf, 0, परिमाण(statbuf));
	यदि(stat("/tmp/dlt", &statbuf) == 0){
		यदि(S_ISFIFO(statbuf.st_mode)){
			पूर्णाङ्क fd = खोलो("/tmp/dlt", O_NONBLOCK | O_WRONLY);
			यदि(fd != -1){
				dlt_allowed = true;
				बंद(fd);
			}
		}
	}
}
#endif


स्थिर पूर्णाङ्क get_time(संरचना पंचांग **ti)
{
	समय_प्रकार s;

	s = db.now_real_s;

	*ti = स_स्थानीय(&s);
	यदि(!(*ti)){
		ख_लिखो(मानक_त्रुटि, "Error obtaining system time.\n");
		फर्काऊ 1;
	}

	फर्काऊ 0;
}


पूर्णाङ्क log__init(संरचना mosquitto__config *config)
{
	पूर्णाङ्क rc = 0;

	libcommon_vprintf = libcommon__vprintf;

	log_priorities = config->log_type;
	log_destinations = config->log_dest;

	यदि(log_destinations & MQTT3_LOG_SYSLOG){
#ifndef WIN32
		openlog("mosquitto", LOG_PID|LOG_CONS, config->log_facility);
#अन्यथा
		syslog_h = OpenEventLog(रिक्त, "mosquitto");
#endif
	}

	यदि(log_destinations & MQTT3_LOG_FILE){
		config->log_fptr = mosquitto_fopen(config->log_file, "at", true);
		यदि(config->log_fptr){
			रखो_भबफ(config->log_fptr, log_fptr_buffer, _IOLBF, परिमाण(log_fptr_buffer));
		}अन्यथा{
			log_destinations = MQTT3_LOG_STDERR;
			log_priorities = MOSQ_LOG_ERR;
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to open log file %s for writing.", config->log_file);
		}
	}
#ifndef WIN32
	यदि(log_destinations & MQTT3_LOG_STDOUT){
		रखो_भबफ(मानक_निकास, रिक्त, _IOLBF, 0);
	}
#endif
#ifdef WITH_DLT
	यदि(log_destinations & MQTT3_LOG_DLT){
		dlt_fifo_check();
		यदि(dlt_allowed){
			DLT_REGISTER_APP("MQTT", "mosquitto log");
			dlt_register_context(&dltContext, "MQTT", "mosquitto DLT context");
		}
	}
#endif
	फर्काऊ rc;
}


पूर्णाङ्क log__close(संरचना mosquitto__config *config)
{
	यदि(log_destinations & MQTT3_LOG_SYSLOG){
#ifndef WIN32
		closelog();
#अन्यथा
		CloseEventLog(syslog_h);
#endif
	}
	यदि(log_destinations & MQTT3_LOG_FILE){
		यदि(config->log_fptr){
			ख_बंद(config->log_fptr);
			config->log_fptr = रिक्त;
		}
	}

#ifdef WITH_DLT
	यदि(dlt_allowed){
		dlt_unregister_context(&dltContext);
		DLT_UNREGISTER_APP();
	}
#endif
	/* FIXME - do something for all destinations! */
	फर्काऊ MOSQ_ERR_SUCCESS;
}

#ifdef WITH_DLT


DltLogLevelType get_dlt_level(अचिन्हित पूर्णाङ्क priority)
{
	विकल्प(priority){
		पक्ष MOSQ_LOG_ERR:
			फर्काऊ DLT_LOG_ERROR;
		पक्ष MOSQ_LOG_WARNING:
			फर्काऊ DLT_LOG_WARN;
		पक्ष MOSQ_LOG_INFO:
			फर्काऊ DLT_LOG_INFO;
		पक्ष MOSQ_LOG_DEBUG:
			फर्काऊ DLT_LOG_DEBUG;
		पक्ष MOSQ_LOG_NOTICE:
		पक्ष MOSQ_LOG_SUBSCRIBE:
		पक्ष MOSQ_LOG_UNSUBSCRIBE:
			फर्काऊ DLT_LOG_VERBOSE;
		स्वभाव:
			फर्काऊ DLT_LOG_DEFAULT;
	}
}
#endif

#ifdef ANDROID


android_LogPriority get_android_level(अचिन्हित पूर्णाङ्क priority)
{
	विकल्प(priority){
		पक्ष MOSQ_LOG_ERR:
			फर्काऊ ANDROID_LOG_ERROR;
		पक्ष MOSQ_LOG_WARNING:
			फर्काऊ ANDROID_LOG_WARN;
		पक्ष MOSQ_LOG_INFO:
			फर्काऊ ANDROID_LOG_INFO;
		पक्ष MOSQ_LOG_DEBUG:
			फर्काऊ ANDROID_LOG_DEBUG;
		पक्ष MOSQ_LOG_NOTICE:
		पक्ष MOSQ_LOG_SUBSCRIBE:
		पक्ष MOSQ_LOG_UNSUBSCRIBE:
			फर्काऊ ANDROID_LOG_VERBOSE;
		स्वभाव:
			फर्काऊ ANDROID_LOG_DEBUG;
	}
}
#endif


स्थिर पूर्णाङ्क log__vprintf(अचिन्हित पूर्णाङ्क priority, अचर वर्ण *fmt, बहु_सूची va)
{
	अचर वर्ण *topic;
	पूर्णाङ्क syslog_priority;
	वर्ण log_पङ्क्ति[1000];
	माप_प्रकार log_पङ्क्ति_pos;
#ifdef WIN32
	वर्ण *sp;
#endif
	bool log_timestamp = true;
	वर्ण *log_timestamp_format = रिक्त;
	खाता *log_fptr = रिक्त;

	यदि(db.config){
		log_timestamp = db.config->log_timestamp;
		log_timestamp_format = db.config->log_timestamp_format;
		log_fptr = db.config->log_fptr;
	}

	यदि((log_priorities & priority) && log_destinations != MQTT3_LOG_NONE){
		विकल्प(priority){
			पक्ष MOSQ_LOG_SUBSCRIBE:
				topic = "$SYS/broker/log/M/subscribe";
#ifndef WIN32
				syslog_priority = LOG_NOTICE;
#अन्यथा
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				रोक;
			पक्ष MOSQ_LOG_UNSUBSCRIBE:
				topic = "$SYS/broker/log/M/unsubscribe";
#ifndef WIN32
				syslog_priority = LOG_NOTICE;
#अन्यथा
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				रोक;
			पक्ष MOSQ_LOG_DEBUG:
				topic = "$SYS/broker/log/D";
#ifndef WIN32
				syslog_priority = LOG_DEBUG;
#अन्यथा
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				रोक;
			पक्ष MOSQ_LOG_ERR:
				topic = "$SYS/broker/log/E";
#ifndef WIN32
				syslog_priority = LOG_ERR;
#अन्यथा
				syslog_priority = EVENTLOG_ERROR_TYPE;
#endif
				रोक;
			पक्ष MOSQ_LOG_WARNING:
				topic = "$SYS/broker/log/W";
#ifndef WIN32
				syslog_priority = LOG_WARNING;
#अन्यथा
				syslog_priority = EVENTLOG_WARNING_TYPE;
#endif
				रोक;
			पक्ष MOSQ_LOG_NOTICE:
				topic = "$SYS/broker/log/N";
#ifndef WIN32
				syslog_priority = LOG_NOTICE;
#अन्यथा
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				रोक;
			पक्ष MOSQ_LOG_INFO:
				topic = "$SYS/broker/log/I";
#ifndef WIN32
				syslog_priority = LOG_INFO;
#अन्यथा
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				रोक;
#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			पक्ष MOSQ_LOG_WEBSOCKETS:
				topic = "$SYS/broker/log/WS";
#ifndef WIN32
				syslog_priority = LOG_DEBUG;
#अन्यथा
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#endif
				रोक;
#endif
			स्वभाव:
				topic = "$SYS/broker/log/E";
#ifndef WIN32
				syslog_priority = LOG_ERR;
#अन्यथा
				syslog_priority = EVENTLOG_ERROR_TYPE;
#endif
		}
		यदि(log_timestamp){
			यदि(log_timestamp_format){
				संरचना पंचांग *ti = रिक्त;
				get_time(&ti);
				log_पङ्क्ति_pos = स_माला(log_पङ्क्ति, परिमाण(log_पङ्क्ति), log_timestamp_format, ti);
				यदि(log_पङ्क्ति_pos == 0){
					log_पङ्क्ति_pos = (माप_प्रकार)snprintf(log_पङ्क्ति, परिमाण(log_पङ्क्ति), "Time error");
				}
			}अन्यथा{
				log_पङ्क्ति_pos = (माप_प्रकार)snprintf(log_पङ्क्ति, परिमाण(log_पङ्क्ति), "%" PRIu64, (uint64_t)db.now_real_s);
			}
			यदि(log_पङ्क्ति_pos < परिमाण(log_पङ्क्ति)-3){
				log_पङ्क्ति[log_पङ्क्ति_pos] = ':';
				log_पङ्क्ति[log_पङ्क्ति_pos+1] = ' ';
				log_पङ्क्ति[log_पङ्क्ति_pos+2] = '\0';
				log_पङ्क्ति_pos += 2;
			}
		}अन्यथा{
			log_पङ्क्ति_pos = 0;
		}
		vsnprintf(&log_पङ्क्ति[log_पङ्क्ति_pos], परिमाण(log_पङ्क्ति)-log_पङ्क्ति_pos, fmt, va);
		log_पङ्क्ति[परिमाण(log_पङ्क्ति)-1] = '\0'; /* Ensure string is null terminated. */

		यदि(log_destinations & MQTT3_LOG_STDOUT){
			ख_लिखो(मानक_निकास, "%s\n", log_पङ्क्ति);
		}
		यदि(log_destinations & MQTT3_LOG_STDERR){
			ख_लिखो(मानक_त्रुटि, "%s\n", log_पङ्क्ति);
		}
		यदि(log_destinations & MQTT3_LOG_FILE && log_fptr){
			ख_लिखो(log_fptr, "%s\n", log_पङ्क्ति);
#ifdef WIN32
			/* Windows doesn't support line buffering, so flush. */
			ख_साफ(log_fptr);
#endif
		}
		यदि(log_destinations & MQTT3_LOG_SYSLOG){
#ifndef WIN32
			syslog(syslog_priority, "%s", log_पङ्क्ति);
#अन्यथा
			sp = (वर्ण *)log_पङ्क्ति;
			ReportEvent(syslog_h, syslog_priority, 0, 0, रिक्त, 1, 0, (अचर वर्ण **)&sp, रिक्त);
#endif
		}
		यदि(log_destinations & MQTT3_LOG_TOPIC && priority != MOSQ_LOG_DEBUG && priority != MOSQ_LOG_INTERNAL){
			db__messages_easy_queue(रिक्त, topic, 2, (uint32_t)म_माप(log_पङ्क्ति), log_पङ्क्ति, 0, 20, रिक्त);
		}
#ifdef WITH_DLT
		यदि(log_destinations & MQTT3_LOG_DLT && priority != MOSQ_LOG_INTERNAL){
			DLT_LOG_STRING(dltContext, get_dlt_level(priority), log_पङ्क्ति);
		}
#endif
#ifdef ANDROID
		यदि(log_destinations & MQTT3_LOG_ANDROID && priority != MOSQ_LOG_INTERNAL){
			__android_log_write(get_android_level(priority), LOG_TAG, log_पङ्क्ति);
		}
#endif
	}

	फर्काऊ MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क log__printf(संरचना mosquitto *mosq, अचिन्हित पूर्णाङ्क priority, अचर वर्ण *fmt, ...)
{
	बहु_सूची va;
	पूर्णाङ्क rc;

	UNUSED(mosq);

	बहु_शुरू(va, fmt);
	rc = log__vprintf(priority, fmt, va);
	बहु_पूर्ण(va);

	फर्काऊ rc;
}


शून्य log__internal(अचर वर्ण *fmt, ...)
{
	बहु_सूची va;
	वर्ण buf[200];
	पूर्णाङ्क len;

	बहु_शुरू(va, fmt);
	len = vsnprintf(buf, 200, fmt, va);
	बहु_पूर्ण(va);

	यदि(len >= 200){
		log__printf(रिक्त, MOSQ_LOG_INTERNAL, "Internal log buffer too short (%d)", len);
		फर्काऊ;
	}

#ifdef WIN32
	log__printf(रिक्त, MOSQ_LOG_INTERNAL, "%s", buf);
#अन्यथा
	log__printf(रिक्त, MOSQ_LOG_INTERNAL, "%s%s%s", "\e[32m", buf, "\e[0m");
#endif
}


BROKER_EXPORT पूर्णाङ्क mosquitto_log_vprintf(पूर्णाङ्क level, अचर वर्ण *fmt, बहु_सूची va)
{
	फर्काऊ log__vprintf((अचिन्हित पूर्णाङ्क)level, fmt, va);
}


BROKER_EXPORT शून्य mosquitto_log_printf(पूर्णाङ्क level, अचर वर्ण *fmt, ...)
{
	बहु_सूची va;

	बहु_शुरू(va, fmt);
	log__vprintf((अचिन्हित पूर्णाङ्क)level, fmt, va);
	बहु_पूर्ण(va);
}


स्थिर शून्य libcommon__vprintf(अचर वर्ण *fmt, बहु_सूची va)
{
	log__vprintf(MOSQ_LOG_INFO, fmt, va);
}
