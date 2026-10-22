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

#यदि_अपरिभाषितम् PERSIST_H
#परिभाषय PERSIST_H

#अन्तर्भाव "mosquitto_broker_internal.h"

#परिभाषय MOSQ_DB_संस्करणम् 6

/* DB read/write */
बाह्य ध्रुव अचिह्नित वर्ण magic[15];
#परिभाषय DB_CHUNK_CFG 1
#परिभाषय DB_CHUNK_आधारः_सन्देशः 2
#परिभाषय DB_CHUNK_CLIENT_सन्देशः 3
#परिभाषय DB_CHUNK_RETAIN 4
#परिभाषय DB_CHUNK_SUB 5
#परिभाषय DB_CHUNK_CLIENT 6
/* End DB read/write */

#परिभाषय read_e(f, b, c) यदि(fread(b, 1, c, f) != c){ rc = MOSQ_ERR_UNKNOWN; गच्छ error; }
#परिभाषय write_e(f, b, c) यदि(fwrite(b, 1, c, f) != c){ गच्छ error; }

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

संरचना PF_शीर्षकम् {
	uint32_t chunk;
	uint32_t दीर्घता;
};


संरचना PF_cfg {
	uint64_t चरमः_db_id;
	uint8_t shutdown;
	uint8_t dbid_आकार;
};

संरचना PF_client_v5 {
	int64_t session_expiry_time;
	uint32_t session_expiry_interval;
	uint16_t चरमः_mid;
	uint16_t id_len;
};
संरचना PF_client {
	/* struct PF_client_v5; */
	int64_t session_expiry_time;
	uint32_t session_expiry_interval;
	uint16_t चरमः_mid;
	uint16_t id_len;

	uint16_t listener_पत्तनम्;
	uint16_t username_len;
	/* tail: 4 byte padding, because 64bit member
	 * forces multiple of 8 for struct size */
};
संरचना P_client {
	संरचना PF_client F;
	वर्ण *clientid;
	वर्ण *username;
};


संरचना PF_client_सन्देशः {
	dbid_t store_id;
	uint16_t mid;
	uint16_t id_len;
	uint8_t qos;
	uint8_t state;
	uint8_t retain_dup;
	uint8_t direction;
};
संरचना P_client_सन्देशः {
	संरचना PF_client_सन्देशः F;
	वर्ण *clientid;
	uint32_t subscription_identifier;
};


संरचना PF_आधारः_सन्देशः {
	dbid_t store_id;
	int64_t expiry_time;
	uint32_t payloadlen;
	uint16_t source_mid;
	uint16_t source_id_len;
	uint16_t source_username_len;
	uint16_t topic_len;
	uint16_t source_पत्तनम्;
	uint8_t qos;
	uint8_t retain;
};
संरचना P_आधारः_सन्देशः {
	संरचना PF_आधारः_सन्देशः F;
	शून्य *payload;
	संरचना mosquitto source;
	वर्ण *topic;
	mosquitto_property *properties;
};


संरचना PF_sub {
	uint32_t identifier;
	uint16_t id_len;
	uint16_t topic_len;
	uint8_t qos;
	uint8_t options;
};
संरचना P_sub {
	संरचना PF_sub F;
	वर्ण *clientid;
	वर्ण *topic;
};


संरचना PF_retain {
	dbid_t store_id;
};
संरचना P_retain {
	संरचना PF_retain F;
};


पूर्णाङ्क persist__read_string_len(FILE *db_fptr, वर्ण **str, uint16_t len);
पूर्णाङ्क persist__read_string(FILE *db_fptr, वर्ण **str);

पूर्णाङ्क persist_chunk_शीर्षकम्_read(FILE *db_fptr, uint32_t *chunk, uint32_t *दीर्घता);

पूर्णाङ्क persist_chunk_शीर्षकम्_read_v234(FILE *db_fptr, uint32_t *chunk, uint32_t *दीर्घता);
पूर्णाङ्क persist__chunk_cfg_read_v234(FILE *db_fptr, संरचना PF_cfg *chunk);
पूर्णाङ्क persist__chunk_client_read_v234(FILE *db_fptr, संरचना P_client *chunk, uint32_t db_संस्करणम्);
पूर्णाङ्क persist_chunk_client_सन्देशः_read_v234(FILE *db_fptr, संरचना P_client_सन्देशः *chunk);
पूर्णाङ्क persist_chunk_आधारः_सन्देशः_read_v234(FILE *db_fptr, संरचना P_आधारः_सन्देशः *chunk, uint32_t db_संस्करणम्);
पूर्णाङ्क persist__chunk_retain_read_v234(FILE *db_fptr, संरचना P_retain *chunk);
पूर्णाङ्क persist__chunk_sub_read_v234(FILE *db_fptr, संरचना P_sub *chunk);

पूर्णाङ्क persist_chunk_शीर्षकम्_read_v56(FILE *db_fptr, uint32_t *chunk, uint32_t *दीर्घता);
पूर्णाङ्क persist__chunk_cfg_read_v56(FILE *db_fptr, संरचना PF_cfg *chunk);
पूर्णाङ्क persist__chunk_client_read_v56(FILE *db_fptr, संरचना P_client *chunk, uint32_t db_संस्करणम्);
पूर्णाङ्क persist_chunk_client_सन्देशः_read_v56(FILE *db_fptr, संरचना P_client_सन्देशः *chunk, uint32_t दीर्घता);
पूर्णाङ्क persist_chunk_आधारः_सन्देशः_read_v56(FILE *db_fptr, संरचना P_आधारः_सन्देशः *chunk, uint32_t दीर्घता);
पूर्णाङ्क persist__chunk_retain_read_v56(FILE *db_fptr, संरचना P_retain *chunk);
पूर्णाङ्क persist__chunk_sub_read_v56(FILE *db_fptr, संरचना P_sub *chunk);

पूर्णाङ्क persist__chunk_cfg_write_v6(FILE *db_fptr, संरचना PF_cfg *chunk);
पूर्णाङ्क persist__chunk_client_write_v6(FILE *db_fptr, संरचना P_client *chunk);
पूर्णाङ्क persist_chunk_client_सन्देशः_write_v6(FILE *db_fptr, संरचना P_client_सन्देशः *chunk);
पूर्णाङ्क persist_chunk_सन्देशः_store_write_v6(FILE *db_fptr, संरचना P_आधारः_सन्देशः *chunk);
पूर्णाङ्क persist__chunk_retain_write_v6(FILE *db_fptr, संरचना P_retain *chunk);
पूर्णाङ्क persist__chunk_sub_write_v6(FILE *db_fptr, संरचना P_sub *chunk);

#यदि_अन्तः
