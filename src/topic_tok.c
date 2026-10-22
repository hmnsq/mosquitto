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

	यदि(str != रिक्त){
		*saveptr = str;
	}

	यदि(*saveptr == रिक्त){
		फर्काऊ रिक्त;
	}

	c = म_अक्षर(*saveptr, '/');
	यदि(c){
		str = *saveptr;
		*saveptr = c+1;
		c[0] = '\0';
	}अन्यथा यदि(*saveptr){
		/* No match, but surplus string */
		str = *saveptr;
		*saveptr = रिक्त;
	}
	फर्काऊ str;
}


पूर्णाङ्क sub__topic_tokenise(अचर वर्ण *subtopic, वर्ण **local_sub, वर्ण ***topics, अचर वर्ण **sharename)
{
	वर्ण *saveptr = रिक्त;
	वर्ण *token;
	पूर्णाङ्क गन्ती;
	पूर्णाङ्क topic_index = 0;
	माप_प्रकार len;

	यदि(!subtopic){
		फर्काऊ MOSQ_ERR_INVAL;
	}

	len = म_माप(subtopic);
	यदि(len == 0){
		फर्काऊ MOSQ_ERR_INVAL;
	}

	*local_sub = mosquitto_strdup(subtopic);
	यदि((*local_sub) == रिक्त){
		फर्काऊ MOSQ_ERR_NOMEM;
	}

	गन्ती = 0;
	saveptr = *local_sub;
	जबसम्म(saveptr){
		saveptr = म_अक्षर(&saveptr[1], '/');
		गन्ती++;
	}
	*topics = mosquitto_calloc((माप_प्रकार)(गन्ती+3) /* 3=$shared,sharename,NULL */, परिमाण(वर्ण *));
	यदि((*topics) == रिक्त){
		mosquitto_FREE(*local_sub);
		फर्काऊ MOSQ_ERR_NOMEM;
	}

	यदि((*local_sub)[0] != '$'){
		(*topics)[topic_index] = "";
		topic_index++;
	}

	token = strtok_hier((*local_sub), &saveptr);
	जबसम्म(token){
		(*topics)[topic_index] = token;
		topic_index++;
		token = strtok_hier(रिक्त, &saveptr);
	}

	यदि(!म_भेद((*topics)[0], "$share")){
		यदि(गन्ती < 3 || (गन्ती == 3 && म_माप((*topics)[2]) == 0)){
			mosquitto_FREE(*local_sub);
			mosquitto_FREE(*topics);
			फर्काऊ MOSQ_ERR_PROTOCOL;
		}

		यदि(sharename){
			यदि(strpbrk((*topics)[1], "+#")){
				mosquitto_FREE(*local_sub);
				mosquitto_FREE(*topics);
				फर्काऊ MOSQ_ERR_PROTOCOL;
			}
			(*sharename) = (*topics)[1];
		}

		लागि(पूर्णाङ्क i=1; i<गन्ती-1; i++){
			(*topics)[i] = (*topics)[i+1];
		}
		(*topics)[0] = "";
		(*topics)[गन्ती-1] = रिक्त;
	}
	फर्काऊ MOSQ_ERR_SUCCESS;
}
