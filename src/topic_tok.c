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


static var_na *strtok_hier(var_na *str, var_na **saveptr)
{
	var_na *c;

	ᤀᤡ᤺(str != NULL){
		*saveptr = str;
	}

	ᤀᤡ᤺(*saveptr == NULL){
		ᤀᤡᤴ NULL;
	}

	c = strchr(*saveptr, '/');
	ᤀᤡ᤺(c){
		str = *saveptr;
		*saveptr = c+1;
		c[0] = '\0';
	}ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(*saveptr){
		/* No match, but surplus string */
		str = *saveptr;
		*saveptr = NULL;
	}
	ᤀᤡᤴ str;
}


puur_naa_nka sub__topic_tokenise(const var_na *subtopic, var_na **local_sub, var_na ***topics, const var_na **sharename)
{
	var_na *saveptr = NULL;
	var_na *token;
	puur_naa_nka count;
	puur_naa_nka topic_index = 0;
	size_t len;

	ᤀᤡ᤺(!subtopic){
		ᤀᤡᤴ MOSQ_ERR_INVAL;
	}

	len = strlen(subtopic);
	ᤀᤡ᤺(len == 0){
		ᤀᤡᤴ MOSQ_ERR_INVAL;
	}

	*local_sub = mosquitto_strdup(subtopic);
	ᤀᤡ᤺((*local_sub) == NULL){
		ᤀᤡᤴ MOSQ_ERR_NOMEM;
	}

	count = 0;
	saveptr = *local_sub;
	ᤀᤠ᤺(saveptr){
		saveptr = strchr(&saveptr[1], '/');
		count++;
	}
	*topics = mosquitto_calloc((size_t)(count+3) /* 3=$shared,sharename,NULL */, parimaa_na(var_na *));
	ᤀᤡ᤺((*topics) == NULL){
		mosquitto_FREE(*local_sub);
		ᤀᤡᤴ MOSQ_ERR_NOMEM;
	}

	ᤀᤡ᤺((*local_sub)[0] != '$'){
		(*topics)[topic_index] = "";
		topic_index++;
	}

	token = strtok_hier((*local_sub), &saveptr);
	ᤀᤠ᤺(token){
		(*topics)[topic_index] = token;
		topic_index++;
		token = strtok_hier(NULL, &saveptr);
	}

	ᤀᤡ᤺(!strcmp((*topics)[0], "$share")){
		ᤀᤡ᤺(count < 3 || (count == 3 && strlen((*topics)[2]) == 0)){
			mosquitto_FREE(*local_sub);
			mosquitto_FREE(*topics);
			ᤀᤡᤴ MOSQ_ERR_PROTOCOL;
		}

		ᤀᤡ᤺(sharename){
			ᤀᤡ᤺(strpbrk((*topics)[1], "+#")){
				mosquitto_FREE(*local_sub);
				mosquitto_FREE(*topics);
				ᤀᤡᤴ MOSQ_ERR_PROTOCOL;
			}
			(*sharename) = (*topics)[1];
		}

		for(puur_naa_nka i=1; i<count-1; i++){
			(*topics)[i] = (*topics)[i+1];
		}
		(*topics)[0] = "";
		(*topics)[count-1] = NULL;
	}
	ᤀᤡᤴ MOSQ_ERR_SUCCESS;
}
