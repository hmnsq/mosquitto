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

#ଅନ୍ତର୍ଭାଵ <uthash.h>

ସଂରଚନା acl__entry {
	ସଂରଚନା acl__entry *next, *prev;
	ଅକ୍ଷର *topic;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା access;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ucount;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ccount;
};


ସଂରଚନା acl__user {
	UT_hash_handle hh;
	ଅକ୍ଷର *username;
	ସଂରଚନା acl__entry *acl;
};


ସଂରଚନା acl_file_data {
	ଅକ୍ଷର *acl_file;
	ସଂରଚନା acl__user *acl_users;
	ସଂରଚନା acl__user acl_anon;
	ସଂରଚନା acl__entry *acl_patterns;
};


ପୂର୍ଣ୍ଣସଂଖ୍ୟା acl_file__parse(ସଂରଚନା acl_file_data *data);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା acl_file__check(ପୂର୍ଣ୍ଣସଂଖ୍ୟା event, ଶୂନ୍ୟ *event_data, ଶୂନ୍ୟ *userdata);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା acl_file__reload(ପୂର୍ଣ୍ଣସଂଖ୍ୟା event, ଶୂନ୍ୟ *event_data, ଶୂନ୍ୟ *userdata);
ଶୂନ୍ୟ acl_file__cleanup(ସଂରଚନା acl_file_data *data);

#endif
