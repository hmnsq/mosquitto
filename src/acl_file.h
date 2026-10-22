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

#ضمن <uthash.h>

بنية acl__entry {
	بنية acl__entry *next, *prev;
	حرف *topic;
	صحيح access;
	صحيح ucount;
	صحيح ccount;
};


بنية acl__user {
	UT_hash_handle hh;
	حرف *username;
	بنية acl__entry *acl;
};


بنية acl_file_data {
	حرف *acl_file;
	بنية acl__user *acl_users;
	بنية acl__user acl_anon;
	بنية acl__entry *acl_patterns;
};


صحيح acl_file__parse(بنية acl_file_data *data);
صحيح acl_file__check(صحيح event, فراغ *event_data, فراغ *userdata);
صحيح acl_file__reload(صحيح event, فراغ *event_data, فراغ *userdata);
فراغ acl_file__cleanup(بنية acl_file_data *data);

#endif
