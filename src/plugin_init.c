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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

#ଅନ୍ତର୍ଭାଵ "mosquitto/broker.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "mosquitto/broker_plugin.h"
#ଅନ୍ତର୍ଭାଵ "lib_load.h"
#ଅନ୍ତର୍ଭାଵ "utlist.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*FUNC_auth_plugin_version)(ଶୂନ୍ୟ);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*FUNC_plugin_version)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା *);


ଶୂନ୍ୟ LIB_ERROR(ଶୂନ୍ୟ)
{
#ifdef WIN32
	ଅକ୍ଷର *buf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			ଖାଲି, GetLastError(), LANG_NEUTRAL, (LPTSTR)&buf, 0, ଖାଲି);
	log__printf(ଖାଲି, MOSQ_LOG_ERR, "Load error: %s", buf);
	LocalFree(buf);
#ଅନ୍ୟଥା
	log__printf(ଖାଲି, MOSQ_LOG_ERR, "Load error: %s", dlerror());
#endif
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା plugin__load_single(mosquitto_plugin_id_t *plugin)
{
	ଶୂନ୍ୟ *lib;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*plugin_version)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା *) = ଖାଲି;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*plugin_auth_version)(ଶୂନ୍ୟ) = ଖାଲି;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା version;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା plugin_versions[] = {5, 4, 3, 2};
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା plugin_version_ଗଣନା = ଆକାର(plugin_versions)/ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା);

	ଯଦି(plugin->config.security_option_ଗଣନା == 0){
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}

	memset(&plugin->lib, 0, ଆକାର(ସଂରଚନା mosquitto__plugin_lib));

	log__printf(ଖାଲି, MOSQ_LOG_INFO, "Loading plugin: %s", plugin->config.path);

	lib = LIB_LOAD(plugin->config.path);
	ଯଦି(!lib){
		log__printf(ଖାଲି, MOSQ_LOG_ERR,
				"Error: Unable to load plugin \"%s\".", plugin->config.path);
		LIB_ERROR();
		ଫେରନ୍ତୁ MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.lib = ଖାଲି;
	ଯଦି((plugin_version = (FUNC_plugin_version)LIB_SYM(lib, "mosquitto_plugin_version"))){
		version = plugin_version(plugin_version_ଗଣନା, plugin_versions);
	}ଅନ୍ୟଥା ଯଦି((plugin_auth_version = (FUNC_auth_plugin_version)LIB_SYM(lib, "mosquitto_auth_plugin_version"))){
		version = plugin_auth_version();
	}ଅନ୍ୟଥା{
		log__printf(ଖାଲି, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_version() or mosquitto_plugin_version().");
		LIB_ERROR();
		LIB_CLOSE(lib);
		ଫେରନ୍ତୁ MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.version = version;
	ଯଦି(version == 5){
		rc = plugin__load_v5(plugin, lib);
		ଯଦି(rc){
			ଫେରନ୍ତୁ rc;
		}
	}ଅନ୍ୟଥା ଯଦି(version == 4){
		rc = plugin__load_v4(plugin, lib);
		ଯଦି(rc){
			ଫେରନ୍ତୁ rc;
		}
	}ଅନ୍ୟଥା ଯଦି(version == 3){
		rc = plugin__load_v3(plugin, lib);
		ଯଦି(rc){
			ଫେରନ୍ତୁ rc;
		}
	}ଅନ୍ୟଥା ଯଦି(version == 2){
		rc = plugin__load_v2(plugin, lib);
		ଯଦି(rc){
			ଫେରନ୍ତୁ rc;
		}
	}ଅନ୍ୟଥା{
		log__printf(ଖାଲି, MOSQ_LOG_ERR,
				"Error: Unsupported auth plugin version (got %d, expected %d).",
				version, MOSQ_PLUGIN_VERSION);
		LIB_ERROR();

		LIB_CLOSE(lib);
		ଫେରନ୍ତୁ MOSQ_ERR_UNKNOWN;
	}
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା plugin__load_all(ଶୂନ୍ୟ)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = MOSQ_ERR_SUCCESS;

	ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<db.plugin_ଗଣନା; i++){
		rc = plugin__load_single(db.plugins[i]);
		ଯଦି(rc){
			ଫେରନ୍ତୁ rc;
		}
	}
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା plugin__security_init_single(mosquitto_plugin_id_t *plugin, bool reload)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ଯଦି(plugin->lib.version == 5){
		rc = MOSQ_ERR_SUCCESS;
	}ଅନ୍ୟଥା ଯଦି(plugin->lib.version == 4){
		rc = plugin->lib.security_init_v4(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_ଗଣନା,
				reload);

	}ଅନ୍ୟଥା ଯଦି(plugin->lib.version == 3){
		rc = plugin->lib.security_init_v3(
				plugin->lib.user_data,
				plugin->config.options,
				plugin->config.option_ଗଣନା,
				reload);

	}ଅନ୍ୟଥା ଯଦି(plugin->lib.version == 2){
		rc = plugin->lib.security_init_v2(
				plugin->lib.user_data,
				(ସଂରଚନା mosquitto_auth_opt *)plugin->config.options,
				plugin->config.option_ଗଣନା,
				reload);
	}ଅନ୍ୟଥା{
		rc = MOSQ_ERR_INVAL;
	}

	ଫେରନ୍ତୁ rc;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_security_init(bool reload)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<db.plugin_ଗଣନା; i++){
		rc = plugin__security_init_single(db.plugins[i], reload);
		ଯଦି(rc != MOSQ_ERR_SUCCESS){
			ଫେରନ୍ତୁ rc;
		}
	}
	rc = mosquitto_security_init_default();
	ଫେରନ୍ତୁ rc;
}
