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

#అన్తర్భావ "config.h"

#అన్తర్భావ <ctype.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <string.h>

#అన్తర్భావ "acl_file.h"
#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "mosquitto/mqtt_protocol.h"
#అన్తర్భావ "send_mosq.h"
#అన్తర్భావ "util_mosq.h"


పూర్ణాంకం broker_acl_file__init(శూన్య)
{
	పూర్ణాంకం rc;

	/* Load acl data if required. */
	ఒకవేళ(db.config->చొప్పున_listener_settings){
		కొరకు(పూర్ణాంకం i=0; i<db.config->listener_లెక్క; i++){
			ఒకవేళ(db.config->listeners[i].security_options->acl_దత్తాంశం.acl_file){
				rc = acl_file__parse(&db.config->listeners[i].security_options->acl_దత్తాంశం);
				ఒకవేళ(rc){
					log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error opening acl file \"%s\".", db.config->listeners[i].security_options->acl_దత్తాంశం.acl_file);
					తిరిగి rc;
				}
				ఒకవేళ(db.config->listeners[i].security_options->plugin_లెక్క == 0){
					config_plugin_కూడు_secopt(db.config->listeners[i].security_options->pid, db.config->listeners[i].security_options);
				}

				mosquitto_callback_రిజిస్టరు(db.config->listeners[i].security_options->pid,
						MOSQ_EVT_ACL_CHECK, acl_file__check, ఖాళీ, &db.config->listeners[i].security_options->acl_దత్తాంశం);
			}
		}
	}లేకపోతే{
		ఒకవేళ(db.config->security_options.acl_దత్తాంశం.acl_file){
			rc = acl_file__parse(&db.config->security_options.acl_దత్తాంశం);
			ఒకవేళ(rc){
				log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error opening acl file \"%s\".", db.config->security_options.acl_దత్తాంశం.acl_file);
				తిరిగి rc;
			}
			ఒకవేళ(db.config->security_options.plugin_లెక్క == 0){
				config_plugin_కూడు_secopt(db.config->security_options.pid, &db.config->security_options);
			}

			mosquitto_callback_రిజిస్టరు(db.config->security_options.pid,
					MOSQ_EVT_ACL_CHECK, acl_file__check, ఖాళీ, &db.config->security_options.acl_దత్తాంశం);
		}
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


శూన్య broker_acl_file__cleanup(శూన్య)
{
	ఒకవేళ(db.config->చొప్పున_listener_settings){
		కొరకు(పూర్ణాంకం i=0; i<db.config->listener_లెక్క; i++){
			ఒకవేళ(db.config->listeners[i].security_options->pid){
				mosquitto_callback_unregister(db.config->listeners[i].security_options->pid,
						MOSQ_EVT_ACL_CHECK, acl_file__check, ఖాళీ);

				acl_file__cleanup(&db.config->listeners[i].security_options->acl_దత్తాంశం);
			}
		}
	}లేకపోతే{
		ఒకవేళ(db.config->security_options.pid){
			mosquitto_callback_unregister(db.config->security_options.pid,
					MOSQ_EVT_ACL_CHECK, acl_file__check, ఖాళీ);

			acl_file__cleanup(&db.config->security_options.acl_దత్తాంశం);
		}
	}
}
