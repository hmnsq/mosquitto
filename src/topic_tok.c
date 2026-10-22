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

	もし(str != NULL){
		*saveptr = str;
	}

	もし(*saveptr == NULL){
		戻る NULL;
	}

	c = strchr(*saveptr, '/');
	もし(c){
		str = *saveptr;
		*saveptr = c+1;
		c[0] = '\0';
	}否则 もし(*saveptr){
		/* No match, but surplus string */
		str = *saveptr;
		*saveptr = NULL;
	}
	戻る str;
}


整数 sub__topic_tokenise(const char *subtopic, char **local_sub, char ***topics, const char **sharename)
{
	char *saveptr = NULL;
	char *token;
	整数 count;
	整数 topic_index = 0;
	size_t len;

	もし(!subtopic){
		戻る MOSQ_ERR_INVAL;
	}

	len = strlen(subtopic);
	もし(len == 0){
		戻る MOSQ_ERR_INVAL;
	}

	*local_sub = mosquitto_strdup(subtopic);
	もし((*local_sub) == NULL){
		戻る MOSQ_ERR_NOMEM;
	}

	count = 0;
	saveptr = *local_sub;
	ながら(saveptr){
		saveptr = strchr(&saveptr[1], '/');
		count++;
	}
	*topics = mosquitto_calloc((size_t)(count+3) /* 3=$shared,sharename,NULL */, sizeof(char *));
	もし((*topics) == NULL){
		mosquitto_FREE(*local_sub);
		戻る MOSQ_ERR_NOMEM;
	}

	もし((*local_sub)[0] != '$'){
		(*topics)[topic_index] = "";
		topic_index++;
	}

	token = strtok_hier((*local_sub), &saveptr);
	ながら(token){
		(*topics)[topic_index] = token;
		topic_index++;
		token = strtok_hier(NULL, &saveptr);
	}

	もし(!strcmp((*topics)[0], "$share")){
		もし(count < 3 || (count == 3 && strlen((*topics)[2]) == 0)){
			mosquitto_FREE(*local_sub);
			mosquitto_FREE(*topics);
			戻る MOSQ_ERR_PROTOCOL;
		}

		もし(sharename){
			もし(strpbrk((*topics)[1], "+#")){
				mosquitto_FREE(*local_sub);
				mosquitto_FREE(*topics);
				戻る MOSQ_ERR_PROTOCOL;
			}
			(*sharename) = (*topics)[1];
		}

		ために(整数 i=1; i<count-1; i++){
			(*topics)[i] = (*topics)[i+1];
		}
		(*topics)[0] = "";
		(*topics)[count-1] = NULL;
	}
	戻る MOSQ_ERR_SUCCESS;
}
