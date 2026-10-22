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

#شامل "mosquitto_broker_internal.h"

#define MOSQ_DB_VERSION 6

/* DB read/write */
بیرونی ثابت unsigned نویسه magic[15];
#define DB_CHUNK_CFG 1
#define DB_CHUNK_BASE_پیام 2
#define DB_CHUNK_CLIENT_پیام 3
#define DB_CHUNK_RETAIN 4
#define DB_CHUNK_SUB 5
#define DB_CHUNK_CLIENT 6
/* End DB read/write */

#define read_e(f, b, c) اگر(fread(b, 1, c, f) != c){ rc = MOSQ_ERR_UNKNOWN; برو error; }
#define write_e(f, b, c) اگر(fwrite(b, 1, c, f) != c){ برو error; }

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

ساختار PF_header {
	uint32_t chunk;
	uint32_t length;
};


ساختار PF_cfg {
	uint64_t last_db_id;
	uint8_t shutdown;
	uint8_t dbid_size;
};

ساختار PF_client_v5 {
	int64_t session_expiry_time;
	uint32_t session_expiry_interval;
	uint16_t last_mid;
	uint16_t id_len;
};
ساختار PF_client {
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
ساختار P_client {
	ساختار PF_client F;
	نویسه *clientid;
	نویسه *username;
};


ساختار PF_client_پیام {
	dbid_t store_id;
	uint16_t mid;
	uint16_t id_len;
	uint8_t qos;
	uint8_t state;
	uint8_t retain_dup;
	uint8_t direction;
};
ساختار P_client_پیام {
	ساختار PF_client_پیام F;
	نویسه *clientid;
	uint32_t subscription_identifier;
};


ساختار PF_base_پیام {
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
ساختار P_base_پیام {
	ساختار PF_base_پیام F;
	تهی *payload;
	ساختار mosquitto source;
	نویسه *topic;
	mosquitto_property *properties;
};


ساختار PF_sub {
	uint32_t identifier;
	uint16_t id_len;
	uint16_t topic_len;
	uint8_t qos;
	uint8_t options;
};
ساختار P_sub {
	ساختار PF_sub F;
	نویسه *clientid;
	نویسه *topic;
};


ساختار PF_retain {
	dbid_t store_id;
};
ساختار P_retain {
	ساختار PF_retain F;
};


صحیح persist__read_string_len(FILE *db_fptr, نویسه **str, uint16_t len);
صحیح persist__read_string(FILE *db_fptr, نویسه **str);

صحیح persist__chunk_header_read(FILE *db_fptr, uint32_t *chunk, uint32_t *length);

صحیح persist__chunk_header_read_v234(FILE *db_fptr, uint32_t *chunk, uint32_t *length);
صحیح persist__chunk_cfg_read_v234(FILE *db_fptr, ساختار PF_cfg *chunk);
صحیح persist__chunk_client_read_v234(FILE *db_fptr, ساختار P_client *chunk, uint32_t db_version);
صحیح persist_chunk_client_پیام_read_v234(FILE *db_fptr, ساختار P_client_پیام *chunk);
صحیح persist_chunk_base_پیام_read_v234(FILE *db_fptr, ساختار P_base_پیام *chunk, uint32_t db_version);
صحیح persist__chunk_retain_read_v234(FILE *db_fptr, ساختار P_retain *chunk);
صحیح persist__chunk_sub_read_v234(FILE *db_fptr, ساختار P_sub *chunk);

صحیح persist__chunk_header_read_v56(FILE *db_fptr, uint32_t *chunk, uint32_t *length);
صحیح persist__chunk_cfg_read_v56(FILE *db_fptr, ساختار PF_cfg *chunk);
صحیح persist__chunk_client_read_v56(FILE *db_fptr, ساختار P_client *chunk, uint32_t db_version);
صحیح persist_chunk_client_پیام_read_v56(FILE *db_fptr, ساختار P_client_پیام *chunk, uint32_t length);
صحیح persist_chunk_base_پیام_read_v56(FILE *db_fptr, ساختار P_base_پیام *chunk, uint32_t length);
صحیح persist__chunk_retain_read_v56(FILE *db_fptr, ساختار P_retain *chunk);
صحیح persist__chunk_sub_read_v56(FILE *db_fptr, ساختار P_sub *chunk);

صحیح persist__chunk_cfg_write_v6(FILE *db_fptr, ساختار PF_cfg *chunk);
صحیح persist__chunk_client_write_v6(FILE *db_fptr, ساختار P_client *chunk);
صحیح persist_chunk_client_پیام_write_v6(FILE *db_fptr, ساختار P_client_پیام *chunk);
صحیح persist__chunk_message_store_write_v6(FILE *db_fptr, ساختار P_base_پیام *chunk);
صحیح persist__chunk_retain_write_v6(FILE *db_fptr, ساختار P_retain *chunk);
صحیح persist__chunk_sub_write_v6(FILE *db_fptr, ساختار P_sub *chunk);

#endif
