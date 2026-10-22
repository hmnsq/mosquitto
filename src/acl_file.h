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

#অন্তর্ভাৱ <uthash.h>

সংরচনা acl__entry {
	সংরচনা acl__entry *next, *prev;
	ৱর্ণ *topic;
	পূর্ণাঙ্ক access;
	পূর্ণাঙ্ক ucount;
	পূর্ণাঙ্ক ccount;
};


সংরচনা acl__user {
	UT_hash_handle hh;
	ৱর্ণ *username;
	সংরচনা acl__entry *acl;
};


সংরচনা acl_file_data {
	ৱর্ণ *acl_file;
	সংরচনা acl__user *acl_users;
	সংরচনা acl__user acl_anon;
	সংরচনা acl__entry *acl_patterns;
};


পূর্ণাঙ্ক acl_file__parse(সংরচনা acl_file_data *data);
পূর্ণাঙ্ক acl_file__check(পূর্ণাঙ্ক event, শূন্য *event_data, শূন্য *userdata);
পূর্ণাঙ্ক acl_file__reload(পূর্ণাঙ্ক event, শূন্য *event_data, শূন্য *userdata);
শূন্য acl_file__cleanup(সংরচনা acl_file_data *data);

#endif
