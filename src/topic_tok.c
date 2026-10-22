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

#અન્તર્ભાવ "config.h"

#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <string.h>

#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "mosquitto/mqtt_protocol.h"
#અન્તર્ભાવ "util_mosq.h"

#અન્તર્ભાવ "utlist.h"


સ્થિર અક્ષર *strtok_hier(અક્ષર *str, અક્ષર **saveptr)
{
	અક્ષર *c;

	જો(str != ખાલી){
		*saveptr = str;
	}

	જો(*saveptr == ખાલી){
		પાછા ખાલી;
	}

	c = strchr(*saveptr, '/');
	જો(c){
		str = *saveptr;
		*saveptr = c+1;
		c[0] = '\0';
	}નહીંતર જો(*saveptr){
		/* No match, but surplus string */
		str = *saveptr;
		*saveptr = ખાલી;
	}
	પાછા str;
}


પૂર્ણાંક sub__topic_tokenise(અચલ અક્ષર *subtopic, અક્ષર **local_sub, અક્ષર ***topics, અચલ અક્ષર **sharename)
{
	અક્ષર *saveptr = ખાલી;
	અક્ષર *token;
	પૂર્ણાંક ગણતરી;
	પૂર્ણાંક topic_index = 0;
	size_t len;

	જો(!subtopic){
		પાછા MOSQ_ERR_INVAL;
	}

	len = strlen(subtopic);
	જો(len == 0){
		પાછા MOSQ_ERR_INVAL;
	}

	*local_sub = mosquitto_strdup(subtopic);
	જો((*local_sub) == ખાલી){
		પાછા MOSQ_ERR_NOMEM;
	}

	ગણતરી = 0;
	saveptr = *local_sub;
	જ્યારે(saveptr){
		saveptr = strchr(&saveptr[1], '/');
		ગણતરી++;
	}
	*topics = mosquitto_calloc((size_t)(ગણતરી+3) /* 3=$shared,sharename,NULL */, કદ(અક્ષર *));
	જો((*topics) == ખાલી){
		mosquitto_FREE(*local_sub);
		પાછા MOSQ_ERR_NOMEM;
	}

	જો((*local_sub)[0] != '$'){
		(*topics)[topic_index] = "";
		topic_index++;
	}

	token = strtok_hier((*local_sub), &saveptr);
	જ્યારે(token){
		(*topics)[topic_index] = token;
		topic_index++;
		token = strtok_hier(ખાલી, &saveptr);
	}

	જો(!strcmp((*topics)[0], "$share")){
		જો(ગણતરી < 3 || (ગણતરી == 3 && strlen((*topics)[2]) == 0)){
			mosquitto_FREE(*local_sub);
			mosquitto_FREE(*topics);
			પાછા MOSQ_ERR_PROTOCOL;
		}

		જો(sharename){
			જો(strpbrk((*topics)[1], "+#")){
				mosquitto_FREE(*local_sub);
				mosquitto_FREE(*topics);
				પાછા MOSQ_ERR_PROTOCOL;
			}
			(*sharename) = (*topics)[1];
		}

		માટે(પૂર્ણાંક i=1; i<ગણતરી-1; i++){
			(*topics)[i] = (*topics)[i+1];
		}
		(*topics)[0] = "";
		(*topics)[ગણતરી-1] = ખાલી;
	}
	પાછા MOSQ_ERR_SUCCESS;
}
