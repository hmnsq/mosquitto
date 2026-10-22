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

	如果(str != NULL){
		*saveptr = str;
	}

	如果(*saveptr == NULL){
		返回 NULL;
	}

	c = strchr(*saveptr, '/');
	如果(c){
		str = *saveptr;
		*saveptr = c+1;
		c[0] = '\0';
	}否则 如果(*saveptr){
		/* No match, but surplus string */
		str = *saveptr;
		*saveptr = NULL;
	}
	返回 str;
}


整数 sub__topic_tokenise(const char *subtopic, char **local_sub, char ***topics, const char **sharename)
{
	char *saveptr = NULL;
	char *token;
	整数 count;
	整数 topic_index = 0;
	size_t len;

	如果(!subtopic){
		返回 MOSQ_ERR_INVAL;
	}

	len = strlen(subtopic);
	如果(len == 0){
		返回 MOSQ_ERR_INVAL;
	}

	*local_sub = mosquitto_strdup(subtopic);
	如果((*local_sub) == NULL){
		返回 MOSQ_ERR_NOMEM;
	}

	count = 0;
	saveptr = *local_sub;
	当(saveptr){
		saveptr = strchr(&saveptr[1], '/');
		count++;
	}
	*topics = mosquitto_calloc((size_t)(count+3) /* 3=$shared,sharename,NULL */, sizeof(char *));
	如果((*topics) == NULL){
		mosquitto_FREE(*local_sub);
		返回 MOSQ_ERR_NOMEM;
	}

	如果((*local_sub)[0] != '$'){
		(*topics)[topic_index] = "";
		topic_index++;
	}

	token = strtok_hier((*local_sub), &saveptr);
	当(token){
		(*topics)[topic_index] = token;
		topic_index++;
		token = strtok_hier(NULL, &saveptr);
	}

	如果(!strcmp((*topics)[0], "$share")){
		如果(count < 3 || (count == 3 && strlen((*topics)[2]) == 0)){
			mosquitto_FREE(*local_sub);
			mosquitto_FREE(*topics);
			返回 MOSQ_ERR_PROTOCOL;
		}

		如果(sharename){
			如果(strpbrk((*topics)[1], "+#")){
				mosquitto_FREE(*local_sub);
				mosquitto_FREE(*topics);
				返回 MOSQ_ERR_PROTOCOL;
			}
			(*sharename) = (*topics)[1];
		}

		为(整数 i=1; i<count-1; i++){
			(*topics)[i] = (*topics)[i+1];
		}
		(*topics)[0] = "";
		(*topics)[count-1] = NULL;
	}
	返回 MOSQ_ERR_SUCCESS;
}
