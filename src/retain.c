/*
Copyright (c) 2010-2021 Roger Light <roger@atchoo.org>

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

#అన్తర్భావ <assert.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <string.h>

#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "mosquitto/mqtt_protocol.h"
#అన్తర్భావ "util_mosq.h"

#అన్తర్భావ "utlist.h"

స్థిర time_t తదుపరి_expire_check = 0;

స్థిర నిర్మాణం mosquitto__retainhier *retain_కూడు_hier_నమోదు(నిర్మాణం mosquitto__retainhier *మాతృ, నిర్మాణం mosquitto__retainhier **sibling, స్థిరరాశి అక్షరం *topic, uint16_t len)
{
	నిర్మాణం mosquitto__retainhier *బిడ్డ;

	ప్రతిజ్ఞా(sibling);

	బిడ్డ = mosquitto_calloc(1, పరిమాణం(నిర్మాణం mosquitto__retainhier) + len + 1);
	ఒకవేళ(!బిడ్డ){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Out of memory.");
		తిరిగి ఖాళీ;
	}
	బిడ్డ->మాతృ = మాతృ;
	బిడ్డ->topic_len = len;
	ఒకవేళ(len > 0){
		strncpy(బిడ్డ->topic, topic, (size_t)(len+1));
	}

	HASH_కూడు(hh, *sibling, topic, బిడ్డ->topic_len, బిడ్డ);

	తిరిగి బిడ్డ;
}


పూర్ణాంకం retain__init(శూన్య)
{
	నిర్మాణం mosquitto__retainhier *retainhier;

	retainhier = retain_కూడు_hier_నమోదు(ఖాళీ, &db.retains, "", 0);
	ఒకవేళ(!retainhier){
		తిరిగి MOSQ_ERR_NOMEM;
	}

	retainhier = retain_కూడు_hier_నమోదు(ఖాళీ, &db.retains, "$SYS", (uint16_t)అక్షరశ్రేణి_పొడవు("$SYS"));
	ఒకవేళ(!retainhier){
		తిరిగి MOSQ_ERR_NOMEM;
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


BROKER_EXPORT పూర్ణాంకం mosquitto_persist_retain_సందేశం_సమితి(స్థిరరాశి అక్షరం *topic, uint64_t పునాది_సందేశం_id)
{
	నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం;
	పూర్ణాంకం rc = MOSQ_ERR_UNKNOWN;
	అక్షరం **విభజించు_topics = ఖాళీ;
	అక్షరం *local_topic = ఖాళీ;

	ఒకవేళ(topic == ఖాళీ){
		తిరిగి MOSQ_ERR_INVAL;
	}

	HASH_కనుగొను(hh, db.సందేశం_store, &పునాది_సందేశం_id, పరిమాణం(పునాది_సందేశం_id), పునాది_సందేశం);
	ఒకవేళ(పునాది_సందేశం){
		ఒకవేళ(sub__topic_tokenise(topic, &local_topic, &విభజించు_topics, ఖాళీ)){
			తిరిగి MOSQ_ERR_NOMEM;
		}

		rc = retain__store(topic, పునాది_సందేశం, విభజించు_topics, false);
		mosquitto_free(విభజించు_topics);
		mosquitto_free(local_topic);
	}

	తిరిగి rc;
}


BROKER_EXPORT పూర్ణాంకం mosquitto_persist_retain_సందేశం_delete(స్థిరరాశి అక్షరం *topic)
{
	నిర్మాణం mosquitto_పునాది_సందేశం పునాది_సందేశం;
	పూర్ణాంకం rc = MOSQ_ERR_UNKNOWN;
	అక్షరం **విభజించు_topics = ఖాళీ;
	అక్షరం *local_topic = ఖాళీ;

	ఒకవేళ(topic == ఖాళీ){
		తిరిగి MOSQ_ERR_INVAL;
	}

	memset(&పునాది_సందేశం, 0, పరిమాణం(పునాది_సందేశం));
	పునాది_సందేశం.ref_లెక్క = 10; /* Ensure this isn't freed */

	ఒకవేళ(sub__topic_tokenise(topic, &local_topic, &విభజించు_topics, ఖాళీ)){
		తిరిగి MOSQ_ERR_NOMEM;
	}

	/* With stored->payloadlen == 0, this means the message will be removed */
	rc = retain__store(topic, &పునాది_సందేశం, విభజించు_topics, false);
	mosquitto_FREE(విభజించు_topics);
	mosquitto_FREE(local_topic);

	తిరిగి rc;
}


శూన్య retain_clean_ఖాళీలు_hierarchy(నిర్మాణం mosquitto__retainhier *retainhier)
{
	అయితే(retainhier){
		ఒకవేళ(retainhier->children || retainhier->retained || retainhier->మాతృ == ఖాళీ){
			/* Entry is being used */
			తిరిగి;
		}లేకపోతే{
			HASH_DELETE(hh, retainhier->మాతృ->children, retainhier);

			నిర్మాణం mosquitto__retainhier *మాతృ = retainhier->మాతృ;
			mosquitto_FREE(retainhier);
			retainhier = మాతృ;
		}
	}
}


పూర్ణాంకం retain__store(స్థిరరాశి అక్షరం *topic, నిర్మాణం mosquitto_పునాది_సందేశం *పునాది_సందేశం, అక్షరం **విభజించు_topics, bool persist)
{
	నిర్మాణం mosquitto__retainhier *retainhier;
	నిర్మాణం mosquitto__retainhier *branch;
	size_t slen;

	ప్రతిజ్ఞా(పునాది_సందేశం);
	ప్రతిజ్ఞా(విభజించు_topics);

	HASH_కనుగొను(hh, db.retains, విభజించు_topics[0], అక్షరశ్రేణి_పొడవు(విభజించు_topics[0]), retainhier);
	ఒకవేళ(retainhier == ఖాళీ){
		retainhier = retain_కూడు_hier_నమోదు(ఖాళీ, &db.retains, విభజించు_topics[0], (uint16_t)అక్షరశ్రేణి_పొడవు(విభజించు_topics[0]));
		ఒకవేళ(!retainhier){
			తిరిగి MOSQ_ERR_NOMEM;
		}
	}

	కొరకు(పూర్ణాంకం i=0; విభజించు_topics[i] != ఖాళీ; i++){
		slen = అక్షరశ్రేణి_పొడవు(విభజించు_topics[i]);
		HASH_కనుగొను(hh, retainhier->children, విభజించు_topics[i], slen, branch);
		ఒకవేళ(branch == ఖాళీ){
			branch = retain_కూడు_hier_నమోదు(retainhier, &retainhier->children, విభజించు_topics[i], (uint16_t)slen);
			ఒకవేళ(branch == ఖాళీ){
				తిరిగి MOSQ_ERR_NOMEM;
			}
		}
		retainhier = branch;
	}

#నిర్వచితమైతే WITH_PERSISTENCE
	ఒకవేళ(అక్షరశ్రేణి_పోలిక(topic, "$SYS", 4)){
		/* Retained messages count as a persistence change, but only if
		 * they aren't for $SYS. */
		db.persistence_changes++;
	}
#లేకపోతే
	వాడని(topic);
#అయితే_ముగింపు

	ఒకవేళ(retainhier->retained){
		ఒకవేళ(retainhier->retained == పునాది_సందేశం){
			/* This may occur if multiple persistence providers are used */
			తిరిగి MOSQ_ERR_SUCCESS;
		}

		ఒకవేళ(persist && retainhier->retained->దత్తాంశం.topic[0] != '$' && పునాది_సందేశం->దత్తాంశం.payloadlen == 0){
			/* Only delete if another retained message isn't replacing this one */
			plugin_persist_handle_retain_సందేశం_delete(retainhier->retained);
		}
		db_సందేశం_store_ref_dec(&retainhier->retained);
#నిర్వచితమైతే WITH_SYS_వృక్షం
		db.retained_లెక్క--;
#అయితే_ముగింపు
		ఒకవేళ(పునాది_సందేశం->దత్తాంశం.payloadlen == 0){
			retainhier->retained = ఖాళీ;
			retain_clean_ఖాళీలు_hierarchy(retainhier);
		}
	}
	ఒకవేళ(పునాది_సందేశం->దత్తాంశం.payloadlen){
		retainhier->retained = పునాది_సందేశం;
		db_సందేశం_store_ref_inc(retainhier->retained);
		ఒకవేళ(persist && retainhier->retained->దత్తాంశం.topic[0] != '$'){
			plugin_persist_handle_పునాది_సందేశం_కూడు(retainhier->retained);
			plugin_persist_handle_retain_సందేశం_సమితి(retainhier->retained);
		}
#నిర్వచితమైతే WITH_SYS_వృక్షం
		db.retained_లెక్క++;
#అయితే_ముగింపు
	}

	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర bool retain_delete_expired_సందేశం(నిర్మాణం mosquitto__retainhier *branch)
{
	ఒకవేళ(branch->retained && branch->retained->దత్తాంశం.expiry_సమయం > 0 && db.now_real_s >= branch->retained->దత్తాంశం.expiry_సమయం){
		plugin_persist_handle_retain_సందేశం_delete(branch->retained);
		db_సందేశం_store_ref_dec(&branch->retained);
		branch->retained = ఖాళీ;
#నిర్వచితమైతే WITH_SYS_వృక్షం
		db.retained_లెక్క--;
#అయితే_ముగింపు
		తిరిగి true;
	}
	తిరిగి false;
}


స్థిర పూర్ణాంకం retain__process(నిర్మాణం mosquitto__retainhier *branch, నిర్మాణం mosquitto *context, స్థిరరాశి నిర్మాణం mosquitto_subscription *sub)
{
	పూర్ణాంకం rc = 0;
	uint8_t qos, sub_qos;
	uint16_t mid;
	నిర్మాణం mosquitto_పునాది_సందేశం *retained;

	ఒకవేళ(retain_delete_expired_సందేశం(branch)){
		తిరిగి MOSQ_ERR_SUCCESS;
	}

	retained = branch->retained;

	rc = mosquitto_acl_check(context, retained->దత్తాంశం.topic, retained->దత్తాంశం.payloadlen, retained->దత్తాంశం.payload,
			retained->దత్తాంశం.qos, retained->దత్తాంశం.retain, retained->దత్తాంశం.properties, MOSQ_ACL_READ);
	ఒకవేళ(rc == MOSQ_ERR_ACL_DENIED){
		తిరిగి MOSQ_ERR_SUCCESS;
	}లేకపోతే ఒకవేళ(rc != MOSQ_ERR_SUCCESS){
		తిరిగి rc;
	}

	/* Check for original source access */
	ఒకవేళ(db.config->check_retain_ఆధారం && retained->origin != mosq_mo_broker && retained->దత్తాంశం.ఆధారం_id){
		నిర్మాణం mosquitto retain_ctxt;
		memset(&retain_ctxt, 0, పరిమాణం(నిర్మాణం mosquitto));

		retain_ctxt.id = retained->దత్తాంశం.ఆధారం_id;
		retain_ctxt.username = retained->దత్తాంశం.ఆధారం_username;
		retain_ctxt.listener = retained->ఆధారం_listener;

		rc = mosquitto_acl_check(&retain_ctxt, retained->దత్తాంశం.topic, retained->దత్తాంశం.payloadlen, retained->దత్తాంశం.payload,
				retained->దత్తాంశం.qos, retained->దత్తాంశం.retain, retained->దత్తాంశం.properties, MOSQ_ACL_WRITE);
		ఒకవేళ(rc == MOSQ_ERR_ACL_DENIED){
			తిరిగి MOSQ_ERR_SUCCESS;
		}లేకపోతే ఒకవేళ(rc != MOSQ_ERR_SUCCESS){
			తిరిగి rc;
		}
	}

	sub_qos = sub->options & 0x03;
	ఒకవేళ(db.config->upgrade_outgoing_qos){
		qos = sub_qos;
	}లేకపోతే{
		qos = retained->దత్తాంశం.qos;
		ఒకవేళ(qos > sub_qos){
			qos = sub_qos;
		}
	}
	ఒకవేళ(qos > 0){
		mid = mosquitto__mid_generate(context);
	}లేకపోతే{
		mid = 0;
	}
	తిరిగి db_సందేశం_చొప్పించు_outgoing(context, 0, mid, qos, true, retained, sub->identifier, false, true);
}


స్థిర పూర్ణాంకం retain_శోధన(నిర్మాణం mosquitto__retainhier *retainhier, అక్షరం **విభజించు_topics, నిర్మాణం mosquitto *context, స్థిరరాశి నిర్మాణం mosquitto_subscription *sub, పూర్ణాంకం level)
{
	నిర్మాణం mosquitto__retainhier *branch, *branch_tmp;
	పూర్ణాంకం flag = 0;

	ఒకవేళ(!strcmp(విభజించు_topics[0], "#") && విభజించు_topics[1] == ఖాళీ){
		HASH_ITER(hh, retainhier->children, branch, branch_tmp){
			/* Set flag to indicate that we should check for retained messages
			 * on "foo" when we are subscribing to e.g. "foo/#" and then exit
			 * this function and return to an earlier retain__search().
			 */
			flag = -1;
			ఒకవేళ(branch->retained){
				retain__process(branch, context, sub);
			}
			ఒకవేళ(branch->children){
				retain_శోధన(branch, విభజించు_topics, context, sub, level+1);
			}
		}
	}లేకపోతే{
		ఒకవేళ(!strcmp(విభజించు_topics[0], "+")){
			HASH_ITER(hh, retainhier->children, branch, branch_tmp){
				ఒకవేళ(విభజించు_topics[1] != ఖాళీ){
					ఒకవేళ(retain_శోధన(branch, &(విభజించు_topics[1]), context, sub, level+1) == -1
							|| (విభజించు_topics[1] != ఖాళీ && !strcmp(విభజించు_topics[1], "#") && level>0)){

						ఒకవేళ(branch->retained){
							retain__process(branch, context, sub);
						}
					}
				}లేకపోతే{
					ఒకవేళ(branch->retained){
						retain__process(branch, context, sub);
					}
				}
			}
		}లేకపోతే{
			HASH_కనుగొను(hh, retainhier->children, విభజించు_topics[0], అక్షరశ్రేణి_పొడవు(విభజించు_topics[0]), branch);
			ఒకవేళ(branch){
				ఒకవేళ(విభజించు_topics[1] != ఖాళీ){
					ఒకవేళ(retain_శోధన(branch, &(విభజించు_topics[1]), context, sub, level+1) == -1
							|| (విభజించు_topics[1] != ఖాళీ && !strcmp(విభజించు_topics[1], "#") && level>0)){

						ఒకవేళ(branch->retained){
							retain__process(branch, context, sub);
						}
					}
				}లేకపోతే{
					ఒకవేళ(branch->retained){
						retain__process(branch, context, sub);
					}
				}
			}
		}
	}
	తిరిగి flag;
}


పూర్ణాంకం retain_వరుస(నిర్మాణం mosquitto *context, స్థిరరాశి నిర్మాణం mosquitto_subscription *sub)
{
	నిర్మాణం mosquitto__retainhier *retainhier;
	అక్షరం *local_sub;
	అక్షరం **విభజించు_topics;
	పూర్ణాంకం rc;

	ప్రతిజ్ఞా(context);
	ప్రతిజ్ఞా(sub);

	ఒకవేళ(!అక్షరశ్రేణి_పోలిక(sub->topic_filter, "$share/", అక్షరశ్రేణి_పొడవు("$share/"))){
		తిరిగి MOSQ_ERR_SUCCESS;
	}

	rc = sub__topic_tokenise(sub->topic_filter, &local_sub, &విభజించు_topics, ఖాళీ);
	ఒకవేళ(rc){
		తిరిగి rc;
	}

	HASH_కనుగొను(hh, db.retains, విభజించు_topics[0], అక్షరశ్రేణి_పొడవు(విభజించు_topics[0]), retainhier);

	ఒకవేళ(retainhier){
		retain_శోధన(retainhier, విభజించు_topics, context, sub, 0);
	}
	mosquitto_FREE(local_sub);
	mosquitto_FREE(విభజించు_topics);

	తిరిగి MOSQ_ERR_SUCCESS;
}


శూన్య retain__expire(నిర్మాణం mosquitto__retainhier **retainhier)
{
	నిర్మాణం mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		retain__expire(&peer->children);

		నిర్మాణం mosquitto__retainhier *మాతృ = peer->మాతృ;
		ఒకవేళ(retain_delete_expired_సందేశం(peer)){
			retain_clean_ఖాళీలు_hierarchy(మాతృ);
		}
	}
}


శూన్య retain__clean(నిర్మాణం mosquitto__retainhier **retainhier)
{
	నిర్మాణం mosquitto__retainhier *peer, *retainhier_tmp;

	HASH_ITER(hh, *retainhier, peer, retainhier_tmp){
		ఒకవేళ(peer->retained){
			db_సందేశం_store_ref_dec(&peer->retained);
		}
		retain__clean(&peer->children);

		HASH_DELETE(hh, *retainhier, peer);
		mosquitto_FREE(peer);
	}
}


శూన్య retain__expiry_check(శూన్య)
{
	ఒకవేళ(db.config->retain_expiry_interval > 0 && db.now_s > తదుపరి_expire_check){
		retain__expire(&db.retains);
		తదుపరి_expire_check = db.now_s + db.config->retain_expiry_interval;
	}
}
