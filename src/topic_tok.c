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


ಸ್ಥಿರ ಅಕ್ಷರ *strtok_hier(ಅಕ್ಷರ *str, ಅಕ್ಷರ **saveptr)
{
	ಅಕ್ಷರ *c;

	ಆದರೆ(str != ಖಾಲಿ){
		*saveptr = str;
	}

	ಆದರೆ(*saveptr == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	c = strchr(*saveptr, '/');
	ಆದರೆ(c){
		str = *saveptr;
		*saveptr = c+1;
		c[0] = '\0';
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(*saveptr){
		/* No match, but surplus string */
		str = *saveptr;
		*saveptr = ಖಾಲಿ;
	}
	ಹಿಂತಿರುಗಿ str;
}


ಪೂರ್ಣಾಂಕ sub__topic_tokenise(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *subtopic, ಅಕ್ಷರ **local_sub, ಅಕ್ಷರ ***topics, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **sharename)
{
	ಅಕ್ಷರ *saveptr = ಖಾಲಿ;
	ಅಕ್ಷರ *token;
	ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2;
	ಪೂರ್ಣಾಂಕ topic_ಸೂಚ್ಯಂಕ = 0;
	size_t len;

	ಆದರೆ(!subtopic){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(subtopic);
	ಆದರೆ(len == 0){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
	}

	*local_sub = mosquitto_strdup(subtopic);
	ಆದರೆ((*local_sub) == ಖಾಲಿ){
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}

	ಎಣಿಕೆ_2 = 0;
	saveptr = *local_sub;
	ಆಗ(saveptr){
		saveptr = strchr(&saveptr[1], '/');
		ಎಣಿಕೆ_2++;
	}
	*topics = mosquitto_calloc((size_t)(ಎಣಿಕೆ_2+3) /* 3=$shared,sharename,NULL */, ಗಾತ್ರ(ಅಕ್ಷರ *));
	ಆದರೆ((*topics) == ಖಾಲಿ){
		mosquitto_FREE(*local_sub);
		ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
	}

	ಆದರೆ((*local_sub)[0] != '$'){
		(*topics)[topic_ಸೂಚ್ಯಂಕ] = "";
		topic_ಸೂಚ್ಯಂಕ++;
	}

	token = strtok_hier((*local_sub), &saveptr);
	ಆಗ(token){
		(*topics)[topic_ಸೂಚ್ಯಂಕ] = token;
		topic_ಸೂಚ್ಯಂಕ++;
		token = strtok_hier(ಖಾಲಿ, &saveptr);
	}

	ಆದರೆ(!strcmp((*topics)[0], "$share")){
		ಆದರೆ(ಎಣಿಕೆ_2 < 3 || (ಎಣಿಕೆ_2 == 3 && ಅಕ್ಷರಸರಣಿ_ಉದ್ದ((*topics)[2]) == 0)){
			mosquitto_FREE(*local_sub);
			mosquitto_FREE(*topics);
			ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
		}

		ಆದರೆ(sharename){
			ಆದರೆ(strpbrk((*topics)[1], "+#")){
				mosquitto_FREE(*local_sub);
				mosquitto_FREE(*topics);
				ಹಿಂತಿರುಗಿ MOSQ_ERR_PROTOCOL;
			}
			(*sharename) = (*topics)[1];
		}

		ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=1; i<ಎಣಿಕೆ_2-1; i++){
			(*topics)[i] = (*topics)[i+1];
		}
		(*topics)[0] = "";
		(*topics)[ಎಣಿಕೆ_2-1] = ಖಾಲಿ;
	}
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}
