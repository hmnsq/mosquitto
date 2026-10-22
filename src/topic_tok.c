/*
Copyright (c) 2010-2021 Roger Light <roger@atchoo.org>

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

#ضمن "config.h"

#ضمن <stdio.h>
#ضمن <string.h>

#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/mqtt_protocol.h"
#ضمن "util_mosq.h"

#ضمن "utlist.h"


ساكن حرف *strtok_hier(حرف *str, حرف **saveptr)
{
	حرف *c;

	אם(str != عدم){
		*saveptr = str;
	}

	אם(*saveptr == عدم){
		החזר عدم;
	}

	c = strchr(*saveptr, '/');
	אם(c){
		str = *saveptr;
		*saveptr = c+1;
		c[0] = '\0';
	}אחרת אם(*saveptr){
		/* No match, but surplus string */
		str = *saveptr;
		*saveptr = عدم;
	}
	החזר str;
}


صحيح sub__topic_tokenise(ثابت حرف *subtopic, حرف **local_sub, حرف ***topics, ثابت حرف **sharename)
{
	حرف *saveptr = عدم;
	حرف *token;
	صحيح count;
	صحيح topic_index = 0;
	size_t len;

	אם(!subtopic){
		החזר MOSQ_ERR_INVAL;
	}

	len = strlen(subtopic);
	אם(len == 0){
		החזר MOSQ_ERR_INVAL;
	}

	*local_sub = mosquitto_strdup(subtopic);
	אם((*local_sub) == عدم){
		החזר MOSQ_ERR_NOMEM;
	}

	count = 0;
	saveptr = *local_sub;
	כלעוד(saveptr){
		saveptr = strchr(&saveptr[1], '/');
		count++;
	}
	*topics = mosquitto_calloc((size_t)(count+3) /* 3=$shared,sharename,NULL */, حجم(حرف *));
	אם((*topics) == عدم){
		mosquitto_FREE(*local_sub);
		החזר MOSQ_ERR_NOMEM;
	}

	אם((*local_sub)[0] != '$'){
		(*topics)[topic_index] = "";
		topic_index++;
	}

	token = strtok_hier((*local_sub), &saveptr);
	כלעוד(token){
		(*topics)[topic_index] = token;
		topic_index++;
		token = strtok_hier(عدم, &saveptr);
	}

	אם(!strcmp((*topics)[0], "$share")){
		אם(count < 3 || (count == 3 && strlen((*topics)[2]) == 0)){
			mosquitto_FREE(*local_sub);
			mosquitto_FREE(*topics);
			החזר MOSQ_ERR_PROTOCOL;
		}

		אם(sharename){
			אם(strpbrk((*topics)[1], "+#")){
				mosquitto_FREE(*local_sub);
				mosquitto_FREE(*topics);
				החזר MOSQ_ERR_PROTOCOL;
			}
			(*sharename) = (*topics)[1];
		}

		עבור(صحيح i=1; i<count-1; i++){
			(*topics)[i] = (*topics)[i+1];
		}
		(*topics)[0] = "";
		(*topics)[count-1] = عدم;
	}
	החזר MOSQ_ERR_SUCCESS;
}
