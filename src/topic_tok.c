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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto/mqtt_protocol.h"
#ଅନ୍ତର୍ଭାଵ "util_mosq.h"

#ଅନ୍ତର୍ଭାଵ "utlist.h"


ସ୍ଥିର ଅକ୍ଷର *strtok_hier(ଅକ୍ଷର *str, ଅକ୍ଷର **saveptr)
{
	ଅକ୍ଷର *c;

	ଯଦି(str != ଖାଲି){
		*saveptr = str;
	}

	ଯଦି(*saveptr == ଖାଲି){
		ଫେରନ୍ତୁ ଖାଲି;
	}

	c = strchr(*saveptr, '/');
	ଯଦି(c){
		str = *saveptr;
		*saveptr = c+1;
		c[0] = '\0';
	}ଅନ୍ୟଥା ଯଦି(*saveptr){
		/* No match, but surplus string */
		str = *saveptr;
		*saveptr = ଖାଲି;
	}
	ଫେରନ୍ତୁ str;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା sub__topic_tokenise(ଅଚଳ ଅକ୍ଷର *subtopic, ଅକ୍ଷର **local_sub, ଅକ୍ଷର ***topics, ଅଚଳ ଅକ୍ଷର **sharename)
{
	ଅକ୍ଷର *saveptr = ଖାଲି;
	ଅକ୍ଷର *token;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା topic_index = 0;
	size_t len;

	ଯଦି(!subtopic){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	len = strlen(subtopic);
	ଯଦି(len == 0){
		ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
	}

	*local_sub = mosquitto_strdup(subtopic);
	ଯଦି((*local_sub) == ଖାଲି){
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}

	ଗଣନା_2 = 0;
	saveptr = *local_sub;
	ଯେତେବେଳେ(saveptr){
		saveptr = strchr(&saveptr[1], '/');
		ଗଣନା_2++;
	}
	*topics = mosquitto_calloc((size_t)(ଗଣନା_2+3) /* 3=$shared,sharename,NULL */, ଆକାର(ଅକ୍ଷର *));
	ଯଦି((*topics) == ଖାଲି){
		mosquitto_FREE(*local_sub);
		ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
	}

	ଯଦି((*local_sub)[0] != '$'){
		(*topics)[topic_index] = "";
		topic_index++;
	}

	token = strtok_hier((*local_sub), &saveptr);
	ଯେତେବେଳେ(token){
		(*topics)[topic_index] = token;
		topic_index++;
		token = strtok_hier(ଖାଲି, &saveptr);
	}

	ଯଦି(!strcmp((*topics)[0], "$share")){
		ଯଦି(ଗଣନା_2 < 3 || (ଗଣନା_2 == 3 && strlen((*topics)[2]) == 0)){
			mosquitto_FREE(*local_sub);
			mosquitto_FREE(*topics);
			ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
		}

		ଯଦି(sharename){
			ଯଦି(strpbrk((*topics)[1], "+#")){
				mosquitto_FREE(*local_sub);
				mosquitto_FREE(*topics);
				ଫେରନ୍ତୁ MOSQ_ERR_PROTOCOL;
			}
			(*sharename) = (*topics)[1];
		}

		ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=1; i<ଗଣନା_2-1; i++){
			(*topics)[i] = (*topics)[i+1];
		}
		(*topics)[0] = "";
		(*topics)[ଗଣନା_2-1] = ଖାଲି;
	}
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}
