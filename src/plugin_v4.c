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

/* This loads v4 plugins in a v5 wrapper to make the core code cleaner */

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


స్థిర పూర్ణాంకం plugin_v4_basic_auth(పూర్ణాంకం event, శూన్య *event_దత్తాంశం, శూన్య *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	నిర్మాణం mosquitto_evt_basic_auth *ed = event_దత్తాంశం;

	వాడని(event);

	ఒకవేళ(plugin->lib.unpwd_check_v4 == ఖాళీ){
		తిరిగి MOSQ_ERR_PLUGIN_DEFER;
	}

	తిరిగి plugin->lib.unpwd_check_v4(
			plugin->lib.user_దత్తాంశం,
			ed->client,
			ed->username,
			ed->password);
}


స్థిర పూర్ణాంకం plugin_v4_acl_check(పూర్ణాంకం event, శూన్య *event_దత్తాంశం, శూన్య *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	నిర్మాణం mosquitto_evt_acl_check *ed = event_దత్తాంశం;
	నిర్మాణం mosquitto_acl_సందేశం సందేశం;
	పూర్ణాంకం rc;

	వాడని(event);

	ఒకవేళ(plugin->lib.acl_check_v4 == ఖాళీ){
		తిరిగి MOSQ_ERR_PLUGIN_DEFER;
	}

	memset(&సందేశం, 0, పరిమాణం(సందేశం));
	సందేశం.topic = ed->topic;
	సందేశం.payloadlen = ed->payloadlen;
	సందేశం.payload = ed->payload;
	సందేశం.qos = ed->qos;
	సందేశం.retain = ed->retain;

	rc = acl__pre_check(plugin, ed->client, ed->access);
	ఒకవేళ(rc == MOSQ_ERR_PLUGIN_DEFER){
		తిరిగి plugin->lib.acl_check_v4(
				plugin->lib.user_దత్తాంశం,
				ed->access,
				ed->client,
				&సందేశం);
	}లేకపోతే{
		తిరిగి rc;
	}
}


స్థిర పూర్ణాంకం plugin_v4_auth_ప్రారంభం(పూర్ణాంకం event, శూన్య *event_దత్తాంశం, శూన్య *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	నిర్మాణం mosquitto_evt_extended_auth *ed = event_దత్తాంశం;

	వాడని(event);

	ఒకవేళ(plugin->lib.auth_ప్రారంభం_v4 == ఖాళీ){
		తిరిగి MOSQ_ERR_PLUGIN_DEFER;
	}

	తిరిగి plugin->lib.auth_ప్రారంభం_v4(
			plugin->lib.user_దత్తాంశం,
			ed->client,
			ed->client->auth_పద్ధతి,
			false,
			ed->దత్తాంశం_లోపల, ed->దత్తాంశం_లోపల_len,
			&ed->దత్తాంశం_బహిర్గతం, &ed->దత్తాంశం_బహిర్గతం_len);
}


స్థిర పూర్ణాంకం plugin_v4_auth_continue(పూర్ణాంకం event, శూన్య *event_దత్తాంశం, శూన్య *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	నిర్మాణం mosquitto_evt_extended_auth *ed = event_దత్తాంశం;

	వాడని(event);

	ఒకవేళ(plugin->lib.auth_continue_v4 == ఖాళీ){
		తిరిగి MOSQ_ERR_PLUGIN_DEFER;
	}

	తిరిగి plugin->lib.auth_continue_v4(
			plugin->lib.user_దత్తాంశం,
			ed->client,
			ed->client->auth_పద్ధతి,
			ed->దత్తాంశం_లోపల, ed->దత్తాంశం_లోపల_len,
			&ed->దత్తాంశం_బహిర్గతం, &ed->దత్తాంశం_బహిర్గతం_len);
}


స్థిర పూర్ణాంకం plugin_v4_psk_కీలకం_get(పూర్ణాంకం event, శూన్య *event_దత్తాంశం, శూన్య *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	నిర్మాణం mosquitto_evt_psk_కీలకం *ed = event_దత్తాంశం;

	వాడని(event);

	ఒకవేళ(plugin->lib.psk_కీలకం_get_v4 == ఖాళీ){
		తిరిగి MOSQ_ERR_PLUGIN_DEFER;
	}

	తిరిగి plugin->lib.psk_కీలకం_get_v4(
			plugin->lib.user_దత్తాంశం,
			ed->client,
			ed->hint,
			ed->తటస్థం,
			ed->కీలకం,
			ed->max_కీలకం_len);
}


స్థిర పూర్ణాంకం plugin_v4_reload(పూర్ణాంకం event, శూన్య *event_దత్తాంశం, శూన్య *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	పూర్ణాంకం rc;

	వాడని(event);
	వాడని(event_దత్తాంశం);

	rc = plugin->lib.security_cleanup_v4(
			plugin->lib.user_దత్తాంశం,
			plugin->config.options,
			plugin->config.option_లెక్క,
			true);
	ఒకవేళ(rc){
		తిరిగి rc;
	}

	rc = plugin->lib.security_init_v4(
			plugin->lib.user_దత్తాంశం,
			plugin->config.options,
			plugin->config.option_లెక్క,
			true);
	తిరిగి rc;
}


పూర్ణాంకం plugin_ఎక్కించు_v4(mosquitto_plugin_id_t *plugin, శూన్య *lib)
{
	పూర్ణాంకం rc;

	ఒకవేళ(!(plugin->lib.plugin_init_v4 = (FUNC_auth_plugin_init_v4)LIB_SYM(lib, "mosquitto_auth_plugin_init"))){
		log__printf(ఖాళీ, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_init().");
		LIB_ERROR();
		తిరిగి MOSQ_ERR_UNKNOWN;
	}
	ఒకవేళ(!(plugin->lib.plugin_cleanup_v4 = (FUNC_auth_plugin_cleanup_v4)LIB_SYM(lib, "mosquitto_auth_plugin_cleanup"))){
		log__printf(ఖాళీ, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_cleanup().");
		LIB_ERROR();
		తిరిగి MOSQ_ERR_UNKNOWN;
	}

	ఒకవేళ(!(plugin->lib.security_init_v4 = (FUNC_auth_plugin_security_init_v4)LIB_SYM(lib, "mosquitto_auth_security_init"))){
		log__printf(ఖాళీ, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_init().");
		LIB_ERROR();
		తిరిగి MOSQ_ERR_UNKNOWN;
	}

	ఒకవేళ(!(plugin->lib.security_cleanup_v4 = (FUNC_auth_plugin_security_cleanup_v4)LIB_SYM(lib, "mosquitto_auth_security_cleanup"))){
		log__printf(ఖాళీ, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_cleanup().");
		LIB_ERROR();
		తిరిగి MOSQ_ERR_UNKNOWN;
	}

	ఒకవేళ(!(plugin->lib.acl_check_v4 = (FUNC_auth_plugin_acl_check_v4)LIB_SYM(lib, "mosquitto_auth_acl_check"))){
		log__printf(ఖాళీ, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_acl_check().");
		LIB_ERROR();
		తిరిగి MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.unpwd_check_v4 = (FUNC_auth_plugin_unpwd_check_v4)LIB_SYM(lib, "mosquitto_auth_unpwd_check");
	ఒకవేళ(plugin->lib.unpwd_check_v4){
		log__printf(ఖాళీ, MOSQ_LOG_INFO,
				" ├── Username/password checking enabled.");
	}లేకపోతే{
		log__printf(ఖాళీ, MOSQ_LOG_INFO,
				" ├── Username/password checking not enabled.");
	}

	plugin->lib.psk_కీలకం_get_v4 = (FUNC_auth_plugin_psk_కీలకం_get_v4)LIB_SYM(lib, "mosquitto_auth_psk_key_get");
	ఒకవేళ(plugin->lib.psk_కీలకం_get_v4){
		log__printf(ఖాళీ, MOSQ_LOG_INFO,
				" ├── TLS-PSK checking enabled.");
	}లేకపోతే{
		log__printf(ఖాళీ, MOSQ_LOG_INFO,
				" ├── TLS-PSK checking not enabled.");
	}

	plugin->lib.auth_ప్రారంభం_v4 = (FUNC_auth_plugin_auth_ప్రారంభం_v4)LIB_SYM(lib, "mosquitto_auth_start");
	plugin->lib.auth_continue_v4 = (FUNC_auth_plugin_auth_continue_v4)LIB_SYM(lib, "mosquitto_auth_continue");

	ఒకవేళ(plugin->lib.auth_ప్రారంభం_v4){
		ఒకవేళ(plugin->lib.auth_continue_v4){
			log__printf(ఖాళీ, MOSQ_LOG_INFO,
					" └── Extended authentication enabled.");
		}లేకపోతే{
			log__printf(ఖాళీ, MOSQ_LOG_ERR,
					"Error: Plugin has missing mosquitto_auth_continue() function.");
			తిరిగి MOSQ_ERR_UNKNOWN;
		}
	}లేకపోతే{
		log__printf(ఖాళీ, MOSQ_LOG_INFO,
				" └── Extended authentication not enabled.");
	}

	plugin->lib.lib = lib;
	plugin->lib.user_దత్తాంశం = ఖాళీ;
	plugin->lib.identifier = plugin;

	ఒకవేళ(plugin->lib.plugin_init_v4){
		rc = plugin->lib.plugin_init_v4(&plugin->lib.user_దత్తాంశం, plugin->config.options, plugin->config.option_లెక్క);
		ఒకవేళ(rc){
			log__printf(ఖాళీ, MOSQ_LOG_ERR,
					"Error: Authentication plugin returned %d when initialising.", rc);
			తిరిగి rc;
		}
	}

	mosquitto_callback_రిజిస్టరు(plugin, MOSQ_EVT_RELOAD, plugin_v4_reload, ఖాళీ, plugin);

	ఒకవేళ(plugin->lib.unpwd_check_v4){
		mosquitto_callback_రిజిస్టరు(plugin, MOSQ_EVT_BASIC_AUTH, plugin_v4_basic_auth, ఖాళీ, plugin);
	}
	ఒకవేళ(plugin->lib.acl_check_v4){
		mosquitto_callback_రిజిస్టరు(plugin, MOSQ_EVT_ACL_CHECK, plugin_v4_acl_check, ఖాళీ, plugin);
	}
	ఒకవేళ(plugin->lib.auth_ప్రారంభం_v4){
		mosquitto_callback_రిజిస్టరు(plugin, MOSQ_EVT_EXT_AUTH_ప్రారంభం, plugin_v4_auth_ప్రారంభం, ఖాళీ, plugin);
	}
	ఒకవేళ(plugin->lib.auth_continue_v4){
		mosquitto_callback_రిజిస్టరు(plugin, MOSQ_EVT_EXT_AUTH_CONTINUE, plugin_v4_auth_continue, ఖాళీ, plugin);
	}
	ఒకవేళ(plugin->lib.psk_కీలకం_get_v4){
		mosquitto_callback_రిజిస్టరు(plugin, MOSQ_EVT_PSK_కీలకం, plugin_v4_psk_కీలకం_get, ఖాళీ, plugin);
	}

	తిరిగి 0;
}
