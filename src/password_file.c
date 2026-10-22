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

#شامل "config.h"

#شامل <ctype.h>
#شامل <stdio.h>
#شامل <string.h>

#شامل "mosquitto_broker_internal.h"
#شامل "password_file.h"


صحیح broker_password_file__init(خالی)
{
	صحیح rc;

	/* Load username/password data if required. */
	اگر(db.config->فی_listener_settings){
		برائے(صحیح i=0; i<db.config->listener_گنتی; i++){
			اگر(db.config->listeners[i].security_options->password_ڈیٹا.password_file){
				rc = password_file__parse(&db.config->listeners[i].security_options->password_ڈیٹا);
				اگر(rc){
					واپس rc;
				}
				اگر(db.config->listeners[i].security_options->plugin_گنتی == 0){
					config_plugin_جوڑو_secopt(db.config->listeners[i].security_options->pid, db.config->listeners[i].security_options);
				}

				mosquitto_callback_رجسٹر(db.config->listeners[i].security_options->pid,
						MOSQ_EVT_BASIC_AUTH, password_file__check, تہی, &db.config->listeners[i].security_options->password_ڈیٹا);
			}
		}
	}ورنہ{
		اگر(db.config->security_options.password_ڈیٹا.password_file){
			rc = password_file__parse(&db.config->security_options.password_ڈیٹا);
			اگر(rc){
				واپس rc;
			}
			اگر(db.config->security_options.plugin_گنتی == 0){
				config_plugin_جوڑو_secopt(db.config->security_options.pid, &db.config->security_options);
			}

			mosquitto_callback_رجسٹر(db.config->security_options.pid,
					MOSQ_EVT_BASIC_AUTH, password_file__check, تہی, &db.config->security_options.password_ڈیٹا);
		}
	}

	واپس MOSQ_ERR_SUCCESS;
}


خالی broker_password_file__cleanup(خالی)
{
	اگر(db.config->فی_listener_settings){
		برائے(صحیح i=0; i<db.config->listener_گنتی; i++){
			اگر(db.config->listeners[i].security_options->pid){
				mosquitto_callback_unregister(db.config->listeners[i].security_options->pid,
						MOSQ_EVT_BASIC_AUTH, password_file__check, تہی);

				password_file__cleanup(&db.config->listeners[i].security_options->password_ڈیٹا);
			}
		}
	}ورنہ{
		اگر(db.config->security_options.pid){
			mosquitto_callback_unregister(db.config->security_options.pid,
					MOSQ_EVT_BASIC_AUTH, password_file__check, تہی);

			password_file__cleanup(&db.config->security_options.password_ڈیٹا);
		}
	}
}
