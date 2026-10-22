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

#অন্তর্ভাব "config.h"

#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <string.h>

#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "mosquitto/mqtt_protocol.h"
#অন্তর্ভাব "util_mosq.h"

#অন্তর্ভাব "utlist.h"


স্থির অক্ষর *strtok_hier(অক্ষর *str, অক্ষর **saveptr)
{
	অক্ষর *c;

	যদি(str != ফাঁকা){
		*saveptr = str;
	}

	যদি(*saveptr == ফাঁকা){
		ফেরত ফাঁকা;
	}

	c = strchr(*saveptr, '/');
	যদি(c){
		str = *saveptr;
		*saveptr = c+1;
		c[0] = '\0';
	}অন্যথা যদি(*saveptr){
		/* No match, but surplus string */
		str = *saveptr;
		*saveptr = ফাঁকা;
	}
	ফেরত str;
}


পূর্ণসংখ্যা sub__topic_tokenise(ধ্রুবক অক্ষর *subtopic, অক্ষর **local_sub, অক্ষর ***topics, ধ্রুবক অক্ষর **sharename)
{
	অক্ষর *saveptr = ফাঁকা;
	অক্ষর *token;
	পূর্ণসংখ্যা গুনতি;
	পূর্ণসংখ্যা topic_সূচক = 0;
	size_t len;

	যদি(!subtopic){
		ফেরত MOSQ_ERR_INVAL;
	}

	len = strlen(subtopic);
	যদি(len == 0){
		ফেরত MOSQ_ERR_INVAL;
	}

	*local_sub = mosquitto_strdup(subtopic);
	যদি((*local_sub) == ফাঁকা){
		ফেরত MOSQ_ERR_NOMEM;
	}

	গুনতি = 0;
	saveptr = *local_sub;
	যখন(saveptr){
		saveptr = strchr(&saveptr[1], '/');
		গুনতি++;
	}
	*topics = mosquitto_calloc((size_t)(গুনতি+3) /* 3=$shared,sharename,NULL */, আকার(অক্ষর *));
	যদি((*topics) == ফাঁকা){
		mosquitto_FREE(*local_sub);
		ফেরত MOSQ_ERR_NOMEM;
	}

	যদি((*local_sub)[0] != '$'){
		(*topics)[topic_সূচক] = "";
		topic_সূচক++;
	}

	token = strtok_hier((*local_sub), &saveptr);
	যখন(token){
		(*topics)[topic_সূচক] = token;
		topic_সূচক++;
		token = strtok_hier(ফাঁকা, &saveptr);
	}

	যদি(!strcmp((*topics)[0], "$share")){
		যদি(গুনতি < 3 || (গুনতি == 3 && strlen((*topics)[2]) == 0)){
			mosquitto_FREE(*local_sub);
			mosquitto_FREE(*topics);
			ফেরত MOSQ_ERR_PROTOCOL;
		}

		যদি(sharename){
			যদি(strpbrk((*topics)[1], "+#")){
				mosquitto_FREE(*local_sub);
				mosquitto_FREE(*topics);
				ফেরত MOSQ_ERR_PROTOCOL;
			}
			(*sharename) = (*topics)[1];
		}

		জন্য(পূর্ণসংখ্যা i=1; i<গুনতি-1; i++){
			(*topics)[i] = (*topics)[i+1];
		}
		(*topics)[0] = "";
		(*topics)[গুনতি-1] = ফাঁকা;
	}
	ফেরত MOSQ_ERR_SUCCESS;
}
