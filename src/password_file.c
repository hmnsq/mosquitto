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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ଅନ୍ତର୍ଭାଵ <ctype.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "password_file.h"


ପୂର୍ଣ୍ଣସଂଖ୍ୟା broker_password_file__init(ଶୂନ୍ୟ)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	/* Load username/password data if required. */
	ଯଦି(db.config->per_listener_settings){
		ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<db.config->listener_ଗଣନା; i++){
			ଯଦି(db.config->listeners[i].security_options->password_data.password_file){
				rc = password_file__parse(&db.config->listeners[i].security_options->password_data);
				ଯଦି(rc){
					ଫେରନ୍ତୁ rc;
				}
				ଯଦି(db.config->listeners[i].security_options->plugin_ଗଣନା == 0){
					config__plugin_add_secopt(db.config->listeners[i].security_options->pid, db.config->listeners[i].security_options);
				}

				mosquitto_callback_register(db.config->listeners[i].security_options->pid,
						MOSQ_EVT_BASIC_AUTH, password_file__check, ଖାଲି, &db.config->listeners[i].security_options->password_data);
			}
		}
	}ଅନ୍ୟଥା{
		ଯଦି(db.config->security_options.password_data.password_file){
			rc = password_file__parse(&db.config->security_options.password_data);
			ଯଦି(rc){
				ଫେରନ୍ତୁ rc;
			}
			ଯଦି(db.config->security_options.plugin_ଗଣନା == 0){
				config__plugin_add_secopt(db.config->security_options.pid, &db.config->security_options);
			}

			mosquitto_callback_register(db.config->security_options.pid,
					MOSQ_EVT_BASIC_AUTH, password_file__check, ଖାଲି, &db.config->security_options.password_data);
		}
	}

	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ଶୂନ୍ୟ broker_password_file__cleanup(ଶୂନ୍ୟ)
{
	ଯଦି(db.config->per_listener_settings){
		ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<db.config->listener_ଗଣନା; i++){
			ଯଦି(db.config->listeners[i].security_options->pid){
				mosquitto_callback_unregister(db.config->listeners[i].security_options->pid,
						MOSQ_EVT_BASIC_AUTH, password_file__check, ଖାଲି);

				password_file__cleanup(&db.config->listeners[i].security_options->password_data);
			}
		}
	}ଅନ୍ୟଥା{
		ଯଦି(db.config->security_options.pid){
			mosquitto_callback_unregister(db.config->security_options.pid,
					MOSQ_EVT_BASIC_AUTH, password_file__check, ଖାଲି);

			password_file__cleanup(&db.config->security_options.password_data);
		}
	}
}
