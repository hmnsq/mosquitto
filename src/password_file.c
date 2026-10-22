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

#ಅನ್ತರ್ಭಾವ "config.h"

#ಅನ್ತರ್ಭಾವ <ctype.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "password_file.h"


ಪೂರ್ಣಾಙ್ಕ broker_password_file__init(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಙ್ಕ rc;

	/* Load username/password data if required. */
	ಯದಿ(db.config->per_listener_settings){
		ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<db.config->listener_ಎಣಿಕೆ; i++){
			ಯದಿ(db.config->listeners[i].security_options->password_data.password_file){
				rc = password_file__parse(&db.config->listeners[i].security_options->password_data);
				ಯದಿ(rc){
					ಪ್ರತ್ಯಾಗಮ rc;
				}
				ಯದಿ(db.config->listeners[i].security_options->plugin_ಎಣಿಕೆ == 0){
					config__plugin_add_secopt(db.config->listeners[i].security_options->pid, db.config->listeners[i].security_options);
				}

				mosquitto_callback_register(db.config->listeners[i].security_options->pid,
						MOSQ_EVT_BASIC_AUTH, password_file__check, ರಿಕ್ತ, &db.config->listeners[i].security_options->password_data);
			}
		}
	}ಅನ್ಯಥಾ{
		ಯದಿ(db.config->security_options.password_data.password_file){
			rc = password_file__parse(&db.config->security_options.password_data);
			ಯದಿ(rc){
				ಪ್ರತ್ಯಾಗಮ rc;
			}
			ಯದಿ(db.config->security_options.plugin_ಎಣಿಕೆ == 0){
				config__plugin_add_secopt(db.config->security_options.pid, &db.config->security_options);
			}

			mosquitto_callback_register(db.config->security_options.pid,
					MOSQ_EVT_BASIC_AUTH, password_file__check, ರಿಕ್ತ, &db.config->security_options.password_data);
		}
	}

	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಶೂನ್ಯ broker_password_file__cleanup(ಶೂನ್ಯ)
{
	ಯದಿ(db.config->per_listener_settings){
		ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i=0; i<db.config->listener_ಎಣಿಕೆ; i++){
			ಯದಿ(db.config->listeners[i].security_options->pid){
				mosquitto_callback_unregister(db.config->listeners[i].security_options->pid,
						MOSQ_EVT_BASIC_AUTH, password_file__check, ರಿಕ್ತ);

				password_file__cleanup(&db.config->listeners[i].security_options->password_data);
			}
		}
	}ಅನ್ಯಥಾ{
		ಯದಿ(db.config->security_options.pid){
			mosquitto_callback_unregister(db.config->security_options.pid,
					MOSQ_EVT_BASIC_AUTH, password_file__check, ರಿಕ್ತ);

			password_file__cleanup(&db.config->security_options.password_data);
		}
	}
}
