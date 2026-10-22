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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/mqtt_protocol.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util_mosq.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "utlist.h"


ਸਥਿਰ ਅੱਖਰ *strtok_hier(ਅੱਖਰ *str, ਅੱਖਰ **saveptr)
{
	ਅੱਖਰ *c;

	ਜੇ(str != ਨਿਰਾ){
		*saveptr = str;
	}

	ਜੇ(*saveptr == ਨਿਰਾ){
		ਵਾਪਸ ਨਿਰਾ;
	}

	c = strchr(*saveptr, '/');
	ਜੇ(c){
		str = *saveptr;
		*saveptr = c+1;
		c[0] = '\0';
	}ਨਹੀਂਤਾਂ ਜੇ(*saveptr){
		/* No match, but surplus string */
		str = *saveptr;
		*saveptr = ਨਿਰਾ;
	}
	ਵਾਪਸ str;
}


ਪੂਰਨ sub__topic_tokenise(ਅਚਲ ਅੱਖਰ *subtopic, ਅੱਖਰ **local_sub, ਅੱਖਰ ***topics, ਅਚਲ ਅੱਖਰ **sharename)
{
	ਅੱਖਰ *saveptr = ਨਿਰਾ;
	ਅੱਖਰ *token;
	ਪੂਰਨ ਗਿਣਤੀ_2;
	ਪੂਰਨ topic_ਸੂਚਕਾਂਕ = 0;
	size_t len;

	ਜੇ(!subtopic){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	len = ਲੜੀ_ਲੰਬਾਈ(subtopic);
	ਜੇ(len == 0){
		ਵਾਪਸ MOSQ_ERR_INVAL;
	}

	*local_sub = mosquitto_strdup(subtopic);
	ਜੇ((*local_sub) == ਨਿਰਾ){
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}

	ਗਿਣਤੀ_2 = 0;
	saveptr = *local_sub;
	ਜਦੋਂ(saveptr){
		saveptr = strchr(&saveptr[1], '/');
		ਗਿਣਤੀ_2++;
	}
	*topics = mosquitto_calloc((size_t)(ਗਿਣਤੀ_2+3) /* 3=$shared,sharename,NULL */, ਆਕਾਰ(ਅੱਖਰ *));
	ਜੇ((*topics) == ਨਿਰਾ){
		mosquitto_FREE(*local_sub);
		ਵਾਪਸ MOSQ_ERR_NOMEM;
	}

	ਜੇ((*local_sub)[0] != '$'){
		(*topics)[topic_ਸੂਚਕਾਂਕ] = "";
		topic_ਸੂਚਕਾਂਕ++;
	}

	token = strtok_hier((*local_sub), &saveptr);
	ਜਦੋਂ(token){
		(*topics)[topic_ਸੂਚਕਾਂਕ] = token;
		topic_ਸੂਚਕਾਂਕ++;
		token = strtok_hier(ਨਿਰਾ, &saveptr);
	}

	ਜੇ(!strcmp((*topics)[0], "$share")){
		ਜੇ(ਗਿਣਤੀ_2 < 3 || (ਗਿਣਤੀ_2 == 3 && ਲੜੀ_ਲੰਬਾਈ((*topics)[2]) == 0)){
			mosquitto_FREE(*local_sub);
			mosquitto_FREE(*topics);
			ਵਾਪਸ MOSQ_ERR_PROTOCOL;
		}

		ਜੇ(sharename){
			ਜੇ(strpbrk((*topics)[1], "+#")){
				mosquitto_FREE(*local_sub);
				mosquitto_FREE(*topics);
				ਵਾਪਸ MOSQ_ERR_PROTOCOL;
			}
			(*sharename) = (*topics)[1];
		}

		ਲਈ(ਪੂਰਨ i=1; i<ਗਿਣਤੀ_2-1; i++){
			(*topics)[i] = (*topics)[i+1];
		}
		(*topics)[0] = "";
		(*topics)[ਗਿਣਤੀ_2-1] = ਨਿਰਾ;
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}
