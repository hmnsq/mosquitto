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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ PERSIST_H
#ವ್ಯಾಖ್ಯಾನಿಸು PERSIST_H

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"

#ವ್ಯಾಖ್ಯಾನಿಸು MOSQ_DB_ಆವೃತ್ತಿ 6

/* DB read/write */
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ magic[15];
#ವ್ಯಾಖ್ಯಾನಿಸು DB_CHUNK_CFG 1
#ವ್ಯಾಖ್ಯಾನಿಸು DB_CHUNK_ತಳ_ಸಂದೇಶ 2
#ವ್ಯಾಖ್ಯಾನಿಸು DB_CHUNK_CLIENT_ಸಂದೇಶ 3
#ವ್ಯಾಖ್ಯಾನಿಸು DB_CHUNK_RETAIN 4
#ವ್ಯಾಖ್ಯಾನಿಸು DB_CHUNK_SUB 5
#ವ್ಯಾಖ್ಯಾನಿಸು DB_CHUNK_CLIENT 6
/* End DB read/write */

#ವ್ಯಾಖ್ಯಾನಿಸು read_e(f, b, c) ಆದರೆ(fread(b, 1, c, f) != c){ rc = MOSQ_ERR_UNKNOWN; ಹೋಗು error; }
#ವ್ಯಾಖ್ಯಾನಿಸು write_e(f, b, c) ಆದರೆ(fwrite(b, 1, c, f) != c){ ಹೋಗು error; }

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

ರಚನೆ PF_ಶೀರ್ಷಿಕೆ {
	uint32_t chunk;
	uint32_t ಉದ್ದಳತೆ;
};


ರಚನೆ PF_cfg {
	uint64_t ಕೊನೆಯ_db_id;
	uint8_t shutdown;
	uint8_t dbid_ಗಾತ್ರ;
};

ರಚನೆ PF_client_v5 {
	int64_t session_expiry_ಸಮಯ;
	uint32_t session_expiry_interval;
	uint16_t ಕೊನೆಯ_mid;
	uint16_t id_len;
};
ರಚನೆ PF_client {
	/* struct PF_client_v5; */
	int64_t session_expiry_ಸಮಯ;
	uint32_t session_expiry_interval;
	uint16_t ಕೊನೆಯ_mid;
	uint16_t id_len;

	uint16_t listener_ಪೋರ್ಟ್;
	uint16_t username_len;
	/* tail: 4 byte padding, because 64bit member
	 * forces multiple of 8 for struct size */
};
ರಚನೆ P_client {
	ರಚನೆ PF_client F;
	ಅಕ್ಷರ *clientid;
	ಅಕ್ಷರ *username;
};


ರಚನೆ PF_client_ಸಂದೇಶ {
	dbid_t store_id;
	uint16_t mid;
	uint16_t id_len;
	uint8_t qos;
	uint8_t state;
	uint8_t retain_dup;
	uint8_t direction;
};
ರಚನೆ P_client_ಸಂದೇಶ {
	ರಚನೆ PF_client_ಸಂದೇಶ F;
	ಅಕ್ಷರ *clientid;
	uint32_t subscription_identifier;
};


ರಚನೆ PF_ತಳ_ಸಂದೇಶ {
	dbid_t store_id;
	int64_t expiry_ಸಮಯ;
	uint32_t payloadlen;
	uint16_t ಮೂಲ_mid;
	uint16_t ಮೂಲ_id_len;
	uint16_t ಮೂಲ_username_len;
	uint16_t topic_len;
	uint16_t ಮೂಲ_ಪೋರ್ಟ್;
	uint8_t qos;
	uint8_t retain;
};
ರಚನೆ P_ತಳ_ಸಂದೇಶ {
	ರಚನೆ PF_ತಳ_ಸಂದೇಶ F;
	ಶೂನ್ಯ *payload;
	ರಚನೆ mosquitto ಮೂಲ;
	ಅಕ್ಷರ *topic;
	mosquitto_property *properties;
};


ರಚನೆ PF_sub {
	uint32_t identifier;
	uint16_t id_len;
	uint16_t topic_len;
	uint8_t qos;
	uint8_t options;
};
ರಚನೆ P_sub {
	ರಚನೆ PF_sub F;
	ಅಕ್ಷರ *clientid;
	ಅಕ್ಷರ *topic;
};


ರಚನೆ PF_retain {
	dbid_t store_id;
};
ರಚನೆ P_retain {
	ರಚನೆ PF_retain F;
};


ಪೂರ್ಣಾಂಕ persist__read_string_len(FILE *db_fptr, ಅಕ್ಷರ **str, uint16_t len);
ಪೂರ್ಣಾಂಕ persist__read_string(FILE *db_fptr, ಅಕ್ಷರ **str);

ಪೂರ್ಣಾಂಕ persist_chunk_ಶೀರ್ಷಿಕೆ_read(FILE *db_fptr, uint32_t *chunk, uint32_t *ಉದ್ದಳತೆ);

ಪೂರ್ಣಾಂಕ persist_chunk_ಶೀರ್ಷಿಕೆ_read_v234(FILE *db_fptr, uint32_t *chunk, uint32_t *ಉದ್ದಳತೆ);
ಪೂರ್ಣಾಂಕ persist__chunk_cfg_read_v234(FILE *db_fptr, ರಚನೆ PF_cfg *chunk);
ಪೂರ್ಣಾಂಕ persist__chunk_client_read_v234(FILE *db_fptr, ರಚನೆ P_client *chunk, uint32_t db_ಆವೃತ್ತಿ);
ಪೂರ್ಣಾಂಕ persist_chunk_client_ಸಂದೇಶ_read_v234(FILE *db_fptr, ರಚನೆ P_client_ಸಂದೇಶ *chunk);
ಪೂರ್ಣಾಂಕ persist_chunk_ತಳ_ಸಂದೇಶ_read_v234(FILE *db_fptr, ರಚನೆ P_ತಳ_ಸಂದೇಶ *chunk, uint32_t db_ಆವೃತ್ತಿ);
ಪೂರ್ಣಾಂಕ persist__chunk_retain_read_v234(FILE *db_fptr, ರಚನೆ P_retain *chunk);
ಪೂರ್ಣಾಂಕ persist__chunk_sub_read_v234(FILE *db_fptr, ರಚನೆ P_sub *chunk);

ಪೂರ್ಣಾಂಕ persist_chunk_ಶೀರ್ಷಿಕೆ_read_v56(FILE *db_fptr, uint32_t *chunk, uint32_t *ಉದ್ದಳತೆ);
ಪೂರ್ಣಾಂಕ persist__chunk_cfg_read_v56(FILE *db_fptr, ರಚನೆ PF_cfg *chunk);
ಪೂರ್ಣಾಂಕ persist__chunk_client_read_v56(FILE *db_fptr, ರಚನೆ P_client *chunk, uint32_t db_ಆವೃತ್ತಿ);
ಪೂರ್ಣಾಂಕ persist_chunk_client_ಸಂದೇಶ_read_v56(FILE *db_fptr, ರಚನೆ P_client_ಸಂದೇಶ *chunk, uint32_t ಉದ್ದಳತೆ);
ಪೂರ್ಣಾಂಕ persist_chunk_ತಳ_ಸಂದೇಶ_read_v56(FILE *db_fptr, ರಚನೆ P_ತಳ_ಸಂದೇಶ *chunk, uint32_t ಉದ್ದಳತೆ);
ಪೂರ್ಣಾಂಕ persist__chunk_retain_read_v56(FILE *db_fptr, ರಚನೆ P_retain *chunk);
ಪೂರ್ಣಾಂಕ persist__chunk_sub_read_v56(FILE *db_fptr, ರಚನೆ P_sub *chunk);

ಪೂರ್ಣಾಂಕ persist__chunk_cfg_write_v6(FILE *db_fptr, ರಚನೆ PF_cfg *chunk);
ಪೂರ್ಣಾಂಕ persist__chunk_client_write_v6(FILE *db_fptr, ರಚನೆ P_client *chunk);
ಪೂರ್ಣಾಂಕ persist_chunk_client_ಸಂದೇಶ_write_v6(FILE *db_fptr, ರಚನೆ P_client_ಸಂದೇಶ *chunk);
ಪೂರ್ಣಾಂಕ persist_chunk_ಸಂದೇಶ_store_write_v6(FILE *db_fptr, ರಚನೆ P_ತಳ_ಸಂದೇಶ *chunk);
ಪೂರ್ಣಾಂಕ persist__chunk_retain_write_v6(FILE *db_fptr, ರಚನೆ P_retain *chunk);
ಪೂರ್ಣಾಂಕ persist__chunk_sub_write_v6(FILE *db_fptr, ರಚನೆ P_sub *chunk);

#ಆದರೆ_ಅಂತ್ಯ
