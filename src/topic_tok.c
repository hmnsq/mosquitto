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

#include "config.h"

#include <stdio.h>
#include <string.h>

#include "mosquitto_broker_internal.h"
#include "mosquitto/mqtt_protocol.h"
#include "util_mosq.h"

#include "utlist.h"


static char *strtok_hier(char *str, char **saveptr)
{
	char *c;

	ако(str != NULL){
		*saveptr = str;
	}

	ако(*saveptr == NULL){
		върни NULL;
	}

	c = strchr(*saveptr, '/');
	ако(c){
		str = *saveptr;
		*saveptr = c+1;
		c[0] = '\0';
	}иначе ако(*saveptr){
		/* No match, but surplus string */
		str = *saveptr;
		*saveptr = NULL;
	}
	върни str;
}


цяло sub__topic_tokenise(const char *subtopic, char **local_sub, char ***topics, const char **sharename)
{
	char *saveptr = NULL;
	char *token;
	цяло count;
	цяло topic_index = 0;
	size_t len;

	ако(!subtopic){
		върни MOSQ_ERR_INVAL;
	}

	len = strlen(subtopic);
	ако(len == 0){
		върни MOSQ_ERR_INVAL;
	}

	*local_sub = mosquitto_strdup(subtopic);
	ако((*local_sub) == NULL){
		върни MOSQ_ERR_NOMEM;
	}

	count = 0;
	saveptr = *local_sub;
	докато(saveptr){
		saveptr = strchr(&saveptr[1], '/');
		count++;
	}
	*topics = mosquitto_calloc((size_t)(count+3) /* 3=$shared,sharename,NULL */, sizeof(char *));
	ако((*topics) == NULL){
		mosquitto_FREE(*local_sub);
		върни MOSQ_ERR_NOMEM;
	}

	ако((*local_sub)[0] != '$'){
		(*topics)[topic_index] = "";
		topic_index++;
	}

	token = strtok_hier((*local_sub), &saveptr);
	докато(token){
		(*topics)[topic_index] = token;
		topic_index++;
		token = strtok_hier(NULL, &saveptr);
	}

	ако(!strcmp((*topics)[0], "$share")){
		ако(count < 3 || (count == 3 && strlen((*topics)[2]) == 0)){
			mosquitto_FREE(*local_sub);
			mosquitto_FREE(*topics);
			върни MOSQ_ERR_PROTOCOL;
		}

		ако(sharename){
			ако(strpbrk((*topics)[1], "+#")){
				mosquitto_FREE(*local_sub);
				mosquitto_FREE(*topics);
				върни MOSQ_ERR_PROTOCOL;
			}
			(*sharename) = (*topics)[1];
		}

		за(цяло i=1; i<count-1; i++){
			(*topics)[i] = (*topics)[i+1];
		}
		(*topics)[0] = "";
		(*topics)[count-1] = NULL;
	}
	върни MOSQ_ERR_SUCCESS;
}
