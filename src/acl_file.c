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

#ಅನ್ತರ್ಭಾವ "acl_file.h"
#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "mosquitto/mqtt_protocol.h"
#ಅನ್ತರ್ಭಾವ "send_mosq.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"


ಪೂರ್ಣಾಂಕ broker_acl_file__init(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಂಕ rc;

	/* Load acl data if required. */
	ಆದರೆ(db.config->ಪ್ರತಿ_listener_settings){
		ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<db.config->listener_ಎಣಿಕೆ; i++){
			ಆದರೆ(db.config->listeners[i].security_options->acl_ದತ್ತಾಂಶ.acl_file){
				rc = acl_file__parse(&db.config->listeners[i].security_options->acl_ದತ್ತಾಂಶ);
				ಆದರೆ(rc){
					log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error opening acl file \"%s\".", db.config->listeners[i].security_options->acl_ದತ್ತಾಂಶ.acl_file);
					ಹಿಂತಿರುಗಿ rc;
				}
				ಆದರೆ(db.config->listeners[i].security_options->plugin_ಎಣಿಕೆ == 0){
					config_plugin_ಕೂಡಿಸು_secopt(db.config->listeners[i].security_options->pid, db.config->listeners[i].security_options);
				}

				mosquitto_callback_ರಿಜಿಸ್ಟರ್(db.config->listeners[i].security_options->pid,
						MOSQ_EVT_ACL_CHECK, acl_file__check, ಖಾಲಿ, &db.config->listeners[i].security_options->acl_ದತ್ತಾಂಶ);
			}
		}
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಆದರೆ(db.config->security_options.acl_ದತ್ತಾಂಶ.acl_file){
			rc = acl_file__parse(&db.config->security_options.acl_ದತ್ತಾಂಶ);
			ಆದರೆ(rc){
				log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error opening acl file \"%s\".", db.config->security_options.acl_ದತ್ತಾಂಶ.acl_file);
				ಹಿಂತಿರುಗಿ rc;
			}
			ಆದರೆ(db.config->security_options.plugin_ಎಣಿಕೆ == 0){
				config_plugin_ಕೂಡಿಸು_secopt(db.config->security_options.pid, &db.config->security_options);
			}

			mosquitto_callback_ರಿಜಿಸ್ಟರ್(db.config->security_options.pid,
					MOSQ_EVT_ACL_CHECK, acl_file__check, ಖಾಲಿ, &db.config->security_options.acl_ದತ್ತಾಂಶ);
		}
	}

	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಶೂನ್ಯ broker_acl_file__cleanup(ಶೂನ್ಯ)
{
	ಆದರೆ(db.config->ಪ್ರತಿ_listener_settings){
		ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i=0; i<db.config->listener_ಎಣಿಕೆ; i++){
			ಆದರೆ(db.config->listeners[i].security_options->pid){
				mosquitto_callback_unregister(db.config->listeners[i].security_options->pid,
						MOSQ_EVT_ACL_CHECK, acl_file__check, ಖಾಲಿ);

				acl_file__cleanup(&db.config->listeners[i].security_options->acl_ದತ್ತಾಂಶ);
			}
		}
	}ಇಲ್ಲದಿದ್ದರೆ{
		ಆದರೆ(db.config->security_options.pid){
			mosquitto_callback_unregister(db.config->security_options.pid,
					MOSQ_EVT_ACL_CHECK, acl_file__check, ಖಾಲಿ);

			acl_file__cleanup(&db.config->security_options.acl_ದತ್ತಾಂಶ);
		}
	}
}
