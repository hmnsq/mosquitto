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
#यदि_अपरिभाषितम् WIN32
#अन्तर्भाव <syslog.h>
#यदि_अन्तः
#अन्तर्भाव <time.h>

#यदि defined(__APPLE__)
#  include <sys/time.h>
#यदि_अन्तः

#यदि_परिभाषितम् WITH_DLT
#अन्तर्भाव <sys/stat.h>
#अन्तर्भाव <dlt/dlt.h>
#यदि_अन्तः

#अन्तर्भाव "logging_mosq.h"
#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "util_mosq.h"

#यदि_परिभाषितम् WIN32
HANDLE syslog_h;
#यदि_अन्तः

#यदि_परिभाषितम् ANDROID
#अन्तर्भाव <android/log.h>
स्थिर ध्रुव वर्ण *LOG_चिह्नकम् = "mosquitto";
#यदि_अन्तः

स्थिर वर्ण log_fptr_बफर[BUFSIZ];
स्थिर शून्य libcommon__vprintf(ध्रुव वर्ण *fmt, va_list va);

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
स्थिर अचिह्नित पूर्णाङ्क log_destinations = MQTT3_LOG_STDERR;
स्थिर अचिह्नित पूर्णाङ्क log_priorities = MOSQ_LOG_ERR | MOSQ_LOG_WARNING | MOSQ_LOG_NOTICE | MOSQ_LOG_INFO;

#यदि_परिभाषितम् WITH_DLT
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
	स्मृतिं_पूरय(&statbuf, 0, परिमाण(statbuf));
	यदि(stat("/tmp/dlt", &statbuf) == 0){
		यदि(S_ISFIFO(statbuf.st_बहुलकम्)){
			पूर्णाङ्क fd = उद्घाटय("/tmp/dlt", O_NONBLOCK | O_WRONLY);
			यदि(fd != -1){
				dlt_allowed = true;
				close(fd);
			}
		}
	}
}
#यदि_अन्तः


स्थिर पूर्णाङ्क get_time(संरचना tm **ti)
{
	time_t s;

	s = db.now_real_s;

	*ti = localtime(&s);
	यदि(!(*ti)){
		fprintf(stderr, "Error obtaining system time.\n");
		प्रत्यागम 1;
	}

	प्रत्यागम 0;
}


पूर्णाङ्क log__init(संरचना mosquitto__config *config)
{
	पूर्णाङ्क rc = 0;

	libcommon_vprintf = libcommon__vprintf;

	log_priorities = config->log_type;
	log_destinations = config->log_dest;

	यदि(log_destinations & MQTT3_LOG_SYSLOG){
#यदि_अपरिभाषितम् WIN32
		openlog("mosquitto", LOG_PID|LOG_CONS, config->log_facility);
#अन्यथा
		syslog_h = OpenEventLog(रिक्त, "mosquitto");
#यदि_अन्तः
	}

	यदि(log_destinations & MQTT3_LOG_FILE){
		config->log_fptr = mosquitto_fopen(config->log_file, "at", true);
		यदि(config->log_fptr){
			setvbuf(config->log_fptr, log_fptr_बफर, _IOLBF, परिमाण(log_fptr_बफर));
		}अन्यथा{
			log_destinations = MQTT3_LOG_STDERR;
			log_priorities = MOSQ_LOG_ERR;
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to open log file %s for writing.", config->log_file);
		}
	}
#यदि_अपरिभाषितम् WIN32
	यदि(log_destinations & MQTT3_LOG_STDOUT){
		setvbuf(stdout, रिक्त, _IOLBF, 0);
	}
#यदि_अन्तः
#यदि_परिभाषितम् WITH_DLT
	यदि(log_destinations & MQTT3_LOG_DLT){
		dlt_fifo_check();
		यदि(dlt_allowed){
			DLT_पञ्जिकाकोशः_APP("MQTT", "mosquitto log");
			dlt_पञ्जिकाकोशः_context(&dltContext, "MQTT", "mosquitto DLT context");
		}
	}
#यदि_अन्तः
	प्रत्यागम rc;
}


पूर्णाङ्क log__close(संरचना mosquitto__config *config)
{
	यदि(log_destinations & MQTT3_LOG_SYSLOG){
#यदि_अपरिभाषितम् WIN32
		closelog();
#अन्यथा
		CloseEventLog(syslog_h);
#यदि_अन्तः
	}
	यदि(log_destinations & MQTT3_LOG_FILE){
		यदि(config->log_fptr){
			fclose(config->log_fptr);
			config->log_fptr = रिक्त;
		}
	}

#यदि_परिभाषितम् WITH_DLT
	यदि(dlt_allowed){
		dlt_unregister_context(&dltContext);
		DLT_UNREGISTER_APP();
	}
#यदि_अन्तः
	/* FIXME - do something for all destinations! */
	प्रत्यागम MOSQ_ERR_SUCCESS;
}

#यदि_परिभाषितम् WITH_DLT


DltLogLevelType get_dlt_level(अचिह्नित पूर्णाङ्क प्राथमिकता)
{
	विकल्प(प्राथमिकता){
		पक्ष MOSQ_LOG_ERR:
			प्रत्यागम DLT_LOG_ERROR;
		पक्ष MOSQ_LOG_WARNING:
			प्रत्यागम DLT_LOG_WARN;
		पक्ष MOSQ_LOG_INFO:
			प्रत्यागम DLT_LOG_INFO;
		पक्ष MOSQ_LOG_DEBUG:
			प्रत्यागम DLT_LOG_DEBUG;
		पक्ष MOSQ_LOG_NOTICE:
		पक्ष MOSQ_LOG_SUBSCRIBE:
		पक्ष MOSQ_LOG_UNSUBSCRIBE:
			प्रत्यागम DLT_LOG_VERBOSE;
		स्वभाव:
			प्रत्यागम DLT_LOG_DEFAULT;
	}
}
#यदि_अन्तः

#यदि_परिभाषितम् ANDROID


android_Log_प्राथमिकता get_android_level(अचिह्नित पूर्णाङ्क प्राथमिकता)
{
	विकल्प(प्राथमिकता){
		पक्ष MOSQ_LOG_ERR:
			प्रत्यागम ANDROID_LOG_ERROR;
		पक्ष MOSQ_LOG_WARNING:
			प्रत्यागम ANDROID_LOG_WARN;
		पक्ष MOSQ_LOG_INFO:
			प्रत्यागम ANDROID_LOG_INFO;
		पक्ष MOSQ_LOG_DEBUG:
			प्रत्यागम ANDROID_LOG_DEBUG;
		पक्ष MOSQ_LOG_NOTICE:
		पक्ष MOSQ_LOG_SUBSCRIBE:
		पक्ष MOSQ_LOG_UNSUBSCRIBE:
			प्रत्यागम ANDROID_LOG_VERBOSE;
		स्वभाव:
			प्रत्यागम ANDROID_LOG_DEBUG;
	}
}
#यदि_अन्तः


स्थिर पूर्णाङ्क log__vprintf(अचिह्नित पूर्णाङ्क प्राथमिकता, ध्रुव वर्ण *fmt, va_list va)
{
	ध्रुव वर्ण *topic;
	पूर्णाङ्क syslog_प्राथमिकता;
	वर्ण log_पङ्क्तिः[1000];
	size_t log_पङ्क्तिः_pos;
#यदि_परिभाषितम् WIN32
	वर्ण *sp;
#यदि_अन्तः
	bool log_timestamp = true;
	वर्ण *log_timestamp_format = रिक्त;
	FILE *log_fptr = रिक्त;

	यदि(db.config){
		log_timestamp = db.config->log_timestamp;
		log_timestamp_format = db.config->log_timestamp_format;
		log_fptr = db.config->log_fptr;
	}

	यदि((log_priorities & प्राथमिकता) && log_destinations != MQTT3_LOG_NONE){
		विकल्प(प्राथमिकता){
			पक्ष MOSQ_LOG_SUBSCRIBE:
				topic = "$SYS/broker/log/M/subscribe";
#यदि_अपरिभाषितम् WIN32
				syslog_प्राथमिकता = LOG_NOTICE;
#अन्यथा
				syslog_प्राथमिकता = EVENTLOG_INFORMATION_TYPE;
#यदि_अन्तः
				भङ्ग;
			पक्ष MOSQ_LOG_UNSUBSCRIBE:
				topic = "$SYS/broker/log/M/unsubscribe";
#यदि_अपरिभाषितम् WIN32
				syslog_प्राथमिकता = LOG_NOTICE;
#अन्यथा
				syslog_प्राथमिकता = EVENTLOG_INFORMATION_TYPE;
#यदि_अन्तः
				भङ्ग;
			पक्ष MOSQ_LOG_DEBUG:
				topic = "$SYS/broker/log/D";
#यदि_अपरिभाषितम् WIN32
				syslog_प्राथमिकता = LOG_DEBUG;
#अन्यथा
				syslog_प्राथमिकता = EVENTLOG_INFORMATION_TYPE;
#यदि_अन्तः
				भङ्ग;
			पक्ष MOSQ_LOG_ERR:
				topic = "$SYS/broker/log/E";
#यदि_अपरिभाषितम् WIN32
				syslog_प्राथमिकता = LOG_ERR;
#अन्यथा
				syslog_प्राथमिकता = EVENTLOG_ERROR_TYPE;
#यदि_अन्तः
				भङ्ग;
			पक्ष MOSQ_LOG_WARNING:
				topic = "$SYS/broker/log/W";
#यदि_अपरिभाषितम् WIN32
				syslog_प्राथमिकता = LOG_WARNING;
#अन्यथा
				syslog_प्राथमिकता = EVENTLOG_WARNING_TYPE;
#यदि_अन्तः
				भङ्ग;
			पक्ष MOSQ_LOG_NOTICE:
				topic = "$SYS/broker/log/N";
#यदि_अपरिभाषितम् WIN32
				syslog_प्राथमिकता = LOG_NOTICE;
#अन्यथा
				syslog_प्राथमिकता = EVENTLOG_INFORMATION_TYPE;
#यदि_अन्तः
				भङ्ग;
			पक्ष MOSQ_LOG_INFO:
				topic = "$SYS/broker/log/I";
#यदि_अपरिभाषितम् WIN32
				syslog_प्राथमिकता = LOG_INFO;
#अन्यथा
				syslog_प्राथमिकता = EVENTLOG_INFORMATION_TYPE;
#यदि_अन्तः
				भङ्ग;
#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			पक्ष MOSQ_LOG_WEBSOCKETS:
				topic = "$SYS/broker/log/WS";
#यदि_अपरिभाषितम् WIN32
				syslog_प्राथमिकता = LOG_DEBUG;
#अन्यथा
				syslog_प्राथमिकता = EVENTLOG_INFORMATION_TYPE;
#यदि_अन्तः
				भङ्ग;
#यदि_अन्तः
			स्वभाव:
				topic = "$SYS/broker/log/E";
#यदि_अपरिभाषितम् WIN32
				syslog_प्राथमिकता = LOG_ERR;
#अन्यथा
				syslog_प्राथमिकता = EVENTLOG_ERROR_TYPE;
#यदि_अन्तः
		}
		यदि(log_timestamp){
			यदि(log_timestamp_format){
				संरचना tm *ti = रिक्त;
				get_time(&ti);
				log_पङ्क्तिः_pos = strftime(log_पङ्क्तिः, परिमाण(log_पङ्क्तिः), log_timestamp_format, ti);
				यदि(log_पङ्क्तिः_pos == 0){
					log_पङ्क्तिः_pos = (size_t)snprintf(log_पङ्क्तिः, परिमाण(log_पङ्क्तिः), "Time error");
				}
			}अन्यथा{
				log_पङ्क्तिः_pos = (size_t)snprintf(log_पङ्क्तिः, परिमाण(log_पङ्क्तिः), "%" PRIu64, (uint64_t)db.now_real_s);
			}
			यदि(log_पङ्क्तिः_pos < परिमाण(log_पङ्क्तिः)-3){
				log_पङ्क्तिः[log_पङ्क्तिः_pos] = ':';
				log_पङ्क्तिः[log_पङ्क्तिः_pos+1] = ' ';
				log_पङ्क्तिः[log_पङ्क्तिः_pos+2] = '\0';
				log_पङ्क्तिः_pos += 2;
			}
		}अन्यथा{
			log_पङ्क्तिः_pos = 0;
		}
		vsnprintf(&log_पङ्क्तिः[log_पङ्क्तिः_pos], परिमाण(log_पङ्क्तिः)-log_पङ्क्तिः_pos, fmt, va);
		log_पङ्क्तिः[परिमाण(log_पङ्क्तिः)-1] = '\0'; /* Ensure string is null terminated. */

		यदि(log_destinations & MQTT3_LOG_STDOUT){
			fprintf(stdout, "%s\n", log_पङ्क्तिः);
		}
		यदि(log_destinations & MQTT3_LOG_STDERR){
			fprintf(stderr, "%s\n", log_पङ्क्तिः);
		}
		यदि(log_destinations & MQTT3_LOG_FILE && log_fptr){
			fprintf(log_fptr, "%s\n", log_पङ्क्तिः);
#यदि_परिभाषितम् WIN32
			/* Windows doesn't support line buffering, so flush. */
			fflush(log_fptr);
#यदि_अन्तः
		}
		यदि(log_destinations & MQTT3_LOG_SYSLOG){
#यदि_अपरिभाषितम् WIN32
			syslog(syslog_प्राथमिकता, "%s", log_पङ्क्तिः);
#अन्यथा
			sp = (वर्ण *)log_पङ्क्तिः;
			ReportEvent(syslog_h, syslog_प्राथमिकता, 0, 0, रिक्त, 1, 0, (ध्रुव वर्ण **)&sp, रिक्त);
#यदि_अन्तः
		}
		यदि(log_destinations & MQTT3_LOG_TOPIC && प्राथमिकता != MOSQ_LOG_DEBUG && प्राथमिकता != MOSQ_LOG_आन्तरिकम्){
			db__messages_easy_queue(रिक्त, topic, 2, (uint32_t)वाक्यदैर्घ्यम्(log_पङ्क्तिः), log_पङ्क्तिः, 0, 20, रिक्त);
		}
#यदि_परिभाषितम् WITH_DLT
		यदि(log_destinations & MQTT3_LOG_DLT && प्राथमिकता != MOSQ_LOG_आन्तरिकम्){
			DLT_LOG_STRING(dltContext, get_dlt_level(प्राथमिकता), log_पङ्क्तिः);
		}
#यदि_अन्तः
#यदि_परिभाषितम् ANDROID
		यदि(log_destinations & MQTT3_LOG_ANDROID && प्राथमिकता != MOSQ_LOG_आन्तरिकम्){
			__android_log_write(get_android_level(प्राथमिकता), LOG_चिह्नकम्, log_पङ्क्तिः);
		}
#यदि_अन्तः
	}

	प्रत्यागम MOSQ_ERR_SUCCESS;
}


पूर्णाङ्क log__printf(संरचना mosquitto *mosq, अचिह्नित पूर्णाङ्क प्राथमिकता, ध्रुव वर्ण *fmt, ...)
{
	va_list va;
	पूर्णाङ्क rc;

	अप्रयुक्तम्(mosq);

	va_start(va, fmt);
	rc = log__vprintf(प्राथमिकता, fmt, va);
	va_अन्तः(va);

	प्रत्यागम rc;
}


शून्य log_आन्तरिकम्(ध्रुव वर्ण *fmt, ...)
{
	va_list va;
	वर्ण buf[200];
	पूर्णाङ्क len;

	va_start(va, fmt);
	len = vsnprintf(buf, 200, fmt, va);
	va_अन्तः(va);

	यदि(len >= 200){
		log__printf(रिक्त, MOSQ_LOG_आन्तरिकम्, "Internal log buffer too short (%d)", len);
		प्रत्यागम;
	}

#यदि_परिभाषितम् WIN32
	log__printf(रिक्त, MOSQ_LOG_आन्तरिकम्, "%s", buf);
#अन्यथा
	log__printf(रिक्त, MOSQ_LOG_आन्तरिकम्, "%s%s%s", "\e[32m", buf, "\e[0m");
#यदि_अन्तः
}


BROKER_EXPORT पूर्णाङ्क mosquitto_log_vprintf(पूर्णाङ्क level, ध्रुव वर्ण *fmt, va_list va)
{
	प्रत्यागम log__vprintf((अचिह्नित पूर्णाङ्क)level, fmt, va);
}


BROKER_EXPORT शून्य mosquitto_log_printf(पूर्णाङ्क level, ध्रुव वर्ण *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	log__vprintf((अचिह्नित पूर्णाङ्क)level, fmt, va);
	va_अन्तः(va);
}


स्थिर शून्य libcommon__vprintf(ध्रुव वर्ण *fmt, va_list va)
{
	log__vprintf(MOSQ_LOG_INFO, fmt, va);
}
