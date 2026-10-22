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

#جيڪڏهن_وصف_نه_ٿيل PERSIST_H
#وصف_ڪريو PERSIST_H

#ضمن "mosquitto_broker_internal.h"

#وصف_ڪريو MOSQ_DB_نسخو 6

/* DB read/write */
خارجي ثابت بنانشان حرف magic[15];
#وصف_ڪريو DB_CHUNK_CFG 1
#وصف_ڪريو DB_CHUNK_بنياد_نياپو 2
#وصف_ڪريو DB_CHUNK_CLIENT_نياپو 3
#وصف_ڪريو DB_CHUNK_RETAIN 4
#وصف_ڪريو DB_CHUNK_SUB 5
#وصف_ڪريو DB_CHUNK_CLIENT 6
/* End DB read/write */

#وصف_ڪريو read_e(f, b, c) إذا(fread(b, 1, c, f) != c){ rc = MOSQ_ERR_UNKNOWN; اذهب error; }
#وصف_ڪريو write_e(f, b, c) إذا(fwrite(b, 1, c, f) != c){ اذهب error; }

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

بنية PF_سرو {
	uint32_t chunk;
	uint32_t ڊيگهائي;
};


بنية PF_cfg {
	uint64_t آخري_db_id;
	uint8_t shutdown;
	uint8_t dbid_سائيز;
};

بنية PF_client_v5 {
	int64_t session_expiry_وقت;
	uint32_t session_expiry_interval;
	uint16_t آخري_mid;
	uint16_t id_len;
};
بنية PF_client {
	/* struct PF_client_v5; */
	int64_t session_expiry_وقت;
	uint32_t session_expiry_interval;
	uint16_t آخري_mid;
	uint16_t id_len;

	uint16_t listener_پورٽ;
	uint16_t username_len;
	/* tail: 4 byte padding, because 64bit member
	 * forces multiple of 8 for struct size */
};
بنية P_client {
	بنية PF_client F;
	حرف *clientid;
	حرف *username;
};


بنية PF_client_نياپو {
	dbid_t store_id;
	uint16_t mid;
	uint16_t id_len;
	uint8_t qos;
	uint8_t state;
	uint8_t retain_dup;
	uint8_t direction;
};
بنية P_client_نياپو {
	بنية PF_client_نياپو F;
	حرف *clientid;
	uint32_t subscription_identifier;
};


بنية PF_بنياد_نياپو {
	dbid_t store_id;
	int64_t expiry_وقت;
	uint32_t payloadlen;
	uint16_t ذريعو_mid;
	uint16_t ذريعو_id_len;
	uint16_t ذريعو_username_len;
	uint16_t topic_len;
	uint16_t ذريعو_پورٽ;
	uint8_t qos;
	uint8_t retain;
};
بنية P_بنياد_نياپو {
	بنية PF_بنياد_نياپو F;
	فراغ *payload;
	بنية mosquitto ذريعو;
	حرف *topic;
	mosquitto_property *properties;
};


بنية PF_sub {
	uint32_t identifier;
	uint16_t id_len;
	uint16_t topic_len;
	uint8_t qos;
	uint8_t options;
};
بنية P_sub {
	بنية PF_sub F;
	حرف *clientid;
	حرف *topic;
};


بنية PF_retain {
	dbid_t store_id;
};
بنية P_retain {
	بنية PF_retain F;
};


صحيح persist__read_string_len(FILE *db_fptr, حرف **str, uint16_t len);
صحيح persist__read_string(FILE *db_fptr, حرف **str);

صحيح persist_chunk_سرو_read(FILE *db_fptr, uint32_t *chunk, uint32_t *ڊيگهائي);

صحيح persist_chunk_سرو_read_v234(FILE *db_fptr, uint32_t *chunk, uint32_t *ڊيگهائي);
صحيح persist__chunk_cfg_read_v234(FILE *db_fptr, بنية PF_cfg *chunk);
صحيح persist__chunk_client_read_v234(FILE *db_fptr, بنية P_client *chunk, uint32_t db_نسخو);
صحيح persist_chunk_client_نياپو_read_v234(FILE *db_fptr, بنية P_client_نياپو *chunk);
صحيح persist_chunk_بنياد_نياپو_read_v234(FILE *db_fptr, بنية P_بنياد_نياپو *chunk, uint32_t db_نسخو);
صحيح persist__chunk_retain_read_v234(FILE *db_fptr, بنية P_retain *chunk);
صحيح persist__chunk_sub_read_v234(FILE *db_fptr, بنية P_sub *chunk);

صحيح persist_chunk_سرو_read_v56(FILE *db_fptr, uint32_t *chunk, uint32_t *ڊيگهائي);
صحيح persist__chunk_cfg_read_v56(FILE *db_fptr, بنية PF_cfg *chunk);
صحيح persist__chunk_client_read_v56(FILE *db_fptr, بنية P_client *chunk, uint32_t db_نسخو);
صحيح persist_chunk_client_نياپو_read_v56(FILE *db_fptr, بنية P_client_نياپو *chunk, uint32_t ڊيگهائي);
صحيح persist_chunk_بنياد_نياپو_read_v56(FILE *db_fptr, بنية P_بنياد_نياپو *chunk, uint32_t ڊيگهائي);
صحيح persist__chunk_retain_read_v56(FILE *db_fptr, بنية P_retain *chunk);
صحيح persist__chunk_sub_read_v56(FILE *db_fptr, بنية P_sub *chunk);

صحيح persist__chunk_cfg_write_v6(FILE *db_fptr, بنية PF_cfg *chunk);
صحيح persist__chunk_client_write_v6(FILE *db_fptr, بنية P_client *chunk);
صحيح persist_chunk_client_نياپو_write_v6(FILE *db_fptr, بنية P_client_نياپو *chunk);
صحيح persist_chunk_پيغام_store_write_v6(FILE *db_fptr, بنية P_بنياد_نياپو *chunk);
صحيح persist__chunk_retain_write_v6(FILE *db_fptr, بنية P_retain *chunk);
صحيح persist__chunk_sub_write_v6(FILE *db_fptr, بنية P_sub *chunk);

#جيڪڏهن_پورو
