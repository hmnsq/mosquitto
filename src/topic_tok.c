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

#अन्तर्भाव "config.h"

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "mosquitto/mqtt_protocol.h"
#अन्तर्भाव "util_mosq.h"

#अन्तर्भाव "utlist.h"


स्थिर वर्ण *strtok_hier(वर्ण *str, वर्ण **saveptr)
{
	वर्ण *c;

	जइ(str != रिक्त){
		*saveptr = str;
	}

	जइ(*saveptr == रिक्त){
		प्रत्यागम रिक्त;
	}

	c = strchr(*saveptr, '/');
	जइ(c){
		str = *saveptr;
		*saveptr = c+1;
		c[0] = '\0';
	}अण्णहा जइ(*saveptr){
		/* No match, but surplus string */
		str = *saveptr;
		*saveptr = रिक्त;
	}
	प्रत्यागम str;
}


पूर्णाङ्क sub__topic_tokenise(ध्रुव वर्ण *subtopic, वर्ण **local_sub, वर्ण ***topics, ध्रुव वर्ण **sharename)
{
	वर्ण *saveptr = रिक्त;
	वर्ण *token;
	पूर्णाङ्क गणनम्;
	पूर्णाङ्क topic_index = 0;
	size_t len;

	जइ(!subtopic){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	len = strlen(subtopic);
	जइ(len == 0){
		प्रत्यागम MOSQ_ERR_INVAL;
	}

	*local_sub = mosquitto_strdup(subtopic);
	जइ((*local_sub) == रिक्त){
		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	गणनम् = 0;
	saveptr = *local_sub;
	जाव(saveptr){
		saveptr = strchr(&saveptr[1], '/');
		गणनम्++;
	}
	*topics = mosquitto_calloc((size_t)(गणनम्+3) /* 3=$shared,sharename,NULL */, परिमाण(वर्ण *));
	जइ((*topics) == रिक्त){
		mosquitto_FREE(*local_sub);
		प्रत्यागम MOSQ_ERR_NOMEM;
	}

	जइ((*local_sub)[0] != '$'){
		(*topics)[topic_index] = "";
		topic_index++;
	}

	token = strtok_hier((*local_sub), &saveptr);
	जाव(token){
		(*topics)[topic_index] = token;
		topic_index++;
		token = strtok_hier(रिक्त, &saveptr);
	}

	जइ(!strcmp((*topics)[0], "$share")){
		जइ(गणनम् < 3 || (गणनम् == 3 && strlen((*topics)[2]) == 0)){
			mosquitto_FREE(*local_sub);
			mosquitto_FREE(*topics);
			प्रत्यागम MOSQ_ERR_PROTOCOL;
		}

		जइ(sharename){
			जइ(strpbrk((*topics)[1], "+#")){
				mosquitto_FREE(*local_sub);
				mosquitto_FREE(*topics);
				प्रत्यागम MOSQ_ERR_PROTOCOL;
			}
			(*sharename) = (*topics)[1];
		}

		प्रति(पूर्णाङ्क i=1; i<गणनम्-1; i++){
			(*topics)[i] = (*topics)[i+1];
		}
		(*topics)[0] = "";
		(*topics)[गणनम्-1] = रिक्त;
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}
