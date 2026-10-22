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

#समावेश "config.h"

#समावेश <ctype.h>
#समावेश <stdio.h>
#समावेश <string.h>

#समावेश "acl_file.h"
#समावेश "mosquitto_broker_internal.h"
#समावेश "mosquitto/mqtt_protocol.h"
#समावेश "send_mosq.h"
#समावेश "util_mosq.h"


पूर्णांक broker_acl_file__init(व्योम)
{
	पूर्णांक rc;

	/* Load acl data if required. */
	अगर(db.config->per_listener_settings){
		क्रम(पूर्णांक i=0; i<db.config->listener_गिनती; i++){
			अगर(db.config->listeners[i].security_options->acl_तथ्य.acl_file){
				rc = acl_file__parse(&db.config->listeners[i].security_options->acl_तथ्य);
				अगर(rc){
					log__printf(शून्य, MOSQ_LOG_ERR, "Error opening acl file \"%s\".", db.config->listeners[i].security_options->acl_तथ्य.acl_file);
					वापस rc;
				}
				अगर(db.config->listeners[i].security_options->plugin_गिनती == 0){
					config__plugin_add_secopt(db.config->listeners[i].security_options->pid, db.config->listeners[i].security_options);
				}

				mosquitto_callback_register(db.config->listeners[i].security_options->pid,
						MOSQ_EVT_ACL_CHECK, acl_file__check, शून्य, &db.config->listeners[i].security_options->acl_तथ्य);
			}
		}
	}अन्यथा{
		अगर(db.config->security_options.acl_तथ्य.acl_file){
			rc = acl_file__parse(&db.config->security_options.acl_तथ्य);
			अगर(rc){
				log__printf(शून्य, MOSQ_LOG_ERR, "Error opening acl file \"%s\".", db.config->security_options.acl_तथ्य.acl_file);
				वापस rc;
			}
			अगर(db.config->security_options.plugin_गिनती == 0){
				config__plugin_add_secopt(db.config->security_options.pid, &db.config->security_options);
			}

			mosquitto_callback_register(db.config->security_options.pid,
					MOSQ_EVT_ACL_CHECK, acl_file__check, शून्य, &db.config->security_options.acl_तथ्य);
		}
	}

	वापस MOSQ_ERR_SUCCESS;
}


व्योम broker_acl_file__cleanup(व्योम)
{
	अगर(db.config->per_listener_settings){
		क्रम(पूर्णांक i=0; i<db.config->listener_गिनती; i++){
			अगर(db.config->listeners[i].security_options->pid){
				mosquitto_callback_unregister(db.config->listeners[i].security_options->pid,
						MOSQ_EVT_ACL_CHECK, acl_file__check, शून्य);

				acl_file__cleanup(&db.config->listeners[i].security_options->acl_तथ्य);
			}
		}
	}अन्यथा{
		अगर(db.config->security_options.pid){
			mosquitto_callback_unregister(db.config->security_options.pid,
					MOSQ_EVT_ACL_CHECK, acl_file__check, शून्य);

			acl_file__cleanup(&db.config->security_options.acl_तथ्य);
		}
	}
}
