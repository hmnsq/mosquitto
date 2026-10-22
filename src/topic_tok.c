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


ساکن حرف *strtok_hier(حرف *str, حرف **saveptr)
{
	حرف *c;

	اگر(str != تہی){
		*saveptr = str;
	}

	اگر(*saveptr == تہی){
		واپس تہی;
	}

	c = strchr(*saveptr, '/');
	اگر(c){
		str = *saveptr;
		*saveptr = c+1;
		c[0] = '\0';
	}ورنہ اگر(*saveptr){
		/* No match, but surplus string */
		str = *saveptr;
		*saveptr = تہی;
	}
	واپس str;
}


صحیح sub__topic_tokenise(ثابت حرف *subtopic, حرف **local_sub, حرف ***topics, ثابت حرف **sharename)
{
	حرف *saveptr = تہی;
	حرف *token;
	صحیح گنتی;
	صحیح topic_اشاریہ = 0;
	size_t len;

	اگر(!subtopic){
		واپس MOSQ_ERR_INVAL;
	}

	len = لڑی_لمبائی(subtopic);
	اگر(len == 0){
		واپس MOSQ_ERR_INVAL;
	}

	*local_sub = mosquitto_strdup(subtopic);
	اگر((*local_sub) == تہی){
		واپس MOSQ_ERR_NOMEM;
	}

	گنتی = 0;
	saveptr = *local_sub;
	جب(saveptr){
		saveptr = strchr(&saveptr[1], '/');
		گنتی++;
	}
	*topics = mosquitto_calloc((size_t)(گنتی+3) /* 3=$shared,sharename,NULL */, حجم(حرف *));
	اگر((*topics) == تہی){
		mosquitto_FREE(*local_sub);
		واپس MOSQ_ERR_NOMEM;
	}

	اگر((*local_sub)[0] != '$'){
		(*topics)[topic_اشاریہ] = "";
		topic_اشاریہ++;
	}

	token = strtok_hier((*local_sub), &saveptr);
	جب(token){
		(*topics)[topic_اشاریہ] = token;
		topic_اشاریہ++;
		token = strtok_hier(تہی, &saveptr);
	}

	اگر(!strcmp((*topics)[0], "$share")){
		اگر(گنتی < 3 || (گنتی == 3 && لڑی_لمبائی((*topics)[2]) == 0)){
			mosquitto_FREE(*local_sub);
			mosquitto_FREE(*topics);
			واپس MOSQ_ERR_PROTOCOL;
		}

		اگر(sharename){
			اگر(strpbrk((*topics)[1], "+#")){
				mosquitto_FREE(*local_sub);
				mosquitto_FREE(*topics);
				واپس MOSQ_ERR_PROTOCOL;
			}
			(*sharename) = (*topics)[1];
		}

		برائے(صحیح i=1; i<گنتی-1; i++){
			(*topics)[i] = (*topics)[i+1];
		}
		(*topics)[0] = "";
		(*topics)[گنتی-1] = تہی;
	}
	واپس MOSQ_ERR_SUCCESS;
}
