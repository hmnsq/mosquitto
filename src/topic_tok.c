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

	ܐܢ(str != عدم){
		*saveptr = str;
	}

	ܐܢ(*saveptr == عدم){
		ܦܢܐ عدم;
	}

	c = strchr(*saveptr, '/');
	ܐܢ(c){
		str = *saveptr;
		*saveptr = c+1;
		c[0] = '\0';
	}ܐܚܪܢܐ ܐܢ(*saveptr){
		/* No match, but surplus string */
		str = *saveptr;
		*saveptr = عدم;
	}
	ܦܢܐ str;
}


صحيح sub__topic_tokenise(ثابت حرف *subtopic, حرف **local_sub, حرف ***topics, ثابت حرف **sharename)
{
	حرف *saveptr = عدم;
	حرف *token;
	صحيح count;
	صحيح topic_index = 0;
	size_t len;

	ܐܢ(!subtopic){
		ܦܢܐ MOSQ_ERR_INVAL;
	}

	len = strlen(subtopic);
	ܐܢ(len == 0){
		ܦܢܐ MOSQ_ERR_INVAL;
	}

	*local_sub = mosquitto_strdup(subtopic);
	ܐܢ((*local_sub) == عدم){
		ܦܢܐ MOSQ_ERR_NOMEM;
	}

	count = 0;
	saveptr = *local_sub;
	ܟܡܐ(saveptr){
		saveptr = strchr(&saveptr[1], '/');
		count++;
	}
	*topics = mosquitto_calloc((size_t)(count+3) /* 3=$shared,sharename,NULL */, حجم(حرف *));
	ܐܢ((*topics) == عدم){
		mosquitto_FREE(*local_sub);
		ܦܢܐ MOSQ_ERR_NOMEM;
	}

	ܐܢ((*local_sub)[0] != '$'){
		(*topics)[topic_index] = "";
		topic_index++;
	}

	token = strtok_hier((*local_sub), &saveptr);
	ܟܡܐ(token){
		(*topics)[topic_index] = token;
		topic_index++;
		token = strtok_hier(عدم, &saveptr);
	}

	ܐܢ(!strcmp((*topics)[0], "$share")){
		ܐܢ(count < 3 || (count == 3 && strlen((*topics)[2]) == 0)){
			mosquitto_FREE(*local_sub);
			mosquitto_FREE(*topics);
			ܦܢܐ MOSQ_ERR_PROTOCOL;
		}

		ܐܢ(sharename){
			ܐܢ(strpbrk((*topics)[1], "+#")){
				mosquitto_FREE(*local_sub);
				mosquitto_FREE(*topics);
				ܦܢܐ MOSQ_ERR_PROTOCOL;
			}
			(*sharename) = (*topics)[1];
		}

		لكل(صحيح i=1; i<count-1; i++){
			(*topics)[i] = (*topics)[i+1];
		}
		(*topics)[0] = "";
		(*topics)[count-1] = عدم;
	}
	ܦܢܐ MOSQ_ERR_SUCCESS;
}
