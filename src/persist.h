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

#अगर_अघोषित PERSIST_H
#घोषणा PERSIST_H

#समावेश "mosquitto_broker_internal.h"

#घोषणा MOSQ_DB_VERSION 6

/* DB read/write */
बाह्य स्थिर अचिन्हित अक्षर magic[15];
#घोषणा DB_CHUNK_CFG 1
#घोषणा DB_CHUNK_BASE_संदेश 2
#घोषणा DB_CHUNK_CLIENT_संदेश 3
#घोषणा DB_CHUNK_RETAIN 4
#घोषणा DB_CHUNK_SUB 5
#घोषणा DB_CHUNK_CLIENT 6
/* End DB read/write */

#घोषणा read_e(f, b, c) अगर(ख_पढ़ो(b, 1, c, f) != c){ rc = MOSQ_ERR_UNKNOWN; जाओ error; }
#घोषणा write_e(f, b, c) अगर(ख_डालो(b, 1, c, f) != c){ जाओ error; }

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

काष्ठा PF_header {
	uint32_t chunk;
	uint32_t लंबाई;
};


काष्ठा PF_cfg {
	uint64_t last_db_id;
	uint8_t shutdown;
	uint8_t dbid_आकार;
};

काष्ठा PF_client_v5 {
	int64_t session_expiry_time;
	uint32_t session_expiry_interval;
	uint16_t last_mid;
	uint16_t id_len;
};
काष्ठा PF_client {
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
काष्ठा P_client {
	काष्ठा PF_client F;
	अक्षर *clientid;
	अक्षर *username;
};


काष्ठा PF_client_संदेश {
	dbid_t store_id;
	uint16_t mid;
	uint16_t id_len;
	uint8_t qos;
	uint8_t state;
	uint8_t retain_dup;
	uint8_t direction;
};
काष्ठा P_client_संदेश {
	काष्ठा PF_client_संदेश F;
	अक्षर *clientid;
	uint32_t subscription_identifier;
};


काष्ठा PF_base_संदेश {
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
काष्ठा P_base_संदेश {
	काष्ठा PF_base_संदेश F;
	व्योम *payload;
	काष्ठा mosquitto source;
	अक्षर *topic;
	mosquitto_property *properties;
};


काष्ठा PF_sub {
	uint32_t identifier;
	uint16_t id_len;
	uint16_t topic_len;
	uint8_t qos;
	uint8_t options;
};
काष्ठा P_sub {
	काष्ठा PF_sub F;
	अक्षर *clientid;
	अक्षर *topic;
};


काष्ठा PF_retain {
	dbid_t store_id;
};
काष्ठा P_retain {
	काष्ठा PF_retain F;
};


पूर्णांक persist__read_string_len(खाता *db_fptr, अक्षर **str, uint16_t len);
पूर्णांक persist__read_string(खाता *db_fptr, अक्षर **str);

पूर्णांक persist__chunk_header_read(खाता *db_fptr, uint32_t *chunk, uint32_t *लंबाई);

पूर्णांक persist__chunk_header_read_v234(खाता *db_fptr, uint32_t *chunk, uint32_t *लंबाई);
पूर्णांक persist__chunk_cfg_read_v234(खाता *db_fptr, काष्ठा PF_cfg *chunk);
पूर्णांक persist__chunk_client_read_v234(खाता *db_fptr, काष्ठा P_client *chunk, uint32_t db_version);
पूर्णांक persist_chunk_client_संदेश_read_v234(खाता *db_fptr, काष्ठा P_client_संदेश *chunk);
पूर्णांक persist_chunk_base_संदेश_read_v234(खाता *db_fptr, काष्ठा P_base_संदेश *chunk, uint32_t db_version);
पूर्णांक persist__chunk_retain_read_v234(खाता *db_fptr, काष्ठा P_retain *chunk);
पूर्णांक persist__chunk_sub_read_v234(खाता *db_fptr, काष्ठा P_sub *chunk);

पूर्णांक persist__chunk_header_read_v56(खाता *db_fptr, uint32_t *chunk, uint32_t *लंबाई);
पूर्णांक persist__chunk_cfg_read_v56(खाता *db_fptr, काष्ठा PF_cfg *chunk);
पूर्णांक persist__chunk_client_read_v56(खाता *db_fptr, काष्ठा P_client *chunk, uint32_t db_version);
पूर्णांक persist_chunk_client_संदेश_read_v56(खाता *db_fptr, काष्ठा P_client_संदेश *chunk, uint32_t लंबाई);
पूर्णांक persist_chunk_base_संदेश_read_v56(खाता *db_fptr, काष्ठा P_base_संदेश *chunk, uint32_t लंबाई);
पूर्णांक persist__chunk_retain_read_v56(खाता *db_fptr, काष्ठा P_retain *chunk);
पूर्णांक persist__chunk_sub_read_v56(खाता *db_fptr, काष्ठा P_sub *chunk);

पूर्णांक persist__chunk_cfg_write_v6(खाता *db_fptr, काष्ठा PF_cfg *chunk);
पूर्णांक persist__chunk_client_write_v6(खाता *db_fptr, काष्ठा P_client *chunk);
पूर्णांक persist_chunk_client_संदेश_write_v6(खाता *db_fptr, काष्ठा P_client_संदेश *chunk);
पूर्णांक persist__chunk_message_store_write_v6(खाता *db_fptr, काष्ठा P_base_संदेश *chunk);
पूर्णांक persist__chunk_retain_write_v6(खाता *db_fptr, काष्ठा P_retain *chunk);
पूर्णांक persist__chunk_sub_write_v6(खाता *db_fptr, काष्ठा P_sub *chunk);

#पूर्ण_अगर
