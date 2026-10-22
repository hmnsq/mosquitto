/*
Copyright (c) 2011-2021 Roger Light <roger@atchoo.org>

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

#अन्तर्भाव <ctype.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "password_file.h"


पूर्णाङ्क broker_password_file__init(शून्य)
{
	पूर्णाङ्क rc;

	/* Load username/password data if required. */
	सचे(db.config->per_listener_settings){
		अत्थाय(पूर्णाङ्क i=0; i<db.config->listener_गणनम्; i++){
			सचे(db.config->listeners[i].security_options->password_data.password_file){
				rc = password_file__parse(&db.config->listeners[i].security_options->password_data);
				सचे(rc){
					पटिनिवत्त rc;
				}
				सचे(db.config->listeners[i].security_options->plugin_गणनम् == 0){
					config__plugin_add_secopt(db.config->listeners[i].security_options->pid, db.config->listeners[i].security_options);
				}

				mosquitto_callback_register(db.config->listeners[i].security_options->pid,
						MOSQ_EVT_BASIC_AUTH, password_file__check, रिक्त, &db.config->listeners[i].security_options->password_data);
			}
		}
	}अञ्ञथा{
		सचे(db.config->security_options.password_data.password_file){
			rc = password_file__parse(&db.config->security_options.password_data);
			सचे(rc){
				पटिनिवत्त rc;
			}
			सचे(db.config->security_options.plugin_गणनम् == 0){
				config__plugin_add_secopt(db.config->security_options.pid, &db.config->security_options);
			}

			mosquitto_callback_register(db.config->security_options.pid,
					MOSQ_EVT_BASIC_AUTH, password_file__check, रिक्त, &db.config->security_options.password_data);
		}
	}

	पटिनिवत्त MOSQ_ERR_SUCCESS;
}


शून्य broker_password_file__cleanup(शून्य)
{
	सचे(db.config->per_listener_settings){
		अत्थाय(पूर्णाङ्क i=0; i<db.config->listener_गणनम्; i++){
			सचे(db.config->listeners[i].security_options->pid){
				mosquitto_callback_unregister(db.config->listeners[i].security_options->pid,
						MOSQ_EVT_BASIC_AUTH, password_file__check, रिक्त);

				password_file__cleanup(&db.config->listeners[i].security_options->password_data);
			}
		}
	}अञ्ञथा{
		सचे(db.config->security_options.pid){
			mosquitto_callback_unregister(db.config->security_options.pid,
					MOSQ_EVT_BASIC_AUTH, password_file__check, रिक्त);

			password_file__cleanup(&db.config->security_options.password_data);
		}
	}
}
