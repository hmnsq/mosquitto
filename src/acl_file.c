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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <ctype.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "acl_file.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto/mqtt_protocol.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "send_mosq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util_mosq.h"


ਪੂਰਨ broker_acl_file__init(ਖਾਲੀ)
{
	ਪੂਰਨ rc;

	/* Load acl data if required. */
	ਜੇ(db.config->ਪ੍ਰਤੀ_listener_settings){
		ਲਈ(ਪੂਰਨ i=0; i<db.config->listener_ਗਿਣਤੀ; i++){
			ਜੇ(db.config->listeners[i].security_options->acl_ਅੰਕੜੇ.acl_file){
				rc = acl_file__parse(&db.config->listeners[i].security_options->acl_ਅੰਕੜੇ);
				ਜੇ(rc){
					log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error opening acl file \"%s\".", db.config->listeners[i].security_options->acl_ਅੰਕੜੇ.acl_file);
					ਵਾਪਸ rc;
				}
				ਜੇ(db.config->listeners[i].security_options->plugin_ਗਿਣਤੀ == 0){
					config_plugin_ਜੋੜੋ_secopt(db.config->listeners[i].security_options->pid, db.config->listeners[i].security_options);
				}

				mosquitto_callback_ਰਜਿਸਟਰ(db.config->listeners[i].security_options->pid,
						MOSQ_EVT_ACL_CHECK, acl_file__check, ਨਿਰਾ, &db.config->listeners[i].security_options->acl_ਅੰਕੜੇ);
			}
		}
	}ਨਹੀਂਤਾਂ{
		ਜੇ(db.config->security_options.acl_ਅੰਕੜੇ.acl_file){
			rc = acl_file__parse(&db.config->security_options.acl_ਅੰਕੜੇ);
			ਜੇ(rc){
				log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error opening acl file \"%s\".", db.config->security_options.acl_ਅੰਕੜੇ.acl_file);
				ਵਾਪਸ rc;
			}
			ਜੇ(db.config->security_options.plugin_ਗਿਣਤੀ == 0){
				config_plugin_ਜੋੜੋ_secopt(db.config->security_options.pid, &db.config->security_options);
			}

			mosquitto_callback_ਰਜਿਸਟਰ(db.config->security_options.pid,
					MOSQ_EVT_ACL_CHECK, acl_file__check, ਨਿਰਾ, &db.config->security_options.acl_ਅੰਕੜੇ);
		}
	}

	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਖਾਲੀ broker_acl_file__cleanup(ਖਾਲੀ)
{
	ਜੇ(db.config->ਪ੍ਰਤੀ_listener_settings){
		ਲਈ(ਪੂਰਨ i=0; i<db.config->listener_ਗਿਣਤੀ; i++){
			ਜੇ(db.config->listeners[i].security_options->pid){
				mosquitto_callback_unregister(db.config->listeners[i].security_options->pid,
						MOSQ_EVT_ACL_CHECK, acl_file__check, ਨਿਰਾ);

				acl_file__cleanup(&db.config->listeners[i].security_options->acl_ਅੰਕੜੇ);
			}
		}
	}ਨਹੀਂਤਾਂ{
		ਜੇ(db.config->security_options.pid){
			mosquitto_callback_unregister(db.config->security_options.pid,
					MOSQ_EVT_ACL_CHECK, acl_file__check, ਨਿਰਾ);

			acl_file__cleanup(&db.config->security_options.acl_ਅੰਕੜੇ);
		}
	}
}
