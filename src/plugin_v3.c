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

/* This loads v3 plugins in a v5 wrapper to make the core code cleaner */

#અન્તર્ભાવ "config.h"

#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <string.h>

#અન્તર્ભાવ "mosquitto/broker.h"
#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "mosquitto/broker_plugin.h"
#અન્તર્ભાવ "lib_load.h"
#અન્તર્ભાવ "utlist.h"

પ્રકારવ્યાખ્યા પૂર્ણાંક (*FUNC_auth_plugin_version)(શૂન્ય);
પ્રકારવ્યાખ્યા પૂર્ણાંક (*FUNC_plugin_version)(પૂર્ણાંક, અચલ પૂર્ણાંક *);


સ્થિર પૂર્ણાંક plugin_v3_basic_auth(પૂર્ણાંક event, શૂન્ય *event_data, શૂન્ય *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	સંરચના mosquitto_evt_basic_auth *ed = event_data;

	UNUSED(event);

	જો(plugin->lib.unpwd_check_v3 == ખાલી){
		પાછા MOSQ_ERR_INVAL;
	}

	પાછા plugin->lib.unpwd_check_v3(
			plugin->lib.user_data,
			ed->client,
			ed->username,
			ed->password);
}


સ્થિર પૂર્ણાંક plugin_v3_acl_check(પૂર્ણાંક event, શૂન્ય *event_data, શૂન્ય *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	સંરચના mosquitto_evt_acl_check *ed = event_data;
	સંરચના mosquitto_acl_સંદેશ સંદેશ;
	પૂર્ણાંક rc;

	UNUSED(event);

	જો(plugin->lib.acl_check_v3 == ખાલી){
		પાછા MOSQ_ERR_INVAL;
	}

	memset(&સંદેશ, 0, કદ(સંદેશ));
	સંદેશ.topic = ed->topic;
	સંદેશ.payloadlen = ed->payloadlen;
	સંદેશ.payload = ed->payload;
	સંદેશ.qos = ed->qos;
	સંદેશ.retain = ed->retain;

	rc = acl__pre_check(plugin, ed->client, ed->access);
	જો(rc == MOSQ_ERR_PLUGIN_DEFER){
		પાછા plugin->lib.acl_check_v3(
				plugin->lib.user_data,
				ed->access,
				ed->client,
				&સંદેશ);
	}નહીંતર{
		પાછા rc;
	}
}


સ્થિર પૂર્ણાંક plugin_v3_psk_ચાવી_get(પૂર્ણાંક event, શૂન્ય *event_data, શૂન્ય *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	સંરચના mosquitto_evt_psk_ચાવી *ed = event_data;

	UNUSED(event);

	જો(plugin->lib.psk_ચાવી_get_v3 == ખાલી){
		પાછા MOSQ_ERR_INVAL;
	}

	પાછા plugin->lib.psk_ચાવી_get_v3(
			plugin->lib.user_data,
			ed->client,
			ed->hint,
			ed->identity,
			ed->ચાવી,
			ed->max_ચાવી_len);
}


સ્થિર પૂર્ણાંક plugin_v3_reload(પૂર્ણાંક event, શૂન્ય *event_data, શૂન્ય *userdata)
{
	mosquitto_plugin_id_t *plugin = userdata;
	પૂર્ણાંક rc;

	UNUSED(event);
	UNUSED(event_data);

	rc = plugin->lib.security_cleanup_v3(
			plugin->lib.user_data,
			plugin->config.options,
			plugin->config.option_ગણતરી,
			true);
	જો(rc){
		પાછા rc;
	}

	rc = plugin->lib.security_init_v3(
			plugin->lib.user_data,
			plugin->config.options,
			plugin->config.option_ગણતરી,
			true);
	પાછા rc;
}


પૂર્ણાંક plugin__load_v3(mosquitto_plugin_id_t *plugin, શૂન્ય *lib)
{
	પૂર્ણાંક rc;

	જો(!(plugin->lib.plugin_init_v3 = (FUNC_auth_plugin_init_v3)LIB_SYM(lib, "mosquitto_auth_plugin_init"))){
		log__printf(ખાલી, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_init().");
		LIB_ERROR();
		પાછા MOSQ_ERR_UNKNOWN;
	}
	જો(!(plugin->lib.plugin_cleanup_v3 = (FUNC_auth_plugin_cleanup_v3)LIB_SYM(lib, "mosquitto_auth_plugin_cleanup"))){
		log__printf(ખાલી, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_plugin_cleanup().");
		LIB_ERROR();
		પાછા MOSQ_ERR_UNKNOWN;
	}

	જો(!(plugin->lib.security_init_v3 = (FUNC_auth_plugin_security_init_v3)LIB_SYM(lib, "mosquitto_auth_security_init"))){
		log__printf(ખાલી, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_init().");
		LIB_ERROR();
		પાછા MOSQ_ERR_UNKNOWN;
	}

	જો(!(plugin->lib.security_cleanup_v3 = (FUNC_auth_plugin_security_cleanup_v3)LIB_SYM(lib, "mosquitto_auth_security_cleanup"))){
		log__printf(ખાલી, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_security_cleanup().");
		LIB_ERROR();
		પાછા MOSQ_ERR_UNKNOWN;
	}

	જો(!(plugin->lib.acl_check_v3 = (FUNC_auth_plugin_acl_check_v3)LIB_SYM(lib, "mosquitto_auth_acl_check"))){
		log__printf(ખાલી, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_acl_check().");
		LIB_ERROR();
		પાછા MOSQ_ERR_UNKNOWN;
	}

	જો(!(plugin->lib.unpwd_check_v3 = (FUNC_auth_plugin_unpwd_check_v3)LIB_SYM(lib, "mosquitto_auth_unpwd_check"))){
		log__printf(ખાલી, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_unpwd_check().");
		LIB_ERROR();
		પાછા MOSQ_ERR_UNKNOWN;
	}

	જો(!(plugin->lib.psk_ચાવી_get_v3 = (FUNC_auth_plugin_psk_ચાવી_get_v3)LIB_SYM(lib, "mosquitto_auth_psk_key_get"))){
		log__printf(ખાલી, MOSQ_LOG_ERR,
				"Error: Unable to load auth plugin function mosquitto_auth_psk_key_get().");
		LIB_ERROR();
		પાછા MOSQ_ERR_UNKNOWN;
	}

	plugin->lib.lib = lib;
	plugin->lib.user_data = ખાલી;
	plugin->lib.identifier = plugin;
	જો(plugin->lib.plugin_init_v3){
		rc = plugin->lib.plugin_init_v3(&plugin->lib.user_data, plugin->config.options, plugin->config.option_ગણતરી);
		જો(rc){
			log__printf(ખાલી, MOSQ_LOG_ERR,
					"Error: Authentication plugin returned %d when initialising.", rc);
			પાછા rc;
		}
	}

	mosquitto_callback_register(plugin, MOSQ_EVT_RELOAD, plugin_v3_reload, ખાલી, plugin);

	જો(plugin->lib.unpwd_check_v3){
		mosquitto_callback_register(plugin, MOSQ_EVT_BASIC_AUTH, plugin_v3_basic_auth, ખાલી, plugin);
	}
	જો(plugin->lib.acl_check_v3){
		mosquitto_callback_register(plugin, MOSQ_EVT_ACL_CHECK, plugin_v3_acl_check, ખાલી, plugin);
	}
	જો(plugin->lib.psk_ચાવી_get_v3){
		mosquitto_callback_register(plugin, MOSQ_EVT_PSK_ચાવી, plugin_v3_psk_ચાવી_get, ખાલી, plugin);
	}

	પાછા 0;
}


