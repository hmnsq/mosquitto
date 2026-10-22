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
#అన్తర్భావ "config.h"

#అన్తర్భావ <inttypes.h>
#అన్తర్భావ <stdarg.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <inttypes.h>
#నిర్వచితం_కాకపోతే WIN32
#అన్తర్భావ <syslog.h>
#అయితే_ముగింపు
#అన్తర్భావ <time.h>

#ఒకవేళ defined(__APPLE__)
#  include <sys/time.h>
#అయితే_ముగింపు

#నిర్వచితమైతే WITH_DLT
#అన్తర్భావ <sys/stat.h>
#అన్తర్భావ <dlt/dlt.h>
#అయితే_ముగింపు

#అన్తర్భావ "logging_mosq.h"
#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "util_mosq.h"

#నిర్వచితమైతే WIN32
HANDLE syslog_h;
#అయితే_ముగింపు

#నిర్వచితమైతే ANDROID
#అన్తర్భావ <android/log.h>
స్థిర స్థిరరాశి అక్షరం *LOG_ట్యాగ్ = "mosquitto";
#అయితే_ముగింపు

స్థిర అక్షరం log_fptr_బఫరు[BUFSIZ];
స్థిర శూన్య libcommon__vprintf(స్థిరరాశి అక్షరం *fmt, va_list va);

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
స్థిర గుర్తులేని పూర్ణాంకం log_destinations = MQTT3_LOG_STDERR;
స్థిర గుర్తులేని పూర్ణాంకం log_priorities = MOSQ_LOG_ERR | MOSQ_LOG_WARNING | MOSQ_LOG_NOTICE | MOSQ_LOG_INFO;

#నిర్వచితమైతే WITH_DLT
స్థిర DltContext dltContext;
స్థిర bool dlt_allowed = false;


శూన్య dlt_fifo_check(శూన్య)
{
	నిర్మాణం stat statbuf;

	/* If we start DLT but the /tmp/dlt fifo doesn't exist, or isn't available
	 * for writing then there is a big delay when we try and close the log
	 * later, so check for it first. This has the side effect of not letting
	 * people using DLT create the fifo after Mosquitto has started, but at the
	 * benefit of not having a massive delay for everybody else. */
	memset(&statbuf, 0, పరిమాణం(statbuf));
	ఒకవేళ(stat("/tmp/dlt", &statbuf) == 0){
		ఒకవేళ(S_ISFIFO(statbuf.st_బహుళకం)){
			పూర్ణాంకం fd = open("/tmp/dlt", O_NONBLOCK | O_WRONLY);
			ఒకవేళ(fd != -1){
				dlt_allowed = true;
				close(fd);
			}
		}
	}
}
#అయితే_ముగింపు


స్థిర పూర్ణాంకం get_సమయం(నిర్మాణం tm **ti)
{
	time_t s;

	s = db.now_real_s;

	*ti = localtime(&s);
	ఒకవేళ(!(*ti)){
		fprintf(stderr, "Error obtaining system time.\n");
		తిరిగి 1;
	}

	తిరిగి 0;
}


పూర్ణాంకం log__init(నిర్మాణం mosquitto__config *config)
{
	పూర్ణాంకం rc = 0;

	libcommon_vprintf = libcommon__vprintf;

	log_priorities = config->log_type;
	log_destinations = config->log_dest;

	ఒకవేళ(log_destinations & MQTT3_LOG_SYSLOG){
#నిర్వచితం_కాకపోతే WIN32
		openlog("mosquitto", LOG_PID|LOG_CONS, config->log_facility);
#లేకపోతే
		syslog_h = OpenEventLog(ఖాళీ, "mosquitto");
#అయితే_ముగింపు
	}

	ఒకవేళ(log_destinations & MQTT3_LOG_FILE){
		config->log_fptr = mosquitto_fopen(config->log_file, "at", true);
		ఒకవేళ(config->log_fptr){
			setvbuf(config->log_fptr, log_fptr_బఫరు, _IOLBF, పరిమాణం(log_fptr_బఫరు));
		}లేకపోతే{
			log_destinations = MQTT3_LOG_STDERR;
			log_priorities = MOSQ_LOG_ERR;
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Unable to open log file %s for writing.", config->log_file);
		}
	}
#నిర్వచితం_కాకపోతే WIN32
	ఒకవేళ(log_destinations & MQTT3_LOG_STDOUT){
		setvbuf(stdout, ఖాళీ, _IOLBF, 0);
	}
#అయితే_ముగింపు
#నిర్వచితమైతే WITH_DLT
	ఒకవేళ(log_destinations & MQTT3_LOG_DLT){
		dlt_fifo_check();
		ఒకవేళ(dlt_allowed){
			DLT_రిజిస్టరు_APP("MQTT", "mosquitto log");
			dlt_రిజిస్టరు_context(&dltContext, "MQTT", "mosquitto DLT context");
		}
	}
#అయితే_ముగింపు
	తిరిగి rc;
}


పూర్ణాంకం log__close(నిర్మాణం mosquitto__config *config)
{
	ఒకవేళ(log_destinations & MQTT3_LOG_SYSLOG){
#నిర్వచితం_కాకపోతే WIN32
		closelog();
#లేకపోతే
		CloseEventLog(syslog_h);
#అయితే_ముగింపు
	}
	ఒకవేళ(log_destinations & MQTT3_LOG_FILE){
		ఒకవేళ(config->log_fptr){
			fclose(config->log_fptr);
			config->log_fptr = ఖాళీ;
		}
	}

#నిర్వచితమైతే WITH_DLT
	ఒకవేళ(dlt_allowed){
		dlt_unregister_context(&dltContext);
		DLT_UNREGISTER_APP();
	}
#అయితే_ముగింపు
	/* FIXME - do something for all destinations! */
	తిరిగి MOSQ_ERR_SUCCESS;
}

#నిర్వచితమైతే WITH_DLT


DltLogLevelType get_dlt_level(గుర్తులేని పూర్ణాంకం ప్రాధాన్యత)
{
	ఎంపిక(ప్రాధాన్యత){
		స్థితి MOSQ_LOG_ERR:
			తిరిగి DLT_LOG_ERROR;
		స్థితి MOSQ_LOG_WARNING:
			తిరిగి DLT_LOG_WARN;
		స్థితి MOSQ_LOG_INFO:
			తిరిగి DLT_LOG_INFO;
		స్థితి MOSQ_LOG_DEBUG:
			తిరిగి DLT_LOG_DEBUG;
		స్థితి MOSQ_LOG_NOTICE:
		స్థితి MOSQ_LOG_SUBSCRIBE:
		స్థితి MOSQ_LOG_UNSUBSCRIBE:
			తిరిగి DLT_LOG_VERBOSE;
		అప్రమేయ:
			తిరిగి DLT_LOG_DEFAULT;
	}
}
#అయితే_ముగింపు

#నిర్వచితమైతే ANDROID


android_Log_ప్రాధాన్యత get_android_level(గుర్తులేని పూర్ణాంకం ప్రాధాన్యత)
{
	ఎంపిక(ప్రాధాన్యత){
		స్థితి MOSQ_LOG_ERR:
			తిరిగి ANDROID_LOG_ERROR;
		స్థితి MOSQ_LOG_WARNING:
			తిరిగి ANDROID_LOG_WARN;
		స్థితి MOSQ_LOG_INFO:
			తిరిగి ANDROID_LOG_INFO;
		స్థితి MOSQ_LOG_DEBUG:
			తిరిగి ANDROID_LOG_DEBUG;
		స్థితి MOSQ_LOG_NOTICE:
		స్థితి MOSQ_LOG_SUBSCRIBE:
		స్థితి MOSQ_LOG_UNSUBSCRIBE:
			తిరిగి ANDROID_LOG_VERBOSE;
		అప్రమేయ:
			తిరిగి ANDROID_LOG_DEBUG;
	}
}
#అయితే_ముగింపు


స్థిర పూర్ణాంకం log__vprintf(గుర్తులేని పూర్ణాంకం ప్రాధాన్యత, స్థిరరాశి అక్షరం *fmt, va_list va)
{
	స్థిరరాశి అక్షరం *topic;
	పూర్ణాంకం syslog_ప్రాధాన్యత;
	అక్షరం log_పంక్తి[1000];
	size_t log_పంక్తి_pos;
#నిర్వచితమైతే WIN32
	అక్షరం *sp;
#అయితే_ముగింపు
	bool log_timestamp = true;
	అక్షరం *log_timestamp_format = ఖాళీ;
	FILE *log_fptr = ఖాళీ;

	ఒకవేళ(db.config){
		log_timestamp = db.config->log_timestamp;
		log_timestamp_format = db.config->log_timestamp_format;
		log_fptr = db.config->log_fptr;
	}

	ఒకవేళ((log_priorities & ప్రాధాన్యత) && log_destinations != MQTT3_LOG_NONE){
		ఎంపిక(ప్రాధాన్యత){
			స్థితి MOSQ_LOG_SUBSCRIBE:
				topic = "$SYS/broker/log/M/subscribe";
#నిర్వచితం_కాకపోతే WIN32
				syslog_ప్రాధాన్యత = LOG_NOTICE;
#లేకపోతే
				syslog_ప్రాధాన్యత = EVENTLOG_INFORMATION_TYPE;
#అయితే_ముగింపు
				ఆపు;
			స్థితి MOSQ_LOG_UNSUBSCRIBE:
				topic = "$SYS/broker/log/M/unsubscribe";
#నిర్వచితం_కాకపోతే WIN32
				syslog_ప్రాధాన్యత = LOG_NOTICE;
#లేకపోతే
				syslog_ప్రాధాన్యత = EVENTLOG_INFORMATION_TYPE;
#అయితే_ముగింపు
				ఆపు;
			స్థితి MOSQ_LOG_DEBUG:
				topic = "$SYS/broker/log/D";
#నిర్వచితం_కాకపోతే WIN32
				syslog_ప్రాధాన్యత = LOG_DEBUG;
#లేకపోతే
				syslog_ప్రాధాన్యత = EVENTLOG_INFORMATION_TYPE;
#అయితే_ముగింపు
				ఆపు;
			స్థితి MOSQ_LOG_ERR:
				topic = "$SYS/broker/log/E";
#నిర్వచితం_కాకపోతే WIN32
				syslog_ప్రాధాన్యత = LOG_ERR;
#లేకపోతే
				syslog_ప్రాధాన్యత = EVENTLOG_ERROR_TYPE;
#అయితే_ముగింపు
				ఆపు;
			స్థితి MOSQ_LOG_WARNING:
				topic = "$SYS/broker/log/W";
#నిర్వచితం_కాకపోతే WIN32
				syslog_ప్రాధాన్యత = LOG_WARNING;
#లేకపోతే
				syslog_ప్రాధాన్యత = EVENTLOG_WARNING_TYPE;
#అయితే_ముగింపు
				ఆపు;
			స్థితి MOSQ_LOG_NOTICE:
				topic = "$SYS/broker/log/N";
#నిర్వచితం_కాకపోతే WIN32
				syslog_ప్రాధాన్యత = LOG_NOTICE;
#లేకపోతే
				syslog_ప్రాధాన్యత = EVENTLOG_INFORMATION_TYPE;
#అయితే_ముగింపు
				ఆపు;
			స్థితి MOSQ_LOG_INFO:
				topic = "$SYS/broker/log/I";
#నిర్వచితం_కాకపోతే WIN32
				syslog_ప్రాధాన్యత = LOG_INFO;
#లేకపోతే
				syslog_ప్రాధాన్యత = EVENTLOG_INFORMATION_TYPE;
#అయితే_ముగింపు
				ఆపు;
#ఒకవేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			స్థితి MOSQ_LOG_WEBSOCKETS:
				topic = "$SYS/broker/log/WS";
#నిర్వచితం_కాకపోతే WIN32
				syslog_ప్రాధాన్యత = LOG_DEBUG;
#లేకపోతే
				syslog_ప్రాధాన్యత = EVENTLOG_INFORMATION_TYPE;
#అయితే_ముగింపు
				ఆపు;
#అయితే_ముగింపు
			అప్రమేయ:
				topic = "$SYS/broker/log/E";
#నిర్వచితం_కాకపోతే WIN32
				syslog_ప్రాధాన్యత = LOG_ERR;
#లేకపోతే
				syslog_ప్రాధాన్యత = EVENTLOG_ERROR_TYPE;
#అయితే_ముగింపు
		}
		ఒకవేళ(log_timestamp){
			ఒకవేళ(log_timestamp_format){
				నిర్మాణం tm *ti = ఖాళీ;
				get_సమయం(&ti);
				log_పంక్తి_pos = strftime(log_పంక్తి, పరిమాణం(log_పంక్తి), log_timestamp_format, ti);
				ఒకవేళ(log_పంక్తి_pos == 0){
					log_పంక్తి_pos = (size_t)snprintf(log_పంక్తి, పరిమాణం(log_పంక్తి), "Time error");
				}
			}లేకపోతే{
				log_పంక్తి_pos = (size_t)snprintf(log_పంక్తి, పరిమాణం(log_పంక్తి), "%" PRIu64, (uint64_t)db.now_real_s);
			}
			ఒకవేళ(log_పంక్తి_pos < పరిమాణం(log_పంక్తి)-3){
				log_పంక్తి[log_పంక్తి_pos] = ':';
				log_పంక్తి[log_పంక్తి_pos+1] = ' ';
				log_పంక్తి[log_పంక్తి_pos+2] = '\0';
				log_పంక్తి_pos += 2;
			}
		}లేకపోతే{
			log_పంక్తి_pos = 0;
		}
		vsnprintf(&log_పంక్తి[log_పంక్తి_pos], పరిమాణం(log_పంక్తి)-log_పంక్తి_pos, fmt, va);
		log_పంక్తి[పరిమాణం(log_పంక్తి)-1] = '\0'; /* Ensure string is null terminated. */

		ఒకవేళ(log_destinations & MQTT3_LOG_STDOUT){
			fprintf(stdout, "%s\n", log_పంక్తి);
		}
		ఒకవేళ(log_destinations & MQTT3_LOG_STDERR){
			fprintf(stderr, "%s\n", log_పంక్తి);
		}
		ఒకవేళ(log_destinations & MQTT3_LOG_FILE && log_fptr){
			fprintf(log_fptr, "%s\n", log_పంక్తి);
#నిర్వచితమైతే WIN32
			/* Windows doesn't support line buffering, so flush. */
			fflush(log_fptr);
#అయితే_ముగింపు
		}
		ఒకవేళ(log_destinations & MQTT3_LOG_SYSLOG){
#నిర్వచితం_కాకపోతే WIN32
			syslog(syslog_ప్రాధాన్యత, "%s", log_పంక్తి);
#లేకపోతే
			sp = (అక్షరం *)log_పంక్తి;
			ReportEvent(syslog_h, syslog_ప్రాధాన్యత, 0, 0, ఖాళీ, 1, 0, (స్థిరరాశి అక్షరం **)&sp, ఖాళీ);
#అయితే_ముగింపు
		}
		ఒకవేళ(log_destinations & MQTT3_LOG_TOPIC && ప్రాధాన్యత != MOSQ_LOG_DEBUG && ప్రాధాన్యత != MOSQ_LOG_అంతర్గత){
			db_messages_easy_వరుస(ఖాళీ, topic, 2, (uint32_t)అక్షరశ్రేణి_పొడవు(log_పంక్తి), log_పంక్తి, 0, 20, ఖాళీ);
		}
#నిర్వచితమైతే WITH_DLT
		ఒకవేళ(log_destinations & MQTT3_LOG_DLT && ప్రాధాన్యత != MOSQ_LOG_అంతర్గత){
			DLT_LOG_STRING(dltContext, get_dlt_level(ప్రాధాన్యత), log_పంక్తి);
		}
#అయితే_ముగింపు
#నిర్వచితమైతే ANDROID
		ఒకవేళ(log_destinations & MQTT3_LOG_ANDROID && ప్రాధాన్యత != MOSQ_LOG_అంతర్గత){
			__android_log_write(get_android_level(ప్రాధాన్యత), LOG_ట్యాగ్, log_పంక్తి);
		}
#అయితే_ముగింపు
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం log__printf(నిర్మాణం mosquitto *mosq, గుర్తులేని పూర్ణాంకం ప్రాధాన్యత, స్థిరరాశి అక్షరం *fmt, ...)
{
	va_list va;
	పూర్ణాంకం rc;

	వాడని(mosq);

	va_ప్రారంభం(va, fmt);
	rc = log__vprintf(ప్రాధాన్యత, fmt, va);
	va_చివర(va);

	తిరిగి rc;
}


శూన్య log_అంతర్గత(స్థిరరాశి అక్షరం *fmt, ...)
{
	va_list va;
	అక్షరం buf[200];
	పూర్ణాంకం len;

	va_ప్రారంభం(va, fmt);
	len = vsnprintf(buf, 200, fmt, va);
	va_చివర(va);

	ఒకవేళ(len >= 200){
		log__printf(ఖాళీ, MOSQ_LOG_అంతర్గత, "Internal log buffer too short (%d)", len);
		తిరిగి;
	}

#నిర్వచితమైతే WIN32
	log__printf(ఖాళీ, MOSQ_LOG_అంతర్గత, "%s", buf);
#లేకపోతే
	log__printf(ఖాళీ, MOSQ_LOG_అంతర్గత, "%s%s%s", "\e[32m", buf, "\e[0m");
#అయితే_ముగింపు
}


BROKER_EXPORT పూర్ణాంకం mosquitto_log_vprintf(పూర్ణాంకం level, స్థిరరాశి అక్షరం *fmt, va_list va)
{
	తిరిగి log__vprintf((గుర్తులేని పూర్ణాంకం)level, fmt, va);
}


BROKER_EXPORT శూన్య mosquitto_log_printf(పూర్ణాంకం level, స్థిరరాశి అక్షరం *fmt, ...)
{
	va_list va;

	va_ప్రారంభం(va, fmt);
	log__vprintf((గుర్తులేని పూర్ణాంకం)level, fmt, va);
	va_చివర(va);
}


స్థిర శూన్య libcommon__vprintf(స్థిరరాశి అక్షరం *fmt, va_list va)
{
	log__vprintf(MOSQ_LOG_INFO, fmt, va);
}
