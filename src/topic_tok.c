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

#شامل "config.h"

#شامل <stdio.h>
#شامل <string.h>

#شامل "mosquitto_broker_internal.h"
#شامل "mosquitto/mqtt_protocol.h"
#شامل "util_mosq.h"

#شامل "utlist.h"


ایستا نویسه *strtok_hier(نویسه *str, نویسه **saveptr)
{
	نویسه *c;

	اگر(str != پوچ){
		*saveptr = str;
	}

	اگر(*saveptr == پوچ){
		بازگشت پوچ;
	}

	c = strchr(*saveptr, '/');
	اگر(c){
		str = *saveptr;
		*saveptr = c+1;
		c[0] = '\0';
	}وگرنه اگر(*saveptr){
		/* No match, but surplus string */
		str = *saveptr;
		*saveptr = پوچ;
	}
	بازگشت str;
}


صحیح sub__topic_tokenise(ثابت نویسه *subtopic, نویسه **local_sub, نویسه ***topics, ثابت نویسه **sharename)
{
	نویسه *saveptr = پوچ;
	نویسه *token;
	صحیح شمار;
	صحیح topic_index = 0;
	size_t len;

	اگر(!subtopic){
		بازگشت MOSQ_ERR_INVAL;
	}

	len = strlen(subtopic);
	اگر(len == 0){
		بازگشت MOSQ_ERR_INVAL;
	}

	*local_sub = mosquitto_strdup(subtopic);
	اگر((*local_sub) == پوچ){
		بازگشت MOSQ_ERR_NOMEM;
	}

	شمار = 0;
	saveptr = *local_sub;
	وقتی(saveptr){
		saveptr = strchr(&saveptr[1], '/');
		شمار++;
	}
	*topics = mosquitto_calloc((size_t)(شمار+3) /* 3=$shared,sharename,NULL */, اندازه(نویسه *));
	اگر((*topics) == پوچ){
		mosquitto_FREE(*local_sub);
		بازگشت MOSQ_ERR_NOMEM;
	}

	اگر((*local_sub)[0] != '$'){
		(*topics)[topic_index] = "";
		topic_index++;
	}

	token = strtok_hier((*local_sub), &saveptr);
	وقتی(token){
		(*topics)[topic_index] = token;
		topic_index++;
		token = strtok_hier(پوچ, &saveptr);
	}

	اگر(!strcmp((*topics)[0], "$share")){
		اگر(شمار < 3 || (شمار == 3 && strlen((*topics)[2]) == 0)){
			mosquitto_FREE(*local_sub);
			mosquitto_FREE(*topics);
			بازگشت MOSQ_ERR_PROTOCOL;
		}

		اگر(sharename){
			اگر(strpbrk((*topics)[1], "+#")){
				mosquitto_FREE(*local_sub);
				mosquitto_FREE(*topics);
				بازگشت MOSQ_ERR_PROTOCOL;
			}
			(*sharename) = (*topics)[1];
		}

		برای(صحیح i=1; i<شمار-1; i++){
			(*topics)[i] = (*topics)[i+1];
		}
		(*topics)[0] = "";
		(*topics)[شمار-1] = پوچ;
	}
	بازگشت MOSQ_ERR_SUCCESS;
}
