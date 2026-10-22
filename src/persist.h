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

#અન્તર્ભાવ "mosquitto_broker_internal.h"

#define MOSQ_DB_VERSION 6

/* DB read/write */
બાહ્ય અચલ unsigned અક્ષર magic[15];
#define DB_CHUNK_CFG 1
#define DB_CHUNK_BASE_સંદેશ 2
#define DB_CHUNK_CLIENT_સંદેશ 3
#define DB_CHUNK_RETAIN 4
#define DB_CHUNK_SUB 5
#define DB_CHUNK_CLIENT 6
/* End DB read/write */

#define read_e(f, b, c) જો(fread(b, 1, c, f) != c){ rc = MOSQ_ERR_UNKNOWN; જાઓ error; }
#define write_e(f, b, c) જો(fwrite(b, 1, c, f) != c){ જાઓ error; }

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

સંરચના PF_header {
	uint32_t chunk;
	uint32_t length;
};


સંરચના PF_cfg {
	uint64_t last_db_id;
	uint8_t shutdown;
	uint8_t dbid_size;
};

સંરચના PF_client_v5 {
	int64_t session_expiry_time;
	uint32_t session_expiry_interval;
	uint16_t last_mid;
	uint16_t id_len;
};
સંરચના PF_client {
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
સંરચના P_client {
	સંરચના PF_client F;
	અક્ષર *clientid;
	અક્ષર *username;
};


સંરચના PF_client_સંદેશ {
	dbid_t store_id;
	uint16_t mid;
	uint16_t id_len;
	uint8_t qos;
	uint8_t state;
	uint8_t retain_dup;
	uint8_t direction;
};
સંરચના P_client_સંદેશ {
	સંરચના PF_client_સંદેશ F;
	અક્ષર *clientid;
	uint32_t subscription_identifier;
};


સંરચના PF_base_સંદેશ {
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
સંરચના P_base_સંદેશ {
	સંરચના PF_base_સંદેશ F;
	શૂન્ય *payload;
	સંરચના mosquitto source;
	અક્ષર *topic;
	mosquitto_property *properties;
};


સંરચના PF_sub {
	uint32_t identifier;
	uint16_t id_len;
	uint16_t topic_len;
	uint8_t qos;
	uint8_t options;
};
સંરચના P_sub {
	સંરચના PF_sub F;
	અક્ષર *clientid;
	અક્ષર *topic;
};


સંરચના PF_retain {
	dbid_t store_id;
};
સંરચના P_retain {
	સંરચના PF_retain F;
};


પૂર્ણાંક persist__read_string_len(FILE *db_fptr, અક્ષર **str, uint16_t len);
પૂર્ણાંક persist__read_string(FILE *db_fptr, અક્ષર **str);

પૂર્ણાંક persist__chunk_header_read(FILE *db_fptr, uint32_t *chunk, uint32_t *length);

પૂર્ણાંક persist__chunk_header_read_v234(FILE *db_fptr, uint32_t *chunk, uint32_t *length);
પૂર્ણાંક persist__chunk_cfg_read_v234(FILE *db_fptr, સંરચના PF_cfg *chunk);
પૂર્ણાંક persist__chunk_client_read_v234(FILE *db_fptr, સંરચના P_client *chunk, uint32_t db_version);
પૂર્ણાંક persist_chunk_client_સંદેશ_read_v234(FILE *db_fptr, સંરચના P_client_સંદેશ *chunk);
પૂર્ણાંક persist_chunk_base_સંદેશ_read_v234(FILE *db_fptr, સંરચના P_base_સંદેશ *chunk, uint32_t db_version);
પૂર્ણાંક persist__chunk_retain_read_v234(FILE *db_fptr, સંરચના P_retain *chunk);
પૂર્ણાંક persist__chunk_sub_read_v234(FILE *db_fptr, સંરચના P_sub *chunk);

પૂર્ણાંક persist__chunk_header_read_v56(FILE *db_fptr, uint32_t *chunk, uint32_t *length);
પૂર્ણાંક persist__chunk_cfg_read_v56(FILE *db_fptr, સંરચના PF_cfg *chunk);
પૂર્ણાંક persist__chunk_client_read_v56(FILE *db_fptr, સંરચના P_client *chunk, uint32_t db_version);
પૂર્ણાંક persist_chunk_client_સંદેશ_read_v56(FILE *db_fptr, સંરચના P_client_સંદેશ *chunk, uint32_t length);
પૂર્ણાંક persist_chunk_base_સંદેશ_read_v56(FILE *db_fptr, સંરચના P_base_સંદેશ *chunk, uint32_t length);
પૂર્ણાંક persist__chunk_retain_read_v56(FILE *db_fptr, સંરચના P_retain *chunk);
પૂર્ણાંક persist__chunk_sub_read_v56(FILE *db_fptr, સંરચના P_sub *chunk);

પૂર્ણાંક persist__chunk_cfg_write_v6(FILE *db_fptr, સંરચના PF_cfg *chunk);
પૂર્ણાંક persist__chunk_client_write_v6(FILE *db_fptr, સંરચના P_client *chunk);
પૂર્ણાંક persist_chunk_client_સંદેશ_write_v6(FILE *db_fptr, સંરચના P_client_સંદેશ *chunk);
પૂર્ણાંક persist__chunk_message_store_write_v6(FILE *db_fptr, સંરચના P_base_સંદેશ *chunk);
પૂર્ણાંક persist__chunk_retain_write_v6(FILE *db_fptr, સંરચના P_retain *chunk);
પૂર્ણાંક persist__chunk_sub_write_v6(FILE *db_fptr, સંરચના P_sub *chunk);

#endif
