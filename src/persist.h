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

#ifndef PERSIST_H
#define PERSIST_H

#অন্তর্ভাব "mosquitto_broker_internal.h"

#define MOSQ_DB_VERSION 6

/* DB read/write */
বাহ্য ধ্রুব unsigned বর্ণ magic[15];
#define DB_CHUNK_CFG 1
#define DB_CHUNK_BASE_বার্তা 2
#define DB_CHUNK_CLIENT_বার্তা 3
#define DB_CHUNK_RETAIN 4
#define DB_CHUNK_SUB 5
#define DB_CHUNK_CLIENT 6
/* End DB read/write */

#define read_e(f, b, c) যদি(fread(b, 1, c, f) != c){ rc = MOSQ_ERR_UNKNOWN; গচ্ছ error; }
#define write_e(f, b, c) যদি(fwrite(b, 1, c, f) != c){ গচ্ছ error; }

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

সংরচনা PF_header {
	uint32_t chunk;
	uint32_t length;
};


সংরচনা PF_cfg {
	uint64_t last_db_id;
	uint8_t shutdown;
	uint8_t dbid_size;
};

সংরচনা PF_client_v5 {
	int64_t session_expiry_time;
	uint32_t session_expiry_interval;
	uint16_t last_mid;
	uint16_t id_len;
};
সংরচনা PF_client {
	/* struct PF_client_v5; */
	int64_t session_expiry_time;
	uint32_t session_expiry_interval;
	uint16_t last_mid;
	uint16_t id_len;

	uint16_t listener_port;
	uint16_t username_len;
	/* tail: 4 byte padding, because 64bit member
	 * forces multiple of 8 for struct size */
};
সংরচনা P_client {
	সংরচনা PF_client F;
	বর্ণ *clientid;
	বর্ণ *username;
};


সংরচনা PF_client_বার্তা {
	dbid_t store_id;
	uint16_t mid;
	uint16_t id_len;
	uint8_t qos;
	uint8_t state;
	uint8_t retain_dup;
	uint8_t direction;
};
সংরচনা P_client_বার্তা {
	সংরচনা PF_client_বার্তা F;
	বর্ণ *clientid;
	uint32_t subscription_identifier;
};


সংরচনা PF_base_বার্তা {
	dbid_t store_id;
	int64_t expiry_time;
	uint32_t payloadlen;
	uint16_t source_mid;
	uint16_t source_id_len;
	uint16_t source_username_len;
	uint16_t topic_len;
	uint16_t source_port;
	uint8_t qos;
	uint8_t retain;
};
সংরচনা P_base_বার্তা {
	সংরচনা PF_base_বার্তা F;
	শূন্য *payload;
	সংরচনা mosquitto source;
	বর্ণ *topic;
	mosquitto_property *properties;
};


সংরচনা PF_sub {
	uint32_t identifier;
	uint16_t id_len;
	uint16_t topic_len;
	uint8_t qos;
	uint8_t options;
};
সংরচনা P_sub {
	সংরচনা PF_sub F;
	বর্ণ *clientid;
	বর্ণ *topic;
};


সংরচনা PF_retain {
	dbid_t store_id;
};
সংরচনা P_retain {
	সংরচনা PF_retain F;
};


পূর্ণাঙ্ক persist__read_string_len(FILE *db_fptr, বর্ণ **str, uint16_t len);
পূর্ণাঙ্ক persist__read_string(FILE *db_fptr, বর্ণ **str);

পূর্ণাঙ্ক persist__chunk_header_read(FILE *db_fptr, uint32_t *chunk, uint32_t *length);

পূর্ণাঙ্ক persist__chunk_header_read_v234(FILE *db_fptr, uint32_t *chunk, uint32_t *length);
পূর্ণাঙ্ক persist__chunk_cfg_read_v234(FILE *db_fptr, সংরচনা PF_cfg *chunk);
পূর্ণাঙ্ক persist__chunk_client_read_v234(FILE *db_fptr, সংরচনা P_client *chunk, uint32_t db_version);
পূর্ণাঙ্ক persist_chunk_client_বার্তা_read_v234(FILE *db_fptr, সংরচনা P_client_বার্তা *chunk);
পূর্ণাঙ্ক persist_chunk_base_বার্তা_read_v234(FILE *db_fptr, সংরচনা P_base_বার্তা *chunk, uint32_t db_version);
পূর্ণাঙ্ক persist__chunk_retain_read_v234(FILE *db_fptr, সংরচনা P_retain *chunk);
পূর্ণাঙ্ক persist__chunk_sub_read_v234(FILE *db_fptr, সংরচনা P_sub *chunk);

পূর্ণাঙ্ক persist__chunk_header_read_v56(FILE *db_fptr, uint32_t *chunk, uint32_t *length);
পূর্ণাঙ্ক persist__chunk_cfg_read_v56(FILE *db_fptr, সংরচনা PF_cfg *chunk);
পূর্ণাঙ্ক persist__chunk_client_read_v56(FILE *db_fptr, সংরচনা P_client *chunk, uint32_t db_version);
পূর্ণাঙ্ক persist_chunk_client_বার্তা_read_v56(FILE *db_fptr, সংরচনা P_client_বার্তা *chunk, uint32_t length);
পূর্ণাঙ্ক persist_chunk_base_বার্তা_read_v56(FILE *db_fptr, সংরচনা P_base_বার্তা *chunk, uint32_t length);
পূর্ণাঙ্ক persist__chunk_retain_read_v56(FILE *db_fptr, সংরচনা P_retain *chunk);
পূর্ণাঙ্ক persist__chunk_sub_read_v56(FILE *db_fptr, সংরচনা P_sub *chunk);

পূর্ণাঙ্ক persist__chunk_cfg_write_v6(FILE *db_fptr, সংরচনা PF_cfg *chunk);
পূর্ণাঙ্ক persist__chunk_client_write_v6(FILE *db_fptr, সংরচনা P_client *chunk);
পূর্ণাঙ্ক persist_chunk_client_বার্তা_write_v6(FILE *db_fptr, সংরচনা P_client_বার্তা *chunk);
পূর্ণাঙ্ক persist__chunk_message_store_write_v6(FILE *db_fptr, সংরচনা P_base_বার্তা *chunk);
পূর্ণাঙ্ক persist__chunk_retain_write_v6(FILE *db_fptr, সংরচনা P_retain *chunk);
পূর্ণাঙ্ক persist__chunk_sub_write_v6(FILE *db_fptr, সংরচনা P_sub *chunk);

#endif
