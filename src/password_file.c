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

#அந்தர்பாவ "config.h"

#அந்தர்பாவ <ctype.h>
#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <string.h>

#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "password_file.h"


முழு broker_password_file__init(வெறுமை)
{
	முழு rc;

	/* Load username/password data if required. */
	எனில்(db.config->per_listener_settings){
		க்கு(முழு i=0; i<db.config->listener_மொத்தம்; i++){
			எனில்(db.config->listeners[i].security_options->password_data.password_file){
				rc = password_file__parse(&db.config->listeners[i].security_options->password_data);
				எனில்(rc){
					திரும்பு rc;
				}
				எனில்(db.config->listeners[i].security_options->plugin_மொத்தம் == 0){
					config__plugin_add_secopt(db.config->listeners[i].security_options->pid, db.config->listeners[i].security_options);
				}

				mosquitto_callback_register(db.config->listeners[i].security_options->pid,
						MOSQ_EVT_BASIC_AUTH, password_file__check, வெற்று, &db.config->listeners[i].security_options->password_data);
			}
		}
	}இல்லையெனில்{
		எனில்(db.config->security_options.password_data.password_file){
			rc = password_file__parse(&db.config->security_options.password_data);
			எனில்(rc){
				திரும்பு rc;
			}
			எனில்(db.config->security_options.plugin_மொத்தம் == 0){
				config__plugin_add_secopt(db.config->security_options.pid, &db.config->security_options);
			}

			mosquitto_callback_register(db.config->security_options.pid,
					MOSQ_EVT_BASIC_AUTH, password_file__check, வெற்று, &db.config->security_options.password_data);
		}
	}

	திரும்பு MOSQ_ERR_SUCCESS;
}


வெறுமை broker_password_file__cleanup(வெறுமை)
{
	எனில்(db.config->per_listener_settings){
		க்கு(முழு i=0; i<db.config->listener_மொத்தம்; i++){
			எனில்(db.config->listeners[i].security_options->pid){
				mosquitto_callback_unregister(db.config->listeners[i].security_options->pid,
						MOSQ_EVT_BASIC_AUTH, password_file__check, வெற்று);

				password_file__cleanup(&db.config->listeners[i].security_options->password_data);
			}
		}
	}இல்லையெனில்{
		எனில்(db.config->security_options.pid){
			mosquitto_callback_unregister(db.config->security_options.pid,
					MOSQ_EVT_BASIC_AUTH, password_file__check, வெற்று);

			password_file__cleanup(&db.config->security_options.password_data);
		}
	}
}
