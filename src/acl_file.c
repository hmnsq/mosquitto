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

#અન્તર્ભાવ "config.h"

#અન્તર્ભાવ <ctype.h>
#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <string.h>

#અન્તર્ભાવ "acl_file.h"
#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "mosquitto/mqtt_protocol.h"
#અન્તર્ભાવ "send_mosq.h"
#અન્તર્ભાવ "util_mosq.h"


પૂર્ણાંક broker_acl_file__init(શૂન્ય)
{
	પૂર્ણાંક rc;

	/* Load acl data if required. */
	જો(db.config->per_listener_settings){
		માટે(પૂર્ણાંક i=0; i<db.config->listener_ગણતરી; i++){
			જો(db.config->listeners[i].security_options->acl_data.acl_file){
				rc = acl_file__parse(&db.config->listeners[i].security_options->acl_data);
				જો(rc){
					log__printf(ખાલી, MOSQ_LOG_ERR, "Error opening acl file \"%s\".", db.config->listeners[i].security_options->acl_data.acl_file);
					પાછા rc;
				}
				જો(db.config->listeners[i].security_options->plugin_ગણતરી == 0){
					config__plugin_add_secopt(db.config->listeners[i].security_options->pid, db.config->listeners[i].security_options);
				}

				mosquitto_callback_register(db.config->listeners[i].security_options->pid,
						MOSQ_EVT_ACL_CHECK, acl_file__check, ખાલી, &db.config->listeners[i].security_options->acl_data);
			}
		}
	}નહીંતર{
		જો(db.config->security_options.acl_data.acl_file){
			rc = acl_file__parse(&db.config->security_options.acl_data);
			જો(rc){
				log__printf(ખાલી, MOSQ_LOG_ERR, "Error opening acl file \"%s\".", db.config->security_options.acl_data.acl_file);
				પાછા rc;
			}
			જો(db.config->security_options.plugin_ગણતરી == 0){
				config__plugin_add_secopt(db.config->security_options.pid, &db.config->security_options);
			}

			mosquitto_callback_register(db.config->security_options.pid,
					MOSQ_EVT_ACL_CHECK, acl_file__check, ખાલી, &db.config->security_options.acl_data);
		}
	}

	પાછા MOSQ_ERR_SUCCESS;
}


શૂન્ય broker_acl_file__cleanup(શૂન્ય)
{
	જો(db.config->per_listener_settings){
		માટે(પૂર્ણાંક i=0; i<db.config->listener_ગણતરી; i++){
			જો(db.config->listeners[i].security_options->pid){
				mosquitto_callback_unregister(db.config->listeners[i].security_options->pid,
						MOSQ_EVT_ACL_CHECK, acl_file__check, ખાલી);

				acl_file__cleanup(&db.config->listeners[i].security_options->acl_data);
			}
		}
	}નહીંતર{
		જો(db.config->security_options.pid){
			mosquitto_callback_unregister(db.config->security_options.pid,
					MOSQ_EVT_ACL_CHECK, acl_file__check, ખાલી);

			acl_file__cleanup(&db.config->security_options.acl_data);
		}
	}
}
