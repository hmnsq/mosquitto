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
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ACL_FILE_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ACL_FILE_H

#ਅਨ੍ਤਰ੍ਭਾਵ <uthash.h>

ਬਣਤਰ acl_ਇੰਦਰਾਜ {
	ਬਣਤਰ acl_ਇੰਦਰਾਜ *ਅਗਲਾ, *prev;
	ਅੱਖਰ *topic;
	ਪੂਰਨ access;
	ਪੂਰਨ ucount;
	ਪੂਰਨ ccount;
};


ਬਣਤਰ acl__user {
	UT_hash_handle hh;
	ਅੱਖਰ *username;
	ਬਣਤਰ acl_ਇੰਦਰਾਜ *acl;
};


ਬਣਤਰ acl_file_ਅੰਕੜੇ {
	ਅੱਖਰ *acl_file;
	ਬਣਤਰ acl__user *acl_users;
	ਬਣਤਰ acl__user acl_anon;
	ਬਣਤਰ acl_ਇੰਦਰਾਜ *acl_patterns;
};


ਪੂਰਨ acl_file__parse(ਬਣਤਰ acl_file_ਅੰਕੜੇ *ਅੰਕੜੇ);
ਪੂਰਨ acl_file__check(ਪੂਰਨ event, ਖਾਲੀ *event_ਅੰਕੜੇ, ਖਾਲੀ *userdata);
ਪੂਰਨ acl_file__reload(ਪੂਰਨ event, ਖਾਲੀ *event_ਅੰਕੜੇ, ਖਾਲੀ *userdata);
ਖਾਲੀ acl_file__cleanup(ਬਣਤਰ acl_file_ਅੰਕੜੇ *ਅੰਕੜੇ);

#ਜੇ_ਅੰਤ
