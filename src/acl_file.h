/*
Copyright (c) 2011-2021 Roger Light <roger@atchoo.org>

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
#ifndef ACL_FILE_H
#define ACL_FILE_H

#அந்தர்பாவ <uthash.h>

கட்டமைப்பு acl__entry {
	கட்டமைப்பு acl__entry *next, *prev;
	எழுத்து *topic;
	முழு access;
	முழு ucount;
	முழு ccount;
};


கட்டமைப்பு acl__user {
	UT_hash_handle hh;
	எழுத்து *username;
	கட்டமைப்பு acl__entry *acl;
};


கட்டமைப்பு acl_file_data {
	எழுத்து *acl_file;
	கட்டமைப்பு acl__user *acl_users;
	கட்டமைப்பு acl__user acl_anon;
	கட்டமைப்பு acl__entry *acl_patterns;
};


முழு acl_file__parse(கட்டமைப்பு acl_file_data *data);
முழு acl_file__check(முழு event, வெறுமை *event_data, வெறுமை *userdata);
முழு acl_file__reload(முழு event, வெறுமை *event_data, வெறுமை *userdata);
வெறுமை acl_file__cleanup(கட்டமைப்பு acl_file_data *data);

#endif
