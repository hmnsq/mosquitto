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

#അന്തര്ഭാവ "mosquitto_broker_internal.h"

#define MOSQ_DB_VERSION 6

/* DB read/write */
ബാഹ്യ സ്ഥിരം unsigned അക്ഷരം magic[15];
#define DB_CHUNK_CFG 1
#define DB_CHUNK_BASE_സന്ദേശം 2
#define DB_CHUNK_CLIENT_സന്ദേശം 3
#define DB_CHUNK_RETAIN 4
#define DB_CHUNK_SUB 5
#define DB_CHUNK_CLIENT 6
/* End DB read/write */

#define read_e(f, b, c) എങ്കിൽ(fread(b, 1, c, f) != c){ rc = MOSQ_ERR_UNKNOWN; പോകുക error; }
#define write_e(f, b, c) എങ്കിൽ(fwrite(b, 1, c, f) != c){ പോകുക error; }

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

ഘടന PF_header {
	uint32_t chunk;
	uint32_t length;
};


ഘടന PF_cfg {
	uint64_t last_db_id;
	uint8_t shutdown;
	uint8_t dbid_size;
};

ഘടന PF_client_v5 {
	int64_t session_expiry_time;
	uint32_t session_expiry_interval;
	uint16_t last_mid;
	uint16_t id_len;
};
ഘടന PF_client {
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
ഘടന P_client {
	ഘടന PF_client F;
	അക്ഷരം *clientid;
	അക്ഷരം *username;
};


ഘടന PF_client_സന്ദേശം {
	dbid_t store_id;
	uint16_t mid;
	uint16_t id_len;
	uint8_t qos;
	uint8_t state;
	uint8_t retain_dup;
	uint8_t direction;
};
ഘടന P_client_സന്ദേശം {
	ഘടന PF_client_സന്ദേശം F;
	അക്ഷരം *clientid;
	uint32_t subscription_identifier;
};


ഘടന PF_base_സന്ദേശം {
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
ഘടന P_base_സന്ദേശം {
	ഘടന PF_base_സന്ദേശം F;
	ശൂന്യം *payload;
	ഘടന mosquitto source;
	അക്ഷരം *topic;
	mosquitto_property *properties;
};


ഘടന PF_sub {
	uint32_t identifier;
	uint16_t id_len;
	uint16_t topic_len;
	uint8_t qos;
	uint8_t options;
};
ഘടന P_sub {
	ഘടന PF_sub F;
	അക്ഷരം *clientid;
	അക്ഷരം *topic;
};


ഘടന PF_retain {
	dbid_t store_id;
};
ഘടന P_retain {
	ഘടന PF_retain F;
};


പൂർണ്ണസംഖ്യ persist__read_string_len(FILE *db_fptr, അക്ഷരം **str, uint16_t len);
പൂർണ്ണസംഖ്യ persist__read_string(FILE *db_fptr, അക്ഷരം **str);

പൂർണ്ണസംഖ്യ persist__chunk_header_read(FILE *db_fptr, uint32_t *chunk, uint32_t *length);

പൂർണ്ണസംഖ്യ persist__chunk_header_read_v234(FILE *db_fptr, uint32_t *chunk, uint32_t *length);
പൂർണ്ണസംഖ്യ persist__chunk_cfg_read_v234(FILE *db_fptr, ഘടന PF_cfg *chunk);
പൂർണ്ണസംഖ്യ persist__chunk_client_read_v234(FILE *db_fptr, ഘടന P_client *chunk, uint32_t db_version);
പൂർണ്ണസംഖ്യ persist_chunk_client_സന്ദേശം_read_v234(FILE *db_fptr, ഘടന P_client_സന്ദേശം *chunk);
പൂർണ്ണസംഖ്യ persist_chunk_base_സന്ദേശം_read_v234(FILE *db_fptr, ഘടന P_base_സന്ദേശം *chunk, uint32_t db_version);
പൂർണ്ണസംഖ്യ persist__chunk_retain_read_v234(FILE *db_fptr, ഘടന P_retain *chunk);
പൂർണ്ണസംഖ്യ persist__chunk_sub_read_v234(FILE *db_fptr, ഘടന P_sub *chunk);

പൂർണ്ണസംഖ്യ persist__chunk_header_read_v56(FILE *db_fptr, uint32_t *chunk, uint32_t *length);
പൂർണ്ണസംഖ്യ persist__chunk_cfg_read_v56(FILE *db_fptr, ഘടന PF_cfg *chunk);
പൂർണ്ണസംഖ്യ persist__chunk_client_read_v56(FILE *db_fptr, ഘടന P_client *chunk, uint32_t db_version);
പൂർണ്ണസംഖ്യ persist_chunk_client_സന്ദേശം_read_v56(FILE *db_fptr, ഘടന P_client_സന്ദേശം *chunk, uint32_t length);
പൂർണ്ണസംഖ്യ persist_chunk_base_സന്ദേശം_read_v56(FILE *db_fptr, ഘടന P_base_സന്ദേശം *chunk, uint32_t length);
പൂർണ്ണസംഖ്യ persist__chunk_retain_read_v56(FILE *db_fptr, ഘടന P_retain *chunk);
പൂർണ്ണസംഖ്യ persist__chunk_sub_read_v56(FILE *db_fptr, ഘടന P_sub *chunk);

പൂർണ്ണസംഖ്യ persist__chunk_cfg_write_v6(FILE *db_fptr, ഘടന PF_cfg *chunk);
പൂർണ്ണസംഖ്യ persist__chunk_client_write_v6(FILE *db_fptr, ഘടന P_client *chunk);
പൂർണ്ണസംഖ്യ persist_chunk_client_സന്ദേശം_write_v6(FILE *db_fptr, ഘടന P_client_സന്ദേശം *chunk);
പൂർണ്ണസംഖ്യ persist__chunk_message_store_write_v6(FILE *db_fptr, ഘടന P_base_സന്ദേശം *chunk);
പൂർണ്ണസംഖ്യ persist__chunk_retain_write_v6(FILE *db_fptr, ഘടന P_retain *chunk);
പൂർണ്ണസംഖ്യ persist__chunk_sub_write_v6(FILE *db_fptr, ഘടന P_sub *chunk);

#endif
