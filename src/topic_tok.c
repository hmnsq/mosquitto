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

	إذا(str != عدم){
		*saveptr = str;
	}

	إذا(*saveptr == عدم){
		إرجاع عدم;
	}

	c = strchr(*saveptr, '/');
	إذا(c){
		str = *saveptr;
		*saveptr = c+1;
		c[0] = '\0';
	}وإلا إذا(*saveptr){
		/* No match, but surplus string */
		str = *saveptr;
		*saveptr = عدم;
	}
	إرجاع str;
}


صحيح sub__topic_tokenise(ثابت حرف *subtopic, حرف **local_sub, حرف ***topics, ثابت حرف **sharename)
{
	حرف *saveptr = عدم;
	حرف *token;
	صحيح تعداد_2;
	صحيح topic_index = 0;
	size_t len;

	إذا(!subtopic){
		إرجاع MOSQ_ERR_INVAL;
	}

	len = strlen(subtopic);
	إذا(len == 0){
		إرجاع MOSQ_ERR_INVAL;
	}

	*local_sub = mosquitto_strdup(subtopic);
	إذا((*local_sub) == عدم){
		إرجاع MOSQ_ERR_NOMEM;
	}

	تعداد_2 = 0;
	saveptr = *local_sub;
	بينما(saveptr){
		saveptr = strchr(&saveptr[1], '/');
		تعداد_2++;
	}
	*topics = mosquitto_calloc((size_t)(تعداد_2+3) /* 3=$shared,sharename,NULL */, حجم(حرف *));
	إذا((*topics) == عدم){
		mosquitto_FREE(*local_sub);
		إرجاع MOSQ_ERR_NOMEM;
	}

	إذا((*local_sub)[0] != '$'){
		(*topics)[topic_index] = "";
		topic_index++;
	}

	token = strtok_hier((*local_sub), &saveptr);
	بينما(token){
		(*topics)[topic_index] = token;
		topic_index++;
		token = strtok_hier(عدم, &saveptr);
	}

	إذا(!strcmp((*topics)[0], "$share")){
		إذا(تعداد_2 < 3 || (تعداد_2 == 3 && strlen((*topics)[2]) == 0)){
			mosquitto_FREE(*local_sub);
			mosquitto_FREE(*topics);
			إرجاع MOSQ_ERR_PROTOCOL;
		}

		إذا(sharename){
			إذا(strpbrk((*topics)[1], "+#")){
				mosquitto_FREE(*local_sub);
				mosquitto_FREE(*topics);
				إرجاع MOSQ_ERR_PROTOCOL;
			}
			(*sharename) = (*topics)[1];
		}

		لكل(صحيح i=1; i<تعداد_2-1; i++){
			(*topics)[i] = (*topics)[i+1];
		}
		(*topics)[0] = "";
		(*topics)[تعداد_2-1] = عدم;
	}
	إرجاع MOSQ_ERR_SUCCESS;
}
