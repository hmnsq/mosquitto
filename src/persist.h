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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ PERSIST_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PERSIST_H

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOSQ_DB_ਸੰਸਕਰਣ 6

/* DB read/write */
ਬਾਹਰੀ ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ magic[15];
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DB_CHUNK_CFG 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DB_CHUNK_ਅਧਾਰ_ਸੁਨੇਹਾ 2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DB_CHUNK_CLIENT_ਸੁਨੇਹਾ 3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DB_CHUNK_RETAIN 4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DB_CHUNK_SUB 5
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DB_CHUNK_CLIENT 6
/* End DB read/write */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ read_e(f, b, c) ਜੇ(fread(b, 1, c, f) != c){ rc = MOSQ_ERR_UNKNOWN; ਜਾਓ error; }
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ write_e(f, b, c) ਜੇ(fwrite(b, 1, c, f) != c){ ਜਾਓ error; }

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

ਬਣਤਰ PF_ਸਿਰਲੇਖ {
	uint32_t chunk;
	uint32_t ਲੰਮਾਈ;
};


ਬਣਤਰ PF_cfg {
	uint64_t ਆਖਰੀ_db_id;
	uint8_t shutdown;
	uint8_t dbid_ਮਾਪ;
};

ਬਣਤਰ PF_client_v5 {
	int64_t session_expiry_ਸਮਾਂ;
	uint32_t session_expiry_interval;
	uint16_t ਆਖਰੀ_mid;
	uint16_t id_len;
};
ਬਣਤਰ PF_client {
	/* struct PF_client_v5; */
	int64_t session_expiry_ਸਮਾਂ;
	uint32_t session_expiry_interval;
	uint16_t ਆਖਰੀ_mid;
	uint16_t id_len;

	uint16_t listener_ਪੋਰਟ;
	uint16_t username_len;
	/* tail: 4 byte padding, because 64bit member
	 * forces multiple of 8 for struct size */
};
ਬਣਤਰ P_client {
	ਬਣਤਰ PF_client F;
	ਅੱਖਰ *clientid;
	ਅੱਖਰ *username;
};


ਬਣਤਰ PF_client_ਸੁਨੇਹਾ {
	dbid_t store_id;
	uint16_t mid;
	uint16_t id_len;
	uint8_t qos;
	uint8_t state;
	uint8_t retain_dup;
	uint8_t direction;
};
ਬਣਤਰ P_client_ਸੁਨੇਹਾ {
	ਬਣਤਰ PF_client_ਸੁਨੇਹਾ F;
	ਅੱਖਰ *clientid;
	uint32_t subscription_identifier;
};


ਬਣਤਰ PF_ਅਧਾਰ_ਸੁਨੇਹਾ {
	dbid_t store_id;
	int64_t expiry_ਸਮਾਂ;
	uint32_t payloadlen;
	uint16_t ਸਰੋਤ_mid;
	uint16_t ਸਰੋਤ_id_len;
	uint16_t ਸਰੋਤ_username_len;
	uint16_t topic_len;
	uint16_t ਸਰੋਤ_ਪੋਰਟ;
	uint8_t qos;
	uint8_t retain;
};
ਬਣਤਰ P_ਅਧਾਰ_ਸੁਨੇਹਾ {
	ਬਣਤਰ PF_ਅਧਾਰ_ਸੁਨੇਹਾ F;
	ਖਾਲੀ *payload;
	ਬਣਤਰ mosquitto ਸਰੋਤ;
	ਅੱਖਰ *topic;
	mosquitto_property *properties;
};


ਬਣਤਰ PF_sub {
	uint32_t identifier;
	uint16_t id_len;
	uint16_t topic_len;
	uint8_t qos;
	uint8_t options;
};
ਬਣਤਰ P_sub {
	ਬਣਤਰ PF_sub F;
	ਅੱਖਰ *clientid;
	ਅੱਖਰ *topic;
};


ਬਣਤਰ PF_retain {
	dbid_t store_id;
};
ਬਣਤਰ P_retain {
	ਬਣਤਰ PF_retain F;
};


ਪੂਰਨ persist__read_string_len(FILE *db_fptr, ਅੱਖਰ **str, uint16_t len);
ਪੂਰਨ persist__read_string(FILE *db_fptr, ਅੱਖਰ **str);

ਪੂਰਨ persist_chunk_ਸਿਰਲੇਖ_read(FILE *db_fptr, uint32_t *chunk, uint32_t *ਲੰਮਾਈ);

ਪੂਰਨ persist_chunk_ਸਿਰਲੇਖ_read_v234(FILE *db_fptr, uint32_t *chunk, uint32_t *ਲੰਮਾਈ);
ਪੂਰਨ persist__chunk_cfg_read_v234(FILE *db_fptr, ਬਣਤਰ PF_cfg *chunk);
ਪੂਰਨ persist__chunk_client_read_v234(FILE *db_fptr, ਬਣਤਰ P_client *chunk, uint32_t db_ਸੰਸਕਰਣ);
ਪੂਰਨ persist_chunk_client_ਸੁਨੇਹਾ_read_v234(FILE *db_fptr, ਬਣਤਰ P_client_ਸੁਨੇਹਾ *chunk);
ਪੂਰਨ persist_chunk_ਅਧਾਰ_ਸੁਨੇਹਾ_read_v234(FILE *db_fptr, ਬਣਤਰ P_ਅਧਾਰ_ਸੁਨੇਹਾ *chunk, uint32_t db_ਸੰਸਕਰਣ);
ਪੂਰਨ persist__chunk_retain_read_v234(FILE *db_fptr, ਬਣਤਰ P_retain *chunk);
ਪੂਰਨ persist__chunk_sub_read_v234(FILE *db_fptr, ਬਣਤਰ P_sub *chunk);

ਪੂਰਨ persist_chunk_ਸਿਰਲੇਖ_read_v56(FILE *db_fptr, uint32_t *chunk, uint32_t *ਲੰਮਾਈ);
ਪੂਰਨ persist__chunk_cfg_read_v56(FILE *db_fptr, ਬਣਤਰ PF_cfg *chunk);
ਪੂਰਨ persist__chunk_client_read_v56(FILE *db_fptr, ਬਣਤਰ P_client *chunk, uint32_t db_ਸੰਸਕਰਣ);
ਪੂਰਨ persist_chunk_client_ਸੁਨੇਹਾ_read_v56(FILE *db_fptr, ਬਣਤਰ P_client_ਸੁਨੇਹਾ *chunk, uint32_t ਲੰਮਾਈ);
ਪੂਰਨ persist_chunk_ਅਧਾਰ_ਸੁਨੇਹਾ_read_v56(FILE *db_fptr, ਬਣਤਰ P_ਅਧਾਰ_ਸੁਨੇਹਾ *chunk, uint32_t ਲੰਮਾਈ);
ਪੂਰਨ persist__chunk_retain_read_v56(FILE *db_fptr, ਬਣਤਰ P_retain *chunk);
ਪੂਰਨ persist__chunk_sub_read_v56(FILE *db_fptr, ਬਣਤਰ P_sub *chunk);

ਪੂਰਨ persist__chunk_cfg_write_v6(FILE *db_fptr, ਬਣਤਰ PF_cfg *chunk);
ਪੂਰਨ persist__chunk_client_write_v6(FILE *db_fptr, ਬਣਤਰ P_client *chunk);
ਪੂਰਨ persist_chunk_client_ਸੁਨੇਹਾ_write_v6(FILE *db_fptr, ਬਣਤਰ P_client_ਸੁਨੇਹਾ *chunk);
ਪੂਰਨ persist_chunk_ਸੁਨੇਹਾ_store_write_v6(FILE *db_fptr, ਬਣਤਰ P_ਅਧਾਰ_ਸੁਨੇਹਾ *chunk);
ਪੂਰਨ persist__chunk_retain_write_v6(FILE *db_fptr, ਬਣਤਰ P_retain *chunk);
ਪੂਰਨ persist__chunk_sub_write_v6(FILE *db_fptr, ਬਣਤਰ P_sub *chunk);

#ਜੇ_ਅੰਤ
