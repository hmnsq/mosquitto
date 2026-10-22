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

#include <uthash.h>

sa_nracanaa acl__entry {
	sa_nracanaa acl__entry *next, *prev;
	var_na *topic;
	පූර්ණ_සංඛ්‍යා access;
	පූර්ණ_සංඛ්‍යා ucount;
	පූර්ණ_සංඛ්‍යා ccount;
};


sa_nracanaa acl__user {
	UT_hash_handle hh;
	var_na *username;
	sa_nracanaa acl__entry *acl;
};


sa_nracanaa acl_file_data {
	var_na *acl_file;
	sa_nracanaa acl__user *acl_users;
	sa_nracanaa acl__user acl_anon;
	sa_nracanaa acl__entry *acl_patterns;
};


පූර්ණ_සංඛ්‍යා acl_file__parse(sa_nracanaa acl_file_data *data);
පූර්ණ_සංඛ්‍යා acl_file__check(පූර්ණ_සංඛ්‍යා event, void *event_data, void *userdata);
පූර්ණ_සංඛ්‍යා acl_file__reload(පූර්ණ_සංඛ්‍යා event, void *event_data, void *userdata);
void acl_file__cleanup(sa_nracanaa acl_file_data *data);

#endif
