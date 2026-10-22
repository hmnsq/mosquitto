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

#ضمن "config.h"

#ضمن <ctype.h>
#ضمن <stdio.h>
#ضمن <string.h>

#ضمن "mosquitto_broker_internal.h"
#ضمن "password_file.h"


صحيح broker_password_file__init(فراغ)
{
	صحيح rc;

	/* Load username/password data if required. */
	אם(db.config->per_listener_settings){
		עבור(صحيح i=0; i<db.config->listener_count; i++){
			אם(db.config->listeners[i].security_options->password_data.password_file){
				rc = password_file__parse(&db.config->listeners[i].security_options->password_data);
				אם(rc){
					החזר rc;
				}
				אם(db.config->listeners[i].security_options->plugin_count == 0){
					config__plugin_add_secopt(db.config->listeners[i].security_options->pid, db.config->listeners[i].security_options);
				}

				mosquitto_callback_register(db.config->listeners[i].security_options->pid,
						MOSQ_EVT_BASIC_AUTH, password_file__check, عدم, &db.config->listeners[i].security_options->password_data);
			}
		}
	}אחרת{
		אם(db.config->security_options.password_data.password_file){
			rc = password_file__parse(&db.config->security_options.password_data);
			אם(rc){
				החזר rc;
			}
			אם(db.config->security_options.plugin_count == 0){
				config__plugin_add_secopt(db.config->security_options.pid, &db.config->security_options);
			}

			mosquitto_callback_register(db.config->security_options.pid,
					MOSQ_EVT_BASIC_AUTH, password_file__check, عدم, &db.config->security_options.password_data);
		}
	}

	החזר MOSQ_ERR_SUCCESS;
}


فراغ broker_password_file__cleanup(فراغ)
{
	אם(db.config->per_listener_settings){
		עבור(صحيح i=0; i<db.config->listener_count; i++){
			אם(db.config->listeners[i].security_options->pid){
				mosquitto_callback_unregister(db.config->listeners[i].security_options->pid,
						MOSQ_EVT_BASIC_AUTH, password_file__check, عدم);

				password_file__cleanup(&db.config->listeners[i].security_options->password_data);
			}
		}
	}אחרת{
		אם(db.config->security_options.pid){
			mosquitto_callback_unregister(db.config->security_options.pid,
					MOSQ_EVT_BASIC_AUTH, password_file__check, عدم);

			password_file__cleanup(&db.config->security_options.password_data);
		}
	}
}
