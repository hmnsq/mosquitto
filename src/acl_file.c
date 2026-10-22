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

#অন্তর্ভাব "config.h"

#অন্তর্ভাব <ctype.h>
#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <string.h>

#অন্তর্ভাব "acl_file.h"
#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "mosquitto/mqtt_protocol.h"
#অন্তর্ভাব "send_mosq.h"
#অন্তর্ভাব "util_mosq.h"


পূর্ণসংখ্যা broker_acl_file__init(শূন্য)
{
	পূর্ণসংখ্যা rc;

	/* Load acl data if required. */
	যদি(db.config->per_listener_settings){
		জন্য(পূর্ণসংখ্যা i=0; i<db.config->listener_গুনতি; i++){
			যদি(db.config->listeners[i].security_options->acl_তথ্য.acl_file){
				rc = acl_file__parse(&db.config->listeners[i].security_options->acl_তথ্য);
				যদি(rc){
					log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error opening acl file \"%s\".", db.config->listeners[i].security_options->acl_তথ্য.acl_file);
					ফেরত rc;
				}
				যদি(db.config->listeners[i].security_options->plugin_গুনতি == 0){
					config__plugin_add_secopt(db.config->listeners[i].security_options->pid, db.config->listeners[i].security_options);
				}

				mosquitto_callback_register(db.config->listeners[i].security_options->pid,
						MOSQ_EVT_ACL_CHECK, acl_file__check, ফাঁকা, &db.config->listeners[i].security_options->acl_তথ্য);
			}
		}
	}অন্যথা{
		যদি(db.config->security_options.acl_তথ্য.acl_file){
			rc = acl_file__parse(&db.config->security_options.acl_তথ্য);
			যদি(rc){
				log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error opening acl file \"%s\".", db.config->security_options.acl_তথ্য.acl_file);
				ফেরত rc;
			}
			যদি(db.config->security_options.plugin_গুনতি == 0){
				config__plugin_add_secopt(db.config->security_options.pid, &db.config->security_options);
			}

			mosquitto_callback_register(db.config->security_options.pid,
					MOSQ_EVT_ACL_CHECK, acl_file__check, ফাঁকা, &db.config->security_options.acl_তথ্য);
		}
	}

	ফেরত MOSQ_ERR_SUCCESS;
}


শূন্য broker_acl_file__cleanup(শূন্য)
{
	যদি(db.config->per_listener_settings){
		জন্য(পূর্ণসংখ্যা i=0; i<db.config->listener_গুনতি; i++){
			যদি(db.config->listeners[i].security_options->pid){
				mosquitto_callback_unregister(db.config->listeners[i].security_options->pid,
						MOSQ_EVT_ACL_CHECK, acl_file__check, ফাঁকা);

				acl_file__cleanup(&db.config->listeners[i].security_options->acl_তথ্য);
			}
		}
	}অন্যথা{
		যদি(db.config->security_options.pid){
			mosquitto_callback_unregister(db.config->security_options.pid,
					MOSQ_EVT_ACL_CHECK, acl_file__check, ফাঁকা);

			acl_file__cleanup(&db.config->security_options.acl_তথ্য);
		}
	}
}
