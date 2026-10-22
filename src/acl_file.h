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

#അന്തര്ഭാവ <uthash.h>

ഘടന acl__entry {
	ഘടന acl__entry *next, *prev;
	അക്ഷരം *topic;
	പൂർണ്ണസംഖ്യ access;
	പൂർണ്ണസംഖ്യ ucount;
	പൂർണ്ണസംഖ്യ ccount;
};


ഘടന acl__user {
	UT_hash_handle hh;
	അക്ഷരം *username;
	ഘടന acl__entry *acl;
};


ഘടന acl_file_data {
	അക്ഷരം *acl_file;
	ഘടന acl__user *acl_users;
	ഘടന acl__user acl_anon;
	ഘടന acl__entry *acl_patterns;
};


പൂർണ്ണസംഖ്യ acl_file__parse(ഘടന acl_file_data *data);
പൂർണ്ണസംഖ്യ acl_file__check(പൂർണ്ണസംഖ്യ event, ശൂന്യം *event_data, ശൂന്യം *userdata);
പൂർണ്ണസംഖ്യ acl_file__reload(പൂർണ്ണസംഖ്യ event, ശൂന്യം *event_data, ശൂന്യം *userdata);
ശൂന്യം acl_file__cleanup(ഘടന acl_file_data *data);

#endif
