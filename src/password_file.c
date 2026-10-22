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

#അന്തര്ഭാവ "config.h"

#അന്തര്ഭാവ <ctype.h>
#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <string.h>

#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "password_file.h"


പൂർണ്ണസംഖ്യ broker_password_file__init(ശൂന്യം)
{
	പൂർണ്ണസംഖ്യ rc;

	/* Load username/password data if required. */
	എങ്കിൽ(db.config->per_listener_settings){
		വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<db.config->listener_എണ്ണം; i++){
			എങ്കിൽ(db.config->listeners[i].security_options->password_data.password_file){
				rc = password_file__parse(&db.config->listeners[i].security_options->password_data);
				എങ്കിൽ(rc){
					തിരികെ rc;
				}
				എങ്കിൽ(db.config->listeners[i].security_options->plugin_എണ്ണം == 0){
					config__plugin_add_secopt(db.config->listeners[i].security_options->pid, db.config->listeners[i].security_options);
				}

				mosquitto_callback_register(db.config->listeners[i].security_options->pid,
						MOSQ_EVT_BASIC_AUTH, password_file__check, ശൂന്യ, &db.config->listeners[i].security_options->password_data);
			}
		}
	}അല്ലെങ്കിൽ{
		എങ്കിൽ(db.config->security_options.password_data.password_file){
			rc = password_file__parse(&db.config->security_options.password_data);
			എങ്കിൽ(rc){
				തിരികെ rc;
			}
			എങ്കിൽ(db.config->security_options.plugin_എണ്ണം == 0){
				config__plugin_add_secopt(db.config->security_options.pid, &db.config->security_options);
			}

			mosquitto_callback_register(db.config->security_options.pid,
					MOSQ_EVT_BASIC_AUTH, password_file__check, ശൂന്യ, &db.config->security_options.password_data);
		}
	}

	തിരികെ MOSQ_ERR_SUCCESS;
}


ശൂന്യം broker_password_file__cleanup(ശൂന്യം)
{
	എങ്കിൽ(db.config->per_listener_settings){
		വേണ്ടി(പൂർണ്ണസംഖ്യ i=0; i<db.config->listener_എണ്ണം; i++){
			എങ്കിൽ(db.config->listeners[i].security_options->pid){
				mosquitto_callback_unregister(db.config->listeners[i].security_options->pid,
						MOSQ_EVT_BASIC_AUTH, password_file__check, ശൂന്യ);

				password_file__cleanup(&db.config->listeners[i].security_options->password_data);
			}
		}
	}അല്ലെങ്കിൽ{
		എങ്കിൽ(db.config->security_options.pid){
			mosquitto_callback_unregister(db.config->security_options.pid,
					MOSQ_EVT_BASIC_AUTH, password_file__check, ശൂന്യ);

			password_file__cleanup(&db.config->security_options.password_data);
		}
	}
}
