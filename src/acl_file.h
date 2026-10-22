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

#شامل <uthash.h>

ساختار acl__entry {
	ساختار acl__entry *next, *prev;
	نویسه *topic;
	صحیح access;
	صحیح ucount;
	صحیح ccount;
};


ساختار acl__user {
	UT_hash_handle hh;
	نویسه *username;
	ساختار acl__entry *acl;
};


ساختار acl_file_data {
	نویسه *acl_file;
	ساختار acl__user *acl_users;
	ساختار acl__user acl_anon;
	ساختار acl__entry *acl_patterns;
};


صحیح acl_file__parse(ساختار acl_file_data *data);
صحیح acl_file__check(صحیح event, تهی *event_data, تهی *userdata);
صحیح acl_file__reload(صحیح event, تهی *event_data, تهی *userdata);
تهی acl_file__cleanup(ساختار acl_file_data *data);

#endif
