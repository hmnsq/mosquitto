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

#اگر_غیر_تعریف_شدہ PERSIST_H
#تعریف_کرو PERSIST_H

#شامل "mosquitto_broker_internal.h"

#تعریف_کرو MOSQ_DB_نسخہ 6

/* DB read/write */
خارجی ثابت غیرعلامتی حرف magic[15];
#تعریف_کرو DB_CHUNK_CFG 1
#تعریف_کرو DB_CHUNK_بنیاد_پیغام 2
#تعریف_کرو DB_CHUNK_CLIENT_پیغام 3
#تعریف_کرو DB_CHUNK_RETAIN 4
#تعریف_کرو DB_CHUNK_SUB 5
#تعریف_کرو DB_CHUNK_CLIENT 6
/* End DB read/write */

#تعریف_کرو read_e(f, b, c) اگر(fread(b, 1, c, f) != c){ rc = MOSQ_ERR_UNKNOWN; جاؤ error; }
#تعریف_کرو write_e(f, b, c) اگر(fwrite(b, 1, c, f) != c){ جاؤ error; }

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

ساخت PF_سرخی {
	uint32_t chunk;
	uint32_t لمبائی;
};


ساخت PF_cfg {
	uint64_t آخری_db_id;
	uint8_t shutdown;
	uint8_t dbid_سائز;
};

ساخت PF_client_v5 {
	int64_t session_expiry_وقت;
	uint32_t session_expiry_interval;
	uint16_t آخری_mid;
	uint16_t id_len;
};
ساخت PF_client {
	/* struct PF_client_v5; */
	int64_t session_expiry_وقت;
	uint32_t session_expiry_interval;
	uint16_t آخری_mid;
	uint16_t id_len;

	uint16_t listener_پورٹ;
	uint16_t username_len;
	/* tail: 4 byte padding, because 64bit member
	 * forces multiple of 8 for struct size */
};
ساخت P_client {
	ساخت PF_client F;
	حرف *clientid;
	حرف *username;
};


ساخت PF_client_پیغام {
	dbid_t store_id;
	uint16_t mid;
	uint16_t id_len;
	uint8_t qos;
	uint8_t state;
	uint8_t retain_dup;
	uint8_t direction;
};
ساخت P_client_پیغام {
	ساخت PF_client_پیغام F;
	حرف *clientid;
	uint32_t subscription_identifier;
};


ساخت PF_بنیاد_پیغام {
	dbid_t store_id;
	int64_t expiry_وقت;
	uint32_t payloadlen;
	uint16_t ماخذ_mid;
	uint16_t ماخذ_id_len;
	uint16_t ماخذ_username_len;
	uint16_t topic_len;
	uint16_t ماخذ_پورٹ;
	uint8_t qos;
	uint8_t retain;
};
ساخت P_بنیاد_پیغام {
	ساخت PF_بنیاد_پیغام F;
	خالی *payload;
	ساخت mosquitto ماخذ;
	حرف *topic;
	mosquitto_property *properties;
};


ساخت PF_sub {
	uint32_t identifier;
	uint16_t id_len;
	uint16_t topic_len;
	uint8_t qos;
	uint8_t options;
};
ساخت P_sub {
	ساخت PF_sub F;
	حرف *clientid;
	حرف *topic;
};


ساخت PF_retain {
	dbid_t store_id;
};
ساخت P_retain {
	ساخت PF_retain F;
};


صحیح persist__read_string_len(FILE *db_fptr, حرف **str, uint16_t len);
صحیح persist__read_string(FILE *db_fptr, حرف **str);

صحیح persist_chunk_سرخی_read(FILE *db_fptr, uint32_t *chunk, uint32_t *لمبائی);

صحیح persist_chunk_سرخی_read_v234(FILE *db_fptr, uint32_t *chunk, uint32_t *لمبائی);
صحیح persist__chunk_cfg_read_v234(FILE *db_fptr, ساخت PF_cfg *chunk);
صحیح persist__chunk_client_read_v234(FILE *db_fptr, ساخت P_client *chunk, uint32_t db_نسخہ);
صحیح persist_chunk_client_پیغام_read_v234(FILE *db_fptr, ساخت P_client_پیغام *chunk);
صحیح persist_chunk_بنیاد_پیغام_read_v234(FILE *db_fptr, ساخت P_بنیاد_پیغام *chunk, uint32_t db_نسخہ);
صحیح persist__chunk_retain_read_v234(FILE *db_fptr, ساخت P_retain *chunk);
صحیح persist__chunk_sub_read_v234(FILE *db_fptr, ساخت P_sub *chunk);

صحیح persist_chunk_سرخی_read_v56(FILE *db_fptr, uint32_t *chunk, uint32_t *لمبائی);
صحیح persist__chunk_cfg_read_v56(FILE *db_fptr, ساخت PF_cfg *chunk);
صحیح persist__chunk_client_read_v56(FILE *db_fptr, ساخت P_client *chunk, uint32_t db_نسخہ);
صحیح persist_chunk_client_پیغام_read_v56(FILE *db_fptr, ساخت P_client_پیغام *chunk, uint32_t لمبائی);
صحیح persist_chunk_بنیاد_پیغام_read_v56(FILE *db_fptr, ساخت P_بنیاد_پیغام *chunk, uint32_t لمبائی);
صحیح persist__chunk_retain_read_v56(FILE *db_fptr, ساخت P_retain *chunk);
صحیح persist__chunk_sub_read_v56(FILE *db_fptr, ساخت P_sub *chunk);

صحیح persist__chunk_cfg_write_v6(FILE *db_fptr, ساخت PF_cfg *chunk);
صحیح persist__chunk_client_write_v6(FILE *db_fptr, ساخت P_client *chunk);
صحیح persist_chunk_client_پیغام_write_v6(FILE *db_fptr, ساخت P_client_پیغام *chunk);
صحیح persist_chunk_پیغام_store_write_v6(FILE *db_fptr, ساخت P_بنیاد_پیغام *chunk);
صحیح persist__chunk_retain_write_v6(FILE *db_fptr, ساخت P_retain *chunk);
صحیح persist__chunk_sub_write_v6(FILE *db_fptr, ساخت P_sub *chunk);

#اگر_ختم
