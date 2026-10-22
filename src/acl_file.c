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

#ضمن "acl_file.h"
#ضمن "mosquitto_broker_internal.h"
#ضمن "mosquitto/mqtt_protocol.h"
#ضمن "send_mosq.h"
#ضمن "util_mosq.h"


صحيح broker_acl_file__init(فراغ)
{
	صحيح rc;

	/* Load acl data if required. */
	𐡄𐡍(db.config->per_listener_settings){
		لكل(صحيح i=0; i<db.config->listener_count; i++){
			𐡄𐡍(db.config->listeners[i].security_options->acl_data.acl_file){
				rc = acl_file__parse(&db.config->listeners[i].security_options->acl_data);
				𐡄𐡍(rc){
					log__printf(عدم, MOSQ_LOG_ERR, "Error opening acl file \"%s\".", db.config->listeners[i].security_options->acl_data.acl_file);
					أرجع rc;
				}
				𐡄𐡍(db.config->listeners[i].security_options->plugin_count == 0){
					config__plugin_add_secopt(db.config->listeners[i].security_options->pid, db.config->listeners[i].security_options);
				}

				mosquitto_callback_register(db.config->listeners[i].security_options->pid,
						MOSQ_EVT_ACL_CHECK, acl_file__check, عدم, &db.config->listeners[i].security_options->acl_data);
			}
		}
	}وإلا{
		𐡄𐡍(db.config->security_options.acl_data.acl_file){
			rc = acl_file__parse(&db.config->security_options.acl_data);
			𐡄𐡍(rc){
				log__printf(عدم, MOSQ_LOG_ERR, "Error opening acl file \"%s\".", db.config->security_options.acl_data.acl_file);
				أرجع rc;
			}
			𐡄𐡍(db.config->security_options.plugin_count == 0){
				config__plugin_add_secopt(db.config->security_options.pid, &db.config->security_options);
			}

			mosquitto_callback_register(db.config->security_options.pid,
					MOSQ_EVT_ACL_CHECK, acl_file__check, عدم, &db.config->security_options.acl_data);
		}
	}

	أرجع MOSQ_ERR_SUCCESS;
}


فراغ broker_acl_file__cleanup(فراغ)
{
	𐡄𐡍(db.config->per_listener_settings){
		لكل(صحيح i=0; i<db.config->listener_count; i++){
			𐡄𐡍(db.config->listeners[i].security_options->pid){
				mosquitto_callback_unregister(db.config->listeners[i].security_options->pid,
						MOSQ_EVT_ACL_CHECK, acl_file__check, عدم);

				acl_file__cleanup(&db.config->listeners[i].security_options->acl_data);
			}
		}
	}وإلا{
		𐡄𐡍(db.config->security_options.pid){
			mosquitto_callback_unregister(db.config->security_options.pid,
					MOSQ_EVT_ACL_CHECK, acl_file__check, عدم);

			acl_file__cleanup(&db.config->security_options.acl_data);
		}
	}
}
