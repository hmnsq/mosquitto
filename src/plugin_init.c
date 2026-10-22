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

#అన్తర్భావ <stdio.h>
#అన్తర్భావ <string.h>

#అన్తర్భావ "mosquitto/broker.h"
#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "mosquitto/broker_plugin.h"
#అన్తర్భావ "lib_load.h"
#అన్తర్భావ "utlist.h"

రకనిర్వచనం పూర్ణాంకం (*FUNC_auth_plugin_సంస్కరణ)(శూన్య);
రకనిర్వచనం పూర్ణాంకం (*FUNC_plugin_సంస్కరణ)(పూర్ణాంకం, స్థిరరాశి పూర్ణాంకం *);


శూన్య LIB_ERROR(శూన్య)
{
#నిర్వచితమైతే WIN32
	అక్షరం *buf;
	Format_సందేశం(FORMAT_సందేశం_ALLOCATE_బఫరు | FORMAT_సందేశం_FROM_SYSTEM,
			ఖాళీ, Get_చివరి_Error(), LANG_NEUTRAL, (LPTSTR)&buf, 0, ఖాళీ);
	log__printf(ఖాళీ, MOSQ_LOG_ERR, "Load error: %s", buf);
	LocalFree(buf);
#లేకపోతే
	log__printf(ఖాళీ, MOSQ_LOG_ERR, "Load error: %s", dlerror());
#అయితే_ముగింపు
}


స్థిర పూర్ణాంకం plugin_ఎక్కించు_single(mosquitto_plugin_id_t *plugin)
{
	శూన్య *lib;
	పూర్ణాంకం (*plugin_సంస్కరణ)(పూర్ణాంకం, స్థిరరాశి పూర్ణాంకం *) = ఖాళీ;
	పూర్ణాంకం (*plugin_auth_సంస్కరణ)(శూన్య) = ఖాళీ;
	పూర్ణాంకం సంస్కరణ_2;
	పూర్ణాంకం rc;
	స్థిరరాశి పూర్ణాంకం plugin_versions[] = {5, 4, 3, 2};
	పూర్ణాంకం plugin_సంస్కరణ_లెక్క = పరిమాణం(plugin_versions)/పరిమాణం(పూర్ణాంకం);

	ఒకవేళ(plugin->config.security_option_లెక్క == 0){
		తిరిగి MOSQ_ERR_SUCCESS;
	}

	memset(&plugin->lib, 0, పరిమాణం(నిర్మాణం mosquitto__plugin_lib));

	log__printf(ఖాళీ, MOSQ_LOG_INFO, "Loading plugin: %s", plugin->config.మార్గం);

	lib = LIB_ఎక్కించు(plugin->config.మార్గం);
	ఒకవేళ(!lib){
		log__printf(ఖాళీ, MOSQ_LOG_ERR,
				"Error: Unable to load plugin \"%s\".", plugin->config.మార్గం);
		LIB_ERROR();
		తిరిగి MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.lib = ఖాళీ;
	ఒకవేళ((plugin_సంస్కరణ = (FUNC_plugin_సంస్కరణ)LIB_SYM(lib, "mosquitto_plugin_version"))){
		సంస్కరణ_2 = plugin_సంస్కరణ(plugin_సంస్కరణ_లెక్క, plugin_versions);
	}లేకపోతే ఒకవేళ((plugin_auth_సంస్కరణ = (FUNC_auth_plugin_సంస్కరణ)LIB_SYM(lib, "mosquitto_auth_plugin_version"))){
		సంస్కరణ_2 = plugin_auth_సంస్కరణ();
	}లేకపోతే{
		log__printf(ఖాళీ, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_version() or mosquitto_plugin_version().");
		LIB_ERROR();
		LIB_CLOSE(lib);
		తిరిగి MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.సంస్కరణ_2 = సంస్కరణ_2;
	ఒకవేళ(సంస్కరణ_2 == 5){
		rc = plugin_ఎక్కించు_v5(plugin, lib);
		ఒకవేళ(rc){
			తిరిగి rc;
		}
	}లేకపోతే ఒకవేళ(సంస్కరణ_2 == 4){
		rc = plugin_ఎక్కించు_v4(plugin, lib);
		ఒకవేళ(rc){
			తిరిగి rc;
		}
	}లేకపోతే ఒకవేళ(సంస్కరణ_2 == 3){
		rc = plugin_ఎక్కించు_v3(plugin, lib);
		ఒకవేళ(rc){
			తిరిగి rc;
		}
	}లేకపోతే ఒకవేళ(సంస్కరణ_2 == 2){
		rc = plugin_ఎక్కించు_v2(plugin, lib);
		ఒకవేళ(rc){
			తిరిగి rc;
		}
	}లేకపోతే{
		log__printf(ఖాళీ, MOSQ_LOG_ERR,
				"Error: Unsupported auth plugin version (got %d, expected %d).",
				సంస్కరణ_2, MOSQ_PLUGIN_సంస్కరణ);
		LIB_ERROR();

		LIB_CLOSE(lib);
		తిరిగి MOSQ_ERR_UNKNOWN;
	}
	తిరిగి MOSQ_ERR_SUCCESS;
}


పూర్ణాంకం plugin_ఎక్కించు_all(శూన్య)
{
	పూర్ణాంకం rc = MOSQ_ERR_SUCCESS;

	కొరకు(పూర్ణాంకం i=0; i<db.plugin_లెక్క; i++){
		rc = plugin_ఎక్కించు_single(db.plugins[i]);
		ఒకవేళ(rc){
			తిరిగి rc;
		}
	}
	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర పూర్ణాంకం plugin__security_init_single(mosquitto_plugin_id_t *plugin, bool reload)
{
	పూర్ణాంకం rc;

	ఒకవేళ(plugin->lib.సంస్కరణ_2 == 5){
		rc = MOSQ_ERR_SUCCESS;
	}లేకపోతే ఒకవేళ(plugin->lib.సంస్కరణ_2 == 4){
		rc = plugin->lib.security_init_v4(
				plugin->lib.user_దత్తాంశం,
				plugin->config.options,
				plugin->config.option_లెక్క,
				reload);

	}లేకపోతే ఒకవేళ(plugin->lib.సంస్కరణ_2 == 3){
		rc = plugin->lib.security_init_v3(
				plugin->lib.user_దత్తాంశం,
				plugin->config.options,
				plugin->config.option_లెక్క,
				reload);

	}లేకపోతే ఒకవేళ(plugin->lib.సంస్కరణ_2 == 2){
		rc = plugin->lib.security_init_v2(
				plugin->lib.user_దత్తాంశం,
				(నిర్మాణం mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_లెక్క,
				reload);
	}లేకపోతే{
		rc = MOSQ_ERR_INVAL;
	}

	తిరిగి rc;
}


పూర్ణాంకం mosquitto_security_init(bool reload)
{
	పూర్ణాంకం rc;

	కొరకు(పూర్ణాంకం i=0; i<db.plugin_లెక్క; i++){
		rc = plugin__security_init_single(db.plugins[i], reload);
		ఒకవేళ(rc != MOSQ_ERR_SUCCESS){
			తిరిగి rc;
		}
	}
	rc = mosquitto_security_init_default();
	తిరిగి rc;
}
