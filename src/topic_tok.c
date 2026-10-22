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

#समावेश "config.h"

#समावेश <stdio.h>
#समावेश <string.h>

#समावेश "mosquitto_broker_internal.h"
#समावेश "mosquitto/mqtt_protocol.h"
#समावेश "util_mosq.h"

#समावेश "utlist.h"


अटल अक्षर *strtok_hier(अक्षर *str, अक्षर **saveptr)
{
	अक्षर *c;

	अगर(str != शून्य){
		*saveptr = str;
	}

	अगर(*saveptr == शून्य){
		वापस शून्य;
	}

	c = म_अक्षर(*saveptr, '/');
	अगर(c){
		str = *saveptr;
		*saveptr = c+1;
		c[0] = '\0';
	}अन्यथा अगर(*saveptr){
		/* No match, but surplus string */
		str = *saveptr;
		*saveptr = शून्य;
	}
	वापस str;
}


पूर्णांक sub__topic_tokenise(स्थिर अक्षर *subtopic, अक्षर **local_sub, अक्षर ***topics, स्थिर अक्षर **sharename)
{
	अक्षर *saveptr = शून्य;
	अक्षर *token;
	पूर्णांक गिनती;
	पूर्णांक topic_सूचकांक = 0;
	माप_प्रकार len;

	अगर(!subtopic){
		वापस MOSQ_ERR_INVAL;
	}

	len = म_माप(subtopic);
	अगर(len == 0){
		वापस MOSQ_ERR_INVAL;
	}

	*local_sub = mosquitto_strdup(subtopic);
	अगर((*local_sub) == शून्य){
		वापस MOSQ_ERR_NOMEM;
	}

	गिनती = 0;
	saveptr = *local_sub;
	जबतक(saveptr){
		saveptr = म_अक्षर(&saveptr[1], '/');
		गिनती++;
	}
	*topics = mosquitto_calloc((माप_प्रकार)(गिनती+3) /* 3=$shared,sharename,NULL */, माप(अक्षर *));
	अगर((*topics) == शून्य){
		mosquitto_FREE(*local_sub);
		वापस MOSQ_ERR_NOMEM;
	}

	अगर((*local_sub)[0] != '$'){
		(*topics)[topic_सूचकांक] = "";
		topic_सूचकांक++;
	}

	token = strtok_hier((*local_sub), &saveptr);
	जबतक(token){
		(*topics)[topic_सूचकांक] = token;
		topic_सूचकांक++;
		token = strtok_hier(शून्य, &saveptr);
	}

	अगर(!म_भेद((*topics)[0], "$share")){
		अगर(गिनती < 3 || (गिनती == 3 && म_माप((*topics)[2]) == 0)){
			mosquitto_FREE(*local_sub);
			mosquitto_FREE(*topics);
			वापस MOSQ_ERR_PROTOCOL;
		}

		अगर(sharename){
			अगर(strpbrk((*topics)[1], "+#")){
				mosquitto_FREE(*local_sub);
				mosquitto_FREE(*topics);
				वापस MOSQ_ERR_PROTOCOL;
			}
			(*sharename) = (*topics)[1];
		}

		क्रम(पूर्णांक i=1; i<गिनती-1; i++){
			(*topics)[i] = (*topics)[i+1];
		}
		(*topics)[0] = "";
		(*topics)[गिनती-1] = शून्य;
	}
	वापस MOSQ_ERR_SUCCESS;
}
