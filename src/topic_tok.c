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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/mqtt_protocol.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"

#ಅನ್ತರ್ಭಾವ "utlist.h"


ಸ್ಥಿರ ವರ್ಣ *strtok_hier(ವರ್ಣ *str, ವರ್ಣ **saveptr)
{
	ವರ್ಣ *c;

	ಯದಿ(str != ರಿಕ್ತ){
		*saveptr = str;
	}

	ಯದಿ(*saveptr == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	c = strchr(*saveptr, '/');
	ಯದಿ(c){
		str = *saveptr;
		*saveptr = c+1;
		c[0] = '\0';
	}ಅನ್ಯಥಾ ಯದಿ(*saveptr){
		/* No match, but surplus string */
		str = *saveptr;
		*saveptr = ರಿಕ್ತ;
	}
	ಪ್ರತ್ಯಾಗಮ str;
}


ಪೂರ್ಣಾಙ್ಕ sub__topic_tokenise(ಧ್ರುವ ವರ್ಣ *subtopic, ವರ್ಣ **local_sub, ವರ್ಣ ***topics, ಧ್ರುವ ವರ್ಣ **sharename)
{
	ವರ್ಣ *saveptr = ರಿಕ್ತ;
	ವರ್ಣ *token;
	ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ;
	ಪೂರ್ಣಾಙ್ಕ topic_index = 0;
	size_t len;

	ಯದಿ(!subtopic){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	len = strlen(subtopic);
	ಯದಿ(len == 0){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
	}

	*local_sub = mosquitto_strdup(subtopic);
	ಯದಿ((*local_sub) == ರಿಕ್ತ){
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}

	ಎಣಿಕೆ = 0;
	saveptr = *local_sub;
	ಯಾವತ್(saveptr){
		saveptr = strchr(&saveptr[1], '/');
		ಎಣಿಕೆ++;
	}
	*topics = mosquitto_calloc((size_t)(ಎಣಿಕೆ+3) /* 3=$shared,sharename,NULL */, ಪರಿಮಾಣ(ವರ್ಣ *));
	ಯದಿ((*topics) == ರಿಕ್ತ){
		mosquitto_FREE(*local_sub);
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
	}

	ಯದಿ((*local_sub)[0] != '$'){
		(*topics)[topic_index] = "";
		topic_index++;
	}

	token = strtok_hier((*local_sub), &saveptr);
	ಯಾವತ್(token){
		(*topics)[topic_index] = token;
		topic_index++;
		token = strtok_hier(ರಿಕ್ತ, &saveptr);
	}

	ಯದಿ(!strcmp((*topics)[0], "$share")){
		ಯದಿ(ಎಣಿಕೆ < 3 || (ಎಣಿಕೆ == 3 && strlen((*topics)[2]) == 0)){
			mosquitto_FREE(*local_sub);
			mosquitto_FREE(*topics);
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
		}

		ಯದಿ(sharename){
			ಯದಿ(strpbrk((*topics)[1], "+#")){
				mosquitto_FREE(*local_sub);
				mosquitto_FREE(*topics);
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_PROTOCOL;
			}
			(*sharename) = (*topics)[1];
		}

		ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=1; i<ಎಣಿಕೆ-1; i++){
			(*topics)[i] = (*topics)[i+1];
		}
		(*topics)[0] = "";
		(*topics)[ಎಣಿಕೆ-1] = ರಿಕ್ತ;
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}
