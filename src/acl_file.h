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

#ಅನ್ತರ್ಭಾವ <uthash.h>

ಸಂರಚನಾ acl__entry {
	ಸಂರಚನಾ acl__entry *next, *prev;
	ವರ್ಣ *topic;
	ಪೂರ್ಣಾಙ್ಕ access;
	ಪೂರ್ಣಾಙ್ಕ ucount;
	ಪೂರ್ಣಾಙ್ಕ ccount;
};


ಸಂರಚನಾ acl__user {
	UT_hash_handle hh;
	ವರ್ಣ *username;
	ಸಂರಚನಾ acl__entry *acl;
};


ಸಂರಚನಾ acl_file_data {
	ವರ್ಣ *acl_file;
	ಸಂರಚನಾ acl__user *acl_users;
	ಸಂರಚನಾ acl__user acl_anon;
	ಸಂರಚನಾ acl__entry *acl_patterns;
};


ಪೂರ್ಣಾಙ್ಕ acl_file__parse(ಸಂರಚನಾ acl_file_data *data);
ಪೂರ್ಣಾಙ್ಕ acl_file__check(ಪೂರ್ಣಾಙ್ಕ event, ಶೂನ್ಯ *event_data, ಶೂನ್ಯ *userdata);
ಪೂರ್ಣಾಙ್ಕ acl_file__reload(ಪೂರ್ಣಾಙ್ಕ event, ಶೂನ್ಯ *event_data, ಶೂನ್ಯ *userdata);
ಶೂನ್ಯ acl_file__cleanup(ಸಂರಚನಾ acl_file_data *data);

#endif
