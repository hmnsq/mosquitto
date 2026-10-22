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
#समावेश "config.h"

#समावेश <inttypes.h>
#समावेश <stdarg.h>
#समावेश <stdio.h>
#समावेश <string.h>
#समावेश <inttypes.h>
#अगर_अघोषित WIN32
#समावेश <syslog.h>
#पूर्ण_अगर
#समावेश <time.h>

#अगर defined(__APPLE__)
#  include <sys/time.h>
#पूर्ण_अगर

#अगर_घोषित WITH_DLT
#समावेश <sys/stat.h>
#समावेश <dlt/dlt.h>
#पूर्ण_अगर

#समावेश "logging_mosq.h"
#समावेश "mosquitto_broker_internal.h"
#समावेश "util_mosq.h"

#अगर_घोषित WIN32
HANDLE syslog_h;
#पूर्ण_अगर

#अगर_घोषित ANDROID
#समावेश <android/log.h>
अटल स्थिर अक्षर *LOG_TAG = "mosquitto";
#पूर्ण_अगर

अटल अक्षर log_fptr_बफर[बफ_मान];
अटल व्योम libcommon__vprintf(स्थिर अक्षर *fmt, बहु_सूची va);

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
अटल अचिन्हित पूर्णांक log_destinations = MQTT3_LOG_STDERR;
अटल अचिन्हित पूर्णांक log_priorities = MOSQ_LOG_ERR | MOSQ_LOG_WARNING | MOSQ_LOG_NOTICE | MOSQ_LOG_INFO;

#अगर_घोषित WITH_DLT
अटल DltContext dltContext;
अटल bool dlt_allowed = false;


व्योम dlt_fifo_check(व्योम)
{
	काष्ठा stat statbuf;

	/* If we start DLT but the /tmp/dlt fifo doesn't exist, or isn't available
	 * for writing then there is a big delay when we try and close the log
	 * later, so check for it first. This has the side effect of not letting
	 * people using DLT create the fifo after Mosquitto has started, but at the
	 * benefit of not having a massive delay for everybody else. */
	स_रखो(&statbuf, 0, माप(statbuf));
	अगर(stat("/tmp/dlt", &statbuf) == 0){
		अगर(S_ISFIFO(statbuf.st_mode)){
			पूर्णांक fd = खोलो("/tmp/dlt", O_NONBLOCK | O_WRONLY);
			अगर(fd != -1){
				dlt_allowed = true;
				बंद(fd);
			}
		}
	}
}
#पूर्ण_अगर


अटल पूर्णांक get_time(काष्ठा पंचांग **ti)
{
	समय_प्रकार s;

	s = db.now_real_s;

	*ti = स_स्थानीय(&s);
	अगर(!(*ti)){
		ख_लिखो(मानक_त्रुटि, "Error obtaining system time.\n");
		वापस 1;
	}

	वापस 0;
}


पूर्णांक log__init(काष्ठा mosquitto__config *config)
{
	पूर्णांक rc = 0;

	libcommon_vprintf = libcommon__vprintf;

	log_priorities = config->log_type;
	log_destinations = config->log_dest;

	अगर(log_destinations & MQTT3_LOG_SYSLOG){
#अगर_अघोषित WIN32
		openlog("mosquitto", LOG_PID|LOG_CONS, config->log_facility);
#अन्यथा
		syslog_h = OpenEventLog(शून्य, "mosquitto");
#पूर्ण_अगर
	}

	अगर(log_destinations & MQTT3_LOG_FILE){
		config->log_fptr = mosquitto_fopen(config->log_file, "at", true);
		अगर(config->log_fptr){
			रखो_भबफ(config->log_fptr, log_fptr_बफर, _IOLBF, माप(log_fptr_बफर));
		}अन्यथा{
			log_destinations = MQTT3_LOG_STDERR;
			log_priorities = MOSQ_LOG_ERR;
			log__printf(शून्य, MOSQ_LOG_ERR, "Error: Unable to open log file %s for writing.", config->log_file);
		}
	}
#अगर_अघोषित WIN32
	अगर(log_destinations & MQTT3_LOG_STDOUT){
		रखो_भबफ(मानक_निकास, शून्य, _IOLBF, 0);
	}
#पूर्ण_अगर
#अगर_घोषित WITH_DLT
	अगर(log_destinations & MQTT3_LOG_DLT){
		dlt_fifo_check();
		अगर(dlt_allowed){
			DLT_REGISTER_APP("MQTT", "mosquitto log");
			dlt_register_context(&dltContext, "MQTT", "mosquitto DLT context");
		}
	}
#पूर्ण_अगर
	वापस rc;
}


पूर्णांक log__close(काष्ठा mosquitto__config *config)
{
	अगर(log_destinations & MQTT3_LOG_SYSLOG){
#अगर_अघोषित WIN32
		closelog();
#अन्यथा
		CloseEventLog(syslog_h);
#पूर्ण_अगर
	}
	अगर(log_destinations & MQTT3_LOG_FILE){
		अगर(config->log_fptr){
			ख_बंद(config->log_fptr);
			config->log_fptr = शून्य;
		}
	}

#अगर_घोषित WITH_DLT
	अगर(dlt_allowed){
		dlt_unregister_context(&dltContext);
		DLT_UNREGISTER_APP();
	}
#पूर्ण_अगर
	/* FIXME - do something for all destinations! */
	वापस MOSQ_ERR_SUCCESS;
}

#अगर_घोषित WITH_DLT


DltLogLevelType get_dlt_level(अचिन्हित पूर्णांक priority)
{
	चयन(priority){
		हाल MOSQ_LOG_ERR:
			वापस DLT_LOG_ERROR;
		हाल MOSQ_LOG_WARNING:
			वापस DLT_LOG_WARN;
		हाल MOSQ_LOG_INFO:
			वापस DLT_LOG_INFO;
		हाल MOSQ_LOG_DEBUG:
			वापस DLT_LOG_DEBUG;
		हाल MOSQ_LOG_NOTICE:
		हाल MOSQ_LOG_SUBSCRIBE:
		हाल MOSQ_LOG_UNSUBSCRIBE:
			वापस DLT_LOG_VERBOSE;
		शेष:
			वापस DLT_LOG_DEFAULT;
	}
}
#पूर्ण_अगर

#अगर_घोषित ANDROID


android_LogPriority get_android_level(अचिन्हित पूर्णांक priority)
{
	चयन(priority){
		हाल MOSQ_LOG_ERR:
			वापस ANDROID_LOG_ERROR;
		हाल MOSQ_LOG_WARNING:
			वापस ANDROID_LOG_WARN;
		हाल MOSQ_LOG_INFO:
			वापस ANDROID_LOG_INFO;
		हाल MOSQ_LOG_DEBUG:
			वापस ANDROID_LOG_DEBUG;
		हाल MOSQ_LOG_NOTICE:
		हाल MOSQ_LOG_SUBSCRIBE:
		हाल MOSQ_LOG_UNSUBSCRIBE:
			वापस ANDROID_LOG_VERBOSE;
		शेष:
			वापस ANDROID_LOG_DEBUG;
	}
}
#पूर्ण_अगर


अटल पूर्णांक log__vprintf(अचिन्हित पूर्णांक priority, स्थिर अक्षर *fmt, बहु_सूची va)
{
	स्थिर अक्षर *topic;
	पूर्णांक syslog_priority;
	अक्षर log_पंक्ति[1000];
	माप_प्रकार log_पंक्ति_pos;
#अगर_घोषित WIN32
	अक्षर *sp;
#पूर्ण_अगर
	bool log_timestamp = true;
	अक्षर *log_timestamp_format = शून्य;
	खाता *log_fptr = शून्य;

	अगर(db.config){
		log_timestamp = db.config->log_timestamp;
		log_timestamp_format = db.config->log_timestamp_format;
		log_fptr = db.config->log_fptr;
	}

	अगर((log_priorities & priority) && log_destinations != MQTT3_LOG_NONE){
		चयन(priority){
			हाल MOSQ_LOG_SUBSCRIBE:
				topic = "$SYS/broker/log/M/subscribe";
#अगर_अघोषित WIN32
				syslog_priority = LOG_NOTICE;
#अन्यथा
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#पूर्ण_अगर
				अवरोध;
			हाल MOSQ_LOG_UNSUBSCRIBE:
				topic = "$SYS/broker/log/M/unsubscribe";
#अगर_अघोषित WIN32
				syslog_priority = LOG_NOTICE;
#अन्यथा
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#पूर्ण_अगर
				अवरोध;
			हाल MOSQ_LOG_DEBUG:
				topic = "$SYS/broker/log/D";
#अगर_अघोषित WIN32
				syslog_priority = LOG_DEBUG;
#अन्यथा
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#पूर्ण_अगर
				अवरोध;
			हाल MOSQ_LOG_ERR:
				topic = "$SYS/broker/log/E";
#अगर_अघोषित WIN32
				syslog_priority = LOG_ERR;
#अन्यथा
				syslog_priority = EVENTLOG_ERROR_TYPE;
#पूर्ण_अगर
				अवरोध;
			हाल MOSQ_LOG_WARNING:
				topic = "$SYS/broker/log/W";
#अगर_अघोषित WIN32
				syslog_priority = LOG_WARNING;
#अन्यथा
				syslog_priority = EVENTLOG_WARNING_TYPE;
#पूर्ण_अगर
				अवरोध;
			हाल MOSQ_LOG_NOTICE:
				topic = "$SYS/broker/log/N";
#अगर_अघोषित WIN32
				syslog_priority = LOG_NOTICE;
#अन्यथा
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#पूर्ण_अगर
				अवरोध;
			हाल MOSQ_LOG_INFO:
				topic = "$SYS/broker/log/I";
#अगर_अघोषित WIN32
				syslog_priority = LOG_INFO;
#अन्यथा
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#पूर्ण_अगर
				अवरोध;
#अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
			हाल MOSQ_LOG_WEBSOCKETS:
				topic = "$SYS/broker/log/WS";
#अगर_अघोषित WIN32
				syslog_priority = LOG_DEBUG;
#अन्यथा
				syslog_priority = EVENTLOG_INFORMATION_TYPE;
#पूर्ण_अगर
				अवरोध;
#पूर्ण_अगर
			शेष:
				topic = "$SYS/broker/log/E";
#अगर_अघोषित WIN32
				syslog_priority = LOG_ERR;
#अन्यथा
				syslog_priority = EVENTLOG_ERROR_TYPE;
#पूर्ण_अगर
		}
		अगर(log_timestamp){
			अगर(log_timestamp_format){
				काष्ठा पंचांग *ti = शून्य;
				get_time(&ti);
				log_पंक्ति_pos = स_माला(log_पंक्ति, माप(log_पंक्ति), log_timestamp_format, ti);
				अगर(log_पंक्ति_pos == 0){
					log_पंक्ति_pos = (माप_प्रकार)snprintf(log_पंक्ति, माप(log_पंक्ति), "Time error");
				}
			}अन्यथा{
				log_पंक्ति_pos = (माप_प्रकार)snprintf(log_पंक्ति, माप(log_पंक्ति), "%" PRIu64, (uint64_t)db.now_real_s);
			}
			अगर(log_पंक्ति_pos < माप(log_पंक्ति)-3){
				log_पंक्ति[log_पंक्ति_pos] = ':';
				log_पंक्ति[log_पंक्ति_pos+1] = ' ';
				log_पंक्ति[log_पंक्ति_pos+2] = '\0';
				log_पंक्ति_pos += 2;
			}
		}अन्यथा{
			log_पंक्ति_pos = 0;
		}
		vsnprintf(&log_पंक्ति[log_पंक्ति_pos], माप(log_पंक्ति)-log_पंक्ति_pos, fmt, va);
		log_पंक्ति[माप(log_पंक्ति)-1] = '\0'; /* Ensure string is null terminated. */

		अगर(log_destinations & MQTT3_LOG_STDOUT){
			ख_लिखो(मानक_निकास, "%s\n", log_पंक्ति);
		}
		अगर(log_destinations & MQTT3_LOG_STDERR){
			ख_लिखो(मानक_त्रुटि, "%s\n", log_पंक्ति);
		}
		अगर(log_destinations & MQTT3_LOG_FILE && log_fptr){
			ख_लिखो(log_fptr, "%s\n", log_पंक्ति);
#अगर_घोषित WIN32
			/* Windows doesn't support line buffering, so flush. */
			ख_साफ(log_fptr);
#पूर्ण_अगर
		}
		अगर(log_destinations & MQTT3_LOG_SYSLOG){
#अगर_अघोषित WIN32
			syslog(syslog_priority, "%s", log_पंक्ति);
#अन्यथा
			sp = (अक्षर *)log_पंक्ति;
			ReportEvent(syslog_h, syslog_priority, 0, 0, शून्य, 1, 0, (स्थिर अक्षर **)&sp, शून्य);
#पूर्ण_अगर
		}
		अगर(log_destinations & MQTT3_LOG_TOPIC && priority != MOSQ_LOG_DEBUG && priority != MOSQ_LOG_INTERNAL){
			db__messages_easy_queue(शून्य, topic, 2, (uint32_t)म_माप(log_पंक्ति), log_पंक्ति, 0, 20, शून्य);
		}
#अगर_घोषित WITH_DLT
		अगर(log_destinations & MQTT3_LOG_DLT && priority != MOSQ_LOG_INTERNAL){
			DLT_LOG_STRING(dltContext, get_dlt_level(priority), log_पंक्ति);
		}
#पूर्ण_अगर
#अगर_घोषित ANDROID
		अगर(log_destinations & MQTT3_LOG_ANDROID && priority != MOSQ_LOG_INTERNAL){
			__android_log_write(get_android_level(priority), LOG_TAG, log_पंक्ति);
		}
#पूर्ण_अगर
	}

	वापस MOSQ_ERR_SUCCESS;
}


पूर्णांक log__printf(काष्ठा mosquitto *mosq, अचिन्हित पूर्णांक priority, स्थिर अक्षर *fmt, ...)
{
	बहु_सूची va;
	पूर्णांक rc;

	UNUSED(mosq);

	बहु_शुरू(va, fmt);
	rc = log__vprintf(priority, fmt, va);
	बहु_पूर्ण(va);

	वापस rc;
}


व्योम log__internal(स्थिर अक्षर *fmt, ...)
{
	बहु_सूची va;
	अक्षर buf[200];
	पूर्णांक len;

	बहु_शुरू(va, fmt);
	len = vsnprintf(buf, 200, fmt, va);
	बहु_पूर्ण(va);

	अगर(len >= 200){
		log__printf(शून्य, MOSQ_LOG_INTERNAL, "Internal log buffer too short (%d)", len);
		वापस;
	}

#अगर_घोषित WIN32
	log__printf(शून्य, MOSQ_LOG_INTERNAL, "%s", buf);
#अन्यथा
	log__printf(शून्य, MOSQ_LOG_INTERNAL, "%s%s%s", "\e[32m", buf, "\e[0m");
#पूर्ण_अगर
}


BROKER_EXPORT पूर्णांक mosquitto_log_vprintf(पूर्णांक level, स्थिर अक्षर *fmt, बहु_सूची va)
{
	वापस log__vprintf((अचिन्हित पूर्णांक)level, fmt, va);
}


BROKER_EXPORT व्योम mosquitto_log_printf(पूर्णांक level, स्थिर अक्षर *fmt, ...)
{
	बहु_सूची va;

	बहु_शुरू(va, fmt);
	log__vprintf((अचिन्हित पूर्णांक)level, fmt, va);
	बहु_पूर्ण(va);
}


अटल व्योम libcommon__vprintf(स्थिर अक्षर *fmt, बहु_सूची va)
{
	log__vprintf(MOSQ_LOG_INFO, fmt, va);
}
