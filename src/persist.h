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

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"

#define MOSQ_DB_VERSION 6

/* DB read/write */
ಬಾಹ್ಯ ಧ್ರುವ unsigned ವರ್ಣ magic[15];
#define DB_CHUNK_CFG 1
#define DB_CHUNK_BASE_ಸಂದೇಶ 2
#define DB_CHUNK_CLIENT_ಸಂದೇಶ 3
#define DB_CHUNK_RETAIN 4
#define DB_CHUNK_SUB 5
#define DB_CHUNK_CLIENT 6
/* End DB read/write */

#define read_e(f, b, c) ಯದಿ(fread(b, 1, c, f) != c){ rc = MOSQ_ERR_UNKNOWN; ಗಚ್ಛ error; }
#define write_e(f, b, c) ಯದಿ(fwrite(b, 1, c, f) != c){ ಗಚ್ಛ error; }

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

ಸಂರಚನಾ PF_header {
	uint32_t chunk;
	uint32_t length;
};


ಸಂರಚನಾ PF_cfg {
	uint64_t last_db_id;
	uint8_t shutdown;
	uint8_t dbid_size;
};

ಸಂರಚನಾ PF_client_v5 {
	int64_t session_expiry_time;
	uint32_t session_expiry_interval;
	uint16_t last_mid;
	uint16_t id_len;
};
ಸಂರಚನಾ PF_client {
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
ಸಂರಚನಾ P_client {
	ಸಂರಚನಾ PF_client F;
	ವರ್ಣ *clientid;
	ವರ್ಣ *username;
};


ಸಂರಚನಾ PF_client_ಸಂದೇಶ {
	dbid_t store_id;
	uint16_t mid;
	uint16_t id_len;
	uint8_t qos;
	uint8_t state;
	uint8_t retain_dup;
	uint8_t direction;
};
ಸಂರಚನಾ P_client_ಸಂದೇಶ {
	ಸಂರಚನಾ PF_client_ಸಂದೇಶ F;
	ವರ್ಣ *clientid;
	uint32_t subscription_identifier;
};


ಸಂರಚನಾ PF_base_ಸಂದೇಶ {
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
ಸಂರಚನಾ P_base_ಸಂದೇಶ {
	ಸಂರಚನಾ PF_base_ಸಂದೇಶ F;
	ಶೂನ್ಯ *payload;
	ಸಂರಚನಾ mosquitto source;
	ವರ್ಣ *topic;
	mosquitto_property *properties;
};


ಸಂರಚನಾ PF_sub {
	uint32_t identifier;
	uint16_t id_len;
	uint16_t topic_len;
	uint8_t qos;
	uint8_t options;
};
ಸಂರಚನಾ P_sub {
	ಸಂರಚನಾ PF_sub F;
	ವರ್ಣ *clientid;
	ವರ್ಣ *topic;
};


ಸಂರಚನಾ PF_retain {
	dbid_t store_id;
};
ಸಂರಚನಾ P_retain {
	ಸಂರಚನಾ PF_retain F;
};


ಪೂರ್ಣಾಙ್ಕ persist__read_string_len(FILE *db_fptr, ವರ್ಣ **str, uint16_t len);
ಪೂರ್ಣಾಙ್ಕ persist__read_string(FILE *db_fptr, ವರ್ಣ **str);

ಪೂರ್ಣಾಙ್ಕ persist__chunk_header_read(FILE *db_fptr, uint32_t *chunk, uint32_t *length);

ಪೂರ್ಣಾಙ್ಕ persist__chunk_header_read_v234(FILE *db_fptr, uint32_t *chunk, uint32_t *length);
ಪೂರ್ಣಾಙ್ಕ persist__chunk_cfg_read_v234(FILE *db_fptr, ಸಂರಚನಾ PF_cfg *chunk);
ಪೂರ್ಣಾಙ್ಕ persist__chunk_client_read_v234(FILE *db_fptr, ಸಂರಚನಾ P_client *chunk, uint32_t db_version);
ಪೂರ್ಣಾಙ್ಕ persist_chunk_client_ಸಂದೇಶ_read_v234(FILE *db_fptr, ಸಂರಚನಾ P_client_ಸಂದೇಶ *chunk);
ಪೂರ್ಣಾಙ್ಕ persist_chunk_base_ಸಂದೇಶ_read_v234(FILE *db_fptr, ಸಂರಚನಾ P_base_ಸಂದೇಶ *chunk, uint32_t db_version);
ಪೂರ್ಣಾಙ್ಕ persist__chunk_retain_read_v234(FILE *db_fptr, ಸಂರಚನಾ P_retain *chunk);
ಪೂರ್ಣಾಙ್ಕ persist__chunk_sub_read_v234(FILE *db_fptr, ಸಂರಚನಾ P_sub *chunk);

ಪೂರ್ಣಾಙ್ಕ persist__chunk_header_read_v56(FILE *db_fptr, uint32_t *chunk, uint32_t *length);
ಪೂರ್ಣಾಙ್ಕ persist__chunk_cfg_read_v56(FILE *db_fptr, ಸಂರಚನಾ PF_cfg *chunk);
ಪೂರ್ಣಾಙ್ಕ persist__chunk_client_read_v56(FILE *db_fptr, ಸಂರಚನಾ P_client *chunk, uint32_t db_version);
ಪೂರ್ಣಾಙ್ಕ persist_chunk_client_ಸಂದೇಶ_read_v56(FILE *db_fptr, ಸಂರಚನಾ P_client_ಸಂದೇಶ *chunk, uint32_t length);
ಪೂರ್ಣಾಙ್ಕ persist_chunk_base_ಸಂದೇಶ_read_v56(FILE *db_fptr, ಸಂರಚನಾ P_base_ಸಂದೇಶ *chunk, uint32_t length);
ಪೂರ್ಣಾಙ್ಕ persist__chunk_retain_read_v56(FILE *db_fptr, ಸಂರಚನಾ P_retain *chunk);
ಪೂರ್ಣಾಙ್ಕ persist__chunk_sub_read_v56(FILE *db_fptr, ಸಂರಚನಾ P_sub *chunk);

ಪೂರ್ಣಾಙ್ಕ persist__chunk_cfg_write_v6(FILE *db_fptr, ಸಂರಚನಾ PF_cfg *chunk);
ಪೂರ್ಣಾಙ್ಕ persist__chunk_client_write_v6(FILE *db_fptr, ಸಂರಚನಾ P_client *chunk);
ಪೂರ್ಣಾಙ್ಕ persist_chunk_client_ಸಂದೇಶ_write_v6(FILE *db_fptr, ಸಂರಚನಾ P_client_ಸಂದೇಶ *chunk);
ಪೂರ್ಣಾಙ್ಕ persist__chunk_message_store_write_v6(FILE *db_fptr, ಸಂರಚನಾ P_base_ಸಂದೇಶ *chunk);
ಪೂರ್ಣಾಙ್ಕ persist__chunk_retain_write_v6(FILE *db_fptr, ಸಂರಚನಾ P_retain *chunk);
ಪೂರ್ಣಾಙ್ಕ persist__chunk_sub_write_v6(FILE *db_fptr, ಸಂರಚನಾ P_sub *chunk);

#endif
