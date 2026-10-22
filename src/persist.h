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

#నిర్వచితం_కాకపోతే PERSIST_H
#నిర్వచించు PERSIST_H

#అన్తర్భావ "mosquitto_broker_internal.h"

#నిర్వచించు MOSQ_DB_సంస్కరణ 6

/* DB read/write */
బాహ్య స్థిరరాశి గుర్తులేని అక్షరం magic[15];
#నిర్వచించు DB_CHUNK_CFG 1
#నిర్వచించు DB_CHUNK_పునాది_సందేశం 2
#నిర్వచించు DB_CHUNK_CLIENT_సందేశం 3
#నిర్వచించు DB_CHUNK_RETAIN 4
#నిర్వచించు DB_CHUNK_SUB 5
#నిర్వచించు DB_CHUNK_CLIENT 6
/* End DB read/write */

#నిర్వచించు read_e(f, b, c) ఒకవేళ(fread(b, 1, c, f) != c){ rc = MOSQ_ERR_UNKNOWN; వెళ్ళు error; }
#నిర్వచించు write_e(f, b, c) ఒకవేళ(fwrite(b, 1, c, f) != c){ వెళ్ళు error; }

/* COMPATIBILITY NOTES
 *
 * The P_* structs (persist structs) contain all of the data for a particular
 * data chunk. They are loaded in multiple parts, so can be rearranged without
 * updating the db format version.
 *
 * The PF_* structs (persist fixed structs) contain the fixed size data for a
 * particular data chunk. They are written to disk as is, so they must not be
 * rearranged without updating the db format version. When adding new members,
 * always use explicit sized datatypes ("uint32_t", not "long"), and check
 * whether what is being added can go in an existing hole in the struct.
 */

నిర్మాణం PF_శీర్షిక {
	uint32_t chunk;
	uint32_t నిడివి;
};


నిర్మాణం PF_cfg {
	uint64_t చివరి_db_id;
	uint8_t shutdown;
	uint8_t dbid_కొలత;
};

నిర్మాణం PF_client_v5 {
	int64_t session_expiry_సమయం;
	uint32_t session_expiry_interval;
	uint16_t చివరి_mid;
	uint16_t id_len;
};
నిర్మాణం PF_client {
	/* struct PF_client_v5; */
	int64_t session_expiry_సమయం;
	uint32_t session_expiry_interval;
	uint16_t చివరి_mid;
	uint16_t id_len;

	uint16_t listener_పోర్టు;
	uint16_t username_len;
	/* tail: 4 byte padding, because 64bit member
	 * forces multiple of 8 for struct size */
};
నిర్మాణం P_client {
	నిర్మాణం PF_client F;
	అక్షరం *clientid;
	అక్షరం *username;
};


నిర్మాణం PF_client_సందేశం {
	dbid_t store_id;
	uint16_t mid;
	uint16_t id_len;
	uint8_t qos;
	uint8_t state;
	uint8_t retain_dup;
	uint8_t direction;
};
నిర్మాణం P_client_సందేశం {
	నిర్మాణం PF_client_సందేశం F;
	అక్షరం *clientid;
	uint32_t subscription_identifier;
};


నిర్మాణం PF_పునాది_సందేశం {
	dbid_t store_id;
	int64_t expiry_సమయం;
	uint32_t payloadlen;
	uint16_t ఆధారం_mid;
	uint16_t ఆధారం_id_len;
	uint16_t ఆధారం_username_len;
	uint16_t topic_len;
	uint16_t ఆధారం_పోర్టు;
	uint8_t qos;
	uint8_t retain;
};
నిర్మాణం P_పునాది_సందేశం {
	నిర్మాణం PF_పునాది_సందేశం F;
	శూన్య *payload;
	నిర్మాణం mosquitto ఆధారం;
	అక్షరం *topic;
	mosquitto_property *properties;
};


నిర్మాణం PF_sub {
	uint32_t identifier;
	uint16_t id_len;
	uint16_t topic_len;
	uint8_t qos;
	uint8_t options;
};
నిర్మాణం P_sub {
	నిర్మాణం PF_sub F;
	అక్షరం *clientid;
	అక్షరం *topic;
};


నిర్మాణం PF_retain {
	dbid_t store_id;
};
నిర్మాణం P_retain {
	నిర్మాణం PF_retain F;
};


పూర్ణాంకం persist__read_string_len(FILE *db_fptr, అక్షరం **str, uint16_t len);
పూర్ణాంకం persist__read_string(FILE *db_fptr, అక్షరం **str);

పూర్ణాంకం persist_chunk_శీర్షిక_read(FILE *db_fptr, uint32_t *chunk, uint32_t *నిడివి);

పూర్ణాంకం persist_chunk_శీర్షిక_read_v234(FILE *db_fptr, uint32_t *chunk, uint32_t *నిడివి);
పూర్ణాంకం persist__chunk_cfg_read_v234(FILE *db_fptr, నిర్మాణం PF_cfg *chunk);
పూర్ణాంకం persist__chunk_client_read_v234(FILE *db_fptr, నిర్మాణం P_client *chunk, uint32_t db_సంస్కరణ);
పూర్ణాంకం persist_chunk_client_సందేశం_read_v234(FILE *db_fptr, నిర్మాణం P_client_సందేశం *chunk);
పూర్ణాంకం persist_chunk_పునాది_సందేశం_read_v234(FILE *db_fptr, నిర్మాణం P_పునాది_సందేశం *chunk, uint32_t db_సంస్కరణ);
పూర్ణాంకం persist__chunk_retain_read_v234(FILE *db_fptr, నిర్మాణం P_retain *chunk);
పూర్ణాంకం persist__chunk_sub_read_v234(FILE *db_fptr, నిర్మాణం P_sub *chunk);

పూర్ణాంకం persist_chunk_శీర్షిక_read_v56(FILE *db_fptr, uint32_t *chunk, uint32_t *నిడివి);
పూర్ణాంకం persist__chunk_cfg_read_v56(FILE *db_fptr, నిర్మాణం PF_cfg *chunk);
పూర్ణాంకం persist__chunk_client_read_v56(FILE *db_fptr, నిర్మాణం P_client *chunk, uint32_t db_సంస్కరణ);
పూర్ణాంకం persist_chunk_client_సందేశం_read_v56(FILE *db_fptr, నిర్మాణం P_client_సందేశం *chunk, uint32_t నిడివి);
పూర్ణాంకం persist_chunk_పునాది_సందేశం_read_v56(FILE *db_fptr, నిర్మాణం P_పునాది_సందేశం *chunk, uint32_t నిడివి);
పూర్ణాంకం persist__chunk_retain_read_v56(FILE *db_fptr, నిర్మాణం P_retain *chunk);
పూర్ణాంకం persist__chunk_sub_read_v56(FILE *db_fptr, నిర్మాణం P_sub *chunk);

పూర్ణాంకం persist__chunk_cfg_write_v6(FILE *db_fptr, నిర్మాణం PF_cfg *chunk);
పూర్ణాంకం persist__chunk_client_write_v6(FILE *db_fptr, నిర్మాణం P_client *chunk);
పూర్ణాంకం persist_chunk_client_సందేశం_write_v6(FILE *db_fptr, నిర్మాణం P_client_సందేశం *chunk);
పూర్ణాంకం persist_chunk_సందేశం_store_write_v6(FILE *db_fptr, నిర్మాణం P_పునాది_సందేశం *chunk);
పూర్ణాంకం persist__chunk_retain_write_v6(FILE *db_fptr, నిర్మాణం P_retain *chunk);
పూర్ణాంకం persist__chunk_sub_write_v6(FILE *db_fptr, నిర్మాణం P_sub *chunk);

#అయితే_ముగింపు
