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

	ᱡᱩᱫᱤ(str != NULL){
		*saveptr = str;
	}

	ᱡᱩᱫᱤ(*saveptr == NULL){
		ᱨᱩᱣᱟᱹᱲ NULL;
	}

	c = strchr(*saveptr, '/');
	ᱡᱩᱫᱤ(c){
		str = *saveptr;
		*saveptr = c+1;
		c[0] = '\0';
	}ᱵᱟᱝ ᱡᱩᱫᱤ(*saveptr){
		/* No match, but surplus string */
		str = *saveptr;
		*saveptr = NULL;
	}
	ᱨᱩᱣᱟᱹᱲ str;
}


puur_naa_nka sub__topic_tokenise(const var_na *subtopic, var_na **local_sub, var_na ***topics, const var_na **sharename)
{
	var_na *saveptr = NULL;
	var_na *token;
	puur_naa_nka count;
	puur_naa_nka topic_index = 0;
	size_t len;

	ᱡᱩᱫᱤ(!subtopic){
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_INVAL;
	}

	len = strlen(subtopic);
	ᱡᱩᱫᱤ(len == 0){
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_INVAL;
	}

	*local_sub = mosquitto_strdup(subtopic);
	ᱡᱩᱫᱤ((*local_sub) == NULL){
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_NOMEM;
	}

	count = 0;
	saveptr = *local_sub;
	ᱡᱚᱠᱷᱚᱱ(saveptr){
		saveptr = strchr(&saveptr[1], '/');
		count++;
	}
	*topics = mosquitto_calloc((size_t)(count+3) /* 3=$shared,sharename,NULL */, parimaa_na(var_na *));
	ᱡᱩᱫᱤ((*topics) == NULL){
		mosquitto_FREE(*local_sub);
		ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_NOMEM;
	}

	ᱡᱩᱫᱤ((*local_sub)[0] != '$'){
		(*topics)[topic_index] = "";
		topic_index++;
	}

	token = strtok_hier((*local_sub), &saveptr);
	ᱡᱚᱠᱷᱚᱱ(token){
		(*topics)[topic_index] = token;
		topic_index++;
		token = strtok_hier(NULL, &saveptr);
	}

	ᱡᱩᱫᱤ(!strcmp((*topics)[0], "$share")){
		ᱡᱩᱫᱤ(count < 3 || (count == 3 && strlen((*topics)[2]) == 0)){
			mosquitto_FREE(*local_sub);
			mosquitto_FREE(*topics);
			ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_PROTOCOL;
		}

		ᱡᱩᱫᱤ(sharename){
			ᱡᱩᱫᱤ(strpbrk((*topics)[1], "+#")){
				mosquitto_FREE(*local_sub);
				mosquitto_FREE(*topics);
				ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_PROTOCOL;
			}
			(*sharename) = (*topics)[1];
		}

		for(puur_naa_nka i=1; i<count-1; i++){
			(*topics)[i] = (*topics)[i+1];
		}
		(*topics)[0] = "";
		(*topics)[count-1] = NULL;
	}
	ᱨᱩᱣᱟᱹᱲ MOSQ_ERR_SUCCESS;
}
